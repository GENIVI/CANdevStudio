#ifndef __SOCKETCAN_H
#define __SOCKETCAN_H

#include <common/component.h>
#include <common/event_canframe.h>

struct SocketCan : public Component<CanFrameSink, CanFrameSource> {

    SocketCan(const std::string&& ifname);

private:
    std::string mName;
};

#endif /* !__SOCKETCAN_H */
