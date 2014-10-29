// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/constelm.h
/// \brief Add your file description here.

#ifndef MCRL2_LPS_CONSTELM_H
#define MCRL2_LPS_CONSTELM_H

#include <algorithm>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/remove.h"
#include "mcrl2/lps/rewrite.h"
#include "mcrl2/lps/detail/lps_algorithm.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2
{

namespace lps
{

/// \brief Algorithm class for elimination of constant parameters
template <typename DataRewriter, typename Specification = specification>
class constelm_algorithm: public lps::detail::lps_algorithm<Specification>
{
  typedef typename lps::detail::lps_algorithm<Specification> super;

  protected:
    /// \brief If true, then the algorithm is allowed to instantiate free variables
    /// as a side effect.
    bool m_instantiate_global_variables;

    /// \brief If true, conditions are not evaluated and assumed to be true.
    bool m_ignore_conditions;

    /// \brief Maps process parameters to their index.
    std::map<data::variable, size_t> m_index_of;

    /// \brief The rewriter used by the constelm algorithm.
    const DataRewriter& R;

    void LOG_CONSTANT_PARAMETERS(const data::mutable_map_substitution<>& sigma, const std::string& msg = "")
    {
      if (mCRL2logEnabled(log::verbose))
      {
        mCRL2log(log::verbose) << msg;
        for (auto i = sigma.begin(); i != sigma.end(); ++i)
        {
          mCRL2log(log::verbose) << data::pp(i->first) << " := " << data::pp(i->second) << std::endl;
        }
      }
    }

    void LOG_PARAMETER_CHANGE(const data::data_expression& d_j,
                              const data::data_expression& Rd_j,
                              const data::data_expression& Rg_ij,
                              const data::mutable_map_substitution<>& sigma,
                              const std::string& msg = ""
                             )
    {
      if (mCRL2logEnabled(log::debug))
      {
        mCRL2log(log::debug) << msg
                        << data::pp(d_j) << "\n"
                        << "      value before: " << Rd_j << "\n"
                        << "      value after:  " << Rg_ij << "\n"
                        << "      replacements: " << sigma << std::endl;
      }
    }

    void LOG_CONDITION(const data::data_expression& cond,
                       const data::data_expression& c_i,
                       const data::mutable_map_substitution<>& sigma,
                       const std::string& msg = ""
                      )

    {
      if (mCRL2logEnabled(log::debug))
      {
        mCRL2log(log::debug) << msg
                        << cond
                        << sigma
                        << " -> "
                        << c_i << std::endl;
      }
    }

  public:

    /// \brief Constructor
    constelm_algorithm(Specification& spec, const DataRewriter& R_)
      : lps::detail::lps_algorithm<Specification>(spec),
        m_instantiate_global_variables(false),
        m_ignore_conditions(false),
        R(R_)
    {}

    /// \brief Runs the constelm algorithm
    /// \param instantiate_global_variables If true, the algorithm is allowed to instantiate free variables
    /// as a side effect
    /// \param ignore_conditions If true, the algorithm is allowed to ignore the conditions in the LPS.
    void run(bool instantiate_global_variables = false, bool ignore_conditions = false)
    {
      using utilities::detail::contains;

      m_instantiate_global_variables = instantiate_global_variables;
      m_ignore_conditions = ignore_conditions;
      data::data_expression_list vl = super::m_spec.initial_process().state(super::m_spec.process().process_parameters());
      data::data_expression_vector r(vl.begin(), vl.end());

      // essential: rewrite the initial state vector r to normal form. Essential
      // because this value is used in W below, and assigned to the right hand side of a substitution, which
      // must be a normal form.
      lps::rewrite(r, R);

      auto& p = super::m_spec.process();
      const std::set<data::variable>& global_variables = super::m_spec.global_variables();
      data::variable_list V(global_variables.begin(), global_variables.end());
      const data::variable_list& d = p.process_parameters();

      // initialize m_index_of
      unsigned index = 0;
      for (auto i = d.begin(); i != d.end(); ++i)
      {
        m_index_of[*i] = index++;
      }

      // sigma contains substitutions of free variables and process parameters
      data::mutable_map_substitution<> sigma;

      std::set<data::variable> G(d.begin(), d.end());
      std::set<data::variable> dG;
      const data::assignment_list& assignments = super::m_spec.initial_process().assignments();
      for (auto i = assignments.begin(); i != assignments.end(); ++i)
      {
        // The rewriter requires that the rhs's of a substitution are in normal form.
        sigma[i->lhs()] = R(i->rhs());
      }

      // undo contains undo information of instantiations of free variables
      std::map<data::variable, std::set<data::variable> > undo;

      do
      {
        dG.clear();
        for (auto i = p.action_summands().begin(); i != p.action_summands().end(); ++i)
        {
          const action_summand& s = *i;
          const data::data_expression& c_i = s.condition();
          if (m_ignore_conditions || (R(c_i, sigma) != data::sort_bool::false_()))
          {
            for (auto j = G.begin(); j != G.end(); ++j)
            {
              if (dG.find(*j) != dG.end())
              {
                continue;
              }
              size_t index_j = m_index_of[*j];
              const data::variable& d_j = *j;
              data::data_expression g_ij = super::next_state(s, d_j);

              if (R(g_ij, sigma) != R(d_j, sigma))
              {
                LOG_PARAMETER_CHANGE(d_j, R(d_j, sigma), R(g_ij, sigma), sigma, "POSSIBLE CHANGE FOR PARAMETER ");
                data::data_expression z = R(g_ij, sigma);
                if (is_variable(z) && contains(V, atermpp::down_cast<data::variable>(z)))
                {
                  sigma[atermpp::down_cast<data::variable>(z)] = r[index_j];
                  undo[d_j].insert(atermpp::down_cast<data::variable>(z));
                }
                else
                {
                  dG.insert(d_j);
                  sigma[d_j] = d_j; // erase d_j
                  std::set<data::variable>& var = undo[d_j];
                  for (auto w = var.begin(); w != var.end(); ++w)
                  {
                    sigma[*w] = *w; // erase *w
                  }
                  undo[d_j].clear();
                }
              }
              else
              {
                LOG_PARAMETER_CHANGE(d_j, R(d_j, sigma), R(g_ij, sigma), sigma, "NO CHANGE FOR PARAMETER ");
              }
            }
          }
          else
          {
            LOG_CONDITION(i->condition(), R(c_i, sigma), sigma, "CONDITION IS FALSE: ");
          }
        }
        for (auto k = dG.begin(); k != dG.end(); ++k)
        {
          G.erase(*k);
        }
      }
      while (!dG.empty());

      LOG_CONSTANT_PARAMETERS(sigma, "Removing the following constant parameters:\n");

      // N.B. The order of removing constant parameters and rewriting has been reversed
      // as requested by Jan Friso Groote. This may lead to some gain in performance (13%
      // in the case of 6x6 othello). Note that due to this change the intermediate result
      // after removing parameters may not be a valid LPS.

      // remove the constant parameters from the specification spec
      std::set<data::variable> constant_parameters;
      for (auto i = sigma.begin(); i != sigma.end(); ++i)
      {
        constant_parameters.insert(i->first);
      }
      lps::remove_parameters(super::m_spec, constant_parameters);

      // rewrite the specification with substitution sigma
      lps::rewrite(super::m_spec, R, sigma);
    }
};

/// \brief Removes zero or more constant parameters from the specification spec.
/// \param spec A linear process specification
/// \param R A data rewriter
/// \param instantiate_global_variables If true, free variables may be instantiated as a side effect of the algorithm
template <typename DataRewriter, typename Specification = specification>
void constelm(Specification& spec, const DataRewriter& R, bool instantiate_global_variables = false)
{
  constelm_algorithm<DataRewriter, Specification> algorithm(spec, R);
  algorithm.run(instantiate_global_variables);
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_CONSTELM_H
