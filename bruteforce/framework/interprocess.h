#ifndef INTERPROCESS_H
#define INTERPROCESS_H

#include "bruteforce/framework/candidates.h"

u8 isParentProcess();
void initializeMultiProcess(InputSequence *original_inputs);
void childUpdateMessages(Candidate *survivors);
void parentMergeCandidates(Candidate *survivors);
void safePrintf(const char* fmt, ...);

#endif // INTERPROCESS_H