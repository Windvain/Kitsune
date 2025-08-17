# Kitsune Engine
Kitsune Engine is an up-and-coming open-sourced game engine written in C++20. This project is mainly for learning low-level systems programming, so expect a whole lot of bugs. Kitsune is licensed under the [MIT license](https://opensource.org/license/mit).
\
\
Thats all I have to say, have fun with this project!

---

## Getting Started
Follow the instructions below to build and run Kitsune from source. As of now, Kitsune does not support any other platforms except for Windows.

**1. Requirements**
- A C/C++ compiler. You can use one of the following tested compilers:
    - [Visual Studio Community 2022](https://visualstudio.microsoft.com/vs/), be sure to install the **Desktop development with C++** workload or install the MSVC compiler with the Windows SDK.
    - [MinGW-w64](https://www.mingw-w64.org/) with Clang can also be used, but keep in mind that some features will be disabled because of compiler support.
- The [CMake](https://cmake.org/) build system. The later the release version, the better.
- [Git](https://git-scm.com/), which will be required to clone this repository.

**2. Additional Requirements**
- If you are building with a compiler other than MSVC, the following have to be installed:
    - [libbacktrace](https://github.com/ianlancetaylor/libbacktrace/) for stack trace support.
    - [Ninja](https://ninja-build.org/) as the build system.

**3. Getting the Source Code** \
Start by cloning the repository with `git clone https://github.com/Windvain/Kitsune`. We strongly advice cloning the repository with the `--recursive` flag. \
If the repository was not cloned recursively, run `git submodule update --init` to clone all the required submodules.

**4. Build the Engine** \
Run the following command with this repository as your current working directory. The engine can be built with three different build types, **Debug**, **RelWithDbgInfo**, and **Release**. Replace `<your build type>` with the desired build type.
\
\
For Visual Studio 2022:
```bash
$ cmake --preset "Visual Studio 2022"
...
-- Configuring done (18.8s)
-- Generating done (0.2s)
-- Build files have been written to: <path to Kitsune>/build
```

Or alternatively for MinGW-w64 with Clang:
```bash
$ cmake --preset "Clang MinGW" -DCMAKE_BUILD_TYPE="<your build type>"
...
-- Configuring done (2.7s)
-- Generating done (0.2s)
-- Build files have been written to: <path to Kitsune>/build
```
After configuring the project using CMake, we can go on to the last step, which is building the engine.
\
\
Visual Studio 2022 users can use this command:
```bash
$ cmake --build build/ --preset "Visual Studio 2022 -- <your build type>"
```
Or with MinGW-Clang:
```bash
$ cmake --build build/
```
