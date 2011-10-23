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
#include <ostream>
#include <string>
#include <sstream>
#include <vector>
#include "mcrl2/exception.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/set.h"
#include "mcrl2/core/traverser.h"
#include "mcrl2/core/detail/precedence.h"
#include "mcrl2/exception.h"

namespace mcrl2
{
namespace core
{

using namespace core::detail::precedences;

/// \brief t_pp_format represents the available pretty print formats
typedef enum { ppDefault, ppDebug, ppInternal, ppInternalDebug} t_pp_format;

/// \brief Print string representation of pretty print format
/// \param pp_format a pretty print format
/// \return string representation of the pretty print format
/// \throws mcrl2::runtime error if an unknown pretty print format
///         is passed into the function.
inline
std::string pp_format_to_string(const t_pp_format pp_format)
{
  switch (pp_format)
  {
    case ppDefault:
      return "default";
    case ppDebug:
      return "debug";
    case ppInternal:
      return "internal";
    case ppInternalDebug:
      return "internal_debug";
    default:
      throw mcrl2::runtime_error("Unknown pretty print format");
  }
}


/** \brief Print a textual description of an ATerm representation of an
 *         mCRL2 specification or expression to an output stream.
 *  \param[in] out_stream A stream to which can be written.
 *  \param[in] part An ATerm representation of a part of an mCRL2
 *             specification or expression.
 *  \param[in] pp_format A pretty print format.
 *  \post A textual representation of part is written to out_stream using
 *        method pp_format.
**/
void PrintPart_CXX(std::ostream& out_stream, const ATerm part,
                   t_pp_format pp_format = ppDefault);

/** \brief Return a textual description of an ATerm representation of an
 *         mCRL2 specification or expression.
 *  \param[in] part An ATerm representation of a part of an mCRL2
 *             specification or expression.
 *  \param[in] pp_format A pretty print format.
 *  \return A textual representation of part according to method pp_format.
**/
std::string PrintPart_CXX(const ATerm part, t_pp_format pp_format = ppDefault);

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
    print_expression(x.operand(), precedence(x));
  }

  template <typename T>
  void print_binary_operation(const T& x, const std::string& op)
  {
#ifdef MCRL2_DEBUG_BINARY_OPERATION
    std::cout << "<binary>" << std::endl;
    std::cout << "<x>" << x.to_string() << " precedence = " << precedence(x) << std::endl;
    std::cout << "<left>" << x.left().to_string() << " precedence = " << precedence(x.left()) << std::endl;
    std::cout << "<right>" << x.right().to_string() << " precedence = " << precedence(x.right()) << std::endl;
#endif
    print_expression(x.left(), is_same_different_precedence(x, x.left()) ? precedence(x) + 1 : precedence(x));
    derived().print(op);
    print_expression(x.right(), is_same_different_precedence(x, x.right()) ? precedence(x) + 1 : precedence(x));
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
    static_cast<Derived&>(*this).print(x.to_string());
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
  void operator()(const atermpp::set<T>& x)
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

  void operator()(aterm::ATerm x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).print(atermpp::aterm(x).to_string());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(aterm::ATermList x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).print(atermpp::aterm_list(x).to_string());
    static_cast<Derived&>(*this).leave(x);
  }

  void operator()(aterm::ATermAppl x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).print(atermpp::aterm_appl(x).to_string());
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

} // namespace core

} // namespace mcrl2

#endif // MCRL2_PRINT_H
