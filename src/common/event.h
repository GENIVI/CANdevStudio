#ifndef __EVENT_H
#define __EVENT_H

#include <vector>

enum class EventType {
    Undefined,
    CanFrame,
    CanSignal
};

struct SinkTag {
};

template <EventType eventType, typename Interface>
struct Sink : public SinkTag {
    constexpr static EventType event{ eventType };

    Interface mDataIn{ nullptr };
};

struct SourceTag {
};

template <EventType eventType, typename Interface>
struct Source : public SourceTag {
    constexpr static EventType event{ eventType };

    template <typename E>
    void emitEvent(const E& ev)
    {
        for(auto&& i : mDataOut) {
            i(ev);
        }
    }

    std::vector<Interface> mDataOut;
};

#endif //__EVENT_H
