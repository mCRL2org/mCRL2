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

#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/substitutions/mutable_indexed_substitution.h"

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
  const std::set<data::sort_expression>& sorts = s.sorts();
  std::set<data::sort_expression> result;

  for(const data::sort_expression& sort : sorts)
  {
    if(s.is_certainly_finite(sort))
    {
      result.insert(sort);
    }
  }
  return result;
}

template<typename DataRewriter, typename Specification>
class suminst_algorithm: public detail::lps_algorithm<Specification>
{
  typedef detail::lps_algorithm<Specification> super;
  typedef data::enumerator_list_element_with_substitution<> enumerator_element;
  typedef data::enumerator_algorithm_with_iterator<> enumerator_type;
  typedef typename Specification::process_type process_type;
  typedef typename process_type::action_summand_type action_summand_type;
  typedef std::vector<action_summand_type> action_summand_vector_type;
  using super::m_spec;

  protected:
    /// Sorts to be instantiated
    std::set<data::sort_expression> m_sorts;

    /// Only instantiate tau summands
    bool m_tau_summands_only;

    /// Rewriter
    DataRewriter m_rewriter;
    enumerator_type m_enumerator;
    data::enumerator_identifier_generator m_id_generator;

    /// Statistiscs for verbose output
    size_t m_processed;
    size_t m_deleted;
    size_t m_added;

    template <typename SummandType, typename Container>
    size_t instantiate_summand(const SummandType& s, Container& result)
    {
      using namespace data;
      size_t nr_summands = 0; // Counter for the number of new summands, used for verbose output
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
        nr_summands = 1;
      }
      else
      {
        // List of variables with the instantiated variables removed (can be done upfront, which is more efficient,
        // because we only need to calculate it once.
        const variable_list vl(variables.begin(),variables.end());
        variable_list new_summation_variables = term_list_difference(s.summation_variables(), vl);

        try
        {
          mCRL2log(log::debug, "suminst") << "enumerating variables " << vl << " in condition: " << data::pp(s.condition()) << std::endl;

          mcrl2::data::mutable_indexed_substitution<> local_sigma;
          std::deque <enumerator_element> enumerator_deque(1, enumerator_element(vl, s.condition()));
          for (auto i = m_enumerator.begin(local_sigma, enumerator_deque); i != m_enumerator.end(); ++i)
          {
            mutable_indexed_substitution<> sigma;
            i->add_assignments(vl,sigma,m_rewriter);
            /* data_expression_list::const_iterator k=i->begin();
            for(auto j=vl.begin(); j!=vl.end(); ++j, ++k)
            {
              sigma[*j]=*k;
            } */
            mCRL2log(log::debug, "suminst") << "substitutions: " << sigma << std::endl;

            SummandType t(s);
            t.summation_variables() = new_summation_variables;
            lps::rewrite(t, m_rewriter, sigma);
            result.push_back(t);
            ++nr_summands;
          }
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
          nr_summands = 1;
        }
      }
      return nr_summands;
    }

    bool must_instantiate(const action_summand_type& summand)
    {
      return !m_tau_summands_only || summand.is_tau();
    }

    bool must_instantiate(const deadlock_summand& )
    {
      return !m_tau_summands_only;
    }

    template <typename SummandListType, typename Container>
    void run(const SummandListType& list, Container& result)
    {
      for (typename SummandListType::const_iterator i = list.begin(); i != list.end(); ++i)
      {
        if (must_instantiate(*i))
        {
          size_t newsummands = instantiate_summand(*i, result);
          if (newsummands > 0)
          {
            m_added += newsummands - 1;
          }
          else
          {
            ++m_deleted;
          }
        }
        else
        {
          result.push_back(*i);
        }
        ++m_processed;
        mCRL2log(log::status) << "Replaced " << m_processed << " summands by " << (m_processed + m_added - m_deleted)
                              << " summands (" << m_deleted << " were deleted)" << std::endl;
      }
    }

  public:
    suminst_algorithm(Specification& spec,
                      DataRewriter& r,
                      std::set<data::sort_expression> sorts = std::set<data::sort_expression>(),
                      bool tau_summands_only = false)
      : detail::lps_algorithm<Specification>(spec),
        m_sorts(sorts),
        m_tau_summands_only(tau_summands_only),
        m_rewriter(r),
        m_enumerator(r, spec.data(), r, m_id_generator),
        m_processed(0),
        m_deleted(0),
        m_added(0)
    {
      if(sorts.empty())
      {
        mCRL2log(log::info, "suminst") << "an empty set of sorts to be unfolded was provided; defaulting to all finite sorts" << std::endl;
        m_sorts = finite_sorts(spec.data());
      }
    }

    void run()
    {
      action_summand_vector_type action_summands;
      deadlock_summand_vector deadlock_summands;
      m_added = 0;
      m_deleted = 0;
      m_processed = 0;
      run(m_spec.process().action_summands(), action_summands);
      run(m_spec.process().deadlock_summands(), deadlock_summands);
      m_spec.process().action_summands().swap(action_summands);
      m_spec.process().deadlock_summands().swap(deadlock_summands);
      mCRL2log(log::status) << std::endl;
    }

}; // suminst_algorithm

} // namespace lps
} // namespace mcrl2

#endif // MCRL2_LPS_SUMINST_H

