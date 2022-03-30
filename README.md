# CANdevStudio 
[![CANdevStudio](https://github.com/GENIVI/CANdevStudio/actions/workflows/build.yml/badge.svg?branch=master)](https://github.com/GENIVI/CANdevStudio/actions/workflows/build.yml?query=branch%3Amaster++) [![codecov](https://codecov.io/gh/GENIVI/CANdevStudio/branch/master/graph/badge.svg)](https://codecov.io/gh/GENIVI/CANdevStudio) [![Doxygen](https://img.shields.io/badge/Doxygen-master-blue.svg)](https://genivi.github.io/CANdevStudio/)



* [Overview](#overview)
  * [Compatible CAN interfaces](#compatible-can-interfaces)
  * [Supported operating systems](#supported-operating-systems)
* [Build instructions](#build-instructions)
  * [Linux](#linux)
    * [To choose compiler](#to-choose-compiler)
    * [Qt in CMake](#qt-in-cmake)
  * [Windows](#windows)
    * [Visual Studio 2019 Win64](#visual-studio-2019-win64)
  * [macOS / OS X](#macos--os-x)
* [Prebuilt packages](#prebuilt-packages)
  * [Download](#download)
  * [Package naming](#package-naming)
  * [Linux](#linux-1)
  * [ARCH Linux](#arch-linux)
  * [Windows](#windows-1)
  * [macOS / OS X](#macos--os-x-1)
* [Quick Start](#quick-start)
  * [CAN Hardware](#can-hardware)
    * [Microchip CAN BUS Analyzer](#microchip-can-bus-analyzer)
    * [Lawicel CANUSB](#lawicel-canusb)
    * [PeakCAN PCAN-USB](#peakcan-pcan-usb)
    * [PassThruCAN Plugin](#passthrucan-plugin)
  * [CANdevStudio without CAN hardware](#candevstudio-without-can-hardware)
    * [VCAN](#vcan)
    * [Cannelloni](#cannelloni)
* [Help](#help)
  * [Scripting](#scripting)
  * [CAN Signals](#can-signals)
  * [CanDevice configuration](#candevice-configuration)
  * [CanRawFilter](#canrawfilter)
  * [Adding new components](#adding-new-components)
  
## Overview
Most of automotive projects need to have an access to the Controller Area Network (CAN) bus. There are plenty of commercial frameworks that provides CAN stacks and hardware/software tools necessary to develop proper CAN networks. They are very comprehensive and thus expensive. CANdevStudio aims to be cost-effective replacement for CAN simulation software. It can work with variety of CAN hardware interfaces (e.g. Microchip, Vector, PEAK-Systems) or even without it (vcan and [cannelloni](https://github.com/mguentner/cannelloni)) . CANdevStudio enables to simulate CAN signals such as ignition status, doors status or reverse gear by every automotive developer. Thanks to modularity it is easy to implement new, custom features.

Checkout CANdevStudio on [YouTube](https://www.youtube.com/watch?v=1TfAyg6DG04)

<p align="center">
<img src="https://at.projects.genivi.org/wiki/download/attachments/14976114/CDS_V1.png" width="65%" />
</p>

### Compatible CAN interfaces
Access to CAN bus is based on Qt framework. Current list of supported CAN interfaces can be found [here](https://doc.qt.io/qt-5/qtcanbus-backends.html).

Current list of devices compatible with SocketCAN (Linux only) can be found [here](http://elinux.org/CAN_Bus).
### Supported operating systems
* Linux
* Windows
* macOS

## Build instructions
CANdevStudio project uses GitHub Actions as continuous integration environment. You can check [build.yml](https://github.com/GENIVI/CANdevStudio/blob/master/.github/workflows/build.yml) for details. 

To lower maintenance effort and allow for usage of modern C++ features, since v1.2.0 CANdevStudio dropped "official" support for legacy compilers like gcc5.3, vs2015 or MinGW. Current CI configuration uses the latest compilers available for each GitHub Actions environment:
* ubuntu-latest (clang and gcc)
* macos-latest (clang)
* windows-latest (vs2019 x64)

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
cmake .. -DCMAKE_PREFIX_PATH=/home/genivi/Qt5.12.0/5.12.0/gcc_64
make
```
### Windows
#### Visual Studio 2019 Win64
```
git clone https://github.com/GENIVI/CANdevStudio.git
cd CANdevStudio
git submodule update --init --recursive
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 16 2019" -A x64
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
Each GitHub Actions job stores prebuilt packages for 90 days. Additionally official releases are stored on GitHub Releases page.
### Package naming
***CANdevStudio-X.Y.ZZZZZZZ-SYS[-standalone]***

**X** - major version number of previous stable version<br/>
**Y** - minor version of previous stable version<br/>
**Z** - SHA commit ID<br/>
**SYS** - either **win64**, **Linux** or **Darwin**<br/>
**standalone** - bundle version that contains Qt libraries and all relevant plugins.<br/>
### Linux
All packages are being built on ubuntu-latest environment. Refer to [this](https://github.com/actions/virtual-environments) page to determine the exact Ubuntu version. You may experience problems with missing or incompatible libraries when trying to run the package on other distros. 

To run standalone version use CANdevStudio.sh script.
### ARCH Linux
Install AUR package: [candevstudio-git](https://aur.archlinux.org/packages/candevstudio-git/)

### Windows
Packages built with Visual Studio 2019.

Standalone version contains Qt. Installation of VS2019 redist packages may be still required. 
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
The list below shows hardware that has been successfully used with CANdevStudio.
#### Microchip CAN BUS Analyzer
* Tested on Linux
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
* Tested on Linux
* Based on FTDI Serial driver
* Requires slcand to "convert" serial device to SocketCAN.
* Officially supported in Linux Kernel v2.6.38

Configuration:
1. Create SocketCAN device from serial interface<br/>
```sudo slcand -o -c -s8 -S1000000 /dev/ttyUSB0 can0```
2. Bring the device up<br/>
```sudo ip link set can0 up```

CanDevice backend: socketcan

#### PeakCAN PCAN-USB
* Tested on Windows

CanDevice settings example:
```
backend: peakcan
interface: usb0
configuration: BitRateKey = 250000
```
#### PassThruCAN Plugin
* Tested on Windows

CanDevice settings example for PEAK-PCAN:
```
backend: passthrucan
configuration: BitRateKey = 250000
interface: PCANPT32
```
CanDevice settings example for SIE_CANUSB:
```
backend: passthrucan
configuration: BitRateKey = 250000
interface: CANUSB
```
CanDevice settings example for Kvaser USBcan:
```
backend: passthrucan
configuration: BitRateKey = 250000
interface: J2534 (kline) for Kvaser Hardware
```
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
A SocketCAN over Ethernet tunnel. Available for Linux only.

Let's consider setup as before:
<p align="left">
<img src="https://at.projects.genivi.org/wiki/download/attachments/14976114/CANdevStudio-cannelloni.png" width="50%" />
</p>

##### Configuration with qtCannelloniCanBusPlugin
Target configuration:
```
sudo modprobe vcan
sudo ip link add dev can0 type vcan
sudo ip link set can0 up
cannelloni -I can0 -R 192.168.0.1 -r 30000 -l 20000
```
PC configuration:

1. Install libqtCannelloniCanBusPlugin.so that is built along with CANdevStudio. You can either copy it manually to Qt plugins directory (e.g. /usr/lib/qt/plugins/canbus) or use "make install" to do it automatically.
2. Create new project in CANdevStudio and add CanDevice node
3. Configure CanDevice:
   1. backend: cannelloni
   2. interface: 30000,192.168.0.2,20000 (local_port,remote_ip,remote_port)
4. Start simulation

##### Configuration without qtCannelloniCanBusPlugin
Target configuration:
```
sudo modprobe vcan
sudo ip link add dev can0 type vcan
sudo ip link set can0 up
cannelloni -I can0 -R 192.168.0.1 -r 30000 -l 20000
```
PC configuration:
1. Execute following lines in a shell
```
sudo modprobe vcan
sudo ip link add dev can0 type vcan
sudo ip link set can0 up
cannelloni -I can0 -R 192.168.0.2 -r 20000 -l 30000
```
2. Create new project in CANdevStudio and add CanDevice node
3. Configure CanDevice:
   1. backend: socketcan
   2. interface: can0
4. Start simulation

## Help
### Scripting
As of v1.1 CANdevStudio supports creation of [QML](https://doc.qt.io/qt-5/qmlapplications.html) based scripts. Scripts can be developed and loaded dynamically without a need to restart the main applications. Scripting adds a lot of different possibilities to CANdevStudio that includes:
* Creation of custom GUIs
* Raw frames and signals handling
* Time triggered actions
* Message triggered actions
* ... and many more, as all QML functionalities are supported.

Try it yourself by loading one of the [examples](https://github.com/GENIVI/CANdevStudio/tree/master/src/components/qmlexecutor/examples) into QMLExecutor component. You are welcome to share your scripts via Pull Requests!

### CAN Signals
CANdevStudio provides support for CAN signals handling. [DBC](http://socialledge.com/sjsu/index.php/DBC_Format) database description format is supported. Reverse engineered DBC files can be found in [opendbc](https://github.com/commaai/opendbc) project.

Support for others CAN database formats can be added via extension of [CANdb](https://www.github.com/GENIVI/CANdb).

#### Sending signals
1. Start new project and setup CanDevice as described in quick start section
2. **Add CanSignalData** component that serves as CAN signals database for other components. You may have multiple CanSignalData components per projecthttps://github.com/commaai/opendbc
3. Open CanSignalData properties and configure path to DBC file
4. List of messages and signals shall be now loaded and visible in CanSignalData window
5. You may configure cycle and initial value per each message
6. **Add CanSignalEncoder** component and connect it with CanDevice. CanSignalEncoder act as a translator between signals and CAN frames. It is also  responsible for sending cyclical messages.
7. CanSignalSender has been automatically configured to use previously added CAN database. CAN database can be manually selected in component properties (this applies to all components from "Signals" group)
8. **Add CanSignalSender** component and connect it with CanSignalEncoder
9. Add signals in CanSignalSender window
10. Start simulation
11. CanSignalEncoder will start sending cyclical messages
12. You can send previously configured signals from CanSignalSender:
    * if signal is a part of periodic message its value will be updated in a next cycle
    * if signal is not a part of periodic message it will be sent out immediately

#### Receiving signals
1. Start new project and setup CanDevice as described in quick start section
2. **Add CanSignalData** component that serves as CAN signals database for other components. You may have multiple CanSignalData components per project
3. Open CanSignalData properties and configure path to DBC file
4. List of messages and signals shall be now loaded and visible in CanSignalData window
5. **Add CanSignalDecoder** component and connect it with CanDevice. CanSignalDecoder act as a translator between signals and CAN frames.
6. CanSignalDecoder has been automatically configured to use previously added CAN database. CAN database can be manually selected in component properties (this applies to all components from "Signals" group)
7. **Add CanSignalViewer** component and connect it with CanSignalDecoder
8. Start simulation
9. Signals shall now appear in CanSignalViewer. Note that CanSignalDecoder is sending over only signals which values has changed.

### CanDevice configuration
CanDevice component can be configured using "configuration" property:
* Format - "key1=value1;key2=value2;keyX=valueX"
* Keys names are case sensitive, values are case insensitive
* Configuration keys are taken from [ConfigurationKey enum](https://doc.qt.io/qt-5/qcanbusdevice.html#ConfigurationKey-enum). 
* RawFilterKey and ErrorFilterKey are currently not supported
* Whitespaces are ignored

E.g.
```
BitRateKey=100000;ReceiveOwnKey=false;LoopbackKey=true
```
### CanRawFilter
CanRawFilter component enables to filter (i.e. accept or drop) incoming and outgoing frames:
* [Qt](https://doc.qt.io/qt-5/qregularexpression.html) regular expressions are used to match filter rules.
* Rules are matched from top to bottom
* Default policy is applied to frames unmatched by any filter

Examples:
* match 0x222 and 0x333 frames only [id field]
```
222|333
```
* match 0x200 - 0x300 frames only [id field]
```
^[23]..$
```
* match empty payload (DLC 0) [payload field]
```
^$
```
* match 2 byte payload (DLC 2) [payload field]
```
^.{4}$
```
### Adding new components
1. Configure build to include *templategen* tool
```
cd build
cmake .. -DWITH_TOOLS=ON
make
```
2. Generate component (use -g option if you don't need component to have GUI)
```
./tools/templategen/templategen -n MyNewComponent -o ../src/components -g
```
3. CMake script automatically detects new components. It has to be invoked manually.
```
cmake ..
```
4. Build project 
``` 
make
```
5. Your component is now integrated with CANdevStudio
6. You may want to modify *src/components/mynewcomponent/mynewcomponentplugin.h* to configure section name, color and spacing
7. Define component inputs and outputs in *src/components/mynewcomponent/mynewcomponentmodel.cpp*. Look for examples in other components.
8. Modify automatically generated unit tests *src/components/mynewcomponent/tests*
