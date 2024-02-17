# util.mk - Miscellaneous utility functions for use in Makefiles

NEWLINE_TOKEN := __NL__

# A literal space.
noop=
space = $(noop) $(noop)

# Joins elements of the list in arg 2 with the given separator.
#   1. Element separator.
#   2. The list.
join-with = $(subst $(space),$1,$(strip $2))

# Throws an error if the value of the variable named by $(1) is not in the list given by $(2)
define validate-option
  # value must be part of the list
  ifeq ($$(filter $($(1)),$(2)),)
    $$(error Value of $(1) must be one of the following: $(2))
  endif
  # value must be a single word (no whitespace)
  ifneq ($$(words $($(1))),1)
    $$(error Value of $(1) must be one of the following: $(2))
  endif
endef

# Creates a text file documentation by running the preprocessor on a given source file
define create-state-definition-file
	$(CC) -E -CC -P $(foreach i,$(INCLUDE_DIRS),-I$(i)) -DMODULE_PATH=$(NAME) $1 \
		| sed -e 's/$(NEWLINE_TOKEN) /\n/g' -e 's/$(NEWLINE_TOKEN)//g' -e 's/""//g' > $(BINARY_DIR)/$(NAME)/$2
endef

# Returns the path to the command $(1) if exists. Otherwise returns an empty string.
find-command = $(shell which $(1) 2>/dev/null)

# List of bruteforcing algorithms
ALGORITHMS := genetic

# List of o files compiled from the same source, but into different module directories, and their dependencies, separated by <o-file>?<dependency>
SPECIAL_O ?= $(addprefix framework/,states?bf_state_definitions.inc.c interprocess?state.h interface/interface?state.h interface/debug_json_writer?state.h) algorithms/genetic/candidates?state.h
SPECIAL_O += $(foreach a,$(ALGORITHMS),algorithms/$(a)/algorithm?state.h)

MARIO_STEP_OBJECTS := mario_step.o mario.o mario_actions_airborne.o mario_actions_moving.o mario_actions_stationary.o mario_actions_submerged.o

word-dot = $(word $2,$(subst ?, ,$1))

# Rule to compile a SPECIAL_O file
define special
$(eval _2 := $(call word-dot,$2,1))
$(eval _3 := $(call word-dot,$2,2))
$(shell mkdir -p $(dir $(BUILD_DIR)/bruteforce/$(1)/$(_2)))
$(BUILD_DIR)/bruteforce/$(1)/$(_2).o: bruteforce/$(_2).c bruteforce/modules/$(1)/$(_3)
	$(call print,Compiling special:,$$<,$$@)
	$(CC_CHECK) $(CC_CHECK_CFLAGS) -DMODULE_PATH=$(1) -MMD -MP -MT $$@ -MF $(BUILD_DIR)/bruteforce/$(1)/$(_2).d $$<
	$(V)$(CC) -c $(CFLAGS) -DMODULE_PATH=$(1) -o $$@ $$<
endef

# Function to register a module from its make.split file
define register-module
$(eval $(NAME)REQUIRED_OBJECTS += $(addprefix bruteforce/framework/interface/, m64.o readers.o json.o interface.o debug_json_writer.o))
$(eval $(NAME)REQUIRED_OBJECTS += $(addprefix bruteforce/framework/, engine_feed.o engine_stubs.o misc_util.o pipeex.o quarter_steps.o))
$(eval $(NAME)REQUIRED_OBJECTS += $(addprefix bruteforce/$(NAME)/framework/, states.o interprocess.o))
$(eval $(NAME)REQUIRED_O_FILES := $(addprefix $(BUILD_DIR)/, $($(NAME)REQUIRED_OBJECTS)))
ALL_TARGETS += $(NAME)
SRC_DIRS += bruteforce/$(NAME)
BIN_DIRS += $(NAME)
$(eval $(NAME): MODULE_PATH := $(NAME))
$(eval $(NAME): $($(NAME)ADDITIONAL_DEPENDENCIES))
$(NAME): $($(NAME)REQUIRED_O_FILES)
	$(CC) -o $(BINARY_DIR)/$(NAME)/main.exe $($(NAME)REQUIRED_O_FILES)
	$(call create-state-definition-file,./bruteforce/framework/interface/generate_state_definition.c,state_definitions.txt)

$(foreach o,$(SPECIAL_O),$(eval $(call special,$(NAME),$(o))))
endef