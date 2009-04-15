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
#include "mcrl2/new_data/find.h"
#include "mcrl2/new_data/utility.h"
#include "mcrl2/new_data/detail/sequence_algorithm.h"
#include "mcrl2/new_data/detail/sorted_sequence_algorithm.h"
#include "mcrl2/lps/summand.h"
#include "mcrl2/lps/process_initializer.h"
#include "mcrl2/lps/detail/free_variables.h"

namespace mcrl2 {

namespace lps {

/// \cond INTERNAL_DOCS
struct is_non_delta_summand
{
  /// \brief Function call operator
  /// \param s A linear process summand
  /// \return True if the summand is not delta
  bool operator()(const summand& s) const
  {
    return !s.is_delta();
  }
};
/// \endcond

/// Filtered list containing non-delta summands.
typedef atermpp::filtered_list<summand_list, is_non_delta_summand> non_delta_summand_list;

class linear_process; // prototype declaration

inline
std::set<new_data::variable> compute_free_variables(const linear_process& process); // prototype declaration

///////////////////////////////////////////////////////////////////////////////
// linear_process
/// \brief linear process.
class linear_process: public atermpp::aterm_appl
{
  protected:
    /// \brief The free variables of the process
    new_data::variable_list m_free_variables;

    /// \brief The process parameters of the process
    new_data::variable_list m_process_parameters;

    /// \brief The summands of the process
    summand_list m_summands;

  public:
    /// \brief Constructor.
    linear_process()
      : atermpp::aterm_appl(mcrl2::core::detail::constructLinearProcess())
    {}

    /// \brief Constructor.
    linear_process(new_data::variable_list free_variables,
        new_data::variable_list process_parameters,
        summand_list       summands
       )
     : atermpp::aterm_appl(core::detail::gsMakeLinearProcess(
            free_variables,
            process_parameters,
            summands)),
       m_free_variables    (free_variables    ),
       m_process_parameters(process_parameters),
       m_summands          (summands          )
    { }

    /// \brief Constructor.
    /// \param lps A term
    linear_process(atermpp::aterm_appl lps)
      : atermpp::aterm_appl(lps)
    {
      assert(core::detail::check_term_LinearProcess(m_term));

      // unpack LPS(.,.,.) term
      atermpp::aterm_appl::iterator i = lps.begin();
      m_free_variables = *i++;
      m_process_parameters = *i++;
      m_summands           = *i;
    }

    /// \brief Returns the sequence of LPS summands.
    /// \return The sequence of LPS summands.
    summand_list summands() const
    {
      return m_summands;
    }

    /// \brief Returns the sequence of non delta summands.
    /// \return The sequence of non delta summands.
    non_delta_summand_list non_delta_summands() const
    {
      return non_delta_summand_list(m_summands, is_non_delta_summand());
    }

    /// \brief Returns the sequence of free variables.
    /// \return The sequence of free variables.
    new_data::variable_list const& free_variables() const
    {
      return m_free_variables;
    }

    /// \brief Returns the sequence of process parameters.
    /// \return The sequence of process parameters.
    new_data::variable_list const& process_parameters() const
    {
      return m_process_parameters;
    }

    /// \brief Returns true if time is available in at least one of the summands.
    /// \return True if time is available in at least one of the summands.
    bool has_time() const
    {
      using namespace std::rel_ops; // for definition of operator!= in terms of operator==

      for (summand_list::iterator i = summands().begin(); i != summands().end(); ++i)
      {
        if(i->has_time()) return true;
      }
      return false;
    }

    /// \brief Applies a low level substitution function to this term and returns the result.
    /// \param f A
    /// The function <tt>f</tt> must supply the method <tt>aterm operator()(aterm)</tt>.
    /// This function is applied to all <tt>aterm</tt> noded appearing in this term.
    /// \deprecated
    /// \return The substitution result.
    template <typename Substitution>
    linear_process substitute(Substitution f)
    {
      new_data::variable_list d = substitute(f, m_free_variables);
      new_data::variable_list p = substitute(f, m_process_parameters);
      summand_list       s = m_summands          .substitute(f);
      return linear_process(d, p, s);
    }

    /// \brief Returns the set of free variables that appear in the process.
    /// This set is a subset of <tt>free_variables()</tt>.
    /// \return The set of free variables that appear in the process.
    std::set<new_data::variable> find_free_variables()
    {
      using namespace std::rel_ops; // for definition of operator!= in terms of operator==

      // TODO: the efficiency of this implementation is not optimal
      std::set<new_data::variable> result;
      std::set<new_data::variable> parameters = mcrl2::new_data::detail::make_set(process_parameters());
      for (summand_list::iterator i = m_summands.begin(); i != m_summands.end(); ++i)
      {
        new_data::assignment_list assignments(i->assignments());
        std::set<new_data::variable> summation_variables = mcrl2::new_data::detail::make_set(i->summation_variables());
        std::set<new_data::variable> used_variables = new_data::find_all_variables(atermpp::make_list(i->condition(), i->actions(), i->time(),
           atermpp::term_list< new_data::variable >(assignments.begin(), assignments.end())));
        std::set<new_data::variable> bound_variables = mcrl2::new_data::detail::set_union(parameters, summation_variables);
        std::set<new_data::variable> free_variables = mcrl2::new_data::detail::set_difference(used_variables, bound_variables);
        result.insert(free_variables.begin(), free_variables.end());
      }
      return result;
    }

    /// \brief Checks if the linear process is well typed
    /// \return True if
    /// <ul>
    /// <li>the free variables occurring in the process are declared in free_variables()</li>
    /// <li>the process parameters have unique names</li>
    /// <li>the free variables have unique names</li>
    /// <li>process parameters and summation variables have different names</li>
    /// <li>the left hand sides of the assignments of summands are contained in the process parameters</li>
    /// <li>the summands are well typed</li>
    /// </ul>
    bool is_well_typed() const
    {
      using namespace std::rel_ops; // for definition of operator!= in terms of operator==

      // check 1)
      std::set<new_data::variable> declared_free_variables  = mcrl2::new_data::detail::make_set(free_variables());
      std::set<new_data::variable> occurring_free_variables = compute_free_variables(*this);
      if (!(std::includes(declared_free_variables.begin(),
                          declared_free_variables.end(),
                          occurring_free_variables.begin(),
                          occurring_free_variables.end()
                         )
          ))
      {
        std::cerr << "linear_process::is_well_typed() failed: some of the free variables were not declared\n";
        std::cerr << "declared free variables: ";
        for (std::set<new_data::variable>::const_iterator i = declared_free_variables.begin(); i != declared_free_variables.end(); ++i)
        {
          std::cerr << mcrl2::core::pp(*i) << " ";
        }
        std::cerr << "\noccurring free variables: ";
        for (std::set<new_data::variable>::const_iterator i = occurring_free_variables.begin(); i != occurring_free_variables.end(); ++i)
        {
          std::cerr << mcrl2::core::pp(*i) << " ";
        }
        std::cerr << std::endl;
        return false;
      }

      // check 2)
      if (!mcrl2::new_data::detail::unique_names(m_process_parameters))
      {
        std::cerr << "linear_process::is_well_typed() failed: process parameters " << new_data::pp(m_process_parameters) << " don't have unique names." << std::endl;
        return false;
      }

      // check 3)
      if (!mcrl2::new_data::detail::unique_names(m_free_variables))
      {
        std::cerr << "linear_process::is_well_typed() failed: free variables " << new_data::pp(m_process_parameters) << " don't have unique names." << std::endl;
        return false;
      }

      // check 4)
      std::set<core::identifier_string> names;
      for (new_data::variable_list::const_iterator i = m_process_parameters.begin(); i != m_process_parameters.end(); ++i)
      {
        names.insert(i->name());
      }
      for (summand_list::iterator i = m_summands.begin(); i != m_summands.end(); ++i)
      {
        if (!mcrl2::new_data::detail::check_variable_names(i->summation_variables(), names))
        {
          std::cerr << "linear_process::is_well_typed() failed: some of the names of the summation variables " << new_data::pp(i->summation_variables()) << " also appear as process parameters." << std::endl;
          return false;
        }
      }

      // check 5)
      for (summand_list::iterator i = m_summands.begin(); i != m_summands.end(); ++i)
      {
        if (!mcrl2::new_data::detail::check_assignment_variables(i->assignments(), m_process_parameters))
        {
          std::cerr << "linear_process::is_well_typed() failed: some left hand sides of the assignments " << new_data::pp(i->assignments()) << " do not appear as process parameters." << std::endl;
          return false;
        }
      }

      // check 6)
      for (summand_list::iterator i = m_summands.begin(); i != m_summands.end(); ++i)
      {
        if (!i->is_well_typed())
          return false;
      }

      return true;
    }
  };

/// \brief Returns the free variables that occur in the specification
/// \param process A linear process
/// \return The free variables that occur in the specification
inline
std::set<new_data::variable> compute_free_variables(const linear_process& process)
{
  std::set<new_data::variable> result;
  std::set<new_data::variable> process_parameters = mcrl2::new_data::detail::make_set(process.process_parameters());
  summand_list summands(process.summands());
  for (summand_list::iterator i = summands.begin(); i != summands.end(); ++i)
  {
    std::set<new_data::variable> temporary;
    lps::detail::collect_free_variables(*i, process_parameters, std::inserter(temporary, temporary.end()));

    new_data::variable_vector summation_variables(new_data::make_variable_vector(i->summation_variables()));
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
  atermpp::find_all_if(process, is_action_label, std::inserter(result, result.end()));
  return result;
}

/// \brief Sets the free variables of l and returns the result
/// \param l A linear process
/// \param free_variables A sequence of data variables
/// \return The modified linear process
inline
linear_process set_free_variables(linear_process l, new_data::variable_list free_variables)
{
  return linear_process(free_variables,
             l.process_parameters(),
             l.summands          ()
            );
}

/// \brief Sets the process parameters of l and returns the result
/// \param l A linear process
/// \param process_parameters A sequence of data variables
/// \return The modified linear process
inline
linear_process set_process_parameters(linear_process l, new_data::variable_list process_parameters)
{
  return linear_process(l.free_variables    (),
             process_parameters,
             l.summands          ()
            );
}

/// \brief Sets the summands of l and returns the result
/// \param l A linear process
/// \param summands A sequence of summands
/// \return The modified linear process
inline
linear_process set_summands(linear_process l, summand_list summands)
{
  return linear_process(l.free_variables    (),
             l.process_parameters(),
             summands
            );
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_LINEAR_PROCESS_H
