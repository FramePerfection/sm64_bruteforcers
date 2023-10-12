#include "bruteforce/modules/general_purpose/behavior_function_map.h"
#include "bruteforce/framework/interprocess.h"

#define BF_MAP_BEHAVIOR_FUNC(FUNC, ORIGINAL_ADDRESS) extern void *FUNC;
#include "bruteforce/modules/general_purpose/behavior_function_map.h"
#undef BF_MAP_BEHAVIOR_FUNC

void *bf_map_native_behavior_func(int input) {

	switch(input)
	{
#define BF_MAP_BEHAVIOR_FUNC(FUNC, ORIGINAL_ADDRESS) case ORIGINAL_ADDRESS: return &FUNC;
#include "bruteforce/modules/general_purpose/behavior_function_map.h"
#undef BF_MAP_BEHAVIOR_FUNC
		default:
			bf_safe_printf("Encountered unmapped behavior function address '0x%08X'!\n", input);
			return 0;
	}

}