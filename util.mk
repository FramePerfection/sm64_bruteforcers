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

define register-lib
$(eval $(NAME)REQUIRED_O_FILES := $(addprefix $(BUILD_DIR)/, $($(NAME)REQUIRED_OBJECTS)))
ALL_TARGETS += $(NAME)
SRC_DIRS += bruteforce/$(NAME)
BIN_DIRS += $(NAME)
$(NAME): MODULE_PATH := $(NAME)
$(NAME): $($(NAME)REQUIRED_O_FILES)
	$(CC) -o $(BINARY_DIR)/$(NAME)/main.exe $($(NAME)REQUIRED_O_FILES)
endef