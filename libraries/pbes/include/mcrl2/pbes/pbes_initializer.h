// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/pbes_initializer.h
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

namespace lps {

using atermpp::aterm_appl;
using namespace mcrl2::data;

/// \brief The initial state of a pbes.
// <PBInit>       ::= PBInit(<DataVarId>*, <PropVarInst>)
class pbes_initializer: public aterm_appl
{
  protected:
    data_variable_list   m_free_variables;
    propositional_variable_instantiation m_variable;

  public:
    /// Constructor.
    ///
    pbes_initializer()
      : aterm_appl(mcrl2::core::detail::constructPBInit())
    {}

    /// Constructor.
    ///
    pbes_initializer(data_variable_list free_variables,
                        propositional_variable_instantiation variable
                       )
     : aterm_appl(gsMakePBInit(free_variables, variable)),
       m_free_variables(free_variables),
       m_variable(variable)
    {
    }

    /// Constructor.
    ///
    pbes_initializer(aterm_appl t)
      : aterm_appl(t)
    {
      assert(mcrl2::core::detail::check_term_PBInit(m_term));
      aterm_appl::iterator i   = t.begin();
      m_free_variables = *i++;
      m_variable = propositional_variable_instantiation(*i);
    }

    /// Returns the sequence of free variables.
    ///
    data_variable_list free_variables() const
    {
      return m_free_variables;
    }

    /// Returns the sequence of variables.
    ///
    propositional_variable_instantiation variable() const
    {
      return m_variable;
    }

    /// Applies a substitution to this pbes initializer and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    pbes_initializer substitute(Substitution f)
    {
      return pbes_initializer(f(aterm(*this)));
    }     

    /// Returns true (there are no well typedness checks defined yet).
    ///
    bool is_well_typed() const
    {
      return true;
    }
};

} // namespace lps

/// \cond INTERNAL_DOCS
namespace atermpp
{
using lps::pbes_initializer;

template<>
struct aterm_traits<pbes_initializer>
{
  typedef ATermAppl aterm_type;
  static void protect(pbes_initializer t)   { t.protect(); }
  static void unprotect(pbes_initializer t) { t.unprotect(); }
  static void mark(pbes_initializer t)      { t.mark(); }
  static ATerm term(pbes_initializer t)     { return t.term(); }
  static ATerm* ptr(pbes_initializer& t)    { return &t.term(); }
};

} // namespace atermpp
/// \endcond

#endif // MCRL2_PBES_PBES_INITIALIZER_H
