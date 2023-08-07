#ifndef BF_INTERFACE_H
#define BF_INTERFACE_H

#include "bruteforce/framework/m64.h"

void bf_parse_command_line_args(int argc, char *argv[]);
u8 bf_output_input_sequence(u32 globalTimerAtStart, InputSequence *inputSequence);

const char *bf_read_file(const char *fileName);
void bf_listen_to_inputs();
void bf_update_control_state();

extern char *override_config_file;
extern char *child_args;
extern char *output_mode;

#endif // BF_INTERFACE_H