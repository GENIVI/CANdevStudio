
#ifndef UIBACKEND_H
#define UIBACKEND_H

#include "uibackendiface.h" // UIBackend{,Default}, UsesUIBackendCtor*
#include "uibackendimpls.h" // UIBackend, UIBackendDefault specialisatons
#include "withexplicitinit.h" // WithExplicitInit

#include <QtCore/QScopedPointer>
#include <QtCore/QString>
#include <QtCore/Qt> // SortOrder

#include <cassert> // assert
#include <functional>  // function
#include <memory> // unique_ptr, make_unique
#include <type_traits> // is_{same,base_of}, enable_if, {add_lvalue,remove}_reference, {false,true}_type
#include <utility> // forward, declval



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
#else
 #define void_t       std::void_t
 #define is_invocable std::is_invocable
#endif

    template<class T> struct is_selector : std::false_type {};
    template<class T> struct is_selector<UIBackendSelectorTag<T>> : std::true_type {};

    template<class A, class F>
    struct is_init
      : std::conditional_t<
            is_selector<F>::value
          , std::false_type
          , std::conditional_t<
                is_invocable<F, std::add_lvalue_reference_t<A>>::value
              , std::true_type
              , std::false_type
              >
          >
    {};
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
 *
 *   private:
 *
 *      EXPLICIT_INIT(CanRawView)  // MUST be at the very end of the class!
 *  //  ^^^^^^^^^^^^^^^^^^^^^^^^^
 *  };
 *
 * @endcode
 */
template<class Derived, class PrivateWithUIBackend, class Subject = Derived>
class UsesUIBackend
  : public WithExplicitInit<Derived>
{

 public:

    template<class A, class F>
    static constexpr bool is_init_v =
            UIBackendTraits::template is_init< std::remove_reference_t<A>
                                             , std::remove_reference_t<F> >::value;

    template<class T>
    static constexpr bool is_selector_v =
            UIBackendTraits::template is_selector<std::remove_reference_t<T>>::value;



    /** Acceses d_ptr from the actions passed to constructors. @{ */
    QScopedPointer<PrivateWithUIBackend>&       impl()       { return d_ptr; }
    const QScopedPointer<PrivateWithUIBackend>& impl() const { return d_ptr; }
    /** @} */



    /** Creates and manages lifetime of the default UI backend. */
    UsesUIBackend()
      :
        UsesUIBackend{ UsesUIBackendCtor_Explicit
                     , [](Derived&){}
                     , [](PrivateWithUIBackend&){}
                     , makeSelector<UIBackendDefault<Subject>>() }
    {}

    /** Just references the UI backend object. */
    explicit UsesUIBackend(UIBackend<Subject>& backend)
      :
        UsesUIBackend{ [](Derived&){}, backend }
    {}

    /**
     * Creates and manages UI backend object of default UI backend type and runs the passed
     * action of siganture convertible to void(Derived&) type in this constructor's body,
     * or in the d_ptr constructor's body if @c T is convertible to void(PrivateWithUIBackend&)
     * type.
     *
     * @{
     */
    template<
        class T
      , class... As
      , class = std::enable_if_t<is_init_v<Derived, T>>
      >
    UsesUIBackend(const UsesUIBackendCtorTag_ActionQ&
//                                           ^^^^^^^ runs "in" the q_ptr ctor body
                , T&& t, As&&... args)
      :
        UsesUIBackend{ UsesUIBackendCtor_Explicit
                     , std::forward<T>(t)
                     , [](PrivateWithUIBackend&){}
                     , makeSelector<UIBackendDefault<Subject>>()
                     , std::forward<As>(args)... }
    {}

    template<
        class T
      , class... As
      , class = std::enable_if_t<is_init_v<PrivateWithUIBackend, T>>
      >
    UsesUIBackend(const UsesUIBackendCtorTag_ActionD&
//                                           ^^^^^^^ runs "in" the d_ptr ctor body
                , T&& t, As&&... args)
      :
        UsesUIBackend{ UsesUIBackendCtor_Explicit
                     , [](Derived&){}
                     , std::forward<T>(t)
                     , makeSelector<UIBackendDefault<Subject>>()
                     , std::forward<As>(args)... }
    {}

    /** @} */

    /**
     * Creates and manages UI backend object of given selected UI backend passed as @c T.
     */
    template<
        class T
      , class... As
      , class = std::enable_if_t<is_selector_v<T>>
      >
    UsesUIBackend(const UsesUIBackendCtorTag_Selector&
                , T&& t, As&&... args)
      :
        UsesUIBackend{ UsesUIBackendCtor_Explicit
                     , [](Derived&){}
                     , [](PrivateWithUIBackend&){}
                     , std::forward<T>(t)
                     , std::forward<As>(args)... }
    {}

    /**
     * Takes a user-specified action @c F of signature void(Derived&) that
     * is executed in the body  of the this constructor as a first argument.
     * As a second argument, takes an action of a signature
     * void(PrivateWithUIBackend&) to be executed in the d_ptr's constructor
     * body. Manages the memory of the selected backend.
     */
    template<
        class F
      , class G
      , class... As
      , class = std::enable_if_t< is_init_v<Derived, F>
                               && is_init_v<PrivateWithUIBackend, G> >
      >
    UsesUIBackend(const UsesUIBackendCtorTag_Actions&
                , F&& init, G&& initMember, As&&... args)
      :
        UsesUIBackend{ UsesUIBackendCtor_Explicit
                     , std::forward<F>(init)
                     , std::forward<G>(initMember)
                     , makeSelector<UIBackendDefault<Subject>>()
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
      :
        WithExplicitInit<Derived>{std::forward<F>(init)}
      , d_ptr{new PrivateWithUIBackend{ * static_cast<Derived*>(this), backend}}
    {}

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
    UsesUIBackend(const UsesUIBackendCtorTag_Explicit&
                , F&& init, G&& initMember, const ImplSelector& selector, As&&... args)
      :
        WithExplicitInit<Derived>{std::forward<F>(init)}
      , d_ptr{new PrivateWithUIBackend{ std::forward<G>(initMember)
                                      , selector
                                      , * static_cast<Derived*>(this)
                                      , std::forward<As>(args)... }}
    {}

    /** Creates an manages UI backend object of the default type. */
    template<class... As>
    explicit UsesUIBackend(const UsesUIBackendCtorTag_Args&
                         , As&&... args)
      :
        UsesUIBackend{ UsesUIBackendCtor_Explicit
                     , [](Derived&){}
                     , [](PrivateWithUIBackend&){}
                     , makeSelector<UIBackendDefault<Subject>>()
                     , std::forward<As>(args)... }
    {}


 protected:

    QScopedPointer<PrivateWithUIBackend> d_ptr;


    static_assert(std::is_base_of<WithUIBackend<PrivateWithUIBackend, Derived, Subject>
                                , PrivateWithUIBackend>::value
                , "PrivateWithUIBackend must be derived from WithUIBackend");

 private:

    /** @see uibackendiface.h */
    template<class T>
    constexpr auto makeSelector() const
    {
#if defined(_MSC_VER)
        return UIBackendSelectorTag<T>{};
#else
        return  UIBackendSelector<T>;
#endif
    }
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
 *
 *
 *   private:
 *
 *      EXPLICIT_INIT(CanRawViewPrivate)  // MUST be at the very end of the class!
 *  //  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 *  };
 * @endcode
 */
template<class Derived, class UIBackendUser, class Subject = UIBackendUser>
class WithUIBackend
  : public WithExplicitInit<Derived>
{

 public:

    template<class A, class F>
    static constexpr bool is_init_v =
            UIBackendTraits::template is_init< std::remove_reference_t<A>
                                             , std::remove_reference_t<F> >::value;

    template<class T>
    static constexpr bool is_selector_v =
            UIBackendTraits::template is_selector<std::remove_reference_t<T>>::value;



    template<
        class ImplSelector
      , class... As
      , class = std::enable_if_t<is_selector_v<ImplSelector>>
      >
    WithUIBackend(const ImplSelector& selector, UIBackendUser& user, As&&... args)
      : WithUIBackend{ [](Derived&){}
                     , selector
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
    WithUIBackend(F&& init, const ImplSelector&, UIBackendUser& user, As&&... args)
      :
        WithExplicitInit<Derived>{std::forward<F>(init)}
      , uiRep{std::make_unique<
                    typename std::remove_reference_t<ImplSelector>::type
                  >(std::forward<As>(args)...)}
      , uiHandle{uiRep.get()}
      , q_ptr{&user}
    {
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
        WithExplicitInit<Derived>{std::forward<F>(init)}
      , uiHandle{&backend}
      , q_ptr{&user}
    {}



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

