# BlockIT

### Purpose
BlockIT is a tool intended to help keep yourself focus by auto blocking selected applications in both Windows and Linux.

### Getting Started
The executables are included in the Binaries folder. Otherwise, check the release for the latest version of the application.

### Dependencies
The basis of the application is built on C/Cpp with MSVC. Included libraries and files are referred with in the application itself. There are some packages not included you can find below.

Use a package manager like vcpkg to install the packages to make your life easier. Below you'll find the required items, some fixing is required as included static libraries but this is more of a build side issue that will not imped you trying to work on code or run application.
 
The Backend is opengl3, glfw.
FrontEnd ImGui.

The libraries use in the project are ImGUI, Opengl3, glfw, windows, stbi_image, and nfd. 

### UseCase

Blacklist certain processes your computer is whether they are running, or select them yourself. Once set and either a flag is set or the block is actived the countdown is start. When the timeframe is met it kills any processes that were added.

## Building

The project itself is a C++ project in which utilizes a Core/App project architecture. There are two included projects folders - one called _Core_, and one called _App_. [Premake](https://github.com/premake/premake-core) is used to generate project files.

Core builds into a static library and contains common code for multiple applications. The app builds into an executable and links the Core static library, as well as provides an include path to Core's code. You'll also notice List.Cpp/List.h. This is where the most of the work is placed not related to the Core elements or the backend application setup.

The `Scripts/` directory contains build scripts for Windows and Linux, and the `Vendor/` directory contains Premake binaries.

## Getting Started
1. Clone this repository.
2. `App/` and `Core/` are the two projects
3. The three included Premake build files are `Build.lua`, `Core/Build-Core.lua`, and `App/Build-App.lua` - you can edit these to customize the configurations.
4. Open the `Scripts/` directory and run the appropriate `Setup` script to generate project files. You can edit the setup scripts to change the type of project that is generated - out of the box they are set to Visual Studio 2022 for Windows and gmake2 for Linux.

No macOS setup as I don't own that device; you can duplicate the Linux script and adjust it accordingly, but I won't support it myself.
