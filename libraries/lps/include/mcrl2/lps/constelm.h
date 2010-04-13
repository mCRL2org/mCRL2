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
#include "mcrl2/data/replace.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/atermpp/convert.h"
#include "mcrl2/data/substitution.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/remove.h"
#include "mcrl2/lps/rewrite.h"
#include "mcrl2/lps/detail/lps_algorithm.h"

namespace mcrl2 {

namespace lps {

struct default_global_variable_solver
{
  /// \brief Attempts to find a valuation for global variables that makes the condition
  /// !R(c, sigma) or (R(e, sigma) = R(g, sigma)) true.
  template <typename DataRewriter, typename Substitution>
  data::mutable_map_substitution<> solve(const data::variable_list& V,
                     const data::data_expression& /* c */,
                     const data::data_expression& g,
                     const data::variable& /* d */,
                     const data::data_expression& e,
                     const DataRewriter& R,
                     const Substitution& sigma
                    )
    {
      data::mutable_map_substitution<> result;
      data::data_expression r = R(g, sigma);
      if (is_variable(r))
      {
        data::variable v = r;
        if (std::find(V.begin(), V.end(), v) != V.end())
        {
          result[v] = e;
        }
      }
      return result;
    }
};

/// \brief Algorithm class for elimination of constant parameters
// TODO: add default template argument for free variable solver
template <typename DataRewriter>
class constelm_algorithm: public lps::detail::lps_algorithm
{
  protected:
    /// \brief If true, then the algorithm is allowed to instantiate free variables
    /// as a side effect.
    bool m_instantiate_global_variables;

    /// \brief If true, conditions are not evaluated and assumed to be true.
    bool m_ignore_conditions;

    /// \brief Maps process parameters to their index.
    std::map<data::variable, unsigned int> m_index_of;

    /// \brief The rewriter used by the constelm algorithm.
    const DataRewriter& R;

    void LOG_CONSTANT_PARAMETERS(unsigned int level,
                                 const data::mutable_map_substitution<>& sigma,
                                 const std::string& msg = "")
    {
      if (check_log_level(level))
      {
        std::clog << msg;
        for (data::mutable_map_substitution<>::const_iterator i = sigma.begin(); i != sigma.end(); ++i)
        {
          std::clog << data::pp(i->first) << " := " << data::pp(i->second) << std::endl;
        }
      }
    }

    void LOG_PARAMETER_CHANGE(unsigned int level,
                              const data::data_expression& d_j,
                              const data::data_expression& Rd_j,
                              const data::data_expression& Rg_ij,
                              const data::mutable_map_substitution<>& sigma,
                              const std::string& msg = ""
                             )
    {
      if (check_log_level(level))
      {
        std::clog << msg
                  << pp(d_j) << "\n"
                  << "      value before: " << pp(Rd_j) << "\n"
                  << "      value after:  " << pp(Rg_ij) << "\n"
                  << "      replacements: " << data::to_string(sigma) << std::endl;
      }
    }

    void LOG_CONDITION(unsigned int level,
                       const data::data_expression& cond,
                       const data::data_expression& c_i,
                       const data::mutable_map_substitution<>& sigma,
                       const std::string& msg = ""
                      )
                       
    {
      if (check_log_level(level))
      {
        std::clog << msg
                  << pp(cond)
                  << data::to_string(sigma)
                  << " -> "
                  << pp(c_i) << std::endl;
      }
    }

  public:

    /// \brief Constructor
    constelm_algorithm(specification& spec, const DataRewriter& R_, bool verbose = false)
      : 
        lps::detail::lps_algorithm(spec, verbose),
        m_instantiate_global_variables(false),
        m_ignore_conditions(false),
        R(R_)
    {}

    /// \brief Constructor
    constelm_algorithm(specification& spec, const DataRewriter& R_, unsigned int loglevel)
      : 
        lps::detail::lps_algorithm(spec, loglevel),
        m_instantiate_global_variables(false),
        m_ignore_conditions(false),
        R(R_)
    {}

    /// \brief Runs the constelm algorithm
    /// \param p A linear process
    /// \param e An initial value for the linear process p
    /// \param R A data rewriter
    /// \param instantiate_global_variables If true, the algorithm is allowed to instantiate free variables
    /// as a side effect
    void run(bool instantiate_global_variables = false, bool ignore_conditions = false)
    { 
      m_instantiate_global_variables = instantiate_global_variables;
      m_ignore_conditions = ignore_conditions;
      data::data_expression_vector e = atermpp::convert<data::data_expression_vector>(m_spec.initial_process().state(m_spec.process().process_parameters()));

      // optimization: rewrite the initial state vector e
      lps::rewrite(e, R);
 
      linear_process& p = m_spec.process();
      data::variable_list V = atermpp::convert<data::variable_list>(m_spec.global_variables());
      const data::variable_list& d = p.process_parameters();

      // initialize m_index_of
      unsigned index = 0;
      for (data::variable_list::const_iterator i = d.begin(); i != d.end(); ++i)
      {
        m_index_of[*i] = index++;
      }

      // sigma contains substitutions of free variables and process parameters
      data::mutable_map_substitution<> sigma;
      data::data_expression_vector::iterator e_i = e.begin();

      std::set<data::variable> G(d.begin(), d.end());
      std::set<data::variable> dG;
      const data::assignment_list assignments=m_spec.initial_process().assignments();
      for(data::assignment_list::const_iterator i = assignments.begin();
                   i!=assignments.end(); ++i)
      { sigma[i->lhs()] = i->rhs();
      }
      
      // undo contains undo information of instantiations of free variables
      std::map<data::variable, std::set<data::variable> > undo;

      do
      {
        dG.clear();
        for (action_summand_vector::iterator i = p.action_summands().begin(); i != p.action_summands().end(); ++i)
        {
          const action_summand& s = *i;
          const data::data_expression& c_i = s.condition();
          if (m_ignore_conditions || (R(c_i, sigma) != data::sort_bool::false_()))
          {
            for (std::set<data::variable>::iterator j = G.begin(); j != G.end(); ++j)
            {
              if (dG.find(*j) != dG.end())
              {
                continue;
              }
              unsigned int index_j = m_index_of[*j];
              const data::variable& d_j = *j;
              data::data_expression g_ij = next_state(s, d_j);

              if (R(g_ij, sigma) != R(d_j, sigma))
              {
                LOG_PARAMETER_CHANGE(2, d_j, R(d_j, sigma), R(g_ij, sigma), sigma, "POSSIBLE CHANGE FOR PARAMETER ");
                data::mutable_map_substitution<> W = default_global_variable_solver().solve(V, c_i, g_ij, d_j, e[index_j], R, sigma);
                if (!W.empty())
                {
                  for (data::mutable_map_substitution<>::const_iterator w = W.begin(); w != W.end(); ++w)
                  {
                    sigma[w->first] = w->second;
                    undo[d_j].insert(w->first);
                  }
                }
                else
                {
                  dG.insert(d_j);
                  sigma[d_j] = d_j; // erase d_j
                  std::set<data::variable>& var = undo[d_j];
                  for (std::set<data::variable>::iterator w = var.begin(); w != var.end(); ++w)
                  {
                    sigma[*w] = *w; // erase *w
                  }
                  undo[d_j].clear();
                }
              }
              else
              {
                LOG_PARAMETER_CHANGE(2, d_j, R(d_j, sigma), R(g_ij, sigma), sigma, "NO CHANGE FOR PARAMETER ");
              }
            }
          }
          else
          {
            LOG_CONDITION(2, i->condition(), R(c_i, sigma), sigma, "CONDITION IS FALSE: ");
          }
        }
        for (std::set<data::variable>::iterator k = dG.begin(); k != dG.end(); ++k)
        {
          G.erase(*k);
        }
      } while (!dG.empty());

      LOG_CONSTANT_PARAMETERS(1, sigma, "Removing the following constant parameters:\n");

      // N.B. The order of removing constant parameters and rewriting has been reversed
      // as requested by Jan Friso Groote. This may lead to some gain in performance (13%
      // in the case of 6x6 othello). Note that due to this change the intermediate result
      // after removing parameters may not be a valid LPS.

      // remove the constant parameters from the specification spec
      std::set<data::variable> constant_parameters;
      for (data::mutable_map_substitution<>::iterator i = sigma.begin(); i != sigma.end(); ++i)
      {
        constant_parameters.insert(i->first);
      }
      lps::remove_parameters(m_spec, constant_parameters);

      // rewrite the specification with substitution sigma
      lps::rewrite(m_spec, R, sigma);
    }
};

/// \brief Removes zero or more constant parameters from the specification spec.
/// \param spec A linear process specification
/// \param R A data rewriter
/// \param verbose If true, verbose output is generated
/// \param instantiate_global_variables If true, free variables may be instantiated as a side effect of the algorithm
template <typename DataRewriter>
void constelm(specification& spec, const DataRewriter& R, bool verbose = false, bool instantiate_global_variables = false)
{
  constelm_algorithm<DataRewriter> algorithm(spec, R, verbose);
  algorithm.run(instantiate_global_variables);
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_CONSTELM_H
