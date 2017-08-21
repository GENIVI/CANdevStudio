
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
 * @endcode
 */
class CanNodeDataModelVisitor
{

 public:

    void operator()(CanRawViewModel& a)   { _f(a); }
    void operator()(CanRawSenderModel& a) { _g(a); }
//    void operator()(FooModel& a)          { _h(a); }

    template<
        class CanRawViewModelAction
      , class CanRawSenderModelAction
//      , class FooModelAction
      >
    CanNodeDataModelVisitor(CanRawViewModelAction f, CanRawSenderModelAction g)
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

inline void apply_model_visitor(NodeDataModel& m, CanNodeDataModelVisitor v)
{
    using B = VisitableWith<CanNodeDataModelVisitor>;

#ifdef NDEBUG
    static_cast<B&>(m).visit(v);
#else
    dynamic_cast<B&>(m).visit(v);  // throws!
#endif
}

#endif

