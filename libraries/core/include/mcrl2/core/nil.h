// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/nil.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_NIL_H
#define MCRL2_CORE_NIL_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/detail/default_values.h"
#include "mcrl2/core/detail/function_symbols.h"
#include "mcrl2/core/detail/soundness_checks.h"

namespace mcrl2 {

namespace core {

//--- start generated class nil ---//
/// \brief The value nil
class nil: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    nil()
      : atermpp::aterm_appl(core::detail::default_values::Nil)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit nil(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_Nil(*this));
    }
};

/// \brief list of nils
typedef atermpp::term_list<nil> nil_list;

/// \brief vector of nils
typedef std::vector<nil>    nil_vector;

/// \brief Test for a nil expression
/// \param x A term
/// \return True if \a x is a nil expression
inline
bool is_nil(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::Nil;
}

// prototype declaration
std::string pp(const nil& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const nil& x)
{
  return out << core::pp(x);
}

/// \brief swap overload
inline void swap(nil& t1, nil& t2)
{
  t1.swap(t2);
}
//--- end generated class nil ---//

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_NIL_H
