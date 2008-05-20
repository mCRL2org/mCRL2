// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
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
#include "mcrl2/lps/summand.h"
#include "mcrl2/lps/process_initializer.h"
#include "mcrl2/data/detail/sequence_algorithm.h"
#include "mcrl2/data/detail/sorted_sequence_algorithm.h"
#include "mcrl2/lps/detail/free_variables.h"

namespace mcrl2 {

namespace lps {

using atermpp::aterm_appl;
using atermpp::read_from_named_file;

/// \cond INTERNAL_DOCS
struct is_non_delta_summand
{
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
std::set<data::data_variable> compute_free_variables(const linear_process& process); // prototype declaration

///////////////////////////////////////////////////////////////////////////////
// linear_process
/// \brief linear process.
///
class linear_process: public aterm_appl
{
  protected:
    data::data_variable_list m_free_variables;
    data::data_variable_list m_process_parameters;
    summand_list       m_summands;

  public:
    /// Constructor.
    ///
    linear_process()
      : aterm_appl(mcrl2::core::detail::constructLinearProcess())
    {}

    /// Constructor.
    ///
    linear_process(data::data_variable_list free_variables,
        data::data_variable_list process_parameters,
        summand_list       summands
       )
     : aterm_appl(core::detail::gsMakeLinearProcess(free_variables, process_parameters, summands)),
       m_free_variables    (free_variables    ),
       m_process_parameters(process_parameters),
       m_summands          (summands          )
    { }

    /// Constructor.
    ///
    linear_process(aterm_appl lps)
      : aterm_appl(lps)
    {
      assert(core::detail::check_term_LinearProcess(m_term));

      // unpack LPS(.,.,.) term
      aterm_appl::iterator i = lps.begin();
      m_free_variables     = data::data_variable_list(*i++);
      m_process_parameters = data::data_variable_list(*i++);
      m_summands           = summand_list(*i);
    }

    /// Returns the sequence of LPS summands.
    ///
    summand_list summands() const
    {
      return m_summands;
    }

    /// Returns the sequence of non delta summands.
    ///
    non_delta_summand_list non_delta_summands() const
    {
      return non_delta_summand_list(m_summands, is_non_delta_summand());
    }

    /// Returns the sequence of free variables.
    ///
    data::data_variable_list free_variables() const
    {
      return m_free_variables;
    }

    /// Returns the sequence of process parameters.
    ///
    data::data_variable_list process_parameters() const
    {
      return m_process_parameters;
    }

    /// Returns true if time is available in at least one of the summands.
    ///
    bool has_time() const
    {
      using namespace std::rel_ops; // for definition of operator!= in terms of operator==

      for (summand_list::iterator i = summands().begin(); i != summands().end(); ++i)
      {
        if(i->has_time()) return true;
      }
      return false;
    }

    /// Applies a substitution to this LPS and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    linear_process substitute(Substitution f)
    {
      data::data_variable_list d = m_free_variables    .substitute(f);
      data::data_variable_list p = m_process_parameters.substitute(f);
      summand_list       s = m_summands          .substitute(f);
      return linear_process(d, p, s);
    }

    /// Returns the set of free variables that appear in the process.
    /// This set is a subset of <tt>free_variables()</tt>.
    ///
    std::set<data::data_variable> find_free_variables()
    {
      using namespace std::rel_ops; // for definition of operator!= in terms of operator==

      // TODO: the efficiency of this implementation is not optimal
      std::set<data::data_variable> result;
      std::set<data::data_variable> parameters = mcrl2::data::detail::make_set(process_parameters());
      for (summand_list::iterator i = m_summands.begin(); i != m_summands.end(); ++i)
      {
        std::set<data::data_variable> summation_variables = mcrl2::data::detail::make_set(i->summation_variables());
        std::set<data::data_variable> used_variables = data::find_all_data_variables(make_list(i->condition(), i->actions(), i->time(), i->assignments()));
        std::set<data::data_variable> bound_variables = mcrl2::data::detail::set_union(parameters, summation_variables);
        std::set<data::data_variable> free_variables = mcrl2::data::detail::set_difference(used_variables, bound_variables);
        result.insert(free_variables.begin(), free_variables.end());
      }
      return result;
    }

    /// Returns true if
    /// <ul>
    /// <li>the free variables occurring in the process are declared in free_variables()</li>
    /// <li>the process parameters have unique names</li>
    /// <li>the free variables have unique names</li>
    /// <li>process parameters and summation variables have different names</li>
    /// <li>the left hand sides of the assignments of summands are contained in the process parameters</li>
    ///
    /// <li>the summands are well typed</li>
    /// </ul>
    ///
    bool is_well_typed() const
    {
      using namespace std::rel_ops; // for definition of operator!= in terms of operator==

      // check 1)
      std::set<data::data_variable> declared_free_variables  = mcrl2::data::detail::make_set(free_variables());
      std::set<data::data_variable> occurring_free_variables = compute_free_variables(*this);
      if (!(std::includes(declared_free_variables.begin(),
                          declared_free_variables.end(),
                          occurring_free_variables.begin(),
                          occurring_free_variables.end()
                         )
          ))
      {
        std::cerr << "linear_process::is_well_typed() failed: some of the free variables were not declared\n";
        std::cerr << "declared free variables: ";
        for (std::set<data::data_variable>::iterator i = declared_free_variables.begin(); i != declared_free_variables.end(); ++i)
        {
          std::cerr << pp(*i) << " ";
        }
        std::cerr << "\noccurring free variables: ";
        for (std::set<data::data_variable>::iterator i = occurring_free_variables.begin(); i != occurring_free_variables.end(); ++i)
        {
          std::cerr << pp(*i) << " ";
        }
        std::cerr << std::endl;
        return false;
      }

      // check 2)
      if (!mcrl2::data::detail::unique_names(m_process_parameters))
      {
        std::cerr << "linear_process::is_well_typed() failed: process parameters " << pp(m_process_parameters) << " don't have unique names." << std::endl;
        return false;
      }

      // check 3)
      if (!mcrl2::data::detail::unique_names(m_free_variables))
      {
        std::cerr << "linear_process::is_well_typed() failed: free variables " << pp(m_process_parameters) << " don't have unique names." << std::endl;
        return false;
      }

      // check 4)
      std::set<core::identifier_string> names;
      for (data::data_variable_list::iterator i = m_process_parameters.begin(); i != m_process_parameters.end(); ++i)
      {
        names.insert(i->name());
      }
      for (summand_list::iterator i = m_summands.begin(); i != m_summands.end(); ++i)
      {
        if (!mcrl2::data::detail::check_variable_names(i->summation_variables(), names))
        {
          std::cerr << "linear_process::is_well_typed() failed: some of the names of the summation variables " << pp(i->summation_variables()) << " also appear as process parameters." << std::endl;
          return false;
        }
      }

      // check 5)
      for (summand_list::iterator i = m_summands.begin(); i != m_summands.end(); ++i)
      {
        if (!mcrl2::data::detail::check_assignment_variables(i->assignments(), m_process_parameters))
        {
          std::cerr << "linear_process::is_well_typed() failed: some left hand sides of the assignments " << pp(i->assignments()) << " do not appear as process parameters." << std::endl;
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

/// \brief Computes the free variables that occur in the specification
inline
std::set<data::data_variable> compute_free_variables(const linear_process& process)
{
  std::set<data::data_variable> result;
  std::set<data::data_variable> process_parameters = mcrl2::data::detail::make_set(process.process_parameters());
  for (summand_list::iterator i = process.summands().begin(); i != process.summands().end(); ++i)
  {
    lps::detail::collect_free_variables(*i, process_parameters, std::inserter(result, result.end()));
  }
  return result;
}

/// \brief Computes the action labels that occur in the process
inline
std::set<action_label> compute_action_labels(const linear_process& process)
{
  std::set<action_label> result;
  atermpp::find_all_if(process, is_action_label, std::inserter(result, result.end()));
  return result;
}

/// \brief Sets the free variables of l and returns the result
inline
linear_process set_free_variables(linear_process l, data::data_variable_list free_variables)
{
  return linear_process(free_variables,
             l.process_parameters(),
             l.summands          ()
            );
}

/// \brief Sets the process parameters of l and returns the result
inline
linear_process set_process_parameters(linear_process l, data::data_variable_list process_parameters)
{
  return linear_process(l.free_variables    (),
             process_parameters,
             l.summands          ()
            );
}

/// \brief Sets the summands of l and returns the result
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

/// \cond INTERNAL_DOCS
namespace atermpp
{
using mcrl2::lps::linear_process;

template<>
struct aterm_traits<linear_process>
{
  typedef ATermAppl aterm_type;
  static void protect(linear_process t)   { t.protect(); }
  static void unprotect(linear_process t) { t.unprotect(); }
  static void mark(linear_process t)      { t.mark(); }
  static ATerm term(linear_process t)     { return t.term(); }
  static ATerm* ptr(linear_process& t)    { return &t.term(); }
};

} // namespace atermpp
/// \endcond

#endif // MCRL2_LPS_LINEAR_PROCESS_H
