// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file suminst.h
/// \brief Instantiate summation variables.

#ifndef MCRL2_LPS_SUMINST_H
#define MCRL2_LPS_SUMINST_H

#include "mcrl2/atermpp/set_operations.h"

#include "mcrl2/data/enumerator.h"

#include "mcrl2/lps/detail/lps_algorithm.h"

namespace mcrl2::lps
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
  using super = detail::lps_algorithm<Specification>;
  using enumerator_element = data::enumerator_list_element_with_substitution<>;
  using process_type = typename Specification::process_type;
  using action_summand_type = typename process_type::action_summand_type;
  using action_summand_vector_type = std::vector<action_summand_type>;
  using super::m_spec;

  protected:
    /// Sorts to be instantiated
    std::set<data::sort_expression> m_sorts;

    /// Only instantiate tau summands
    bool m_tau_summands_only;

    /// Rewriter
    DataRewriter m_rewriter;
    data::enumerator_algorithm<> m_enumerator;
    data::enumerator_identifier_generator m_id_generator;

    /// Statistiscs for verbose output
    std::size_t m_processed = 0;
    std::size_t m_deleted = 0;
    std::size_t m_added = 0;

    template <typename SummandType, typename Container>
    std::size_t instantiate_summand(const SummandType& s, Container& result)
    {
      using namespace data;
      std::size_t nr_summands = 0; // Counter for the number of new summands, used for verbose output
      std::deque< variable > variables; // The variables we need to consider in instantiating

      // partition such that variables with finite sort precede those that do not
      for (const variable& v: s.summation_variables())
      {
        if(m_sorts.find(v.sort()) != m_sorts.end())
        {
          if (m_spec.data().is_certainly_finite(v.sort()))
          {
            variables.push_front(v);
          }
          else
          {
            variables.push_back(v);
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
          mCRL2log(log::debug) << "enumerating variables " << vl << " in condition: " << data::pp(s.condition()) << std::endl;
          data::mutable_indexed_substitution<> local_sigma;
          m_enumerator.enumerate(enumerator_element(vl, s.condition()),
                                 local_sigma,
                                 [&](const enumerator_element& p)
                                 {
                                   mutable_indexed_substitution<> sigma;
                                   p.add_assignments(vl, sigma, m_rewriter);
                                   mCRL2log(log::debug) << "substitutions: " << sigma << std::endl;
                                   SummandType t(s);
                                   t.summation_variables() = new_summation_variables;
                                   lps::rewrite(t, m_rewriter, sigma);
                                   result.push_back(t);
                                   ++nr_summands;
                                   return false;
                                 },
                                 sort_bool::is_false_function_symbol
          );
        }
        catch (mcrl2::runtime_error const& e)
        {
          // If an error occurs in enumerating, remove all summands that
          // have been added to result thus far, and re-add the original.
          // This prevents problems e.g. in case of a sort without constructors.
          mCRL2log(log::debug) << "An error occurred in enumeration, removing already added summands, and keeping the original" << std::endl;
          mCRL2log(log::debug) << e.what() << std::endl;

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
          std::size_t newsummands = instantiate_summand(*i, result);
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
        m_enumerator(r, spec.data(), r, m_id_generator, false)
    {
      if(sorts.empty())
      {
        mCRL2log(log::info) << "an empty set of sorts to be unfolded was provided; defaulting to all finite sorts" << std::endl;
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

} // namespace mcrl2::lps

#endif // MCRL2_LPS_SUMINST_H

