[![Build Status](https://travis-ci.org/CanDevStudio/CanDevStudio.svg?branch=master)](https://travis-ci.org/CanDevStudio/CanDevStudio)

## Build instructions
Proven to work with GCC 6.3.1 and Qt5.8
```
git clone https://github.com/CanDevStudio/CanDevStudio.git
cd CanDevStudio
git checkout amm17
git submodule update --init
mkdir build
cd build
cmake ..
make
```

## To run
(From the top of source tree)
```
sudo modprobe vcan
sudo ip link add dev can0 type vcan
sudo ip link set can0 up
build/src/gui/CanDevStudio
```
