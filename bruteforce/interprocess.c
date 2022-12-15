#include <PR/ultratypes.h>
#include "bruteforce/bf_states.h"
#include "bruteforce/candidates.h"
#include <string.h>

#ifdef _WIN32
#include <windows.h>
static HANDLE *parentToChildWritePipes;
static HANDLE *childToParentReadPipes;
static HANDLE childReadPipe, childWritePipe;
static OVERLAPPED readOverlapped;
static BOOL pipeReadPending = FALSE;
static HANDLE hMapFile;
static void *pBuf;
#endif

#define BUF_SIZE sizeof(ProgramState)
u32 transmissionCandidateSize;
u32 sequenceSize;
u32 pipeBufferSize;
char *pipeBuffer;

extern BOOL APIENTRY MyCreatePipeEx(
    OUT LPHANDLE lpReadPipe,
    OUT LPHANDLE lpWritePipe,
    IN LPSECURITY_ATTRIBUTES lpPipeAttributes,
    IN DWORD nSize,
    DWORD dwReadMode,
    DWORD dwWriteMode
    );

static void createPipe(HANDLE *readPipe, HANDLE *writePipe) {
	SECURITY_ATTRIBUTES saAttr;  
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL;

	if (!MyCreatePipeEx(readPipe, writePipe, &saAttr, 0, FILE_FLAG_OVERLAPPED, 0)) {
		printf("Failed to create pipe: Error code %ld\n", GetLastError());
		return;
	}
	printf("Successfully created pipes %p (read) and %p (write)\n", *readPipe, *writePipe);
}

static void createProcess(HANDLE readPipe, HANDLE writePipe, HANDLE hJob) {
	STARTUPINFO si;
	memset(&si, 0, sizeof(STARTUPINFO));
	PROCESS_INFORMATION pi;
	memset(&si, 0, sizeof(PROCESS_INFORMATION));
	
	TCHAR lpszClientPath[1024] = TEXT("main.exe");
	TCHAR lpszArgs[1024];
	sprintf(lpszArgs, " --child=%p;%p;%p", readPipe, writePipe, hMapFile);
	if (override_config_file != NULL)
		sprintf(lpszArgs + strlen(lpszArgs), " --file %s", override_config_file);

	if (!CreateProcess(lpszClientPath, lpszArgs, NULL, NULL, TRUE, CREATE_BREAKAWAY_FROM_JOB, NULL, NULL, &si, &pi)) {
		printf("Failed to launch child process. Error code %ld\n", GetLastError());
		return;
	}

	if (!AssignProcessToJobObject(hJob, pi.hProcess))
		printf("Failed AssignProcessToJobObject: %ld\n", GetLastError());
}

static void createChildProcesses() {
	SECURITY_ATTRIBUTES saAttr;  
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL;

	hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, &saAttr, PAGE_READWRITE, 0, BUF_SIZE, NULL);
	if (hMapFile == NULL) {
    	printf("Could not create file mapping (%ld).\n", GetLastError());
		return;
	}

    HANDLE                               hJob;
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };
	
	hJob = CreateJobObject(NULL, NULL);
    jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
	SetInformationJobObject( hJob, JobObjectExtendedLimitInformation, &jeli, sizeof( jeli ) );

	s32 num_processes = max(bfStaticState.max_processes, 1) - 1;
	
	parentToChildWritePipes = calloc(num_processes, sizeof(HANDLE));
	childToParentReadPipes = calloc(num_processes, sizeof(HANDLE));

	printf("Running %d processes...\n", num_processes);
	s32 i;
	for (i = 0; i < num_processes; i++) {
		HANDLE childReadPipe, childWritePipe;

		createPipe(&childReadPipe, &parentToChildWritePipes[i]);
		createPipe(&childToParentReadPipes[i], &childWritePipe);
		createProcess(childReadPipe, childWritePipe, hJob);
	}
}

void initializeMultiProcess(InputSequence *original_inputs) {
	sequenceSize = sizeof(InputSequence) + original_inputs->count * sizeof(OSContPad);
	transmissionCandidateSize = sizeof(Candidate) + sequenceSize;
	pipeBufferSize = transmissionCandidateSize * bfStaticState.survivors_per_generation;
	pipeBuffer = malloc(pipeBufferSize);
	
	ZeroMemory(&readOverlapped, sizeof(OVERLAPPED));
    readOverlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	if (child_args != NULL) {
		char *arg0, *arg1, *arg2;
		arg0 = strtok(child_args, ";");
		arg1 = strtok(NULL, ";");
		arg2 = strtok(NULL, ";");
		if (arg2 == NULL) {
			printf("Invalid arguments to create child process. Exiting...\n");
			exit(0);
		}
		char *end;
		childReadPipe = (HANDLE)strtol(arg0, &end, 0x10);
		childWritePipe = (HANDLE)strtol(arg1, &end, 0x10);
		hMapFile = (HANDLE)strtol(arg2, &end, 0x10);
	}
	else {
		childReadPipe = NULL;
		childWritePipe = NULL;
		createChildProcesses();
	}
		
   	pBuf = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);
	
	if (pBuf == NULL)
	{
		printf("Could not map view of file (%ld).\n", GetLastError());
    	CloseHandle(hMapFile);
		return;
	}
	programState = pBuf;
}

u8 isParentProcess() {
	return childReadPipe == NULL;
}

static void writeSurvivorsToBuffer(Candidate *survivors) {
	u32 i;
	for (i = 0; i < bfStaticState.survivors_per_generation; i++)
	{
		memcpy(pipeBuffer + transmissionCandidateSize * i, &survivors[i], sizeof(Candidate));
		memcpy(pipeBuffer + transmissionCandidateSize * i + sizeof(Candidate), survivors[i].sequence, sequenceSize);
	}
}

void parentMergeCandidates(Candidate *survivors) {
	s32 numChildProcesses = bfStaticState.max_processes - 1;
	if (numChildProcesses <= 0)
		return;

	DWORD written, read;
	sprintf(pipeBuffer, "merge");
	s32 childProcId;

	// Send a merge request to all children
	for (childProcId = 0; childProcId < bfStaticState.max_processes - 1; childProcId++) {
		if (!WriteFile(parentToChildWritePipes[childProcId], pipeBuffer, pipeBufferSize, &written, NULL)) {
			printf("Failed to write sync message:%ld\n", GetLastError());
		}
	}

	u32 numRuns = bfStaticState.survivors_per_generation;
	u32 k = 0;

	char *mergeBuffer = malloc(pipeBufferSize * numChildProcesses);
	Candidate **externalCandidates = malloc(sizeof(Candidate*) * numRuns * numChildProcesses);
	// Read each merge block in turn
	for (childProcId = 0; childProcId < numChildProcesses; childProcId++) {
		if (!ReadFile(childToParentReadPipes[childProcId], mergeBuffer + childProcId * pipeBufferSize, pipeBufferSize, &read, NULL)) {
			printf("Failed to read merge message:%ld\n", GetLastError());
		}
		
		u32 i;
		for (i = 0; i < numRuns; i++) {
			Candidate *candidate = (Candidate*)(mergeBuffer + i * transmissionCandidateSize + childProcId * pipeBufferSize);
			candidate->sequence = (InputSequence*)(candidate + 1);
			externalCandidates[k++] = candidate;
		}
	}

	mergeCandidates(survivors, externalCandidates, k);
	free(mergeBuffer);

	// Send the merge result to each child process
	writeSurvivorsToBuffer(survivors);
	for (childProcId = 0; childProcId < numChildProcesses; childProcId++) {
		if (!WriteFile(parentToChildWritePipes[childProcId], pipeBuffer, pipeBufferSize, &written, NULL)) {
			printf("Failed to write merge result to child %d: %ld\n", childProcId, GetLastError());
		}
	}
}

void childUpdateMessages(Candidate *survivors) {
	if (!pipeReadPending) {
		pipeReadPending = TRUE;
		DWORD read;
		ReadFile(childReadPipe, pipeBuffer, pipeBufferSize, &read, &readOverlapped);
	}

	if (WAIT_OBJECT_0 == WaitForSingleObject(readOverlapped.hEvent, 0)) {
		char *cmd = malloc(pipeBufferSize);
		sscanf(pipeBuffer, "%s", cmd);
		// The parent has requested a merge. Send our best attempts at this moment, then wait for the merged results.

		if (strcmp(cmd, "merge") == 0) {
			DWORD written, read;
			writeSurvivorsToBuffer(survivors);
			if (!WriteFile(childWritePipe, pipeBuffer, pipeBufferSize, &written, NULL)) {
				printf("Failed to fulfil merge command: %ld", GetLastError());
			}

			if (!ReadFile(childReadPipe, pipeBuffer, pipeBufferSize, &read, NULL)) {
				printf("Failed to read merge message:%ld\n", GetLastError());
			}
			u32 i;
			for (i = 0; i < bfStaticState.survivors_per_generation; i++) {
				Candidate *candidate = (Candidate*)(pipeBuffer + i * transmissionCandidateSize);
				candidate->sequence = (InputSequence*)(candidate + 1);
				survivors[i].score = candidate->score;
				survivors[i].stats = candidate->stats;
				clone_m64_inputs(survivors[i].sequence, candidate->sequence);
			}
		}

		free(cmd);

		ResetEvent(readOverlapped.hEvent);
		pipeReadPending = FALSE;
	}
}