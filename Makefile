#Makefile to build bruteforcer modules and configure build chain

export TEMP=TEMP

BUILD_CHAIN := .buildchain/

SKIP_LAUNCH_JSON := 0

include $(BUILD_CHAIN)util.mk

FIND := $(call find-command,find)

# Default target
default: all

# Preprocessor definitions
DEFINES := NO_SEGMENTED_MEMORY

# Optimization flag
OPT_FLAGS := -O3

#==============================================================================#
# Build Options                                                                #
#==============================================================================#

# These options can either be set by building with 'make SETTING=value'.
# 'make clean' may be required first.


# VERSION - selects the version of the game to build
#   jp - builds the 1996 Japanese version
#   us - builds the 1996 North American version
#   eu - builds the 1997 PAL version
#   sh - builds the 1997 Japanese Shindou version, with rumble pak support
VERSION ?= us
$(eval $(call validate-option,VERSION,jp us eu sh))

ifeq      ($(VERSION),jp)
  DEFINES   += VERSION_JP=1
  VERSION_JP_US  ?= true
else ifeq ($(VERSION),us)
  DEFINES   += VERSION_US=1
  VERSION_JP_US  ?= true
else ifeq ($(VERSION),eu)
  DEFINES   += VERSION_EU=1
  VERSION_JP_US  ?= false
else ifeq ($(VERSION),sh)
  DEFINES   += VERSION_SH=1
  VERSION_JP_US  ?= false
endif

TARGET := sm64.$(VERSION)

DEFINES += NON_MATCHING=1 AVOID_UB=1 _LANGUAGE_C=1

# Whether to hide commands or not
VERBOSE ?= 0
ifeq ($(VERBOSE),0)
  V := @
endif

# Whether to colorize build messages
COLOR ?= 1

# display selected options unless 'make clean' or 'make distclean' is run
ifeq ($(filter clean debug_config,$(MAKECMDGOALS)),)
  $(info ==== Build Options ====)
  $(info Version:        $(VERSION))
  $(info Target:         $(TARGET))
  $(info =======================)
endif

#==============================================================================#
# Target Executable and Sources                                                #
#==============================================================================#

BUILD_DIR_BASE := build
# BUILD_DIR is the location where all build artifacts are placed
BUILD_DIR      := $(BUILD_DIR_BASE)/$(VERSION)
BINARY_DIR     := binaries
LIBULTRA       := $(BUILD_DIR)/libultra.a
LD_SCRIPT      := sm64.ld

BINARY_DIRS := $(foreach d,$(wildcard ./bruteforce/modules/**/main.c),$(subst ./bruteforce/modules/,,binaries/$(dir $(d))))

# Directories containing source files
BRUTEFORCE_DIRS := $(shell $(FIND) bruteforce/* -type d)

SRC_DIRS := src src/engine src/game src/audio src/menu src/buffers $(BRUTEFORCE_DIRS) bin data assets asm lib sound
BIN_DIRS := bin bin/$(VERSION)

ULTRA_SRC_DIRS := lib/src lib/src/math lib/asm lib/data
ULTRA_BIN_DIRS := lib/bin

# Source code files
C_FILES           := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c)) $(LEVEL_C_FILES)
S_FILES           := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.s))
ULTRA_C_FILES     := $(foreach dir,$(ULTRA_SRC_DIRS),$(wildcard $(dir)/*.c))
GODDARD_C_FILES   := $(foreach dir,$(GODDARD_SRC_DIRS),$(wildcard $(dir)/*.c))
ULTRA_S_FILES     := $(foreach dir,$(ULTRA_SRC_DIRS),$(wildcard $(dir)/*.s))


# Object files
O_FILES := $(foreach file,$(C_FILES),$(BUILD_DIR)/$(file:.c=.o)) \
           $(foreach file,$(S_FILES),$(BUILD_DIR)/$(file:.s=.o))

ULTRA_O_FILES := $(foreach file,$(ULTRA_S_FILES),$(BUILD_DIR)/$(file:.s=.o)) \
                 $(foreach file,$(ULTRA_C_FILES),$(BUILD_DIR)/$(file:.c=.o))

# Automatic dependency files
DEP_FILES := $(O_FILES:.o=.d) $(ULTRA_O_FILES:.o=.d) $(GODDARD_O_FILES:.o=.d) $(BUILD_DIR)/$(LD_SCRIPT).d

# Files with GLOBAL_ASM blocks
ifeq ($(NON_MATCHING),0)
  ifeq ($(VERSION),sh)
    GLOBAL_ASM_C_FILES != grep -rl 'GLOBAL_ASM(' $(wildcard src/**/*.c) $(wildcard lib/src/*.c)
  else
    GLOBAL_ASM_C_FILES != grep -rl 'GLOBAL_ASM(' $(wildcard src/**/*.c)
  endif
GLOBAL_ASM_O_FILES = $(foreach file,$(GLOBAL_ASM_C_FILES),$(BUILD_DIR)/$(file:.c=.o))
GLOBAL_ASM_DEP = $(BUILD_DIR)/src/audio/non_matching_dep
endif


#==============================================================================#
# Compiler Options                                                             #
#==============================================================================#

CC		  := $(CROSS)gcc

INCLUDE_DIRS := include $(BUILD_DIR) $(BUILD_DIR)/include src .

C_DEFINES = $(foreach d,$(DEFINES),-D$(d)) -DMODULE_PATH=$(MODULE_PATH)
DEF_INC_CFLAGS = $(foreach i,$(INCLUDE_DIRS),-I$(i)) $(C_DEFINES)

# Prefer clang as C preprocessor if installed on the system
ifneq (,$(call find-command,clang))
  CPP      := clang
  CPPFLAGS := -E -P -x c -Wno-trigraphs $(DEF_INC_CFLAGS)
else
  CPP      := cpp
  CPPFLAGS := -P -Wno-trigraphs $(DEF_INC_CFLAGS)
endif

# Check code syntax with host compiler
CC_CHECK := gcc
CC_CHECK_CFLAGS = -fsyntax-only -fsigned-char $(CC_CFLAGS) $(TARGET_CFLAGS) -std=gnu90 -Wall -Wextra -Wno-format-security -Wno-main -DNON_MATCHING -DAVOID_UB $(DEF_INC_CFLAGS)

# C compiler options
CFLAGS = $(OPT_FLAGS) $(TARGET_CFLAGS) $(MIPSISET) $(DEF_INC_CFLAGS)
CFLAGS += -mhard-float -fno-stack-protector -fno-common -fno-zero-initialized-in-bss -fno-PIC -fno-strict-aliasing -fno-inline-functions -ffreestanding -fwrapv -Wall -Wextra

ifeq ($(shell getconf LONG_BIT), 32)
  # Work around memory allocation bug in QEMU
  export QEMU_GUEST_BASE := 1
else
  # Ensure that gcc treats the code as 32-bit
  CC_CHECK_CFLAGS += -m32
endif

# Prevent a crash with -sopt
export LANG := C

#==============================================================================#
# Miscellaneous Tools                                                          #
#==============================================================================#

ENDIAN_BITWIDTH       := $(BUILD_DIR)/endian-and-bitwidth
PRINT = printf

ifeq ($(COLOR),1)
NO_COL  := \033[0m
RED     := \033[0;31m
GREEN   := \033[0;32m
BLUE    := \033[0;34m
YELLOW  := \033[0;33m
BLINK   := \033[33;5m
endif

# Common build print status function
define print
  @$(PRINT) "$(GREEN)$(1) $(YELLOW)$(2)$(GREEN) -> $(BLUE)$(3)$(NO_COL)\n"
endef

#==============================================================================#
# Main Targets                                                                 #
#==============================================================================#

force: 
#used to force targets to be built

include $(wildcard ./bruteforce/modules/**/make.split)

include $(BUILD_CHAIN)generate_targets.mk

all: $(ALL_TARGETS)

clean:
	$(RM) -r $(BUILD_DIR_BASE)
	

ifeq ($(VERSION),sh)
  $(BUILD_DIR)/src/audio/load_sh.o: $(SOUND_BIN_DIR)/bank_sets.inc.c $(SOUND_BIN_DIR)/sequences_header.inc.c $(SOUND_BIN_DIR)/ctl_header.inc.c $(SOUND_BIN_DIR)/tbl_header.inc.c
endif

ifeq ($(COMPILER),gcc)
  $(BUILD_DIR)/lib/src/math/%.o: CFLAGS += -fno-builtin
endif

ALL_DIRS := $(BUILD_DIR) $(BINARY_DIRS) $(addprefix $(BUILD_DIR)/,$(SRC_DIRS) $(ULTRA_SRC_DIRS) $(ULTRA_BIN_DIRS) $(BIN_DIRS))

# Make sure build directory exists before compiling anything
DUMMY := $(shell mkdir -p $(ALL_DIRS) ./TEMP)


#==============================================================================#
# Compilation Recipes                                                          #
#==============================================================================#

# Compile C code
	
$(BUILD_DIR)/bruteforce/%.o: bruteforce/%.c
	$(call print,Compiling (warnings treated as errors):,$<,$@)
	$(CC_CHECK) $(CC_CHECK_CFLAGS) -Werror -MMD -MP -MT $@ -MF $(BUILD_DIR)/bruteforce/$*.d $<
	$(V)$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR)/%.o: %.c
	$(call print,Compiling:,$<,$@)
	$(CC_CHECK) $(CC_CHECK_CFLAGS) -MMD -MP -MT $@ -MF $(BUILD_DIR)/$*.d $<
	$(V)$(CC) -c $(CFLAGS) -o $@ $<
$(BUILD_DIR)/%.o: $(BUILD_DIR)/%.c
	$(call print,Compiling:,$<,$@)
	@$(CC_CHECK) $(CC_CHECK_CFLAGS) -MMD -MP -MT $@ -MF $(BUILD_DIR)/$*.d $<
	$(V)$(CC) -c $(CFLAGS) -o $@ $<

.PHONY: all clean default diff test load libultra force
# with no prerequisites, .SECONDARY causes no intermediate target to be removed
.SECONDARY:

# Remove built-in rules, to improve performance
MAKEFLAGS += --no-builtin-rules

-include $(DEP_FILES)

print-% : ; $(info $* is a $(flavor $*) variable set to [$($*)]) @true
