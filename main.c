#include <stdio.h>
#include "src/engine/surface_collision.h"
#include "src/engine/surface_load.h"

void main() {
	printf("Hello World");

	struct WallCollisionData data;

	int num_collisions = find_wall_collisions(&data);
	printf("\n%d", num_collisions);
}