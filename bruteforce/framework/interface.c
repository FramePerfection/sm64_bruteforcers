#include "bruteforce/framework/interface.h"
#include "bruteforce/framework/interprocess.h"
#include "bruteforce/framework/m64.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *override_config_file = NULL;
char *child_args = NULL;
char *output_mode = "m64";
char *m64_input = NULL;
char *m64_output = NULL;
u32 max_write_fails = 10;

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
				printf("launching from %s...\n", override_config_file);
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
}

u8 save_to_sequence_file(char *fileName, InputSequence *inputSequence) {
	FILE *dst_file = fopen(fileName, "w");
	if (dst_file == NULL)
		return FALSE;
	
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
		usleep(1000000); \
	} \
	return TRUE;

u8 output_input_sequence(InputSequence *inputSequence) {
	u32 fail_counter = 0;

	if (strcmp(output_mode, "m64") == 0)
	{
		TRY_WRITE(save_to_m64_file(m64_input, m64_output, inputSequence))
	}
	else if (strcmp(output_mode, "sequence") == 0)
	{
		TRY_WRITE(save_to_sequence_file("tmp.m64.part", inputSequence))
	}
}
#undef TRY_WRITE
