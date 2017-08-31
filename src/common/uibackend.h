
#ifndef UIBACKEND_H
#define UIBACKEND_H

#include "uibackendiface.h" // UIBackend, UIBackendDefault
#include "uibackendimpls.h" // UIBackend, UIBackendDefault specialisatons

#include <QtCore/QScopedPointer>
#include <QtCore/QString>
#include <QtCore/Qt> // SortOrder

#include <cassert> // assert
#include <functional>  // function
#include <memory> // unique_ptr, make_unique
#include <type_traits> // is_{same,base_of}, enable_if, {add_lvalue,remove}_reference, {false,true}_type
#include <utility> // forward, declval



/** Tag generator used to select an implementation type to be created. */
template<class T>
struct UIBackendSelectorTag { using type = T; };

template<class Impl>
static constexpr UIBackendSelectorTag<Impl> UIBackendSelector{};




struct UIBackendTraits
{
#if __cplusplus <= 201402L  // not in C++17?
    template<class... Ts> struct make_void { using type = void; };
    template<class... Ts> using void_t = typename make_void<Ts...>::type;

    // works for signle arg only -- param packs must be at the end of params
    template<class F, class A, class = void_t<>>
    struct is_invocable : std::false_type {};

    template<class F, class A>
    struct is_invocable<F, A, void_t<
            decltype(std::declval<F>()(std::declval<A>()))
        >> : std::true_type {};
#endif

    template<class T> struct is_selector : std::false_type {};
    template<class T> struct is_selector<UIBackendSelectorTag<T>> : std::true_type {};

    template<class A, class F>
    struct is_init
      : std::conditional_t<
            is_selector<F>::value
          , std::false_type
          , std::conditional_t<
#if __cplusplus <= 201402L
                is_invocable<F, std::add_lvalue_reference_t<A>>::value
#else
                std::is_invocable_v<F, std::add_lvalue_reference_t<A>>
#endif
              , std::true_type
              , std::false_type
              >
          >
    {};
/*
    template<class A, class R, class C, class... Ts>
    struct is_init<A, R (C::*)(Ts...)> : is_init<A, R (C::*)(Ts...) const> {};

    template<class A, class R, class C, class... Ts>
    struct is_init<A, R (C::*)(Ts...) const> : std::false_type {};

    template<class A, class R, class C, class T>
    struct is_init<A, R (C::*)(T) const>
      : std::conditional_t<
            std::is_same<T, std::add_lvalue_reference_t<A>>::value
          , std::true_type
          , std::false_type
          >
    {};
*/
};




template<class Derived, class UIBackendUser, class Subject>
class WithUIBackend;


/**
 * To be derived publicly by @c Derived type that is going to be composed
 * with the *Private type (as used in the Qt D-Pointer infrastructure).
 *
 * @warning Q_DECLARE_PRIVATE_D macro is used (note the suffix @b _D).
 *
 * NOTE: This base class is not intended to be used in polymorphic delete.
 *
 * @see https://wiki.qt.io/D-Pointer for @c d_ptr
 *
 * Example:
 *
 * @see WithUIBackend for CanRawViewPrivate example -- NOTE the constructor!
 *
 * @code
 *  class CanRawView
 *    : public QObject                  // *MUST* be specified as a first one
 *    , public UsesUIBackend<
 *                  CanRawView         // this type
 *                , CanRawViewPrivate  // type referenced by Q_DECLARE_PRIVATE_D()
 *                , CanRawView         // tag for UIBackend<>
 *                >
 *  {
 *      Q_OBJECT
 *      Q_DECLARE_PRIVATE_D(UsesUIBackend::d_ptr.data(), CanRawView)
 *  //                   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 *
 *   public:
 *
 *      using UsesUIBackend::UsesUIBackend;  // brings ctors in scope
 *
 *      void foo()
 *      {
 *          Q_D(CanRawView);
 *
 *          d->bar(); // calls CanRawViewPrivate::bar
 *      }
 *  };
 *
 *
 *  // two possible uses below, production and test:
 *
 *  CanRawView defaulted;
 *  CanRawView defaultedWithArgs {args...}; // args... are forwarded to UIBackend<Subject> type
 *
 *  CanRawView selected         {UIBackendSelector<other::CanRawView>};
 *  CanRawView selectedWithArgs {UIBackendSelector<other::CanRawView>, args...};
 *
 *  UIBackend<test::CanRawView> backend; // type derived from UIBackend<CanRawView>
 *  CanRawView                  injected         {backend};
 *  CanRawView                  injectedWithInit {[](CanRawView&){}, backend};
 *
 *  CanRawView defaultedWithInit        {[](CanRawView&){}};
 *  CanRawView defaultedWithArgsAndInit {[](CanRawView&){}, args...};
 *
 *  CanRawView selectedWithInit        {[](CanRawView&){}, UIBackendSelector<X>};
 *  CanRawView selectedWithInitAndArgs {[](CanRawView&){}, UIBackendSelector<X>, args...};
 * @endcode
 */
template<class Derived, class PrivateWithUIBackend, class Subject = Derived>
class UsesUIBackend
{

    template<class A, class F>
    static constexpr bool is_init_v =
            UIBackendTraits::template is_init< std::remove_reference_t<A>
                                             , std::remove_reference_t<F> >::value;

    template<class T>
    static constexpr bool is_selector_v =
            UIBackendTraits::template is_selector<std::remove_reference_t<T>>::value;

 public:

    /** Creates an manages UI backend object of the default type. */
    template<class... As>
    explicit UsesUIBackend(As&&... args)
      : UsesUIBackend{UIBackendSelector<UIBackendDefault<Subject>>, std::forward<As>(args)...}
    {}

    /** Just references the UI backend object. */
    explicit UsesUIBackend(UIBackend<Subject>& backend)
      : UsesUIBackend{[](Derived&){}, backend}
    {}

    /**
     * Creates and manages UI backend object of given selected UI backend passed as @c T,
     * or runs a passed action of siganture convertible to void(Derived&) type in this
     * constructor's body, or in the d_ptr constructor's body if @c T is convertible to
     * void(PrivateWithUIBackend&) type.
     */
    template<
        class T
      , class... As
      , class = std::enable_if_t< is_init_v<Derived, T>
                               || is_init_v<PrivateWithUIBackend, T>
                               || is_selector_v<T> >
      >
    explicit UsesUIBackend(T&& t, As&&... args)
      :
        UsesUIBackend{ is_init_v<Derived, T>
                            ? std::forward<T>(t)
                            : [](Derived&){}
                     , is_init_v<PrivateWithUIBackend, T>
                            ? std::forward<T>(t)
                            : [](PrivateWithUIBackend&){}
                     , is_selector_v<T>
                            ? std::forward<T>(t)
                            : UIBackendSelector<UIBackendDefault<Subject>>
                     , std::forward<As>(args)... }
    {}


    /**
     * Takes a user-specified action @c F of signature void(Derived&) that
     * is executed in the body  of the this constructor as a first argument.
     * As a second argument, takes either UIBackendSelector or an action of
     * a signature void(PrivateWithUIBackend&) to be executed in the d_ptr's
     * constructor body. Manages the memory of the selected backend.
     */
    template<
        class F
      , class T
      , class... As
      , class = std::enable_if_t< is_init_v<Derived, F>
                              && (is_init_v<PrivateWithUIBackend, T> || is_selector_v<T>) >
      >
    UsesUIBackend(F&& init, T&& t, As&&... args)
      :
        UsesUIBackend{ std::forward<F>(init)
                     , is_init_v<PrivateWithUIBackend, T>
                                ? std::forward<T>(t)
                                : [](PrivateWithUIBackend&){}
                     , is_selector_v<T>
                                ? std::forward<T>(t)
                                : UIBackendSelector<UIBackendDefault<Subject>>
                     , std::forward<As>(args)... }
    {}

    /**
     * Just references the UI backend object and executes passed action
     * of signature convertible to void(Derived&) in this constructor's body.
     */
    template<
        class F
      , class = std::enable_if_t<is_init_v<Derived, F>>
      >
    UsesUIBackend(F&& init, UIBackend<Subject>& backend)
      : d_ptr{new PrivateWithUIBackend{ * static_cast<Derived*>(this), backend}}
    {
        init( * static_cast<Derived*>(this));
    }

    /**
     * Constructs @c d_ptr with @c initMember action (convertible to
     * void(PrivateWithUIBackend&) type) passed to its constructor, executes
     * @c F action of type convertible to void(Derived&) in this constructor's
     * body.
     */
    template<
        class F
      , class G
      , class ImplSelector
      , class... As
      , class = std::enable_if_t< is_init_v<Derived, F>
                               && is_init_v<PrivateWithUIBackend, G>
                               && is_selector_v<ImplSelector> >
      >
    UsesUIBackend(F&& init, G&& initMember, ImplSelector&& selector, As&&... args)
      :
        d_ptr{new PrivateWithUIBackend{ std::forward<G>(initMember)
                                      , std::forward<ImplSelector>(selector)
                                      , * static_cast<Derived*>(this)
                                      , std::forward<As>(args)... }}
    {
        init( * static_cast<Derived*>(this));
    }


 protected:

    QScopedPointer<PrivateWithUIBackend> d_ptr;


    static_assert(std::is_base_of<WithUIBackend<PrivateWithUIBackend, Derived, Subject>
                                , PrivateWithUIBackend>::value
                , "PrivateWithUIBackend must be derived from WithUIBackend");
};



/**
 * To be derived publicly by the *Private types (as used in the Qt D-Pointer
 * infrastructure). As in @c UsesUIBackend, there is a possiblity to pass
 * @c init function object of signature @c void(Derived&).
 *
 * NOTE: This base class is not intended to be used in polymorphic delete.
 *
 * @see https://wiki.qt.io/D-Pointer for @c q_ptr
 *
 * Example:
 *
 * @code
 *  class CanRawViewPrivate
 *    : public QObject                  // *MUST* be specified as a first one
 *    , public WithUIBackend<
 *                  CanRawViewPrivate  // this type
 *                , CanRawView         // type referenced by Q_DECLARE_PUBLIC()
 *                , CanRawView         // tag for UIBackend<>
 *                >
 *  {
 *      Q_OBJECT
 *      Q_DECLARE_PUBLIC(CanRawView)
 *
 *   public:
 *
 *      using WithUIBackend::WithUIBackend;  // brings ctors in scope
 *
 *
 *      void bar()
 *      {
 *          Q_Q(CanRawView);
 *
 *          q->foo();  // calls CanRawView::foo
 *
 *          backend().updateScroll();  // calls function from UIBackend<Subject> or from dervied
 *      }
 *  };
 * @endcode
 */
template<class Derived, class UIBackendUser, class Subject = UIBackendUser>
class WithUIBackend
{

    template<class A, class F>
    static constexpr bool is_init_v =
            UIBackendTraits::template is_init< std::remove_reference_t<A>
                                    , std::remove_reference_t<F> >::value;

    template<class T>
    static constexpr bool is_selector_v =
            UIBackendTraits::template is_selector<std::remove_reference_t<T>>::value;

 public:

    template<
        class ImplSelector
      , class... As
      , class = std::enable_if_t<is_selector_v<ImplSelector>>
      >
    WithUIBackend(ImplSelector&& selector, UIBackendUser& user, As&&... args)
      : WithUIBackend{ [](Derived&){}
                     , std::forward<ImplSelector>(selector)
                     , user
                     , std::forward<As>(args)... }
    {}

    WithUIBackend(UIBackendUser& user, UIBackend<Subject>& backend)
      : WithUIBackend{[](Derived&){}, user, backend}
    {}



    /**
     * Creates an object with implementation of @c UIBackend<Subject>
     * of type @c Impl is specified, otherwise default (must exist)
     * implementation of type @c UIBackendDefault<Subject> is created.
     * Lifetime of the object is managed by object of this class.
     * Arguments passed to this constructor are forwarded to the Impl
     * constructor.
     */
    template<
        class F
      , class ImplSelector
      , class... As
      , class = std::enable_if_t<is_init_v<Derived, F> && is_selector_v<ImplSelector>>
      >
    WithUIBackend(F&& init, ImplSelector&&, UIBackendUser& user, As&&... args)
      :
        uiRep{std::make_unique<
                    typename std::remove_reference_t<ImplSelector>::type
                  >(std::forward<As>(args)...)}
      , uiHandle{uiRep.get()}
      , q_ptr{&user}
    {
        init( * static_cast<Derived*>(this));

        static_assert(std::is_base_of< UIBackend<Subject>
                                     , typename std::remove_reference_t<ImplSelector>::type
                                     >::value
                    , "Impl does not match interface");
    }

    /** DOES NOT manage lifetime of @c backend variable! */
    template<
        class F
      , class = std::enable_if_t<is_init_v<Derived, F>>
      >
    WithUIBackend(F&& init, UIBackendUser& user, UIBackend<Subject>& backend)
      :
        uiHandle{&backend}
      , q_ptr{&user}
    {
        init( * static_cast<Derived*>(this));
    }



    UIBackend<Subject>& backend()
    {
        assert(nullptr != uiHandle);

        return *uiHandle;
    }

    const UIBackend<Subject>& backend() const
    {
        assert(nullptr != uiHandle);

        return *uiHandle;
    }

 protected:

    /** @{ If backend is passed explicitly in constructor, uiHandle stores
     *     pointer to it, and backend memory is not managed by uiRep.
     *     Otherwise, uiRep manages memory.
     *
     *     NEVER perform delete on uiHandle.
     *     DO NOT reorder uiRep and uiHandle.
     *
     * */
    std::unique_ptr<UIBackend<Subject>> uiRep;    /**< NOTE: stores UIBackend<Subject> subclass */
    UIBackend<Subject>*                 uiHandle; /**< uiRep observer. */
    /** @} */

    UIBackendUser* const q_ptr; /**< Respective *Public type for the *Private one. */
};

#endif

