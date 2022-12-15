#ifndef INTERPROCESS_H
#define INTERPROCESS_H

#include "bruteforce/candidates.h"

u8 isParentProcess();
void initializeMultiProcess(InputSequence *original_inputs);
void writeSurvivorsToBuffer(Candidate *survivors);
void childUpdateMessages(Candidate *survivors);
void parentMergeCandidates(Candidate *survivors);

#endif // INTERPROCESS_H