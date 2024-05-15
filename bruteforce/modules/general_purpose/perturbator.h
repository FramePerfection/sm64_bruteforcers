#ifndef BF_PERTURBATOR_H
#define BF_PERTURBATOR_H

#include <PR/ultratypes.h>

#include "bruteforce/framework/interface/json.h"

typedef struct Perturbator_s
{
    u32 min_frame;
    u32 max_frame;
    f32 perturbation_chance;
    u8 max_perturbation;
} Perturbator;

typedef struct Perturbators_s
{
    u32 nPerturbators;
    Perturbator *perturbators;
} Perturbators;

extern Perturbators perturbators;

void bf_read_Perturbators(Json *, Perturbators *);

#endif // BF_PERTURBATOR_H