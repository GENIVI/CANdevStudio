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

typedef std::function<void(const CanFrame&)> CanFrameClbk;

class CanFrameSink : public Sink<EventType::CanFrame, CanFrameClbk> {
};

struct CanFrameSource : public Source<EventType::CanFrame, CanFrameClbk> {
};

#endif /* !__CANFRAME_H */
