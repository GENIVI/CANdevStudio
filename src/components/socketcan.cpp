#include "socketcan.h"

SocketCan::SocketCan(const std::string &&ifname)
{
    mName = ifname;
}
