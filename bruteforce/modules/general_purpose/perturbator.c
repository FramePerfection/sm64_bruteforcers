#include "perturbator.h"

#include <memory.h>
#include <stdlib.h>
#include <string.h>

void read_Perturbators(Json *jsonNode, Perturbators *target) {
	target->nPerturbators = jsonNode->size;
	target->perturbators = calloc(target->nPerturbators, sizeof(Perturbator));
	Perturbator *currentPerturbator = target->perturbators;

	Json *n = jsonNode->child;
	while (n) {
		Json *innerNode = n->child;
		while (innerNode) {
			if (strcmp(innerNode->name, "min_frame") == 0)
				currentPerturbator->min_frame = innerNode->valueInt;
			if (strcmp(innerNode->name, "max_frame") == 0)
				currentPerturbator->max_frame = innerNode->valueInt;
			if (strcmp(innerNode->name, "perturbation_chance") == 0)
				currentPerturbator->perturbation_chance = innerNode->valueFloat;
			if (strcmp(innerNode->name, "max_perturbation") == 0)
				currentPerturbator->max_perturbation = innerNode->valueInt;
			innerNode = innerNode->next;
		}
		n = n->next;
		currentPerturbator++;
	}
}