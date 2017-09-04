
#ifndef WITHEXPLICITINIT_H
#define WITHEXPLICITINIT_H

#include <cassert> // assert
#include <atomic>
#include <functional>  // function
#include <type_traits> // is_same, add_pointer



template<class WithExplicitInitType>
class ExplicitInitialiser;


/**
 * Base class to be derived by type @c T that does delayed initialisation of the
 * @c T object with passed action @c F of signature convertible to @c R(T&) type.
 * Derived type @c T may redefine @c init function template, and if such is present,
 * it will be selected.
 *
 * This type is useful if the constructor of the type @c T does or cannot perform
 * full initialisation of the object of type @c T, i.e. object is @e not fully
 * usable after its constructor finishes.
 *
 * Class @c T that derives from this type @b MUST add @c EXPLICIT_INIT macro call
 * at the @b VERY end of the class, that is as the last member, preferably private.
 * Otherwise, runtime crashes may occur.
 * */
template<class T, class Tag = T, class R = void>
class WithExplicitInit
{

    using base_type   = T;
    using tag_type    = Tag;
    using result_type = R;

    friend class ExplicitInitialiser<WithExplicitInit<T, Tag, R>>;

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


/**
 * Ensures that explicit initialisation using @c WithExplicitInitType that is an instance
 * of @c WithExplicitInit is performed upon copy and move of an object of this type.
 *
 * Instances of this type must be friends of the respective @c WithExplicitInit type.
 *
 * Use EXPLICIT_INIT or EXPLICIT_INIT_THROUGH macros to construct objects of this type.
 *
 * @warning This implementation is not thread safe.
 */
template<class WithExplicitInitType>
class ExplicitInitialiser
{

 public:

    ExplicitInitialiser(WithExplicitInitType& base)
      : _base{&base}
    {
        init();
    }

    /** Copy and assignment require re-applying the init action. @{ */
    ExplicitInitialiser(const ExplicitInitialiser& other)
      : _base{other._base}
    {
        if (&other != this)  // prevents from double-init
        {
            init();
        }
    }

    ExplicitInitialiser& operator=(const ExplicitInitialiser& other)
    {
        if (&other != this)  // prevents from double-init
        {
            _base = other._base;

            init();
        }
    }
    /* @} */

    /** Does not perform init, the passed object is already inited. @{ */
    ExplicitInitialiser& operator=(ExplicitInitialiser&& other)
    {
        _base = other._base;  // self-assignment is OK
    }

    ExplicitInitialiser(ExplicitInitialiser&& other)
      : _base{other._base}
    {}
    /** @} */

    /**
     * This constructor is provided @b ONLY to make EXPLICIT_INIT macros-generated code
     * compile. Note, calling @c init for a default constructed object of this type causes
     * assertion if NDEBUG is off, and a segmentation fault otherwise.
     */
    ExplicitInitialiser() = default;

    ~ExplicitInitialiser() = default;  // no de-init

 private:

    void init()  //< @see WithExplicitInit::prepare
    {
        assert(nullptr != _base);

        using D = std::add_pointer_t<typename WithExplicitInitType::base_type>; // most-derived

        static_cast<D>(_base)->init(_base->_body);
    }


    WithExplicitInitType* const _base = nullptr;



    static_assert(std::is_same<WithExplicitInitType
                             , WithExplicitInit<
                                  typename WithExplicitInitType::base_type
                                , typename WithExplicitInitType::tag_type
                                , typename WithExplicitInitType::result_type
                                >
                             >::value
            , "Parameter not an instance of WithExplicitInit");
};



#define CONCAT1(a, b) a ## b
#define CONCAT(a, b)  CONCAT1(a, b)

/** Uses init action from @c WithExplicitInit<T> base class. */
#define EXPLICIT_INIT(T)            \
  ExplicitInitialiser<WithExplicitInit<T>> _initialised { * this};

/** Does init using action from @c WithExplicitInit base class @c U explicitly. */
#define EXPLICIT_INIT_THROUGH(U) \
  ExplicitInitialiser<U> CONCAT(_initialised, __LINE__) { * static_cast<U*>(this)};

#endif

