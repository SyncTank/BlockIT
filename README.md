# C++ Project BlockIT

### Getting Started
The example executables are included in the Binaries folder. Otherwise, check the release for the latest version of the application.


### Dependencies

Make sure to have opengl and glfw installed on the machine to compile.
I recommend using vcpkg for this as it automatically sets itself up with studio with no additional configuration needed.

The libraries use in the project are ImGUI, Opengl3, glfw, windows, and nfd. 

### UseCase

BlockIT is an application where you can blacklist certain processes your computer is running, with a user set timeframe where it kills any processes in the list.

## Building

This is a project for C++ projects which utilize a Core/App project architecture. There are two included projects - one called _Core_, and one called _App_. [Premake](https://github.com/premake/premake-core) is used to generate project files.

Core builds into a static library and contains common code for multiple applications. The app builds into an executable and links the Core static library, as well as provides an include path to Core's code.

The `Scripts/` directory contains build scripts for Windows and Linux, and the `Vendor/` directory contains Premake binaries.

## Getting Started
1. Clone this repository.
2. `App/` and `Core/` are the two projects
3. The three included Premake build files are `Build.lua`, `Core/Build-Core.lua`, and `App/Build-App.lua` - you can edit these to customize the configurations, and edit the names of the projects and workspace/solution, etc.
4. Open the `Scripts/` directory and run the appropriate `Setup` script to generate project files. You can edit the setup scripts to change the type of project that is generated - out of the box they are set to Visual Studio 2022 for Windows and gmake2 for Linux.

Note that no macOS setup script is currently provided; you can duplicate the Linux script and adjust it accordingly.

## Included
- Simple `.gitignore` to ignore project files and binaries
- Premake binaries for Win/Mac/Linux

