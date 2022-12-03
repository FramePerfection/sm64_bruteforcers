# util.mk - Miscellaneous utility functions for use in Makefiles

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

# Returns the path to the command $(1) if exists. Otherwise returns an empty string.
find-command = $(shell which $(1) 2>/dev/null)

SPECIAL_O := bf_states?bf_state_definitions.inc.c candidates?state.h
word-dot = $(word $2,$(subst ?, ,$1))

define special
$(eval _2 := $(call word-dot,$2,1))
$(eval _3 := $(call word-dot,$2,2))
$(BUILD_DIR)/bruteforce/$(1)/$(_2).o: bruteforce/$(_2).c bruteforce/$(1)/$(_3)
	$(call print,$(MODULE_PATH),,)
	$(call print,Compiling:,$$<,$$@)
	$(CC_CHECK) $(CC_CHECK_CFLAGS) -DMODULE_PATH=$(1) -MMD -MP -MT $$@ -MF $(BUILD_DIR)/bruteforce/$(1)/$(_2).d $$<
	$(V)$(CC) -c $(CFLAGS) -DMODULE_PATH=$(1) -o $$@ $$<
endef

define register-lib
$(eval $(NAME)REQUIRED_O_FILES := $(addprefix $(BUILD_DIR)/, $($(NAME)REQUIRED_OBJECTS)))
ALL_TARGETS += $(NAME)
SRC_DIRS += bruteforce/$(NAME)
BIN_DIRS += $(NAME)
$(eval $(NAME): MODULE_PATH := $(NAME))
$(NAME): $($(NAME)REQUIRED_O_FILES)
	$(CC) -o $(BINARY_DIR)/$(NAME)/main.exe $($(NAME)REQUIRED_O_FILES)

$(foreach o,$(SPECIAL_O),$(eval $(call special,$(NAME),$(o))))
endef