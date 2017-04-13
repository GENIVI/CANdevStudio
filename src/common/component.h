#ifndef __COMPONENT_H
#define __COMPONENT_H

#include <iostream>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include <common/event.h>

template <typename... A>
struct Component {
    Component()
    {
        (processArguments<A>(), ...);
    }

    virtual ~Component() {}

    template <typename F>
    bool connect(EventType src, F &&func)
    {
        return connect(src, mElements, func);
    }

    const std::vector<EventType> &getSources() const
    {
        return getSources(mElements);
    }

    const std::vector<EventType> &getSinks() const
    {
        return getSources(mElements);
    }

protected:
    template <typename S>
    void processArguments()
    {
        // Will fail to compile if not all elements are unique
        std::get<S>(mElements);

        if constexpr(std::is_base_of<SourceTag, S>::value) {
                mSources.push_back(S::event);
        }
        else if constexpr(std::is_base_of<SinkTag, S>::value) {
            mSinks.push_back(S::event);
        }
        else {
            // Causes compilation error. Only Sources and Sinks are accepted.
            std::get<void>(mElements);
        }
    }

    template <typename F, typename T, int size = std::tuple_size<T>::value>
    bool connect(EventType src, T& t, F& func)
    {
        auto& e = std::get<size - 1>(t);

        if constexpr(std::is_base_of<SourceTag, std::decay_t<decltype(e)>>::value) {
            if (e.event == src) {
                e.mDataOut.push_back(func);
                return true;
            }
        }

        if constexpr(size > 1) {
            return connect<F, T, size - 1>(src, t, func);
        }

        return false;
    }

    std::tuple<A...> mElements;
    std::vector<EventType> mSinks;
    std::vector<EventType> mSources;
};

#endif // __COMPONENT_H
