// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbes_initializer.h
/// \brief The class pbes_initializer.

#ifndef MCRL2_PBES_DETAIL_PBES_INITIALIZER_H
#define MCRL2_PBES_DETAIL_PBES_INITIALIZER_H

#include <cassert>
#include <string>
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/utility.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/lps/detail/specification_utility.h"   // compute_initial_state
#include "mcrl2/pbes/propositional_variable.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// \brief The initial state of a pbes.
// <PBInit>       ::= PBInit(<DataVarId>*, <PropVarInst>)
class pbes_initializer: public atermpp::aterm_appl
{
  protected:
    /// \brief The free variables of the PBES initializer
    data::data_variable_list   m_free_variables;

    /// \brief The value of the PBES initializer
    propositional_variable_instantiation m_variable;

  public:
    /// \brief Constructor.
    pbes_initializer()
      : atermpp::aterm_appl(core::detail::constructPBInit())
    {}

    /// \brief Constructor.
    pbes_initializer(data::data_variable_list free_variables,
                        propositional_variable_instantiation variable
                       )
     : atermpp::aterm_appl(core::detail::gsMakePBInit(free_variables, variable)),
       m_free_variables(free_variables),
       m_variable(variable)
    {
    }

    /// \brief Constructor.
    /// \param t A term
    pbes_initializer(atermpp::aterm_appl t)
      : atermpp::aterm_appl(t)
    {
      assert(core::detail::check_term_PBInit(m_term));
      atermpp::aterm_appl::iterator i   = t.begin();
      m_free_variables = *i++;
      m_variable = propositional_variable_instantiation(*i);
    }

    /// \brief Returns the sequence of free variables.
    /// \return The sequence of free variables.
    data::data_variable_list free_variables() const
    {
      return m_free_variables;
    }

    /// \brief Returns the sequence of variables.
    /// \return The sequence of variables.
    propositional_variable_instantiation variable() const
    {
      return m_variable;
    }

    /// \brief Applies a low level substitution function to this term and returns the result.
    /// \param f A
    /// The function <tt>f</tt> must supply the method <tt>aterm operator()(aterm)</tt>.
    /// This function is applied to all <tt>aterm</tt> noded appearing in this term.
    /// \deprecated
    /// \return The substitution result.
    template <typename Substitution>
    pbes_initializer substitute(Substitution f)
    {
      return pbes_initializer(f(atermpp::aterm(*this)));
    }

    /// \brief Checks if the PBES initializer is well typed.
    /// \return Always returns true.
    bool is_well_typed() const
    {
      return true;
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PBES_INITIALIZER_H
