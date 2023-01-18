#ifndef PERTURBATOR_H
#define PERTURBATOR_H

#include <PR/ultratypes.h>

#include "bruteforce/framework/json.h"

typedef struct Perturbator_s {
	u32 min_frame;
	u32 max_frame;
	f32 perturbation_chance;
	u8 max_perturbation;
} Perturbator;

typedef struct Perturbators_s {
	u32 nPerturbators;
	Perturbator *perturbators;
} Perturbators;

extern Perturbators perturbators;

void read_Perturbators(Json*, Perturbators*);

#endif // PERTURBATOR_H