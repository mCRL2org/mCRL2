// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/print.h
/// \brief Functions for pretty printing ATerms.

#ifndef MCRL2_PRINT_H
#define MCRL2_PRINT_H

#include <cstdio>
#include <cctype>
#include <cassert>
#include <cstdlib>
#include <list>
#include <ostream>
#include <string>
#include <sstream>
#include <vector>
#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/atermpp/detail/utility.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/traverser.h"
#include "mcrl2/core/detail/precedence.h"
#include "mcrl2/core/print_format.h"

namespace mcrl2
{
namespace core
{

using namespace core::detail::precedences;

/// \cond INTERNAL_DOCS
namespace detail
{

template <typename Derived>
struct printer: public core::traverser<Derived>
{
  typedef core::traverser<Derived> super;

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  // using super::enter;
  // using super::leave;
  using super::operator();

  std::ostream* m_out;

  std::ostream& out()
  {
    return *m_out;
  }

  void print(const std::string& s)
  {
    out() << s;
  }

  template <typename T>
  void print_expression(const T& x, int prec = 5)
  {
#ifdef MCRL2_DEBUG_PRECEDENCE
    std::cout << "<print_expression> precedence = " << prec << std::endl;
    std::cout << "<x>" << x << " precedence = " << precedence(x) << std::endl;
#endif
    bool print_parens = (precedence(x) < prec);
    if (print_parens)
    {
      derived().print("(");
    }
    derived()(x);
    if (print_parens)
    {
      derived().print(")");
    }
  }

  template <typename T>
  void print_unary_operation(const T& x, const std::string& op)
  {
    derived().print(op);
    print_expression(unary_operand(x), precedence(x));
  }

  template <typename T>
  void print_binary_operation(const T& x, const std::string& op)
  {
#ifdef MCRL2_DEBUG_PRECEDENCE
    std::cout << "<binary>" << std::endl;
    std::cout << "<x>" << x << " precedence = " << precedence(x) << std::endl;
    std::cout << "<left>" << x.left() << " precedence = " << precedence(binary_left(x)) << std::endl;
    std::cout << "<right>" << x.right() << " precedence = " << precedence(binary_right(x)) << std::endl;
#endif
    print_expression(binary_left(x), is_same_different_precedence(x, binary_left(x)) ? precedence(x) + 1 : precedence(x));
    derived().print(op);
    print_expression(binary_right(x), is_same_different_precedence(x, binary_right(x)) ? precedence(x) + 1 : precedence(x));
  }

  template <typename Container>
  void print_list(const Container& container,
                  const std::string& opener = "(",
                  const std::string& closer = ")",
                  const std::string& separator = ", ",
                  bool print_empty_container = false
                 )
  {
    if (container.empty() && !print_empty_container)
    {
      return;
    }
    derived().print(opener);
    for (typename Container::const_iterator i = container.begin(); i != container.end(); ++i)
    {
      if (i != container.begin())
      {
        derived().print(separator);
      }
      derived()(*i);
    }
    derived().print(closer);
  }

  template <typename T>
  void operator()(const atermpp::term_appl<T>& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).print(utilities::to_string(x));
    static_cast<Derived&>(*this).leave(x);
  }

  template <typename T>
  void operator()(const std::list<T>& x)
  {
    static_cast<Derived&>(*this).enter(x);
    print_list(x, "", "", ", ");
    static_cast<Derived&>(*this).leave(x);
  }

  template <typename T>
  void operator()(const atermpp::term_list<T>& x)
  {
    static_cast<Derived&>(*this).enter(x);
    print_list(x, "", "", ", ");
    static_cast<Derived&>(*this).leave(x);
  }

  template <typename T>
  void operator()(const std::set<T>& x)
  {
    static_cast<Derived&>(*this).enter(x);
    print_list(x, "", "", ", ");
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(const core::identifier_string& x)
  {
    static_cast<Derived&>(*this).enter(x);
    if (x == core::identifier_string())
    {
      static_cast<Derived&>(*this).print("@NoValue");
    }
    else
    {
      static_cast<Derived&>(*this).print(std::string(x));
    }
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(atermpp::aterm x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).print(utilities::to_string(x));
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(atermpp::aterm_list x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).print(utilities::to_string(x));
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(atermpp::aterm_appl x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).print(utilities::to_string(x));
    static_cast<Derived&>(*this).leave(x);
  }
};

template <template <class> class Traverser>
struct apply_printer: public Traverser<apply_printer<Traverser> >
{
  typedef Traverser<apply_printer<Traverser> > super;

  using super::enter;
  using super::leave;
  using super::operator();

  apply_printer(std::ostream& out)
  {
    typedef printer<apply_printer<Traverser> > Super;
    static_cast<Super&>(*this).m_out = &out;
  }

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif
};

} // namespace detail
/// \endcond

/// \brief Prints the object x to a stream.
struct stream_printer
{
  template <typename T>
  void operator()(const T& x, std::ostream& out)
  {
    core::detail::apply_printer<core::detail::printer> printer(out);
    printer(x);
  }
};

/// \brief Returns a string representation of the object x.
template <typename T>
std::string pp(const T& x)
{
  std::ostringstream out;
  stream_printer()(x, out);
  return out.str();
}

/// \brief Prototypes for aterm overloads
std::string pp(const atermpp::aterm& x);
std::string pp(const atermpp::aterm_appl& x);

} // namespace core

} // namespace mcrl2

#endif // MCRL2_PRINT_H
