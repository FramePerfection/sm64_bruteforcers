#ifndef BF_INTERFACE_H
#define BF_INTERFACE_H

#include "m64.h"

extern char *override_config_file;
extern char *child_args;
extern char *output_mode;
extern u8 desynced;

void bf_parse_command_line_args(int argc, char *argv[]);
u8 bf_output_input_sequence(u32 globalTimerAtStart, InputSequence *inputSequence);

const char *bf_read_file(const char *fileName);
void bf_listen_to_inputs();
void bf_update_control_state();

void bf_desync(char *message);

#endif // BF_INTERFACE_H