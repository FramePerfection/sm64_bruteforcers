# Super Mario 64 bruteforcers

This repo is in a very early work in progress stage.
No functionality or fitness for any purpose is to be expected.

This repo is a fork of https://github.com/n64decomp/sm64
Several files have been removed or altered.
It contains a number of modules that can be used to simulate portions of Super Mario 64 or a ROM Hack of thereof with the goal of improving existing .m64 TAS files.
This repo builds no ROMs, nor does it require any ROMs to build.

## Building

GNU gcc or msys is required.
To build the repository, simply run "make".
There exist targets for each module separately, for example "make fp_gwk".

A configuration for Visual Studio Code is provided as well.
Adjust the MODULE_PATH define in c_cpp_properties.json for proper intellisense support on whichever module you are currently working on.
Each module has an associated launch configuration you can select in the "Run and Debug" view.
Note the "OPT_FLAGS := -O3" in line 12 in Makefile; Set to -g for debugging purposes.

## Running

After a successful build, each module will have its executable file located in binaries/<module_name>/
The file "configuration.json" next to it defines the state and parameters with which the bruteforcer is to be run.
 (TODO) For further information, consult the readme files in their respective folders.

## Module structure

Each module consists of 4 files:
-make.split: 
	This file sets the name of the module (should be the same as the folder name) and defines the object files needed to build the module.
	
-bf_state_definitions:
	This file defines the configurable state and parameters of the module.
	It MUST include "bruteforce/common_states/bf_state_definitions_common.inc.c" for the module to build.
	
-state.h:
	Declares variables that are needed by the module, as well as the layout of custom stats for each candidate.
	A "candidate" is an input sequence with its associated run and score and additional data such as hSpeed, angle, etc.
 
-main.c:
	The main code file.
	Defines how to initialize and update the game state, as well as how to perturb inputs and update scores.
	It also runs the main bruteforcing loop.
	Generalizing this file is TODO
	
## Project Structure
	
	sm64
	├── actors: object behaviors, geo layout, and display lists
	├── assets: animation and demo data
	│   ├── anims: animation data
	│   └── demos: demo data
	├── binaries: executable files of modules, along with their respective configuration and m64 files
	├── bruteforce: files making up the bruteforcing framework
	│   ├── common_states: required and optional common configuration definitions
	│   ├── **/main.c: modules
	├── build: output directory for intermediate files
	├── data: behavior scripts, misc. data
	├── include: header files
	├── levels: level scripts, geo layout, and display lists
	├── lib: SDK library code
	├── rsp: audio and Fast3D RSP assembly code
	├── src: C source code for game
	│   ├── audio: audio code
	│   ├── buffers: stacks, heaps, and task buffers
	│   ├── engine: script processing engines and utils
	│   ├── game: behaviors and rest of game source
	│   ├── goddard: Mario intro screen
	│   └── menu: title screen and file, act, and debug level selection menus
	├── text: dialog, level names, act names
