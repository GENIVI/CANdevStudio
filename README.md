# Notice
This branch contains code that has been used during GENIVI AMM 17 demo. It is kept as a reference and will not be developed further.

## Build instructions
Proven to work with GCC 6.3.1 and Qt5.8
```
git clone https://github.com/GENIVI/CANdevStudio.git
cd CANdevStudio
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
