
#ifndef MODELVISITOR_H
#define MODELVISITOR_H

#include <type_traits> // is_same, result_of
#include <utility> // move

#include <nodes/NodeDataModel>

#include "visitablewith.h" // VisitableWith


class CanRawViewModel;
class CanRawSenderModel;

/**
 * Example usage with @c VisitableWith<T>:
 *
 * @code
 *  auto m = node.nodeDataModel();
 *
 *  auto v = make_model_visitor([this](CanRawViewModel& a) { }
 *                            , [this](CanRawSenderModel& a) { });
 *
 *  assert(nullptr != m);
 *
 *  apply_model_visitor(*m, std::move(v));
 *  // or apply_model_visitor(*m, std::move(std::ref(v)));
 * @endcode
 */
template<
    class CanRawViewModelAction
  , class CanRawSenderModelAction
  >
struct NodeDataModelVisitor
{
    void operator()(CanRawViewModel& a)   { _f(a); }
    void operator()(CanRawSenderModel& a) { _g(a); }

    CanRawViewModelAction   _f;
    CanRawSenderModelAction _g;

    // TODO: C++17: use class template argument deduction and get rid of "make_model_visitor"
    NodeDataModelVisitor(CanRawViewModelAction f, CanRawSenderModelAction g)
      : _f(std::move(f)), _g(std::move(g))
    {}
};

template<
    class CanRawViewModelAction
  , class CanRawSenderModelAction
  >
inline NodeDataModelVisitor make_model_visitor(F f, G g)
{
    static_assert(std::is_same<std::result_of_t<F(CanRawViewModel&)>, void>::value
               && std::is_same<std::result_of_t<G(CanRawSenderModel&)>, void>::value
        , "Wrong parameters");

    return NodeDataModelVisitor<F, G>{std::move(f), std::move(g)};
}

inline apply_model_visitor(NodeDataModel& m, NodeDataModelVisitor v)
{
    using B = VisitableWith<NodeDataModelVisitor>;

#ifdef NDEBUG
    static_cast<B&>(m).visit(v);
#else
    dynamic_cast<B&>(m).visit(v);  // throws!
#endif
}

#endif

