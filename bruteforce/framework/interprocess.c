#include "bruteforce/framework/interprocess.h"

#include "bruteforce/framework/states.h"

#include "bruteforce/framework/interface/interface.h"

#include <PR/ultratypes.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
static HANDLE sMapFile = NULL;
static void *sSharedMemory = NULL;
static HANDLE mutex = NULL;
#endif

#define ALIGN_8(input) (((input + 7) / 8) * 8)

static unsigned int sProcessIndex;

static struct SharedMemoryInitNode_s {
    unsigned int offset;
    MemoryAllocatedCallback callback;
    struct SharedMemoryInitNode_s *next;
};
typedef struct SharedMemoryInitNode_s SharedMemoryInitNode;
static SharedMemoryInitNode *sSharedMemoryInitList = NULL;
static sReservedSharedMemoryBytes = ALIGN_8(sizeof(ProgramState)) + ALIGN_8(sizeof(BFControlState));


static void createMutex()
{
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
    mutex = CreateMutex(&saAttr, FALSE, "bf_print_lock");
}

static void createProcess(HANDLE hJob, unsigned int processIndex)
{
    STARTUPINFO si;
    memset(&si, 0, sizeof(STARTUPINFO));
    PROCESS_INFORMATION pi;
    memset(&pi, 0, sizeof(PROCESS_INFORMATION));

    TCHAR lpszArgs[1024];
    sprintf(lpszArgs, "main.exe --child=%p;%d", sMapFile, processIndex);
    if (override_config_file != NULL)
        sprintf(lpszArgs + strlen(lpszArgs), " --file=%s --outputmode=%s", override_config_file, output_mode);

    if (!CreateProcess(NULL, lpszArgs, NULL, NULL, TRUE, CREATE_BREAKAWAY_FROM_JOB, NULL, NULL, &si, &pi))
    {
        bf_safe_printf("Failed to launch child process. Error code %ld\n", GetLastError());
        return;
    }

    if (!AssignProcessToJobObject(hJob, pi.hProcess))
        bf_safe_printf("Failed AssignProcessToJobObject: %ld\n", GetLastError());
}

static void createMapFile(unsigned int bufferSize)
{
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    sMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, &saAttr, PAGE_READWRITE, 0, bufferSize, NULL);
    if (sMapFile == NULL)
    {
        bf_safe_printf("Could not create file mapping (%ld).\n", GetLastError());
        return;
    }
}

static void createMapFileView(unsigned int bufferSize)
{
    sSharedMemory = MapViewOfFile(sMapFile, FILE_MAP_ALL_ACCESS, 0, 0, bufferSize);
    if (sSharedMemory == NULL)
    {
        bf_safe_printf("Could not map view of file (%ld).\n", GetLastError());
        CloseHandle(sMapFile);
        return;
    }
}

static void createChildProcesses()
{
    HANDLE hJob;
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = {0};

    hJob = CreateJobObject(NULL, NULL);
    jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli));

    bf_safe_printf("Running %d processes...\n", bfStaticState.max_processes);
    s32 i;
    for (i = 1; i < bfStaticState.max_processes; i++)
        createProcess(hJob, i);
}

u8 bf_is_parent_process()
{
    return child_args == NULL;
}

unsigned int bf_get_process_index() {
    return sProcessIndex;
}

void bf_reserve_shared_memory(unsigned int size, MemoryAllocatedCallback callback)
{
    SharedMemoryInitNode *n = sSharedMemoryInitList;
    SharedMemoryInitNode *newNode = malloc(sizeof(SharedMemoryInitNode));
    newNode->callback = callback;
    newNode->offset = sReservedSharedMemoryBytes;
    newNode->next = NULL;
    sReservedSharedMemoryBytes += ALIGN_8(size);
    if (n == NULL)
        sSharedMemoryInitList = newNode;
    else
    {
        while (n->next != NULL)
            n = n->next;
        n->next = newNode;
    }
}

void bf_start_multiprocessing()
{
    createMutex();
    if (child_args != NULL)
    {
        char *arg0, *arg1;
        arg0 = strtok(child_args, ";");
        arg1 = strtok(NULL, ";");
        if (arg1 == NULL)
        {
            bf_safe_printf("Invalid arguments to create child process. Exiting...\n");
            exit(0);
        }
        char *end;
        sMapFile = (HANDLE)strtol(arg0, &end, 0x10);
        sProcessIndex = (unsigned int)strtol(arg1, &end, 10);
        createMapFileView(sReservedSharedMemoryBytes);
    }
    else
    {
        sProcessIndex = 0;
        createMapFile(sReservedSharedMemoryBytes);
        createMapFileView(sReservedSharedMemoryBytes);
        createChildProcesses();
        bf_listen_to_inputs();
    }
    
    programState = sSharedMemory;
    BFControlState *initialState = bfControlState;
    bfControlState = (BFControlState *)(sSharedMemory + ALIGN_8(sizeof(ProgramState)));
    memcpy(bfControlState, initialState, sizeof(BFControlState));
    SharedMemoryInitNode *n;
    for (n = sSharedMemoryInitList; n != NULL; n = n->next)
        n->callback(sSharedMemory + n->offset);

    bf_safe_printf("process initialized with id %d.\n", bf_get_process_index());
}

void bf_safe_printf(const char *fmt, ...)
{
    if (mutex)
        WaitForSingleObject(mutex, INFINITE);
    else if (!bf_is_parent_process())
        return;
    va_list argptr;
    va_start(argptr, fmt);
    vprintf(fmt, argptr);
    fflush(stdout);
    va_end(argptr);
    if (mutex)
        ReleaseMutex(mutex);
}