// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/print.h
/// \brief add your file description here.

#ifndef MCRL2_BES_PRINT_H
#define MCRL2_BES_PRINT_H

#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/core/print.h"

namespace mcrl2 {

namespace bes {

namespace detail {

template <typename Derived>
struct printer: public bes::add_traverser_boolean_expressions<core::detail::printer, Derived>
{
  typedef bes::add_traverser_boolean_expressions<core::detail::printer, Derived> super;

  using super::enter;
  using super::leave;
  using super::operator();
  using super::print_expression;
  using super::print_unary_operation;
  using super::print_binary_operation;
  using super::print_list;

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  void operator()(const bes::boolean_equation& x)
  {
    derived().enter(x);
    derived().print(x.symbol().is_mu() ? "mu " : "nu ");
    derived()(x.variable());
    derived().print(" = ");
    derived()(x.formula());
    derived().leave(x);
  }

  template <typename Container>
  void operator()(const bes::boolean_equation_system<Container>& x)
  {
    derived().enter(x);
    print_list(x.equations(), "pbes\n    ", ";\n\n", ";\n    ");
    derived().print("init ");
    print_expression(x.initial_state());
    derived().print(";\n");
    derived().leave(x);
  }

  void operator()(const bes::true_& x)
  {
    derived().enter(x);
    derived().print("true");
    derived().leave(x);
  }

  void operator()(const bes::false_& x)
  {
    derived().enter(x);
    derived().print("false");
    derived().leave(x);
  }

  void operator()(const bes::not_& x)
  {
    derived().enter(x);
    print_unary_operation(x, "!");
    derived().leave(x);
  }

  void operator()(const bes::and_& x)
  {
    derived().enter(x);
    print_binary_operation(x, " && ");
    derived().leave(x);
  }

  void operator()(const bes::or_& x)
  {
    derived().enter(x);
    print_binary_operation(x, " || ");
    derived().leave(x);
  }

  void operator()(const bes::imp& x)
  {
    derived().enter(x);
    print_binary_operation(x, " => ");
    derived().leave(x);
  }

  void operator()(const bes::boolean_variable& x)
  {
    derived().enter(x);
    derived()(x.name());
    derived().leave(x);
  }
};

} // namespace detail

/// \brief Prints the object t to a stream.
template <typename T>
void print(const T& t, std::ostream& out)
{
  core::detail::apply_printer<bes::detail::printer> printer(out);
  printer(t);
}

/// \brief Returns a string representation of the object t.
template <typename T>
std::string print(const T& t)
{
  std::ostringstream out;
  bes::print(t, out);
  return out.str();
}

/// \brief Pretty print function
/// \param v A boolean variable
/// \return A pretty printed representation of the boolean variable
inline
std::string pp(const boolean_variable& v)
{
  std::string result = std::string(v.name());
  MCRL2_CHECK_PP(result, bes::print(v), v.to_string());
  return result;
}

/// \brief Pretty print function
/// \param e A boolean expression
/// \param add_parens If true, parentheses are put around sub-expressions.
/// \return A pretty printed representation of the boolean expression.
// TODO: the implementation is not very efficient
inline
std::string pp(boolean_expression e, bool add_parens = false)
{
  typedef core::term_traits<boolean_expression> tr;

  if (tr::is_variable(e))
  {
    std::string result = bes::pp(boolean_variable(e));
    MCRL2_CHECK_PP(result, bes::print(e), e.to_string());
    return result;
  }
  else if (tr::is_true(e))
  {
    std::string result = "true";
    MCRL2_CHECK_PP(result, bes::print(e), e.to_string());
    return result;
  }
  else if (tr::is_false(e))
  {
    std::string result = "false";
    MCRL2_CHECK_PP(result, bes::print(e), e.to_string());
    return result;
  }
  else if (tr::is_not(e))
  {
    std::string result = std::string("!") + (add_parens ? "(" : "") + bes::pp(tr::arg(e), true) + (add_parens ? ")" : "");
    MCRL2_CHECK_PP(result, bes::print(e), e.to_string());
    return result;
  }
  else if (tr::is_and(e))
  {
    std::string result = (add_parens ? "(" : "") + bes::pp(tr::left(e), true) + " && " + bes::pp(tr::right(e), true) + (add_parens ? ")" : "");
    MCRL2_CHECK_PP(result, bes::print(e), e.to_string());
    return result;
  }
  else if (tr::is_or(e))
  {
    std::string result = (add_parens ? "(" : "") + bes::pp(tr::left(e), true) + " || " + bes::pp(tr::right(e), true) + (add_parens ? ")" : "");
    MCRL2_CHECK_PP(result, bes::print(e), e.to_string());
    return result;
  }
  else if (tr::is_imp(e))
  {
    std::string result = (add_parens ? "(" : "") + bes::pp(tr::left(e), true) + " => " + bes::pp(tr::right(e), true) + (add_parens ? ")" : "");
    MCRL2_CHECK_PP(result, bes::print(e), e.to_string());
    return result;
  }
  throw mcrl2::runtime_error("error in mcrl2::bes::pp: encountered unknown boolean expression " + e.to_string());
  return "";
}

template<typename Container>
inline
std::string pp(const Container& c, bool add_parens = false, typename atermpp::detail::enable_if_container<Container>::type* = 0)
{
  std::vector<std::string> v;
  for(typename Container::const_iterator i = c.begin(); i != c.end(); ++i)
  {
    v.push_back(pp(*i, add_parens));
  }
  std::string result = utilities::string_join(v, ", ");
  MCRL2_CHECK_PP(result, bes::print(c), "bes container");
  return result;
}

/// \brief Pretty print function
/// \param eq A boolean equation
/// \return A pretty printed representation of the boolean equation
inline
std::string pp(const boolean_equation& eq)
{
  std::string result = core::pp(eq.symbol()) + " " + bes::pp(eq.variable()) + " = " + bes::pp(eq.formula());
  MCRL2_CHECK_PP(result, bes::print(eq), "boolean equation");
  return result;
}

/// \brief Pretty print function
/// \param p A boolean equation system
/// \return A pretty printed representation of the boolean equation system
template <typename Container>
std::string pp(const boolean_equation_system<Container>& p)
{
  std::ostringstream out;
  out << "pbes\n";
  BOOST_FOREACH(const boolean_equation& eq, p.equations())
  {
    out << "    " << bes::pp(eq) << ";" << std::endl;
  }
  out << "\ninit " << bes::pp(p.initial_state()) << ";" << std::endl;
  MCRL2_CHECK_PP(out.str(), bes::print(p), "boolean_equation_system");
  return out.str();
}

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_PRINT_H
