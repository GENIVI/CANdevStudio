
#ifndef MODELVISITOR_H
#define MODELVISITOR_H

#include <utility> // move, forward

#include <nodes/NodeDataModel>

#include "visitor.h" // Visitor
#include "visitablewith.h" // VisitableWith


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
  : Visitor<
        CanNodeDataModelVisitor  // tag, not visitable type!
      , CanRawViewModel
      , CanRawSenderModel
      , CanDeviceModel
//    , Other
      >
{
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

