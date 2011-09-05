// Author(s): Aad Mathijssen
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

/** \brief Return a textual description of an ATerm representation of an
 *         mCRL2 specification or expression.
 *  \param[in] part An ATerm representation of a part of an mCRL2
 *             specification or expression.
 *  \param[in] pp_format A pretty print format.
 *  \return A textual representation of part according to method pp_format.
**/
template <typename Term>
std::string pp(Term part, t_pp_format pp_format = ppDefault)
{
  return PrintPart_CXX(atermpp::aterm_traits<Term>::term(part), pp_format);
}

/// \cond INTERNAL_DOCS
namespace detail
{

inline
void check_pp(const std::string& s1, const std::string& s2, const std::string& s3)
{
#ifndef MCRL2_DISABLE_PRINT_CHECKS
  if (s1 != s2)
  {
    std::clog << "--- WARNING: difference detected between old and new pretty printer ---\n";
    std::clog << "old:   " << s1 << std::endl;
    std::clog << "new:   " << s2 << std::endl;
    std::clog << "aterm: " << s3 << std::endl;
#ifdef MCRL2_THROW_ON_PRINT_DIFFERENCES
    throw mcrl2::runtime_error("pretty print difference detected");
#endif
  }
#endif
}

#define MCRL2_CHECK_PP(s1, s2, s3) core::detail::check_pp(s1, s2, s3);

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
  void operator()(const atermpp::term_list<T>& t)
  {
    print_list(t, "", "", ", ");
  }

  void operator()(const core::identifier_string& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).print(std::string(x));
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

/// \brief Prints the object t to a stream.
template <typename T>
void print(const T& t, std::ostream& out)
{
  detail::apply_printer<core::detail::printer> printer(out);
  printer(t);
}

/// \brief Returns a string representation of the object t.
template <typename T>
std::string print(const T& t)
{
  std::ostringstream out;
  print(t, out);
  return out.str();
}

}
}

#endif //MCRL2_PRINT_H
