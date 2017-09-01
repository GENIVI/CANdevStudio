
#ifndef VISITABLEWITH_H
#define VISITABLEWITH_H

/**
 * Example for two types that can be visited: A, B and visitor V.
 *
 * @code
 *  struct V
 *  {
 *   void operator()(A&) { std::cout << "A\n"; }
 *   void operator()(B&) { std::cout << "B\n"; }
 *  };
 *
 *  struct A : VisitableWith<V>
 *  {
 *   void visit(V& v) override { v(*this); }
 *   virtual ~A() = default;
 *  };
 *
 *  struct B : VisitableWith<V>
 *  {
 *   void visit(V& v) override { v(*this); }
 *   virtual ~B() = default;
 *  };
 *
 *  int main()
 *  {
 *   A a;
 *   B b;
 *   V v;
 *
 *   using W = VisitableWith<V>;
 *
 *   W& va = a;
 *   W& vb = b;
 *
 *   va.visit(v);
 *   vb.visit(v);
 *  }
 * @endcode
 *
 * gives
 *
 * A
 * B
 *
 */
template <class Visitor> struct VisitableWith {
    virtual void visit(Visitor& visitor) = 0;

    virtual ~VisitableWith() = default;
};

#endif
