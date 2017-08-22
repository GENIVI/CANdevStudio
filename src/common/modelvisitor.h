
#ifndef MODELVISITOR_H
#define MODELVISITOR_H

#include <functional> // function
#include <utility> // move

#include <nodes/NodeDataModel>

#include "visitablewith.h" // VisitableWith


class CanRawViewModel;
class CanRawSenderModel;
//class FooModel;

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
class CanNodeDataModelVisitor
{

 public:

    void operator()(CanRawViewModel& a)   { _f(a); }
    void operator()(CanRawSenderModel& a) { _g(a); }
//    void operator()(FooModel& a)          { _h(a); }

    // TODO: make apply_* insensitive to order of actions (using tuple indexed by type?)

    template<
        class CanRawViewModelAction
      , class CanRawSenderModelAction
//      , class FooModelAction
      >
    CanNodeDataModelVisitor(CanRawViewModelAction f
                          , CanRawSenderModelAction g
//                          , FooModelAction h
                           )
      :
        _f{std::move(f)}
      , _g{std::move(g)}
//      , _h{std::move(h)}
    {}

 private:

    std::function<void (CanRawViewModel&)>   _f;
    std::function<void (CanRawSenderModel&)> _g;
//    std::function<void (FooModel&)> _h;

};



/** @throws bad_cast if object under @c m is not visitable with @c v. */
inline void apply_model_visitor(NodeDataModel& m, CanNodeDataModelVisitor v)
{
    // NOTE: Cannot use static_cast since NodeDataModel and VisitableWith
    //       are not in direct inheritance relation, i.e. only type derived
    //       from NodeDataModel derives from VisitableWith.

    dynamic_cast<VisitableWith<CanNodeDataModelVisitor>&>(m).visit(v);  // throws!
}

/** @throws bad_cast if object under @c m is not visitable with @c v. */
template<class... Actions>
inline void apply_model_visitor(NodeDataModel& m, Actions... actions)
{
    CanNodeDataModelVisitor v{std::move(actions)...};

    apply_model_visitor(m, std::move(v));
}

#endif

