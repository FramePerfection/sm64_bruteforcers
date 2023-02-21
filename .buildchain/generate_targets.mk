.PHONY: debug_config

all: debug_config

debug_config: .vscode/launch.json .vscode/tasks.json

# Target for vscode's launch.json file, generating debugging targets for all detected modules
ifneq ($(SKIP_LAUNCH_JSON),1)
.vscode/launch.json: force
	@printf "#include \".buildchain/generate_launch.c\"\nLAUNCH_JSON($(call join-with,__COMMA ,$(foreach t,$(ALL_TARGETS),LAUNCH_CONFIG($(t)))))" \
		| $(CC) -E -CC -P -	\
		| sed -e 's/$(NEWLINE_TOKEN) /\n/g' -e 's/$(NEWLINE_TOKEN)//g' -e 's/__QUOTE__/\"/g' -e 's/__TAB__/\t/g' -e 's/\t /\t/g' \
		> .vscode/launch.json
	@echo successfully created .vscode/launch.json
endif
	
# Target for vscode's tasks.json file, generating debugging targets for all detected modules
.vscode/tasks.json: force
	@printf "#include \".buildchain/generate_tasks.c\"\nTASKS_JSON($(call join-with,__COMMA ,$(foreach t,$(ALL_TARGETS),TASK_CONFIG($(t)))))" \
		| $(CC) -E -CC -P -	\
		| sed -e 's/$(NEWLINE_TOKEN) /\n/g' -e 's/$(NEWLINE_TOKEN)//g' -e 's/__QUOTE__/\"/g' -e 's/__TAB__/\t/g' -e 's/\t /\t/g' \
		> .vscode/tasks.json
	@echo successfully created .tasks/launch.json