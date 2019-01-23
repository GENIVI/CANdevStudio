# CANdevStudio 
[![Build Status](https://travis-ci.org/GENIVI/CANdevStudio.svg?branch=master)](https://travis-ci.org/GENIVI/CANdevStudio) [![Build Status](https://ci.appveyor.com/api/projects/status/y7vacbyvso12ud6e/branch/master?svg=true)](https://ci.appveyor.com/project/rkollataj/candevstudio-qflna/branch/master) [![codecov](https://codecov.io/gh/GENIVI/CANdevStudio/branch/master/graph/badge.svg)](https://codecov.io/gh/GENIVI/CANdevStudio) [![Download](https://api.bintray.com/packages/rkollataj/CANdevStudio/releases/images/download.svg) ](https://bintray.com/rkollataj/CANdevStudio/releases/_latestVersion) [![Download](https://api.bintray.com/packages/rkollataj/CANdevStudio/master/images/download.svg) ](https://bintray.com/rkollataj/CANdevStudio/master/_latestVersion)

* [Overview](#overview)
  * [Compatible CAN interfaces](#compatible-can-interfaces)
  * [Supported operating systems](#supported-operating-systems)
* [Build instructions](#build-instructions)
  * [Linux](#linux)
    * [To choose compiler](#to-choose-compiler)
    * [Qt in CMake](#qt-in-cmake)
  * [Windows](#windows)
    * [Visual Studio 2015](#visual-studio-2015)
    * [Visual Studio 2015 Win64](#visual-studio-2015-win64)
    * [MinGW](#mingw)
  * [macOS / OS X](#macos--os-x)
* [Prebuilt packages](#prebuilt-packages)
  * [Download](#download)
  * [Package naming](#package-naming)
  * [Linux](#linux-1)
  * [Windows](#windows-1)
  * [macOS / OS X](#macos--os-x-1)
* [Quick Start](#quick-start)
  * [CAN Hardware](#can-hardware)
    * [Microchip CAN BUS Analyzer](#microchip-can-bus-analyzer)
    * [Lawicel CANUSB](#lawicel-canusb)
  * [CANdevStudio without CAN hardware](#candevstudio-without-can-hardware)
    * [VCAN](#vcan)
    * [Cannelloni](#cannelloni)
* [CAN Signals](#can-signals)

## Overview
Most of automotive projects need to have an access to the Controller Area Network (CAN) bus. There are plenty of commercial frameworks that provides CAN stacks and hardware/software tools necessary to develop proper CAN networks. They are very comprehensive and thus expensive. CANdevStudio aims to be cost-effective replacement for CAN simulation software. It can work with variety of CAN hardware interfaces (e.g. Microchip, Vector, PEAK-Systems) or even without it (vcan and [cannelloni](https://github.com/mguentner/cannelloni)) . CANdevStudio enables to simulate CAN signals such as ignition status, doors status or reverse gear by every automotive developer. Thanks to modularity it is easy to implement new, custom features.


<p align="center">
<img src="https://at.projects.genivi.org/wiki/download/attachments/14976114/06-light.png" width="50%" />
</p>

### Compatible CAN interfaces
Access to CAN bus is based on Qt framework. Current list of supported CAN interfaces can be found [here](https://doc.qt.io/qt-5/qtcanbus-backends.html).

Current list of devices compatible with SocketCAN (Linux only) can be found [here](http://elinux.org/CAN_Bus).
### Supported operating systems
* Linux
* Windows

## Build instructions
CANdevStudio project uses Travis CI (Linux) and AppVeyor (Windows) continuous integration tools. You can always check .travis.yml and .appveyor.yml files for building details.

### Linux
```
git clone https://github.com/GENIVI/CANdevStudio.git
cd CANdevStudio
git submodule update --init --recursive
mkdir build
cd build
cmake ..
make
```
#### To choose compiler
```
cd CANdevStudio/build
rm -rf *
export CC=clang
export CXX=clang++
cmake ..
make
```
#### Qt in CMake
If CMake failed to find Qt in your system:
```
cd CANdevStudio/build
rm -rf *
cmake .. -DCMAKE_PREFIX_PATH=/home/genivi/Qt5.8.0/5.8/gcc_64
make
```
### Windows
#### Visual Studio 2015
```
git clone https://github.com/GENIVI/CANdevStudio.git
cd CANdevStudio
git submodule update --init --recursive
mkdir build
cd build
cmake .. -G "Visual Studio 14 2015" -DCMAKE_PREFIX_PATH=C:\Qt\5.9\msvc2015
cmake --build .
```
#### Visual Studio 2015 Win64
```
git clone https://github.com/GENIVI/CANdevStudio.git
cd CANdevStudio
git submodule update --init --recursive
mkdir build
cd build
cmake .. -G "Visual Studio 14 2015 Win64" -DCMAKE_PREFIX_PATH=C:\Qt\5.9\msvc2015_64
cmake --build .
```
#### MinGW
```
git clone https://github.com/GENIVI/CANdevStudio.git
cd CANdevStudio
git submodule update --init --recursive
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=C:\Qt\5.9\mingw53_32
cmake --build .
```
### macOS / OS X
```
git clone https://github.com/GENIVI/CANdevStudio.git
cd CANdevStudio
git submodule update --init --recursive
mkdir build
cd build
cmake .. -GNinja -DCMAKE_PREFIX_PATH=/path/to/Qt/lib/cmake
ninja
```
## Prebuilt packages
Binary packages are automatically uploaded by CI tools (i.e. Travis and Appveyor) to [Bintray](https://bintray.com/rkollataj/CANdevStudio) artifactory for every commit on master branch.
### Download
Use Bintray badges above to dowload stable or develop version (each commit on master creates corresponding binary packege in Bintray)
### Package naming
***CANdevStudio-X.Y.ZZZZZZZ-SYS[-standalone]***

**X** - major version number of previous stable version<br/>
**Y** - minor version of previous stable version<br/>
**Z** - SHA commit ID<br/>
**SYS** - either **win32**, **Linux** or **Darwin**<br/>
**standalone** - bundle version that contains Qt libraries and all relevant plugins.<br/>
### Linux
All packages are being built on Ubuntu 16.04 LTS. You may experience problems with missing or incompatible libraries when trying to run the package on other distros. 

To run standalone version use CANdevStudio.sh script.
### Windows
Packages built with MinGW 5.3.

Standalone version contains Qt and MinGW runtime libs.
### macOS / OS X
Package is a DMG installer.
## Quick Start
General instructions to start your first simulation:
1. Build the latest master or release.
2. Run the application and start a new project
3. Drag and drop CanDevice and CanRawView components and connect them accordingly.
4. Double click on CanDevice node to open configuration window.
   1. set one of supported backends (e.g. socketcan) [link](http://doc.qt.io/qt-5.10/qtcanbus-backends.html).<br/>**NOTE:** List of supported backends depends on Qt version.
   2. set name of your can interface (e.g. can0)
5. Start the simulation
6. Double click on CanRawView component to see CAN traffic

Steps required to use specific CAN hardware or virtual interfaces require some additional steps listed in following sections.
### CAN Hardware
The list below shows hardware that has been successfuly used with CANdevStudio.
#### Microchip CAN BUS Analyzer
* Requires socketcan [driver](https://github.com/rkollataj/mcba_usb).
* Officially supported in Linux Kernel v4.12+
Configuration:
1. Find your interface name (e.g. can0) <br/>
```ip link```
2. Configure bitrate<br/>
```sudo ip link set can0 type can bitrate 1000000```
3. Bring the device up<br/>
```sudo ip link set can0 up```
4. Optionally configure CAN termination
   1. In GitHUB based driver <br/>
   ```sudo ip link set can0 type can termination 1```
   2. In Linux 4.12+ driver<br/>
   ```sudo ip link set can0 type can termination 120```

CanDevice backend: socketcan

#### Lawicel CANUSB
* Based on FTDI Serial driver
* Requires slcand to "convert" serial device to SocketCAN.
* Officially supported in Linux Kernel v2.6.38

Configuration:
1. Create SocketCAN device from serial interface<br/>
```sudo slcand -o -c -s8 -S1000000 /dev/ttyUSB0 can0```
2. Bring the device up<br/>
```sudo ip link set can0 up```

CanDevice backend: socketcan

### CANdevStudio without CAN hardware
CANdevStudio can be used without actual CAN hardware thanks to Linux's built-in emulation.
#### VCAN
Configuration:
```
sudo modprobe vcan
sudo ip link add dev can0 type vcan
sudo ip link set can0 up
```
CanDevice backend: socketcan
#### Cannelloni
A SocketCAN over Ethernet tunnel.

Examplary configuration:
<p align="left">
<img src="https://at.projects.genivi.org/wiki/download/attachments/14976114/CANdevStudio-cannelloni.png" width="50%" />
</p>

Target configuration:
```
sudo modprobe vcan
sudo ip link add dev can0 type vcan
sudo ip link set can0 up
cannelloni -I can0 -R 192.168.0.1 -r 30000 -l 20000
```
PC configuration:

1. Install libqtCannelloniCanBusPlugin.so that is built along with CANdevStudio. You can either copy it manually to Qt plugins directory or use "make install" to do it automatically.
2. Create new project in CANdevStudio and add CanDevice node
3. Configure CanDevice:
   1. backend: cannelloni
   2. interface: 30000,192.168.0.2,20000 (local_port,remote_ip,remote_port)
4. Start simulation

## CAN Signals
CANdevStudio provides experimental support for CAN Signals (see [signals](https://github.com/GENIVI/CANdevStudio/tree/signals) branch). Currently only [DBC](http://socialledge.com/sjsu/index.php/DBC_Format) format is supported as a description of database, but it shouldn't be hard to add new formats.

The work on moving components from signal to master branch is ongoing.
