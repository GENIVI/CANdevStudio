**(C) 2018 Mobica**

| Linux | Windows | Coverage | Stable | Master |
| :--- | :--- | :--- | :--- | :--- |
| [![Build Status](https://travis-ci.org/GENIVI/CANdevStudio.svg?branch=master)](https://travis-ci.org/GENIVI/CANdevStudio) | [![Build Status](https://ci.appveyor.com/api/projects/status/y7vacbyvso12ud6e/branch/master?svg=true)](https://ci.appveyor.com/project/rkollataj/candevstudio-qflna/branch/master) | [![codecov](https://codecov.io/gh/GENIVI/CANdevStudio/branch/master/graph/badge.svg)](https://codecov.io/gh/GENIVI/CANdevStudio) | [![Download](https://api.bintray.com/packages/rkollataj/CANdevStudio/releases/images/download.svg) ](https://bintray.com/rkollataj/CANdevStudio/releases/_latestVersion) | [![Download](https://api.bintray.com/packages/rkollataj/CANdevStudio/master/images/download.svg) ](https://bintray.com/rkollataj/CANdevStudio/master/_latestVersion) |

List of contents
* [Overview](#overview)
  * [Compatible CAN interfaces](compatible-can-interfaces)
  * [Supported operating systems](supported-operating-systems)
* [Build instructions](build-instructions)
  * [Linux](#linux)
    * [To choose complier](#to-choose-complier)
    * [Qt in CMake](#qt-in-cmake)
  * [Windows](#windows)
    * [Visual Studio 2015](#visual-studio-2015)
    * [Visual Studio 2015 Win64](#visual-studio-2015-win64)
    * [MinGW](#mingw)

# Overview
Most of automotive projects need to have an access to the Controller Area Network (CAN) bus. There are plenty of commercial frameworks that provides CAN stacks and hardware/software tools necessary to develop proper CAN networks. They are very comprehensive and thus expensive. CANdevStudio aims to be cost-effective replacement for CAN simulation software. It can work with variety of CAN hardware interfaces (e.g. Microchip, Vector, PEAK-Systems) or even without it (vcan and [cannelloni](https://github.com/mguentner/cannelloni)) . CANdevStudio enables to simulate CAN signals such as ignition status, doors status or reverse gear by every automotive developer. Thanks to modularity it is easy to implement new, custom features.


<p align="center">
<img src="https://at.projects.genivi.org/wiki/download/attachments/14976114/06-light.png" width="50%" />
</p>

## Compatible CAN interfaces
Access to CAN bus is based Qt framework. Current list of supported CAN interfaces can be found [here](https://doc.qt.io/qt-5/qtcanbus-backends.html).

Current list of devices compatible with SocketCAN (Linux only) can be found [here](http://elinux.org/CAN_Bus).
## Supported operating systems
* Linux
* Windows

# Build instructions
CANdevStudio project uses Travis CI (Linux) and AppVeyor (Windows) continues integration tools. You can always check .travis.yml and .appveyor.yml files for building details.

## Linux
```
git clone https://github.com/GENIVI/CANdevStudio.git
cd CANdevStudio
git submodule update --init --recursive
mkdir build
cd build
cmake ..
make
```
### To choose complier
```
cd CANdevStudio/build
rm -rf *
export CC=clang
export CXX=clang++
cmake ..
make
```
### Qt in CMake
If CMake failed to find Qt in your system:
```
cd CANdevStudio/build
rm -rf *
cmake .. -DCMAKE_PREFIX_PATH=/home/genivi/Qt5.8.0/5.8/gcc_64
make
```
## Windows
### Visual Studio 2015
```
git clone https://github.com/GENIVI/CANdevStudio.git
cd CANdevStudio
git submodule update --init --recursive
mkdir build
cd build
cmake .. -G "Visual Studio 14 2015" -DCMAKE_PREFIX_PATH=C:\Qt\5.9\msvc2015
cmake --build .
```
### Visual Studio 2015 Win64
```
git clone https://github.com/GENIVI/CANdevStudio.git
cd CANdevStudio
git submodule update --init --recursive
mkdir build
cd build
cmake .. -G "Visual Studio 14 2015 Win64" -DCMAKE_PREFIX_PATH=C:\Qt\5.9\msvc2015_64
cmake --build .
```
### MinGW
```
git clone https://github.com/GENIVI/CANdevStudio.git
cd CANdevStudio
git submodule update --init --recursive
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=C:\Qt\5.9\mingw53_32
cmake --build .
```
