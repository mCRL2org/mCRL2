// This example shows how static polymorphism can in principle work
// in the implementation of the find functions in the mCRL2 Library.
// See also http://en.wikipedia.org/wiki/Template_metaprogramming#Static_polymorphism

#include <iostream>

struct where_clause                        {};
struct function_symbol                     {};
struct data_expression                     {};
struct abstraction: public data_expression {};
struct variable: public data_expression    {};
struct application: public data_expression {};
struct identifier_string                   {};
struct lambda                              {};
struct forall                              {};
struct exists                              {};
struct assignment                          {};
struct data_equation                       {};
struct action_label                        {};
struct action                              {};
struct deadlock                            {};
struct multi_action                        {};
struct action_summand                      {};
struct deadlock_summand                    {};
struct process_initializer                 {};
struct linear_process                      {};
struct specification                       {};

template <typename Derived>
class data_traverser
{
  protected:

    template <typename Abstraction >
    void visit(Abstraction const& a)
    {
      static_cast<Derived& >(*this).enter(static_cast<data_expression const&>(a));
      static_cast<Derived& >(*this).enter(static_cast<abstraction const&>(a));
      static_cast<Derived& >(*this).enter(a);

      deadlock_summand s;
      static_cast<Derived& >(*this)(s);

      //(*this)(a.variables());
      //static_cast<Derived& >(*this)(a.body());

      static_cast<Derived&>(*this).leave(a);
      static_cast<Derived&>(*this).leave(static_cast<abstraction const&>(a));
      static_cast<Derived&>(*this).leave(static_cast<data_expression const&>(a));
    }

  public:

    template <typename DataExpression >
    void enter(DataExpression const&)
    {}

    template <typename DataExpression >
    void leave(DataExpression const&)
    {}

    void operator()(identifier_string const& e)
    {
      static_cast<Derived&>(*this).enter(static_cast<identifier_string const&>(e));
      static_cast<Derived&>(*this).leave(static_cast<identifier_string const&>(e));
    }

    void operator()(function_symbol const& e)
    {
      static_cast<Derived&>(*this).enter(static_cast<data_expression const&>(e));
      static_cast<Derived&>(*this).enter(e);
      //static_cast<Derived&>(*this)(e.name());
      static_cast<Derived&>(*this).leave(e);
      static_cast<Derived&>(*this).leave(static_cast<data_expression const&>(e));
    }

    void operator()(variable const& e)
    {
      static_cast<Derived&>(*this).enter(static_cast<data_expression const&>(e));
      static_cast<Derived&>(*this).enter(e);
      //static_cast<Derived&>(*this)(e.name());
      static_cast<Derived&>(*this).leave(e);
      static_cast<Derived&>(*this).leave(static_cast<data_expression const&>(e));
    }

    void operator()(lambda const& e)
    {
      visit(e);
    }

    void operator()(forall const& e)
    {
      visit(e);
    }

    void operator()(exists const& e)
    {
      visit(e);
    }

    void operator()(abstraction const& e)
    {
      //if (e.is_lambda())
      //{
      //  static_cast<Derived&>(*this)(lambda(e));
      //}
      //else if (e.is_exists())
      //{
      //  static_cast<Derived&>(*this)(exists(e));
      //}
      //else if (e.is_forall())
      //{
      //  static_cast<Derived&>(*this)(forall(e));
      //}
    }

    void operator()(application const& e)
    {
      static_cast<Derived&>(*this).enter(static_cast<data_expression const&>(e));
      static_cast<Derived&>(*this).enter(e);

      //static_cast<Derived&>(*this)(e.head());
      //(*this)(e.arguments());

      static_cast<Derived&>(*this).leave(e);
      static_cast<Derived&>(*this).leave(static_cast<data_expression const&>(e));
    }

    void operator()(where_clause const& e)
    {
      static_cast<Derived&>(*this).enter(static_cast<data_expression const&>(e));
      static_cast<Derived&>(*this).enter(e);

      //(*this)(e.declarations());
      //static_cast<Derived&>(*this)(e.body());

      static_cast<Derived&>(*this).leave(e);
      static_cast<Derived&>(*this).leave(static_cast<data_expression const&>(e));
    }

    // Default, no traversal of sort expressions
    void operator()(data_expression const& e)
    {
      std::cout << "data_traverser::operator()(data_expression const& e)" << std::endl;
      static_cast<Derived&>(*this).enter(e);
      action_summand a;
      static_cast<Derived&>(*this)(a);
      static_cast<Derived&>(*this).leave(e);
      //if (e.is_application())
      //{
      //  static_cast<Derived&>(*this)(application(e));
      //}
      //else if (e.is_where_clause())
      //{
      //  static_cast<Derived&>(*this)(where_clause(e));
      //}
      //else if (e.is_abstraction())
      //{
      //  static_cast<Derived&>(*this)(abstraction(e));
      //}
      //else if (e.is_variable())
      //{
      //  static_cast<Derived&>(*this)(variable(e));
      //}
      //else if (e.is_function_symbol())
      //{
      //  static_cast<Derived&>(*this)(function_symbol(e));
      //}
    }

    void operator()(assignment const& a)
    {
      static_cast<Derived&>(*this).enter(a);

      //static_cast<Derived&>(*this)(a.lhs());
      //static_cast<Derived&>(*this)(a.rhs());

      static_cast<Derived&>(*this).leave(a);
    }

    void operator()(data_equation const& e)
    {
      static_cast<Derived&>(*this).enter(e);

      //static_cast<Derived&>(*this)(e.variables());
      //static_cast<Derived&>(*this)(e.condition());
      //static_cast<Derived&>(*this)(e.lhs());
      //static_cast<Derived&>(*this)(e.rhs());

      static_cast<Derived&>(*this).leave(e);
    }

    //template <typename Container>
    //void operator()(Container const& container, typename atermpp::detail::enable_if_container<Container>::type* = 0)
    //{
    //  for (typename Container::const_iterator i = container.begin(); i != container.end(); ++i)
    //  {
    //    static_cast<Derived&>(*this)(*i);
    //  }
    //}
};

template <typename Derived>
class lps_traverser: public data_traverser<Derived>
{
  public:
    typedef data_traverser<Derived> super;
    using super::operator();

    /// \brief Traverses an action label
    void operator()(const action_label& l)
    {
      static_cast<Derived&>(*this).enter(l);
      //static_cast<Derived&>(*this)(l.name());
      static_cast<Derived&>(*this).leave(l);
    }

    /// \brief Traverses an action
    /// \param a An action
    void operator()(const action& a)
    {
      static_cast<Derived&>(*this).enter(a);
      //static_cast<Derived&>(*this)(a.label());
      //static_cast<Derived&>(*this)(a.arguments());
      static_cast<Derived&>(*this).leave(a);
    }

    /// \brief Traverses a deadlock
    /// \param d A deadlock
    void operator()(const deadlock& d)
    {
      static_cast<Derived&>(*this).enter(d);
      //if (d.has_time()) {
      //  static_cast<Derived&>(*this)(d.time());
      //}
      static_cast<Derived&>(*this).leave(d);
    }

    /// \brief Traverses a multi-action
    /// \param a A multi-action
    void operator()(const multi_action& a)
    {
      static_cast<Derived&>(*this).enter(a);
      //if (a.has_time()) {
      //  static_cast<Derived&>(*this)(a.time());
      //}
      //static_cast<Derived&>(*this)(a.actions());
      static_cast<Derived&>(*this).leave(a);
    }

    /// \brief Traverses a summand
    /// \param s A summand
    void operator()(const action_summand& s)
    {
      static_cast<Derived&>(*this).enter(s);
      //static_cast<Derived&>(*this)(s.summation_variables());
      //static_cast<Derived&>(*this)(s.condition());
      //static_cast<Derived&>(*this)(s.multi_action());
      //static_cast<Derived&>(*this)(s.assignments());
      static_cast<Derived&>(*this).leave(s);
    }

    /// \brief Traverses a summand
    /// \param s A summand
    void operator()(const deadlock_summand& s)
    {
      static_cast<Derived&>(*this).enter(s);
      //static_cast<Derived&>(*this)(s.summation_variables());
      //static_cast<Derived&>(*this)(s.condition());
      //static_cast<Derived&>(*this)(s.deadlock());
      static_cast<Derived&>(*this).leave(s);
    }

    /// \brief Traverses a process_initializer
    /// \param s A process_initializer
    void operator()(const process_initializer& i)
    {
      static_cast<Derived&>(*this).enter(i);
      //static_cast<Derived&>(*this)(i.assignments());
      static_cast<Derived&>(*this).leave(i);
    }

    /// \brief Traverses a linear_process
    /// \param s A linear_process
    void operator()(const linear_process& p)
    {
      static_cast<Derived&>(*this).enter(p);
      //static_cast<Derived&>(*this)(p.process_parameters());
      //static_cast<Derived&>(*this)(p.action_summands());
      //static_cast<Derived&>(*this)(p.deadlock_summands());
      static_cast<Derived&>(*this).leave(p);
    }

    /// \brief Traverses a linear process specification
    /// \param spec A linear process specification
    void operator()(const specification& spec)
    {
      static_cast<Derived&>(*this).enter(spec);
      //static_cast<Derived&>(*this)(spec.process());
      //static_cast<Derived&>(*this)(spec.global_variables());
      //static_cast<Derived&>(*this)(spec.initial_process());
      //static_cast<Derived&>(*this)(spec.action_labels());
      static_cast<Derived&>(*this).leave(spec);
    }
};

// This could be some kind of find algorithm.
template <template <class> class Traverser>
struct instantiate: public Traverser<instantiate<Traverser> >
{
  typedef Traverser<instantiate<Traverser> > super;

  using super::operator();

  void enter(const data_expression&)
  {}

  void operator()(const action_summand&)
  {
    std::cout << "instantiate::operator()(const action_summand&)" << std::endl;
  }
};

int main()
{
  instantiate<lps_traverser> f;
  data_expression e;
  f(e);

  return 0;
}
