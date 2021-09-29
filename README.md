OS abstraction layer
====================
[![Build Status](https://github.com/rtlabs-com/osal/workflows/Build/badge.svg?branch=master)](https://github.com/rtlabs-com/osal/actions?workflow=Build)
[![CodeQL](https://github.com/rtlabs-com/osal/workflows/CodeQL/badge.svg?branch=master)](https://github.com/rtlabs-com/osal/actions?workflow=CodeQL)

This repository contains an OS abstraction layer. The goal of the
abstraction layer is to simplify writing software that runs on many
platforms. This abstraction layer is mainly focused on threading
functionality.

Cloning
=======

Clone the source:

```
$ git clone --recurse-submodules https://github.com/rtlabs-com/osal.git
```

This will clone the repository with submodules. If you already cloned
the repository without the `--recurse-submodules` flag then run this
in the osal folder:

```
$ git submodule update --init --recursive
```

Prerequisites for all platforms
===============================

 * CMake 3.14 or later

Windows
=======

 * Visual Studio 2017 or later

You can use a windows or unix shell as preferred. The following
instructions are for a unix shell. CMake is assumed to be in your
path.

```
$ cmake -B build.win64 -A x64
$ cmake --build build.win64 --config Release
$ cmake --build build.win64 --config Release --target check
```

This builds the project and runs the unit tests.

Linux
=====

 * GCC 4.6 or later

```
$ cmake -B build
$ cmake --build build --target all check
```

This builds the project and runs the unit tests.

rt-kernel
=========

 * Workbench 2020.1 or later

You should use a bash shell, such as for instance the Command Line in
your Toolbox installation. Set the BSP variable to the name of the BSP
you wish to build for. Set the RTK variable to the path of your
rt-kernel tree.

Standalone project
------------------

This creates standalone makefiles.

```
$ RTK=/path/to/rt-kernel BSP=xmc48relax cmake \
   -B build.xmc48relax \
   -DCMAKE_TOOLCHAIN_FILE=cmake/tools/toolchain/rt-kernel.cmake \
   -G "Unix Makefiles"
$ cmake --build build.xmc48relax
```

Workbench project
-----------------

This creates a Makefile project that can be imported to Workbench. The
project will be created in the build directory. The build directory
should be located outside of the source tree.

```
$ RTK=/path/to/rt-kernel BSP=xmc48relax cmake \
   -B build.xmc48relax -S /path/to/osal \
   -DCMAKE_TOOLCHAIN_FILE=cmake/tools/toolchain/rt-kernel.cmake \
   -DCMAKE_ECLIPSE_EXECUTABLE=/opt/rt-tools/workbench/Workbench \
   -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE \
   -G "Eclipse CDT4 - Unix Makefiles"
```

A source project will also be created in the osal tree. This project
can also be imported to Workbench. After importing, right-click on the
project and choose *New* -> *Convert to a C/C++ project*. This will
setup the project so that the indexer works correctly and the
Workbench revision control tools can be used.

The library and the unit tests will be built. Note that the tests
require a stack of at least 6 kB. You may have to increase
CFG_MAIN_STACK_SIZE in your bsp include/config.h file.

STM32Cube / FreeRTOS / lwIP
===========================

First, clone the relevant STM32Cube repo for your platform. For
instance, to build for STM32F7 (fetching a specific tag only):

```
$ git clone https://github.com/STMicroelectronics/STM32CubeF7.git \
    -b v1.16.1 --single-branch --depth 1
```

Then, when configuring specify CPU, board and path to the cloned git
repository:

```
$ CPU=cortex-m7fd BOARD=STM32F769I-DISCO CUBE_DIR=/path/to/cube cmake \
    -B build.cube \
    -DCMAKE_TOOLCHAIN_FILE=cmake/tools/toolchain/stm32cube.cmake \
    -G "Unix Makefiles"
$ cmake --build build.cube
```

