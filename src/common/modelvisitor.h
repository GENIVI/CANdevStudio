
#ifndef MODELVISITOR_H
#define MODELVISITOR_H

#include <functional> // function
#include <tuple> // tuple, tuple_element, get
#include <type_traits> // {is,remove}_reference, {true,false}_type, enable_if
#include <utility> // move, forward

#include <nodes/NodeDataModel>

#include "visitablewith.h" // VisitableWith



/**
 * Visitor type generator for types included in visitable_types.
 * Produces type-safe instance of Visitor per given Tag. User
 * shall derive publicly from Visitor<Tag> (tag can be the name
 * of the derived type), define visitable_types tuple to contain
 * allowed visitable types, and inherit Visitor constructor.
 * Visitable types must be derived from VisitableWith<T> where
 * T is the name of the visitor.
 *
 * Example
 * @code
 *  struct Example
 *    : Visitor<Example>
 *  {
 *      using visitable_types = std::tuple<A, B, C, D>;
 *
 *      using Visitor::Visitor;
 *  };
 * @endcode
 *
 * @see CanNodeDataModelVisitor for an example
 * @see http://insooth.github.io/visitor-pattern.md for an article
 */
template<class Tag>
class Visitor
{

 public:

    using visitable_types = std::tuple<>;  // DO NOT PASS REFERENCES

 private:

    template<class... T>
    struct are_bare_types
      : std::true_type
    {};

    template<class T, class... Ts>
    struct are_bare_types<std::tuple<T, Ts...>>
    {
        static const bool value = 
            ! std::is_reference<T>::value
                && are_bare_types<Ts...>::value;
    };

    template<class T>
    struct are_bare_types<std::tuple<T>>
    {
        static const bool value =
            ! std::is_reference<T>::value;
    };


    template<class T, class U>
    struct is_within
      : std::false_type
    {};

    template<class T, class... Ts>
    struct is_within<T, std::tuple<T, Ts...>>
      : std::true_type
    {};

    template<class T, class U, class... Ts>
    struct is_within<T, std::tuple<U, Ts...>>
      : is_within<T, std::tuple<Ts...>>
    {};


    template<class F>
    struct action_traits
      : action_traits<decltype(&std::remove_reference_t<F>::operator())>
    {};

    template<class R, class T, class... As>  // mutable lambda
    struct action_traits<R (T::*)(As...)>
      : action_traits<R (T::*)(As...) const>
    {};

    template<class R, class T, class... As>
    struct action_traits<R (T::*)(As...) const>
    {
        static constexpr auto arity = sizeof...(As);

        using result_type = R;

        template<unsigned I>
        using arg_type = std::tuple_element_t<I, std::tuple<As...>>;
    };


    template<class F>
    using arg0_type = typename action_traits<F>::template arg_type<0>;


    template<class T, class F, class... Fs>
    struct are_actions_allowed
    {
        static const bool value =
           is_within<std::remove_reference_t<arg0_type<F>>, T>::value
            && are_actions_allowed<T, Fs...>::value;
    };

    template<class T, class F>
    struct are_actions_allowed<T, F>
    {
        static const bool value =
            is_within<std::remove_reference_t<arg0_type<F>>, T>::value;
    };


 public:

    static_assert(are_bare_types<visitable_types>::value
                , "Visitable types cannot be references");


    template<class T>
    auto operator()(T& t)
        -> std::enable_if_t<is_within<T, visitable_types>::value, void>
    {
        if (std::get<action_type<T>>(actions))  // action specified?
        {
            std::get<action_type<T>>(actions)(t);
        }
        /** else: noop -- action not specified */
    }


    template<class... Fs>
    Visitor(Fs&&... fs)
    {
        static_assert(are_actions_allowed<visitable_types, Fs...>::value
                    , "Unexpected action passed");

        fill_in(actions, std::forward<Fs>(fs)...);
    }


 private:

    void operator()(...) = delete;  // passed type is not in visitable types!


    template<class T, class F>
    static void assign_action(T&& t, F&& f)
    {
        std::get<action_type<arg0_type<F>>>(std::forward<T>(t)) = std::forward<F>(f);
    }

    template<class T, class... Fs>
    static void fill_in(T&& t, Fs&&... fs)
    {
        int dummy[sizeof...(Fs)] =
            { (assign_action(std::forward<T>(t), std::forward<Fs>(fs)), 0)... };

        (void) dummy;
    }


    template<class T>
    using action_type = std::function<void (T&)>;    

    template<class Ts>
    struct make_action_types;

    template<class... Ts>
    struct make_action_types<std::tuple<Ts...>>
    {
        using type = std::tuple<action_type<Ts>...>;
    };


    using action_types = typename make_action_types<visitable_types>::type;

    action_types actions;
};






/** ------------------------ VISITORS OF FIXED TYPES ----------------------- */

class CanRawViewModel;
class CanRawSenderModel;
class CanDeviceModel;

/**
 * Example usage with @c VisitableWith<CanNodeDataModelVisitor>:
 *
 * @code
 *  auto m = node.nodeDataModel();
 *
 *  CanNodeDataModelVisitor v{ [this](CanRawViewModel& a) { }
 *                           , [this](CanRawSenderModel& a) { } };
 *
 *  assert(nullptr != m);
 *
 *  apply_model_visitor(*m, std::move(v));
 *  // or apply_model_visitor(*m, std::move(std::ref(v)));
 *  // or apply_model_visitor(*m, [](CanRawViewModel&) {}, [](CanRawSenderModel&) {});
 * @endcode
 */
struct CanNodeDataModelVisitor
  : Visitor<CanNodeDataModelVisitor>
{
    using visitable_types =
            std::tuple<
                CanRawViewModel
              , CanRawSenderModel
              , CanDeviceModel
              >;

    using Visitor::Visitor;
};



/** @throws bad_cast if object under @c m is not visitable with @c v. */
inline void apply_model_visitor(QtNodes::NodeDataModel& m, CanNodeDataModelVisitor v)
{
    // NOTE: Cannot use static_cast since NodeDataModel and VisitableWith
    //       are not in direct inheritance relation, i.e. only type derived
    //       from NodeDataModel derives from VisitableWith.

    dynamic_cast<VisitableWith<CanNodeDataModelVisitor>&>(m).visit(v);  // throws!
}

/** @throws bad_cast if object under @c m is not visitable with @c v. */
template<class... Actions>
inline void apply_model_visitor(QtNodes::NodeDataModel& m, Actions&&... actions)
{
    CanNodeDataModelVisitor v{std::forward<Actions>(actions)...};

    apply_model_visitor(m, std::move(v));
}

#endif

