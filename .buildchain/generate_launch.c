
#define __COMMA ,__NL__

#define LAUNCH_JSON(CONFIGURATIONS) \
{ __NL__ \
__TAB__	"version": "0.2.0", __NL__ \
__TAB__	"configurations": [ __NL__ \
		CONFIGURATIONS __NL__ \
__TAB__	] __NL__ \
} __NL__

#define LAUNCH_CONFIG(NAME) \
__TAB__ __TAB__	{ __NL__ \
__TAB__ __TAB__ __TAB__	"name": #NAME, __NL__ \
__TAB__ __TAB__ __TAB__	"type": "cppdbg", __NL__ \
__TAB__ __TAB__ __TAB__	"request": "launch", __NL__ \
__TAB__ __TAB__ __TAB__	"program": __QUOTE__${workspaceFolder}/binaries/NAME/main.exe__QUOTE__, __NL__ \
__TAB__ __TAB__ __TAB__	"args": [], __NL__ \
__TAB__ __TAB__ __TAB__	"stopAtEntry": false, __NL__ \
__TAB__ __TAB__ __TAB__	"cwd": __QUOTE__${workspaceFolder}/binaries/NAME##__QUOTE__, __NL__ \
__TAB__ __TAB__ __TAB__	"environment": [], __NL__ \
__TAB__ __TAB__ __TAB__	"externalConsole": false, __NL__ \
__TAB__ __TAB__ __TAB__	"MIMode": "gdb", __NL__ \
__TAB__ __TAB__ __TAB__ "MIDebuggerPath": "C:\\msys64\\mingw64\\bin\\gdb.exe", __NL__ \
__TAB__ __TAB__ __TAB__	"setupCommands": [ __NL__ \
__TAB__ __TAB__ __TAB__	{ __NL__ \
__TAB__ __TAB__ __TAB__ __TAB__		"description": "Enable pretty-printing for gdb", __NL__ \
__TAB__ __TAB__ __TAB__ __TAB__		"text": "-enable-pretty-printing", __NL__ \
__TAB__ __TAB__ __TAB__ __TAB__		"ignoreFailures": true __NL__ \
__TAB__ __TAB__ __TAB__	} __NL__ \
__TAB__ __TAB__ __TAB__	], __NL__ \
__TAB__ __TAB__ __TAB__	"preLaunchTask": __QUOTE__build NAME##__QUOTE__ __NL__ \
__TAB__ __TAB__}
