#include "bruteforce/framework/interface.h"
#include "bruteforce/framework/interprocess.h"
#include "bruteforce/framework/m64.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

char *override_config_file = NULL;
char *child_args = NULL;
char *output_mode = "m64";
char *m64_input = NULL;
char *m64_output = NULL;
u32 max_write_fails = 10;

const char *read_file(const char *fileName) {
	FILE *file = fopen(fileName, "r");
	if (!file)
		return NULL;
	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	fseek(file, 0, SEEK_SET);
	const char *fileContents = calloc(size, sizeof(char));
	fread((char*)fileContents, sizeof(char), size, file);
	fclose(file);
	return fileContents;
}

void parse_command_line_args(int argc, char *argv[]) {
	int c, long_opt_idx = 0;
	static struct option long_opts[] = {
		{"file", required_argument, 0, 'f'},
		{"child", required_argument, 0, 'c'},
		{"outputmode", required_argument, 0, 'm'}
	};

#define CopyArg(target) \
	target = malloc(sizeof(char) * (strlen(optarg) + 1)); \
	strcpy(target, optarg)

	while ((c = getopt_long(argc, argv, "f:c:", long_opts, &long_opt_idx)) != -1) {
		switch (c) {
			case 'f':
				CopyArg(override_config_file);
				break;
			case 'c':
				CopyArg(child_args);
				break;
			case 'm':
				CopyArg(output_mode);
				break;
		}
	}
#undef CopyArg

	if (override_config_file)
		safePrintf("launching from %s...\n", override_config_file);
}

u8 save_to_sequence_file(char *fileName, u32 globalTimerAtStart, InputSequence *inputSequence) {
	FILE *dst_file = fopen(fileName, "w");
	if (dst_file == NULL)
		return FALSE;
	u8 globalTimerBytes[4] = {globalTimerAtStart >> 0x18, globalTimerAtStart >> 0x10, globalTimerAtStart >> 0x8, globalTimerAtStart};
	fwrite_hex32string(dst_file, globalTimerBytes);
	fwrite_input_sequence(dst_file, inputSequence);
	fclose(dst_file);
	return TRUE;
}

#define TRY_WRITE(expr) \
	while (!expr) { \
		if(++fail_counter > max_write_fails){ \
			safePrintf("Failed to write result to file!\n"); \
			return FALSE; \
		} \
		nanosleep(&ts, &ts); \
	}

u8 output_input_sequence(u32 globalTimerAtStart, InputSequence *inputSequence) {
	u32 fail_counter = 0;
	struct timespec ts;
	ts.tv_sec = 1;
	ts.tv_nsec = 0;
	
	if (strcmp(output_mode, "m64") == 0 || strcmp(output_mode, "m64_and_sequence") == 0)
	{
		TRY_WRITE(save_to_m64_file(m64_input, m64_output, inputSequence))
	}
	if (strcmp(output_mode, "sequence") == 0 || strcmp(output_mode, "m64_and_sequence") == 0)
	{
		TRY_WRITE(save_to_sequence_file("tmp.m64.part", globalTimerAtStart, inputSequence))
	}
	return TRUE;
}
#undef TRY_WRITE
