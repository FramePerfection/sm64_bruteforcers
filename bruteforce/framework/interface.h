#include "bruteforce/framework/m64.h"

void parse_command_line_args(int argc, char *argv[]);
u8 output_input_sequence(u32 globalTimerAtStart, InputSequence *inputSequence);

const char *read_file(const char *fileName);

extern char *override_config_file;
extern char *child_args;
extern char *output_mode;