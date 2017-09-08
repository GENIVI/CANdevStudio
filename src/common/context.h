#ifndef __CONTEXT_H
#define __CONTEXT_H

#include <memory>
#include <tuple>

template <typename... Args> struct Context {
    Context(Args*... args)
        // need explicitely mark unique_ptr type as GCC 5 fails to deduce type
        : _implsPtr(std::unique_ptr<Args>(args)...)
    {
    }

    template <typename T> T& get()
    {
        return *std::get<std::unique_ptr<T>>(_implsPtr).get();
    }

private:
    std::tuple<std::unique_ptr<Args>...> _implsPtr;
};

class CanDeviceInterface;
typedef Context<CanDeviceInterface> CanDeviceCtx;

class CRSGuiInterface;
class NLMFactoryInterface;
typedef Context<CRSGuiInterface, NLMFactoryInterface> CanRawSenderCtx;

class CRVGuiInterface;
typedef Context<CRVGuiInterface> CanRawViewCtx;

#endif /* !__CONTEXT_H */
