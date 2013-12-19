// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file suminst.h
/// \brief Instantiate summation variables.

#ifndef MCRL2_LPS_SUMINST_H
#define MCRL2_LPS_SUMINST_H

#include <deque>
#include "mcrl2/atermpp/set_operations.h"
#include "mcrl2/utilities/logger.h"

#include "mcrl2/data/classic_enumerator.h"

#include "mcrl2/lps/rewrite.h"
#include "mcrl2/lps/replace.h"
#include "mcrl2/lps/detail/lps_algorithm.h"

namespace mcrl2
{
namespace lps
{

/// \brief Return a set with all finite sorts in data specification s.
inline
std::set<data::sort_expression> finite_sorts(const data::data_specification& s)
{
  data::sort_expression_vector sorts = s.sorts();
  std::set<data::sort_expression> result;

  for(data::sort_expression_vector::const_iterator i = sorts.begin(); i != sorts.end(); ++i)
  {
    if(s.is_certainly_finite(*i))
    {
      result.insert(*i);
    }
  }
  return result;
}

template<typename DataRewriter>
class suminst_algorithm: public lps::detail::lps_algorithm
{

    typedef data::classic_enumerator< data::rewriter > enumerator_type;

  protected:
    /// Sorts to be instantiated
    std::set<data::sort_expression> m_sorts;

    /// Only instantiate tau summands
    bool m_tau_summands_only;

    /// Rewriter
    DataRewriter m_rewriter;
    enumerator_type m_enumerator;

    template <typename SummandType, typename Container>
    void instantiate_summand(const SummandType& s, Container& result)
    {
      using namespace data;
      int nr_summands = 0; // Counter for the number of new summands, used for verbose output
      std::deque< variable > variables; // The variables we need to consider in instantiating

      // partition such that variables with finite sort precede those that do not
      for (atermpp::term_list_iterator< variable > i = s.summation_variables().begin();
           i != s.summation_variables().end(); ++i)
      {
        if(m_sorts.find(i->sort()) != m_sorts.end())
        {
          if (m_spec.data().is_certainly_finite(i->sort()))
          {
            variables.push_front(*i);
          }
          else
          {
            variables.push_back(*i);
          }
        }
      }

      if (variables.empty())
      {
        // Nothing to be done, return original summand
        result.push_back(s);
      }
      else
      {
        // List of variables with the instantiated variables removed (can be done upfront, which is more efficient,
        // because we only need to calculate it once.
        const variable_list vl(variables.begin(),variables.end());
        variable_list new_summation_variables = term_list_difference(s.summation_variables(), vl);

        try
        {
          mCRL2log(log::debug, "suminst") << "enumerating condition: " << data::pp(s.condition()) << std::endl;

          mcrl2::data::mutable_indexed_substitution<> local_sigma;
          for (enumerator_type::iterator i=m_enumerator.begin(vl, s.condition(), local_sigma);
                  i != m_enumerator.end(); ++i)
          {
            mutable_indexed_substitution<> sigma;
            data_expression_list::const_iterator k=i->begin();
            for(auto j=vl.begin(); j!=vl.end(); ++j, ++k)
            {
              sigma[*j]=*k;
            }
            mCRL2log(log::debug, "suminst") << "substitutions: " << data::print_substitution(sigma) << std::endl;

            SummandType t(s);
            t.summation_variables() = new_summation_variables;
            lps::rewrite(t, m_rewriter, sigma);
            result.push_back(t);
            ++nr_summands;
          }

          if (nr_summands == 0)
          {
            mCRL2log(log::verbose, "suminst") << "all valuations for the variables in the condition of this summand reduce to false; removing this summand" << std::endl;
          }
          mCRL2log(log::verbose, "suminst") << "replaced a summand with " << nr_summands << " summand" << (nr_summands == 1?"":"s") << std::endl;
        }
        catch (mcrl2::runtime_error const& e)
        {
          // If an error occurs in enumerating, remove all summands that
          // have been added to result thus far, and re-add the original.
          // This prevents problems e.g. in case of a sort without constructors.
          mCRL2log(log::debug, "suminst") << "An error occurred in enumeration, removing already added summands, and keeping the original" << std::endl;
          mCRL2log(log::debug, "suminst") << e.what() << std::endl;

          result.resize(result.size() - nr_summands);
          result.push_back(s);
        }
      }
    }

  public:
    suminst_algorithm(specification& spec,
                      DataRewriter& r,
                      std::set<data::sort_expression> sorts = std::set<data::sort_expression>(),
                      bool tau_summands_only = false)
      : lps_algorithm(spec),
        m_sorts(sorts),
        m_tau_summands_only(tau_summands_only),
        m_rewriter(r),
        m_enumerator(spec.data(),r)
    {
      if(sorts.empty())
      {
        mCRL2log(log::info, "suminst") << "an empty set of sorts to be unfolded was provided; defaulting to all finite sorts" << std::endl;
        m_sorts = finite_sorts(spec.data());
      }
    }

    void run()
    {
      action_summand_vector action_summands;
      for (action_summand_vector::iterator i = m_spec.process().action_summands().begin(); i != m_spec.process().action_summands().end(); ++i)
      {
        if (!m_tau_summands_only || i->is_tau())
        {
          instantiate_summand(*i, action_summands);
        }
        else
        {
          action_summands.push_back(*i);
        }
      }

      deadlock_summand_vector deadlock_summands;
      for (deadlock_summand_vector::iterator i = m_spec.process().deadlock_summands().begin(); i != m_spec.process().deadlock_summands().end(); ++i)
      {
        if (!m_tau_summands_only)
        {
          instantiate_summand(*i, deadlock_summands);
        }
        else
        {
          deadlock_summands.push_back(*i);
        }
      }

      m_spec.process().action_summands() = action_summands;
      m_spec.process().deadlock_summands() = deadlock_summands;
    }

}; // suminst_algorithm

} // namespace lps
} // namespace mcrl2

#endif // MCRL2_LPS_SUMINST_H

