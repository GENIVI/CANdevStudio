#ifndef __CANFRAME_H
#define __CANFRAME_H

#include <vector>
#include <functional>
#include "event.h"

struct CanFrame {
    uint32_t id;
    std::vector<uint8_t> data;
    bool extended;
    bool error;
};

typedef std::function<bool(const CanFrame&)> CanFrameInterface;

class CanFrameSink : public Sink<EventType::CanFrame, CanFrameInterface> {
};

struct CanFrameSource : public Source<EventType::CanFrame, CanFrameInterface> {
};

#endif /* !__CANFRAME_H */
