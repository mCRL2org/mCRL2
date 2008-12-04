// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes_initializer.h
/// \brief The class pbes_initializer.

#ifndef MCRL2_PBES_PBES_INITIALIZER_H
#define MCRL2_PBES_PBES_INITIALIZER_H

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

/// \brief The initial state of a pbes.
// <PBInit>       ::= PBInit(<DataVarId>*, <PropVarInst>)
class pbes_initializer: public atermpp::aterm_appl
{
  protected:
    data::data_variable_list   m_free_variables;
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
    pbes_initializer(atermpp::aterm_appl t)
      : atermpp::aterm_appl(t)
    {
      assert(core::detail::check_term_PBInit(m_term));
      atermpp::aterm_appl::iterator i   = t.begin();
      m_free_variables = *i++;
      m_variable = propositional_variable_instantiation(*i);
    }

    /// \brief Returns the sequence of free variables.
    data::data_variable_list free_variables() const
    {
      return m_free_variables;
    }

    /// \brief Returns the sequence of variables.
    propositional_variable_instantiation variable() const
    {
      return m_variable;
    }

    /// \brief Applies a substitution to this pbes initializer and returns the result.
    /// \brief The Substitution object must supply the method atermpp::aterm operator()(atermpp::aterm).
    template <typename Substitution>
    pbes_initializer substitute(Substitution f)
    {
      return pbes_initializer(f(atermpp::aterm(*this)));
    }

    /// \brief Returns true (there are no well typedness checks defined yet).
    bool is_well_typed() const
    {
      return true;
    }
};

} // namespace pbes_system

} // namespace mcrl2

/// \cond INTERNAL_DOCS
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::pbes_system::pbes_initializer)
/// \endcond

#endif // MCRL2_PBES_PBES_INITIALIZER_H
