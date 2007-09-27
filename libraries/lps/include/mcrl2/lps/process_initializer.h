// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/process_initializer.h
/// \brief Add your file description here.

#ifndef MCRL2_LPS_PROCESS_INITIALIZER_H
#define MCRL2_LPS_PROCESS_INITIALIZER_H

#include <cassert>
#include <string>
#include "atermpp/algorithm.h"
#include "atermpp/utility.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/lps/detail/specification_utility.h"   // compute_initial_state

namespace lps {

using namespace std::rel_ops; // for definition of operator!= in terms of operator==
using atermpp::aterm_appl;

/// \brief initial state of a linear process
// LinearProcessInit(<DataVarId>*, <DataVarIdInit>*)
class process_initializer: public aterm_appl
{
  protected:
    data_variable_list   m_free_variables;
    data_assignment_list m_assignments;

  public:
    process_initializer()
      : aterm_appl(detail::constructLinearProcessInit())
    {}

    process_initializer(data_variable_list free_variables,
                        data_assignment_list assignments
                       )
     : aterm_appl(gsMakeLinearProcessInit(free_variables, assignments)),
       m_free_variables(free_variables),
       m_assignments(assignments)
    {
    }

    process_initializer(aterm_appl t)
      : aterm_appl(t)
    {
      assert(detail::check_term_LinearProcessInit(m_term));
      aterm_appl::iterator i   = t.begin();
      m_free_variables = *i++;
      m_assignments    = *i;
    }

    /// Returns the sequence of free variables.
    ///
    data_variable_list free_variables() const
    {
      return m_free_variables;
    }

    /// Returns the sequence of assignments.
    ///
    data_assignment_list assignments() const
    {
      return m_assignments;
    }

    /// Returns the initial state of the LPS.
    ///
    data_expression_list state() const
    {
      return detail::compute_initial_state(m_assignments);
    }

    /// Applies a substitution to this process initializer and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    process_initializer substitute(Substitution f)
    {
      return process_initializer(f(aterm(*this)));
    }     

    /// Returns true if
    /// <ul>
    // <li>the summands are well typed</li>
    // <li>the process parameters have unique names</li>
    // <li>the free variables have unique names</li>
    // <li>the names of the process parameters do not appear as the name of a summation variable</li>
    // <li>the left hand sides of the assignments of summands are contained in the process parameters</li>
    /// </ul>
    ///
    bool is_well_typed() const
    {
      return true;
    }
};

} // namespace lps

/// \internal
namespace atermpp
{
using lps::process_initializer;

template<>
struct aterm_traits<process_initializer>
{
  typedef ATermAppl aterm_type;
  static void protect(process_initializer t)   { t.protect(); }
  static void unprotect(process_initializer t) { t.unprotect(); }
  static void mark(process_initializer t)      { t.mark(); }
  static ATerm term(process_initializer t)     { return t.term(); }
  static ATerm* ptr(process_initializer& t)    { return &t.term(); }
};

} // namespace atermpp

#endif // MCRL2_LPS_PROCESS_INITIALIZER_H
