// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/boolean_variable.h
/// \brief add your file description here.

#ifndef MCRL2_BES_BOOLEAN_VARIABLE_H
#define MCRL2_BES_BOOLEAN_VARIABLE_H

#include <cassert>
#include <string>
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/print.h"

namespace mcrl2 {

namespace bes {

  /// \brief boolean variable
  class boolean_variable: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      boolean_variable()
        : atermpp::aterm_appl(core::detail::constructBooleanVariable())
      {}

      /// \brief Constructor.
      /// \param term A term
      boolean_variable(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(core::detail::check_rule_BooleanVariable(m_term));
      }

      /// \brief Constructor.
      /// \param name A
      explicit boolean_variable(core::identifier_string name)
        : atermpp::aterm_appl(core::detail::gsMakeBooleanVariable(name))
      {
        assert(core::detail::check_rule_BooleanVariable(m_term));
      }

      /// \brief Constructor.
      /// \param name A string
      boolean_variable(const std::string& name)
        : atermpp::aterm_appl(core::detail::gsMakeBooleanVariable(core::detail::gsString2ATermAppl(name.c_str())))
      {
        assert(core::detail::check_rule_BooleanVariable(m_term));
      }

      /// \brief Returns the name of the boolean variable.
      /// \return The name of the boolean variable.
      core::identifier_string name() const
      {
        return atermpp::arg1(*this);
      }

      /// \brief Applies a low level substitution function to this term and returns the result.
      /// \param f A
      /// The function <tt>f</tt> must supply the method <tt>aterm operator()(aterm)</tt>.
      /// This function is applied to all <tt>aterm</tt> noded appearing in this term.
      /// \deprecated
      /// \return The substitution result.
      template <typename Substitution>
      boolean_variable substitute(Substitution f) const
      {
        return boolean_variable(f(*this));
      }
  };

  /// \brief Read-only singly linked list of boolean variables
  typedef atermpp::term_list<boolean_variable> boolean_variable_list;

  /// \brief Pretty print function
  /// \param v A boolean variable
  /// \return A pretty printed representation of the boolean variable
  inline
  std::string pp(boolean_variable v)
  {
    return core::pp(v.name());
  }

  /// \brief Returns true if the term t is a boolean variable
  /// \param t A boolean variable
  /// \return True if the term t is a boolean variable
  inline
  bool is_boolean_variable(atermpp::aterm_appl t)
  {
    return core::detail::gsIsBooleanVariable(t);
  }

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_BOOLEAN_VARIABLE_H
