// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/linear_process.h
/// \brief The class linear_process.

#ifndef MCRL2_LPS_LINEAR_PROCESS_H
#define MCRL2_LPS_LINEAR_PROCESS_H

#include <string>
#include <cassert>
#include <algorithm>
#include <functional>
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/filtered_list.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/utility.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/print.h"
#include "mcrl2/data/detail/sequence_algorithm.h"
#include "mcrl2/data/detail/sorted_sequence_algorithm.h"
#include "mcrl2/lps/find.h"
#include "mcrl2/lps/summand.h"
#include "mcrl2/lps/process_initializer.h"

namespace mcrl2 {

namespace lps {

class linear_process; // prototype declaration

inline
std::set<data::variable> compute_free_variables(const linear_process& process); // prototype declaration

///////////////////////////////////////////////////////////////////////////////
// linear_process
/// \brief linear process.
class linear_process
{
  protected:
    /// \brief The free variables of the process
    data::variable_list m_global_variables;

    /// \brief The process parameters of the process
    data::variable_list m_process_parameters;

    /// \brief The deadlock summands of the process   
    deadlock_summand_vector m_deadlock_summands;

    /// \brief The action summands of the process   
    action_summand_vector m_action_summands;

  public:
    /// \brief Constructor.
    linear_process()
    {}

    /// \brief Constructor.
    linear_process(data::variable_list free_variables,
        data::variable_list process_parameters,
        const deadlock_summand_vector& deadlock_summands,
        const action_summand_vector& action_summands
       )
     :
       m_global_variables    (free_variables    ),
       m_process_parameters(process_parameters),
       m_deadlock_summands (deadlock_summands ),
       m_action_summands   (action_summands   )
    { }

    /// \brief Set the summands of the linear process
    /// \deprecated
    void set_summands(const summand_list& summands)
    {
      m_deadlock_summands.clear();
      m_action_summands  .clear();
      for (summand_list::iterator j = summands.begin(); j != summands.end(); ++j)
      {
        if (j->is_delta())
        {
          m_deadlock_summands.push_back(deadlock_summand(j->summation_variables(), j->condition(), j->deadlock()));
        }
        else
        {
          m_action_summands.push_back(action_summand(j->summation_variables(), j->condition(), j->multi_action(), j->assignments()));
        }
      }
    }

    /// \brief Constructor.
    /// \param lps A term
    linear_process(atermpp::aterm_appl lps)
    {
      assert(core::detail::check_term_LinearProcess(lps));

      // unpack LPS(.,.,.) term
      atermpp::aterm_appl::iterator i = lps.begin();
      m_global_variables = *i++;
      m_process_parameters = *i++;
      set_summands(*i);
    }

    /// \brief Returns the number of LPS summands.
    /// \return The number of LPS summands.
    unsigned int summand_count() const
    {
      return m_deadlock_summands.size() + m_action_summands.size();
    }

    /// \brief Returns the sequence of LPS summands.
    /// \return The sequence of LPS summands.
    summand_list summands() const
    {
      summand_list result;
      for (deadlock_summand_vector::const_reverse_iterator i = m_deadlock_summands.rbegin(); i != m_deadlock_summands.rend(); ++i)
      {
        summand s = atermpp::aterm_appl(deadlock_summand_to_aterm(*i));
        result = atermpp::push_front(result, s);
      }
      for (action_summand_vector::const_reverse_iterator i = m_action_summands.rbegin(); i != m_action_summands.rend(); ++i)
      {
        summand s = atermpp::aterm_appl(action_summand_to_aterm(*i));
        result = atermpp::push_front(result, s);
      }
      return result;
    }

    /// \brief Returns the sequence of action summands.
    /// \return The sequence of action summands.
    const action_summand_vector& action_summands() const
    {
      return m_action_summands;
    }

    /// \brief Returns the sequence of action summands.
    /// \return The sequence of action summands.
    action_summand_vector& action_summands()
    {
      return m_action_summands;
    }

    /// \brief Returns the sequence of deadlock summands.
    /// \return The sequence of deadlock summands.
    const deadlock_summand_vector& deadlock_summands() const
    {
      return m_deadlock_summands;
    }

    /// \brief Returns the sequence of deadlock summands.
    /// \return The sequence of deadlock summands.
    deadlock_summand_vector& deadlock_summands()
    {
      return m_deadlock_summands;
    }

    /// \brief Returns the sequence of free variables.
    /// \return The sequence of free variables.
    const data::variable_list& global_variables() const
    {
      return m_global_variables;
    }

    /// \brief Returns the sequence of free variables.
    /// \return The sequence of free variables.
    data::variable_list& global_variables()
    {
      return m_global_variables;
    }

    /// \brief Returns the sequence of process parameters.
    /// \return The sequence of process parameters.
    const data::variable_list& process_parameters() const
    {
      return m_process_parameters;
    }

    /// \brief Returns the sequence of process parameters.
    /// \return The sequence of process parameters.
    data::variable_list& process_parameters()
    {
      return m_process_parameters;
    }

    /// \brief Returns true if time is available in at least one of the summands.
    /// \return True if time is available in at least one of the summands.
    bool has_time() const
    {
      for (action_summand_vector::const_iterator i = m_action_summands.begin(); i != m_action_summands.end(); ++i)
      {
        if(i->has_time()) return true;
      }
      for (deadlock_summand_vector::const_iterator i = m_deadlock_summands.begin(); i != m_deadlock_summands.end(); ++i)
      {
        if(i->deadlock().has_time()) return true;
      }
      return false;
    }

    /// \brief Applies a low level substitution function to this term and returns the result.
    /// \param f A
    /// The function <tt>f</tt> must supply the method <tt>aterm operator()(aterm)</tt>.
    /// This function is applied to all <tt>aterm</tt> noded appearing in this term.
    /// \deprecated
    /// \return The substitution result.
/*    
    template <typename Substitution>
    linear_process substitute(Substitution f)
    {
      data::variable_list d = substitute(f, m_global_variables);
      data::variable_list p = substitute(f, m_process_parameters);
      summand_list       s = m_summands          .substitute(f);
      return linear_process(d, p, s);
    }
*/
    /// \brief Returns the set of free variables that appear in the process.
    /// This set is a subset of <tt>global_variables()</tt>.
    /// \return The set of free variables that appear in the process.
    std::set<data::variable> find_free_variables()
    {
      using namespace std::rel_ops; // for definition of operator!= in terms of operator==

      // TODO: the efficiency of this implementation is not optimal
      std::set<data::variable> result;
      std::set<data::variable> parameters = mcrl2::data::convert< std::set< data::variable > >(process_parameters());
      summand_list s = summands();
      for (summand_list::iterator i = s.begin(); i != s.end(); ++i)
      {
        data::assignment_list assignments(i->assignments());
        std::set<data::variable> summation_variables = mcrl2::data::detail::make_set(i->summation_variables());
        std::set<data::variable> used_variables;
        lps::find_all_free_variables(*i, std::inserter(used_variables, used_variables.end()));
        std::set<data::variable> bound_variables = mcrl2::data::detail::set_union(parameters, summation_variables);
        std::set<data::variable> free_variables = mcrl2::data::detail::set_difference(used_variables, bound_variables);
        result.swap(free_variables);
      }
      return result;
    }
  };

/// \brief Conversion to ATermAppl.
/// \return The action summand converted to ATerm format.
inline
atermpp::aterm_appl linear_process_to_aterm(const linear_process& p)
{
  return core::detail::gsMakeLinearProcess(
    p.global_variables(),
    p.process_parameters(),
    p.summands()
  );
}

/// \brief Pretty print the linear process
inline
std::string pp(const linear_process& p)
{
  return core::pp(linear_process_to_aterm(p));
}

/// \brief Returns the free variables that occur in the linear process
/// \param process A linear process
/// \return The free variables that occur in the linear process
inline
std::set<data::variable> compute_free_variables(const linear_process& process)
{
  std::set<data::variable> result;
  std::set<data::variable> process_parameters = mcrl2::data::detail::make_set(process.process_parameters());
  summand_list summands(process.summands());
  for (summand_list::iterator i = summands.begin(); i != summands.end(); ++i)
  {
    std::set<data::variable> temporary;
    lps::detail::find_all_free_variables(*i, process_parameters, std::inserter(temporary, temporary.end()));

    data::variable_vector summation_variables(data::make_variable_vector(i->summation_variables()));
    std::sort(summation_variables.begin(), summation_variables.end());

    std::set_difference(temporary.begin(), temporary.end(), summation_variables.begin(), summation_variables.end(),
                                                std::inserter(result, result.end()));
  }
  return result;
}

/// \brief Returns the action labels that occur in the process
/// \param process A linear process
/// \return The action labels that occur in the process
inline
std::set<action_label> compute_action_labels(const linear_process& process)
{
  std::set<action_label> result;
  atermpp::find_all_if(linear_process_to_aterm(process), is_action_label, std::inserter(result, result.end()));
  return result;
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_LINEAR_PROCESS_H
