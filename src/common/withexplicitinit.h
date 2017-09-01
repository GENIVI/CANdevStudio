
#ifndef WITHEXPLICITINIT_H
#define WITHEXPLICITINIT_H

#include <functional>  // function


/**
 * Base class to be derived by type @c T that does delayed initialisation of the
 * @c T object with passed action @c F of signature convertible to @c R(T&) type.
 * Derived type @c T may redefine @c init function template, and if such is present,
 * it will be selected.
 *
 * This type is useful if the construct of the type @c T does or cannot perform
 * full initialisation of the object of type @c T, i.e. object is @e not fully
 * usable after its constructor finishes.
 *
 * Class @c T that derives from this type @b MUST add @c EXPLICIT_INIT macro call
 * at the @b VERY end of the class, that is as the last member, preferably private.
 * Otherwise, runtime crashes may occur.
 * */
template<class T, class R = void>
class WithExplicitInit
{

 public:

    /** Initialised @c init body with a given @c f action. */
    template<class F>
    explicit WithExplicitInit(F&& f)
      : _body{f}
    {}

    /**
     * Runs @c init against given @c _body exactly once.
     *
     * @warning This function is not thread safe.
     */
    void prepare()
    {
        static int dummy = (static_cast<T*>(this)->init(_body), 0);

        (void) dummy;
    }

 private:

    /** Default init action. Must be private to disable explicit double-init. */
    template<class F>
    void init(F&& f)
    {
        f( * static_cast<T*>(this));
    }

    std::function<R (T&)> _body;
};


#define CONCAT1(a, b) a ## b
#define CONCAT(a, b)  CONCAT1(a, b)

#define EXPLICIT_INIT           bool _initialised = (prepare(), true);

#define EXPLICIT_INIT_TAGGED(T) bool CONCAT(_initialised, __LINE__) = (T::prepare(), true);

#undef CONCAT1
#undef CONCAT

#endif

