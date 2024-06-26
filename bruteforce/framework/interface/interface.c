#include "interface.h"

#include "bruteforce/framework/interprocess.h"
#include "bruteforce/framework/states.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define CONTROL_STATE_JSON_BUFFER 4096

#ifdef _WIN32
#include <windows.h>
DWORD threadIdInputListener;
#endif

char *override_config_file = NULL;
char *child_args = NULL;
char *output_mode = "m64";
char *m64_input = NULL;
char *m64_output = NULL;
u32 max_write_fails = 10;

u8 desynced;

const char *bf_read_file(const char *fileName)
{
    FILE *file = fopen(fileName, "r");
    if (!file)
        return NULL;
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    const char *fileContents = calloc(size, sizeof(char));
    fread((char *)fileContents, sizeof(char), size, file);
    fclose(file);
    return fileContents;
}

void bf_parse_command_line_args(int argc, char *argv[])
{
    int c, long_opt_idx = 0;
    static struct option long_opts[] = {
        {"file", required_argument, 0, 'f'},
        {"child", required_argument, 0, 'c'},
        {"outputmode", required_argument, 0, 'm'}};

    // clang-format off

	#define CopyArg(target)                                   \
		target = malloc(sizeof(char) * (strlen(optarg) + 1)); \
		strcpy(target, optarg)

	while ((c = getopt_long(argc, argv, "f:c:", long_opts, &long_opt_idx)) != -1)
	{
		switch (c)
		{
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
    // clang-format on

    if (override_config_file)
        bf_safe_printf("launching from %s...\n", override_config_file);
}

static char *s_input_string(FILE *fp, size_t bufferSize)
{
    char *str;
    int ch;
    size_t len = 0;
    str = realloc(NULL, sizeof(*str) * bufferSize);
    if (!str)
        return str;
        
    while (EOF != (ch = fgetc(fp)) && ch != '\0')
    {
        str[len++] = ch;
        if (len == bufferSize)
        {
            str = realloc(str, sizeof(*str) * (bufferSize += 16));
            if (!str)
                return str;
        }
    }
    str[len++] = '\0';

    return realloc(str, sizeof(*str) * len);
}

#ifdef _WIN32
static DWORD WINAPI s_listen_to_input_func(__attribute__((unused)) LPVOID lpParam)
#else
static void *s_listen_to_input_func()
#endif
{
    // loop indefinitely
    while (1)
    {
        char *inputText = s_input_string(stdin, CONTROL_STATE_JSON_BUFFER);
        if (inputText) {
            bf_safe_printf("Updating control state!\n");
            bf_update_control_state(inputText);
            free(inputText);
        }
        else
            bf_safe_printf("Failed to read control state input!\n");

        // sleep to not hog the processor, just in case inputs come in too fast
        struct timespec ts;
        ts.tv_sec = 1;
        ts.tv_nsec = 0;
        nanosleep(&ts, &ts);
    }
    return 0;
}

void bf_listen_to_inputs()
{
#ifdef _WIN32
    CreateThread(NULL, 0, s_listen_to_input_func, NULL, 0, &threadIdInputListener);
#else
    // TODO(Important): Implement posix thread start
#endif
}

static u8 s_save_to_sequence_file(char *fileName, u32 globalTimerAtStart, InputSequence *inputSequence)
{
    FILE *dst_file = fopen(fileName, "w");
    if (dst_file == NULL)
        return FALSE;
    u8 globalTimerBytes[4] = {globalTimerAtStart >> 0x18, globalTimerAtStart >> 0x10, globalTimerAtStart >> 0x8, globalTimerAtStart};
    bf_fwrite_hex32string(dst_file, globalTimerBytes);
    bf_fwrite_input_sequence(dst_file, inputSequence);
    fclose(dst_file);
    return TRUE;
}

#define TRY_WRITE(expr)                                          \
    while (!expr)                                                \
    {                                                            \
        if (++fail_counter > max_write_fails)                    \
        {                                                        \
            bf_safe_printf("Failed to write result to file!\n"); \
            return FALSE;                                        \
        }                                                        \
        nanosleep(&ts, &ts);                                     \
    }

u8 bf_output_input_sequence(u32 globalTimerAtStart, InputSequence *inputSequence)
{
    u32 fail_counter = 0;
    struct timespec ts;
    ts.tv_sec = 1;
    ts.tv_nsec = 0;

    if (strcmp(output_mode, "m64") == 0 || strcmp(output_mode, "m64_and_sequence") == 0)
    {
        TRY_WRITE(bf_save_to_m64_file(m64_input, m64_output, inputSequence))
    }
    if (strcmp(output_mode, "sequence") == 0 || strcmp(output_mode, "m64_and_sequence") == 0)
    {
        TRY_WRITE(s_save_to_sequence_file("tmp.m64.part", globalTimerAtStart, inputSequence))
    }
    return TRUE;
}
#undef TRY_WRITE

void bf_desync(char *message)
{
    desynced = TRUE;
    if (bfStaticState.display_desync_messages)
        bf_safe_printf(message);
}