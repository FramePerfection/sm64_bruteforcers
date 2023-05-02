#ifndef INTERPROCESS_H
#define INTERPROCESS_H

#include "bruteforce/framework/candidates.h"

u8 isParentProcess();
void bf_initialize_multi_process(InputSequence *original_inputs);
void bf_child_update_messages(Candidate *survivors);
void bf_parent_merge_candidates(Candidate *survivors);
void bf_safe_printf(const char *fmt, ...);

#endif // INTERPROCESS_H