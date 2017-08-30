
#ifndef UIBACKEND_H
#define UIBACKEND_H

#include <QtCore/QScopedPointer>
#include <QtCore/QString>
#include <QtCore/Qt> // SortOrder

#include <cassert> // assert
#include <functional>  // function
#include <memory> // unique_ptr, make_unique
#include <type_traits> // is_{same,base_of}, common_type, enable_if, remove_reference, result_of
#include <utility> // forward



class CanRawView;
class QAbstractItemModel;
class QWidget;


/**
 * All the UI backends per given subject must derive from UIBackend<Subject> interface.
 * User must implement at least one derived UIBackend type per unique Subject, i.e.
 * UIBackendDefault. If it is not desirable to use virtual member functions (as it
 * could be used in the UIBackend<Subject>), then UIBackend<Subject> shall implement
 * all the non-virtual functions *and* UIBackendDefault shall be empty and derive from
 * it publicly and use constructor inheritance. That's due to fact, that the following
 * implementation uses a reference to the object of type UIBackend<Subject> everywhere.
 *
 * NOTE: The actual shape, number of ctors, args taken by ctors is not forced by this
 *       implementation. Instances of UIBackend<> and the derived types can take any
 *       number of arguments that can be forwarded passed through UsesUIBackend and
 *       WithUIBackend to the selected type of UI backend.
 *
 * @see UIBackend<CanRawView> and UIBackendDefault<CanRawView> (aka CanRawViewBackend).
 *
 * @{
 */
/** UIBackend for given Subject tag (can be any type). */
template<class Subject>
struct UIBackend;

/** Default UIBackend for given Subject tag. */
template<class Subject>
struct UIBackendDefault : UIBackend<Subject>
{
    static_assert(false, "UIBackendDefault not implemented");
};
/** @} */




/** ----------------------- BACKEND INTERFACES FOLLOW ---------------------- */


template<>
struct UIBackend<CanRawView>  // polymorphic as an example, but it's optional, see the note on top
{
    virtual QString getClickedColumn(int ndx) = 0;
    virtual QWidget* getMainWidget() = 0;
    virtual bool isColumnHidden(int column) = 0;
    virtual bool isFrozen() = 0;
    virtual int getSortIndicator() = 0;
    virtual int getSortOrder() = 0;
    virtual void initTableView(QAbstractItemModel& tvModel) = 0;
    virtual void setClearCbk(std::function<void ()> cb) = 0;
    virtual void setDockUndockCbk(std::function<void ()> cb) = 0;
    virtual void setSectionClikedCbk(std::function<void (int)> cb) = 0;
    virtual void setSorting(int sortNdx, int clickedNdx, Qt::SortOrder order) = 0;
    virtual void updateScroll() = 0;

    virtual ~UIBackend() = default;
};

// include CanRawViewBackend

/** ----------------------- BACKEND INTERFACES END ---------------------- */





/** Tag generator used to select an implementation type to be created. */
template<class Impl>
static constexpr std::common_type<Impl> UIBackendSelector{};

template<class ImplSelector>
static constexpr bool IsUIBackendSelector =
        std::is_same<
                std::remove_reference_t<ImplSelector>
              , decltype(UIBackendSelector<
                            typename std::remove_reference_t<ImplSelector>::type
                          >)
              >::value;

// FIXME: extract first arg type instead of the following
template<class Derived, class F>
static constexpr bool IsUIBackendInit =
        std::is_same<std::result_of_t<F(Derived&)>, void>::value;




template<class Derived, class UIBackendUser, class Subject>
class WithUIBackend;


/**
 * To be derived publicly by @c Derived type that is going to be composed
 * with the *Private type (as used in the Qt D-Pointer infrastructure).
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
 *    : public UsesUIBackend<
 *                  CanRawView         // this type
 *                , CanRawViewPrivate  // type referenced by Q_DECLARE_PRIVATE()
 *                , CanRawView         // tag for UIBackend<>
 *                >
 *    , public QObject
 *  {
 *      Q_OBJECT
 *      Q_DECLARE_PRIVATE(CanRawView)
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
     * Takes a user-specified action @c F of signature void(Derived&) that is
     * executed in the constructor body. Uses default UI backend.
     */
    template<
        class F
      , class... As
      // just to disable this ctor for invalid F types
      , class = std::enable_if_t<IsUIBackendInit<Derived, F>, void>
      >
    explicit UsesUIBackend(F&& init, As&&... args)
      :
        UsesUIBackend{ std::forward<F>(init)
                     , UIBackendSelector<UIBackendDefault<Subject>>
                     , std::forward<As>(args)... }
    {}

    /**
     * Takes a user-specified action @c G of signature void(PrivateWithUIBackend&)
     * that is executed in the @c d_[tr constructor body. Uses default UI backend.
     */
    template<
        class G
      , class... As
      // just to disable this ctor for invalid F types
      , class = std::enable_if_t<IsUIBackendInit<PrivateWithUIBackend, G>, void>
      >
    explicit UsesUIBackend(G&& initMember, As&&... args)
      :
        UsesUIBackend{ std::forward<G>(initMember)
                     , UIBackendSelector<UIBackendDefault<Subject>>
                     , std::forward<As>(args)... }
    {}

    /**
     * Takes a user-specified actions @c F and @c G of types convertible to
     * void(Derived&) and void(PrivateWithUIBackend&) respectively. Former is
     * executed in the body of this constructor, second in the constructor of
     * the @c d_ptr member. Default UI backend is used.
     */
    template<
        class F
      , class G
      , class... As
      // just to disable this ctor for invalid F types
      , class = std::enable_if_t<IsUIBackendInit<Derived, F>, void>
      , class = std::enable_if_t<IsUIBackendInit<PrivateWithUIBackend, G>, void>
      >
    UsesUIBackend(F&& init, G&& initMember, As&&... args)
      :
        UsesUIBackend{ std::forward<F>(init)
                     , std::forward<G>(initMember)
                     , UIBackendSelector<UIBackendDefault<Subject>>
                     , std::forward<As>(args)... }
    {}

    /** Just references the UI backend object. */
    template<
        class F
      // just to disable this ctor for invalid F types
      , class = std::enable_if_t<IsUIBackendInit<Derived, F>, void>
      >
    UsesUIBackend(F&& init, UIBackend<Subject>& backend)
      : d_ptr{new PrivateWithUIBackend{ * static_cast<Derived*>(this), backend}}
    {
        init( * static_cast<Derived*>(this));
    }

    /**
     * Creates and manages UI backend object of given selected type accessible
     * via @c ImplSelector::type nested typedef.
     */
    template<
        class ImplSelector // Will create backend object of this type...
      , class... As        // ...and pass these arguments to it constructor.

      // disables this ctor for non-selectors to enable ctor that takes args only
      , class = std::enable_if_t<IsUIBackendSelector<ImplSelector>, void>
      >
    explicit UsesUIBackend(ImplSelector&& selector, As&&... args)
      :
        UsesUIBackend{ [](Derived&){}
                     , [](PrivateWithUIBackend&){}
                     , std::forward<ImplSelector>(selector)
                     , std::forward<As>(args)... }
    {}

    /**
     * Takes a user-specified action @c F of signature void(Derived&) that is
     * executed in the body of the constructor and ImplSelector that chooses
     * UI backend implementation to be created and managed.
     */
    template<
        class F
      , class ImplSelector
      , class... As
      // disables this ctor for non-selectors to enable ctor that takes args only
      , class = std::enable_if_t<IsUIBackendSelector<ImplSelector>, void>
      // just to disable this ctor for invalid F types
      , class = std::enable_if_t<IsUIBackendInit<Derived, F>, void>
      >
    UsesUIBackend(F&& init, ImplSelector&& selector, As&&... args)
      :
        UsesUIBackend{ std::forward<F>(init)
                     , [](PrivateWithUIBackend&){}
                     , std::forward<ImplSelector>(selector)
                     , std::forward<As>(args)... }
    {}

    /**
     * Constructs @c d_ptr with @c initMember action (convertible to
     * void(PrivateWithUIBackend&) type) passed to its constructor.
     */
    template<
        class G
      , class ImplSelector
      , class... As
      // disables this ctor for non-selectors to enable ctor that takes args only
      , class = std::enable_if_t<IsUIBackendSelector<ImplSelector>, void>
      // just to disable this ctor for invalid F types
      , class = std::enable_if_t<IsUIBackendInit<PrivateWithUIBackend, G>, void>
      >
    UsesUIBackend(G&& initMember, ImplSelector&& selector, As&&... args)
      :
        UsesUIBackend{ [](Derived&){}
                     , std::forward<G>(initMember)
                     , std::forward<ImplSelector>(selector)
                     , std::forward<As>(args)... }
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
      // disables this ctor for non-selectors to enable ctor that takes args only
      , class = std::enable_if_t<IsUIBackendSelector<ImplSelector>, void>
      // just to disable this ctor for invalid F types
      , class = std::enable_if_t<IsUIBackendInit<Derived, F>, void>
      , class = std::enable_if_t<IsUIBackendInit<PrivateWithUIBackend, G>, void>
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
 *    : public WithUIBackend<
 *                  CanRawViewPrivate  // this type
 *                , CanRawView         // type referenced by Q_DECLARE_PUBLIC()
 *                , CanRawView         // tag for UIBackend<>
 *                >
 *    , public QObject
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

 public:

    template<
        class ImplSelector
      , class... As
      , class = std::enable_if_t<IsUIBackendSelector<ImplSelector>, void>
      >
    WithUIBackend(ImplSelector&&, UIBackendUser& user, As&&... args)
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
      // disables this ctor for non-selectors
      , class = std::enable_if_t<IsUIBackendSelector<ImplSelector>, void>
      // just to disable this ctor for invalid F types
      , class = std::enable_if_t<IsUIBackendInit<Derived, F>, void>
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
      // just to disable this ctor for invalid F types
      , class = std::enable_if_t<IsUIBackendInit<Derived, F>, void>
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
    std::unique_ptr<UIBackend<Subject>> uiRep;    /**< NOTE: stores derived from UIBackend<Subject> */
    UIBackend<Subject>*                 uiHandle; /**< uiRep observer. */
    /** @} */

    UIBackendUser* const q_ptr; /**< Respective *Public type for the *Private one. */
};

#endif

