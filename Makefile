# Makefile to rebuild SM64 split image

include util.mk

# Default target
default: all

# Preprocessor definitions
DEFINES :=

#==============================================================================#
# Build Options                                                                #
#==============================================================================#

# These options can either be set by building with 'make SETTING=value'.
# 'make clean' may be required first.

# Build for the N64 (turn this off for ports)
TARGET_N64 ?= 0


# COMPILER - selects the C compiler to use
#   ido - uses the SGI IRIS Development Option compiler, which is used to build
#         an original matching N64 ROM
#   gcc - uses the GNU C Compiler
COMPILER ?= gcc
$(eval $(call validate-option,COMPILER,ido gcc))


# VERSION - selects the version of the game to build
#   jp - builds the 1996 Japanese version
#   us - builds the 1996 North American version
#   eu - builds the 1997 PAL version
#   sh - builds the 1997 Japanese Shindou version, with rumble pak support
VERSION ?= us
$(eval $(call validate-option,VERSION,jp us eu sh))

ifeq      ($(VERSION),jp)
  DEFINES   += VERSION_JP=1
  OPT_FLAGS := -g
  GRUCODE   ?= f3d_old
  VERSION_JP_US  ?= true
else ifeq ($(VERSION),us)
  DEFINES   += VERSION_US=1
  OPT_FLAGS := -g
  GRUCODE   ?= f3d_old
  VERSION_JP_US  ?= true
else ifeq ($(VERSION),eu)
  DEFINES   += VERSION_EU=1
  OPT_FLAGS := -O2
  GRUCODE   ?= f3d_new
  VERSION_JP_US  ?= false
else ifeq ($(VERSION),sh)
  DEFINES   += VERSION_SH=1
  OPT_FLAGS := -O2
  GRUCODE   ?= f3d_new
  VERSION_JP_US  ?= false
endif

TARGET := sm64.$(VERSION)

DEFINES += NON_MATCHING=1 AVOID_UB=1 _LANGUAGE_C=1

TARGET_STRING := sm64.$(VERSION).$(GRUCODE)
# If non-default settings were chosen, disable COMPARE
ifeq ($(filter $(TARGET_STRING), sm64.jp.f3d_old sm64.us.f3d_old sm64.eu.f3d_new sm64.sh.f3d_new),)
  COMPARE := 0
endif

# Whether to hide commands or not
VERBOSE ?= 0
ifeq ($(VERBOSE),0)
  V := @
endif

# Whether to colorize build messages
COLOR ?= 1

# display selected options unless 'make clean' or 'make distclean' is run
ifeq ($(filter clean distclean,$(MAKECMDGOALS)),)
  $(info ==== Build Options ====)
  $(info Version:        $(VERSION))
  $(info Microcode:      $(GRUCODE))
  $(info Target:         $(TARGET))
  ifeq ($(COMPARE),1)
    $(info Compare ROM:    yes)
  else
    $(info Compare ROM:    no)
  endif
  ifeq ($(NON_MATCHING),1)
    $(info Build Matching: no)
  else
    $(info Build Matching: yes)
  endif
  $(info =======================)
endif


#==============================================================================#
# Universal Dependencies                                                       #
#==============================================================================#

TOOLS_DIR := tools

# (This is a bit hacky, but a lot of rules implicitly depend
# on tools and assets, and we use directory globs further down
# in the makefile that we want should cover assets.)

PYTHON := python3

ifeq ($(filter clean distclean print-%,$(MAKECMDGOALS)),)

  # Make sure assets exist
  NOEXTRACT ?= 0
  ifeq ($(NOEXTRACT),0)
    DUMMY != $(PYTHON) extract_assets.py $(VERSION) >&2 || echo FAIL
    ifeq ($(DUMMY),FAIL)
      $(error Failed to extract assets)
    endif
  endif

  # Make tools if out of date
  $(info Building tools...)
  DUMMY != $(MAKE) -s -C $(TOOLS_DIR) $(if $(filter-out ido0,$(COMPILER)$(USE_QEMU_IRIX)),all-except-recomp,) >&2 || echo FAIL
    ifeq ($(DUMMY),FAIL)
      $(error Failed to build tools)
    endif
  $(info Building ROM...)

endif


#==============================================================================#
# Target Executable and Sources                                                #
#==============================================================================#

BUILD_DIR_BASE := build
# BUILD_DIR is the location where all build artifacts are placed
BUILD_DIR      := $(BUILD_DIR_BASE)/$(VERSION)
EXE            := $(BUILD_DIR)/$(TARGET).exe
ELF            := $(BUILD_DIR)/$(TARGET).elf
LIBULTRA       := $(BUILD_DIR)/libultra.a
LD_SCRIPT      := sm64.ld

# Directories containing source files
SRC_DIRS := src src/engine src/game src/audio src/menu src/buffers bruteforce bin data assets asm lib sound
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

AS        := $(CROSS)as
ifeq ($(COMPILER),gcc)
  CC      := $(CROSS)gcc
else
  ifeq ($(USE_QEMU_IRIX),1)
    IRIX_ROOT := $(TOOLS_DIR)/ido5.3_compiler
    CC      := $(QEMU_IRIX) -silent -L $(IRIX_ROOT) $(IRIX_ROOT)/usr/bin/cc
    ACPP    := $(QEMU_IRIX) -silent -L $(IRIX_ROOT) $(IRIX_ROOT)/usr/lib/acpp
    COPT    := $(QEMU_IRIX) -silent -L $(IRIX_ROOT) $(IRIX_ROOT)/usr/lib/copt
  else
    IDO_ROOT := $(TOOLS_DIR)/ido5.3_recomp
    CC      := $(IDO_ROOT)/cc
    ACPP    := $(IDO_ROOT)/acpp
    COPT    := $(IDO_ROOT)/copt
  endif
endif
LD        := $(CROSS)ld
AR        := $(CROSS)ar
OBJDUMP   := $(CROSS)objdump
OBJCOPY   := $(CROSS)objcopy

ifeq ($(TARGET_N64),1)
  TARGET_CFLAGS := -nostdinc -DTARGET_N64 -D_LANGUAGE_C
  CC_CFLAGS := -fno-builtin
endif

INCLUDE_DIRS := include $(BUILD_DIR) $(BUILD_DIR)/include src .
ifeq ($(TARGET_N64),1)
  INCLUDE_DIRS += include/libc
endif

C_DEFINES := $(foreach d,$(DEFINES),-D$(d))
DEF_INC_CFLAGS := $(foreach i,$(INCLUDE_DIRS),-I$(i)) $(C_DEFINES)

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
CC_CHECK_CFLAGS := -fsyntax-only -fsigned-char $(CC_CFLAGS) $(TARGET_CFLAGS) -std=gnu90 -Wall -Wextra -Wno-format-security -Wno-main -DNON_MATCHING -DAVOID_UB $(DEF_INC_CFLAGS)

# C compiler options
CFLAGS = $(OPT_FLAGS) $(TARGET_CFLAGS) $(MIPSISET) $(DEF_INC_CFLAGS)
ifeq ($(COMPILER),gcc)
  CFLAGS += -mhard-float -fno-stack-protector -fno-common -fno-zero-initialized-in-bss -fno-PIC -fno-strict-aliasing -fno-inline-functions -ffreestanding -fwrapv -Wall -Wextra
else
  CFLAGS += -non_shared -Wab,-r4300_mul -Xcpluscomm -Xfullwarn -signed -32
endif

ASFLAGS     := -march=vr4300 -mabi=32 $(foreach i,$(INCLUDE_DIRS),-I$(i)) $(foreach d,$(DEFINES),--defsym $(d))
RSPASMFLAGS := $(foreach d,$(DEFINES),-definelabel $(subst =, ,$(d)))

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

# N64 tools
MIO0TOOL              := $(TOOLS_DIR)/mio0
N64CKSUM              := $(TOOLS_DIR)/n64cksum
N64GRAPHICS           := $(TOOLS_DIR)/n64graphics
N64GRAPHICS_CI        := $(TOOLS_DIR)/n64graphics_ci
TEXTCONV              := $(TOOLS_DIR)/textconv
AIFF_EXTRACT_CODEBOOK := $(TOOLS_DIR)/aiff_extract_codebook
VADPCM_ENC            := $(TOOLS_DIR)/vadpcm_enc
EXTRACT_DATA_FOR_MIO  := $(TOOLS_DIR)/extract_data_for_mio
SKYCONV               := $(TOOLS_DIR)/skyconv
# Use the system installed armips if available. Otherwise use the one provided with this repository.
ifneq (,$(call find-command,armips))
  RSPASM              := armips
else
  RSPASM              := $(TOOLS_DIR)/armips
endif
ENDIAN_BITWIDTH       := $(BUILD_DIR)/endian-and-bitwidth
SHA1SUM = sha1sum
PRINT = printf

ifeq ($(COLOR),1)
NO_COL  := \033[0m
RED     := \033[0;31m
GREEN   := \033[0;32m
BLUE    := \033[0;34m
YELLOW  := \033[0;33m
BLINK   := \033[33;5m
endif

# Use Objcopy instead of extract_data_for_mio
ifeq ($(COMPILER),gcc)
  EXTRACT_DATA_FOR_MIO := $(OBJCOPY) -O binary --only-section=.data
endif

# Common build print status function
define print
  @$(PRINT) "$(GREEN)$(1) $(YELLOW)$(2)$(GREEN) -> $(BLUE)$(3)$(NO_COL)\n"
endef

#==============================================================================#
# Main Targets                                                                 #
#==============================================================================#

all: $(TARGET)

REQUIRED_OBJECTS := bruteforce/main.o
REQUIRED_OBJECTS += bruteforce/bf_states.o bruteforce/readers.o bruteforce/json.o bruteforce/_engine_feed.o bruteforce/_engine_stubs.o
REQUIRED_OBJECTS += src/engine/math_util.o src/engine/surface_collision.o src/engine/surface_load_reduced.o src/game/camera_reduced.o
REQUIRED_OBJECTS += src/game/mario_step.o src/game/mario.o src/game/mario_actions_airborne.o
REQUIRED_O_FILES := $(addprefix $(BUILD_DIR)/, $(REQUIRED_OBJECTS))

run: $(TARGET)
	@$(info running...)
	@./$(BUILD_DIR)/main.exe

$(TARGET): $(REQUIRED_O_FILES)
	$(CC) -o $(BUILD_DIR)/main.exe $(REQUIRED_O_FILES)

clean:
	$(RM) -r $(BUILD_DIR_BASE)

distclean: cleanmake 
	$(PYTHON) extract_assets.py --clean
	$(MAKE) -C $(TOOLS_DIR) clean

libultra: $(BUILD_DIR)/libultra.a

ifeq ($(VERSION),sh)
  $(BUILD_DIR)/src/audio/load_sh.o: $(SOUND_BIN_DIR)/bank_sets.inc.c $(SOUND_BIN_DIR)/sequences_header.inc.c $(SOUND_BIN_DIR)/ctl_header.inc.c $(SOUND_BIN_DIR)/tbl_header.inc.c
endif

ifeq ($(COMPILER),gcc)
  $(BUILD_DIR)/lib/src/math/%.o: CFLAGS += -fno-builtin
endif

ALL_DIRS := $(BUILD_DIR) $(addprefix $(BUILD_DIR)/,$(SRC_DIRS) $(ULTRA_SRC_DIRS) $(ULTRA_BIN_DIRS) $(BIN_DIRS))

# Make sure build directory exists before compiling anything
DUMMY := $(shell mkdir -p $(ALL_DIRS))


#==============================================================================#
# Compilation Recipes                                                          #
#==============================================================================#

# Compile C code
$(BUILD_DIR)/%.o: %.c
	$(call print,Compiling:,$<,$@)
	$(CC_CHECK) $(CC_CHECK_CFLAGS) -MMD -MP -MT $@ -MF $(BUILD_DIR)/$*.d $<
	$(V)$(CC) -c $(CFLAGS) -o $@ $<
$(BUILD_DIR)/%.o: $(BUILD_DIR)/%.c
	$(call print,Compiling:,$<,$@)
	@$(CC_CHECK) $(CC_CHECK_CFLAGS) -MMD -MP -MT $@ -MF $(BUILD_DIR)/$*.d $<
	$(V)$(CC) -c $(CFLAGS) -o $@ $<

.PHONY: all clean distclean default diff test load libultra
# with no prerequisites, .SECONDARY causes no intermediate target to be removed
.SECONDARY:

# Remove built-in rules, to improve performance
MAKEFLAGS += --no-builtin-rules

-include $(DEP_FILES)

print-% : ; $(info $* is a $(flavor $*) variable set to [$($*)]) @true
