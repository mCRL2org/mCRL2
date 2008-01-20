// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/process_initializer.h
/// \brief The class process_initializer.

#ifndef MCRL2_LPS_PROCESS_INITIALIZER_H
#define MCRL2_LPS_PROCESS_INITIALIZER_H

#include <cassert>
#include <string>
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/utility.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/detail/data_assignment_functional.h"
#include "mcrl2/lps/detail/specification_utility.h"   // compute_initial_state
#include "mcrl2/data/detail/sequence_algorithm.h"

namespace mcrl2 {

namespace lps {

using atermpp::aterm_appl;
using atermpp::aterm;

/// \brief Initial state of a linear process.
// LinearProcessInit(<DataVarId>*, <DataVarIdInit>*)
class process_initializer: public aterm_appl
{
  protected:
    data::data_variable_list   m_free_variables;
    data::data_assignment_list m_assignments;

  public:
    /// Constructor.
    ///
    process_initializer()
      : aterm_appl(mcrl2::core::detail::constructLinearProcessInit())
    {}

    /// Constructor.
    ///
    process_initializer(data::data_variable_list free_variables,
                        data::data_assignment_list assignments
                       )
     : aterm_appl(core::detail::gsMakeLinearProcessInit(free_variables, assignments)),
       m_free_variables(free_variables),
       m_assignments(assignments)
    {
    }

    /// Constructor.
    ///
    process_initializer(aterm_appl t)
      : aterm_appl(t)
    {
      assert(core::detail::check_term_LinearProcessInit(m_term));
      aterm_appl::iterator i   = t.begin();
      m_free_variables = *i++;
      m_assignments    = *i;
    }

    /// Returns the sequence of free variables.
    ///
    data::data_variable_list free_variables() const
    {
      return m_free_variables;
    }

    /// Returns the sequence of assignments.
    ///
    data::data_assignment_list assignments() const
    {
      return m_assignments;
    }

    /// Returns the initial state of the LPS.
    ///
    data::data_expression_list state() const
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
    // <li>the left hand sides of the data assignments are unique</li>
    /// </ul>
    ///
    bool is_well_typed() const
    {
      // check 1)
      if (mcrl2::data::detail::sequence_contains_duplicates(
               boost::make_transform_iterator(m_assignments.begin(), data::detail::data_assignment_lhs()),
               boost::make_transform_iterator(m_assignments.end()  , data::detail::data_assignment_lhs())
              )
         )
      {
        std::cerr << "process_initializer::is_well_typed() failed: data assignments " << pp(m_assignments) << " don't have unique left hand sides." << std::endl;
        return false;
      }

      return true;
    }
};

} // namespace lps

} // namespace mcrl2

/// \cond INTERNAL_DOCS
namespace atermpp
{
using mcrl2::lps::process_initializer;

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
/// \endcond

#endif // MCRL2_LPS_PROCESS_INITIALIZER_H
