#ifndef __CONTEXT_H
#define __CONTEXT_H

#include <memory>
#include <tuple>

/**
*   The class provides wrapper that enables to inject multiple dependencies
*   to one class.
*
*   NOTE: Interfaces provided as template arguments must be unique
*/
template <typename... Args> struct Context {
    /**
    *   Used to construct the context and and initialize it with implementations. Class takes ownership over
    *   implementations
    *
    *   @param  args pointers to implementation of Args interfaces
    */
    Context(Args*... args)
        // need explicitely mark unique_ptr type as GCC 5 fails to deduce type
        : _implsPtr(std::unique_ptr<Args>(args)...)
    {
    }

    /**
    *   Get implementation of interfaces passed as template parameter
    *
    *   @return implementation of an interface
    */
    template <typename T> T& get() const
    {
        return *std::get<std::unique_ptr<T>>(_implsPtr).get();
    }

private:
    std::tuple<std::unique_ptr<Args>...> _implsPtr;
};

struct CanDeviceInterface;
typedef Context<CanDeviceInterface> CanDeviceCtx;

struct CRSGuiInterface;
struct NLMFactoryInterface;
typedef Context<CRSGuiInterface, NLMFactoryInterface> CanRawSenderCtx;

struct CRVGuiInterface;
typedef Context<CRVGuiInterface> CanRawViewCtx;

#endif /* !__CONTEXT_H */
