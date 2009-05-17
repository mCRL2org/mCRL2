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
#include "mcrl2/data/data.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/substitution.h"
#include "mcrl2/data/map_substitution_adapter.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/lps_algorithm.h"
#include "mcrl2/lps/detail/remove_parameters.h"

namespace mcrl2 {

namespace lps {

/// \brief Returns zero or more constant process parameters of the process p with initial state init.
/// \param p A linear process
/// \param init A sequence of data expressions
/// \param r A data rewriter
/// \return A map m that maps the constant parameters to their constant value
template <typename DataRewriter>
std::map<data::variable, data::data_expression> compute_constant_parameters(const linear_process& p, data::data_expression_list init, DataRewriter& r)
{
  using namespace data;
  using namespace data::sort_bool_;

  std::map<data::variable, data::data_expression> replacements;
  data::variable_list::iterator i = p.process_parameters().begin();
  data::data_expression_list::iterator j = init.begin();
  for ( ; i != p.process_parameters().end(); ++i, ++j)
  {
    replacements[*i] = r(*j);
  }

  bool has_changed;
  do
  {
    has_changed = false;
    for (summand_list::iterator i = p.summands().begin(); i != p.summands().end(); ++i)
    {
      data::data_expression rc = r(i->condition(), make_map_substitution_adapter(replacements));
      if (rc == false_())
      {
        continue;
      }

      for (data::assignment_list::iterator j = i->assignments().begin(); j != i->assignments().end(); ++j)
      {
        std::map<data::variable, data::data_expression>::iterator k = replacements.find(j->lhs());
        if (k != replacements.end())
        {
          data::data_expression gj = data::variable_map_replace(j->rhs(), replacements);
          if (r(or_(not_(rc), not_equal_to(k->second, gj))) == true_())
          {
            replacements.erase(k);
            has_changed = true;
          }
        }
      }
      if (has_changed)
      {
        break;
      }
    }
  } while (has_changed == true);

  return replacements;
}

/// \brief Returns zero or more constant process parameters of the process p with initial state init.
/// \param p A linear process
/// \param init A sequence of data expressions
/// \param r A data rewriter
/// \return A map m that maps constant parameters to their constant value.
std::map<data::variable, data::data_expression> compute_constant_parameters_subst(const linear_process& p, data::data_expression_list init, data::rewriter& r)
{
  using namespace data;
  using namespace data::sort_bool_;
  namespace opt = data::lazy;

  // create a mapping from process parameters to initial values
  atermpp::map<data::variable, data::data_expression> replacements;
  data::data_expression_list::iterator j = init.begin();
  for (data::variable_list::const_iterator i = p.process_parameters().begin(); i != p.process_parameters().end(); ++i, ++j)
  {
    replacements[*i] = *j;
  }

  bool has_changed;
  do
  {
    has_changed = false;
    for (summand_list::iterator i = p.summands().begin(); i != p.summands().end(); ++i)
    {
      data::data_expression rc = r(i->condition(), make_map_substitution_adapter(replacements));

      if (rc == false_())
      {
#ifdef MCRL2_LPSCONSTELM_DEBUG
      std::cerr << "CONDITION IS FALSE: " << pp(i->condition()) << data::to_string(replacements) << " -> " << pp(rc) << std::endl;
#endif
        continue;
      }
      for (data::assignment_list::const_iterator j = i->assignments().begin(); j != i->assignments().end(); ++j)
      {
        std::map<data::variable, data::data_expression>::iterator k = replacements.find(j->lhs());
        if (k != replacements.end())
        {
          data::variable        d  = j->lhs();  // process parameter
          data::data_expression g  = j->rhs();  // assigned value
          if (r(d, make_map_substitution_adapter(replacements)) != r(g, make_map_substitution_adapter(replacements)))
          {
            replacements.erase(k);
            has_changed = true;
#ifdef MCRL2_LPSCONSTELM_DEBUG
            std::cerr << "POSSIBLE CHANGE FOR PARAMETER " << pp(j->lhs()) << "\n"
                      << "      value before: " << pp(r(j->lhs(), make_map_substitution_adapter(replacements))) << "\n"
                      << "      value after:  " << pp(r(j->rhs(), make_map_substitution_adapter(replacements))) << "\n"
                      << "      replacements: " << data::to_string(replacements) << std::endl;
#endif
          }
#ifdef MCRL2_LPSCONSTELM_DEBUG
          else
          {
            std::cerr << "NO CHANGE FOR PARAMETER " << pp(j->lhs()) << "\n"
                      << "      value before: " << pp(r(j->lhs(), make_map_substitution_adapter(replacements))) << "\n"
                      << "      value after:  " << pp(r(j->rhs(), make_map_substitution_adapter(replacements))) << "\n"
                      << "      replacements: " << data::to_string(replacements) << std::endl;
          }
#endif
        }
      }
      if (has_changed)
      {
        break;
      }
    }
  } while (has_changed == true);

  return replacements;
}

/// \brief Removes zero or more constant parameters from the specification p.
/// \param spec A linear process specification
/// \param r A data rewriter
/// \param verbose If true, verbose output is generated
/// \return The transformed specification
template <typename DataRewriter>
specification constelm(const specification& spec, DataRewriter& r, bool verbose = false)
{
  using core::pp;

  std::map<data::variable, data::data_expression> replacements = compute_constant_parameters_subst(spec.process(), spec.initial_process().state(), r);
  std::set<data::variable> constant_parameters;
  for (std::map<data::variable, data::data_expression>::iterator i = replacements.begin(); i != replacements.end(); ++i)
  {
	  constant_parameters.insert(i->first);
  }

  if (verbose)
  {
    std::clog << "Removing the constant process parameters: ";
    for (std::set<data::variable>::iterator i = constant_parameters.begin(); i != constant_parameters.end(); ++i)
    {
      std::clog << data::pp(*i) << " ";
    }
    std::clog << std::endl;
  }

  specification result = repair_free_variables(spec);
  result = detail::remove_parameters(result, constant_parameters);

  // N.B. The replacements may only be applied to the process and the initial process!
  linear_process new_process   = data::variable_map_replace(result.process(), replacements);
  process_initializer new_init = data::variable_map_replace(result.initial_process(), replacements);
  result.process() = new_process;
  result.initial_process() = new_init;

  assert(result.is_well_typed());
  return result;
}

struct default_free_variable_solver
{
  typedef std::map<data::variable, data::data_expression> variable_map;
  
  /// \brief Attempts to find a valuation for free variables that makes the condition
  /// !R(c, sigma) or (R(d1, sigma) = R(g, sigma)) true.
  template <typename Rewriter, typename Substitution>
  variable_map solve(const data::variable_list& V,
                     const data::data_expression& /* c */,
                     const data::data_expression& g,
                     const data::variable& /* d */,
                     const data::data_expression& d1,
                     const Rewriter& R,
                     const Substitution& sigma
                    )
    {
      variable_map result;
      data::data_expression r = R(g, sigma);
      if (r.is_variable())
      {
        data::variable v = r;
        if (std::find(V.begin(), V.end(), v) != V.end())
        {
          result[v] = d1;
        }
      }
      return result;
    }
};

/// \brief Algorithm class for elimination of constant parameters
// TODO: add default template argument for free variable solver
class constelm_algorithm: public lps::detail::lps_rewriter_algorithm
{
  protected:
    typedef std::map<data::variable, data::data_expression> variable_map;
    
    /// \brief If true, then the algorithm is allowed to instantiate free variables
    /// as a side effect.
    bool m_instantiate_free_variables;

    /// \brief If true, verbose output is printed.
    bool m_verbose;

    /// \brief Maps process parameters to their index.
    std::map<data::variable, unsigned int> m_index_of;
   
    /// \brief Applies the next state substitution to the variable v.
    const data::data_expression& next_state(const summand& s, const data::variable& v) const
    {
      const data::assignment_list& a = s.assignments();
      for (data::assignment_list::const_iterator i = a.begin(); i != a.end(); ++i)
      {
        if (i->lhs() == v)
        {
          return i->rhs();
        }
      }
      return v; // no assignment to v found, so return v itself
    }

    // /// \brief Returns true if ... 
    // template <typename DataRewriter, typename Substitution>
    // bool is_constant(const data::variable& dj, const summand& s, DataRewriter& R, Substitution& sigma) const
    // {
    //   data::data_expression gj = next_state(s, dj);
    //   if (R(s.condition(), sigma) == data::sort_bool_::false_() || R(gj, sigma) == d1)
    //   {
    //     return true;
    //   }
    // }

  public:
    
    /// \brief Constructor
    constelm_algorithm(specification& spec, data::rewriter::strategy s = data::rewriter::jitty)
      : lps::detail::lps_rewriter_algorithm(spec, s)
    {}
    
    /// \brief Runs the constelm algorithm
    /// \param p A linear process
    /// \param d1 An initial value for the linear process p
    /// \param R A data rewriter
    /// \param instantiate_free_variables If true, the algorithm is allowed to instantiate free variables
    /// as a side effect
    template <typename DataRewriter>
    void run(bool instantiate_free_variables = false, bool verbose = false)
    {
      // TODO: use convert<> for this?
      data::data_expression_list state = spec.initial_process().state();
      data::data_expression_vector d1(state.begin(), state.end());
      linear_process& p = spec.process();

      m_instantiate_free_variables = instantiate_free_variables;
      m_verbose = verbose;

      data::variable_list V = p.free_variables();
      const data::variable_list& d = p.process_parameters();

      // initialize m_index_of
      unsigned index = 0;
      for (data::variable_list::const_iterator i = d.begin(); i != d.end(); ++i)
      {               
        m_index_of[*i] = index++;
      }   
        
      data::data_expression_vector x(d1.begin(), d1.end());
      std::set<data::variable> G(d.begin(), d.end());
      std::set<data::variable> dG;
      
      // Contains substitutins to free variables
      variable_map sigma;

      std::map<data::variable, std::set<data::variable> > undo;

      do
      {
        dG.clear();
        for (summand_list::iterator i = p.summands().begin(); i != p.summands().end(); ++i)
        {
          const summand& s = *i;  
          const data::data_expression& c_i = s.condition();
          data::data_expression Rc = R(c_i, sigma);
          if (Rc != data::sort_bool_::false_())
          {                                 
            for (std::set<data::variable>::iterator j = G.begin(); j != G.end(); ++j)
            {
              unsigned int index_j = m_index_of[*j];
              const data::variable& d_j = *j;
              const data::data_expression& g_ij = next_state(s, d_j);
              
              // TODO: give map_substitution_adapter a map interface to avoid ugly code like below?
              if (R(d_j, data::make_map_substitution_adapter(sigma)) != R(g_ij, data::make_map_substitution_adapter(sigma)))
              {
                variable_map W = default_free_variable_solver().solve(V, c_i, g_ij, d_j, d1[index_j], R, sigma);
                if (!W.empty())
                {
                  for (variable_map::const_iterator w = W.begin(); w != W.end(); ++w)
                  {
                    sigma[w->first] = w->second;
                    undo[d_j].insert(w->first);
                  }
                }         
                else
                {
                  G.erase(d_j);
                  dG.insert(d_j);
                  x[index_j] = d_j;
                  std::set<data::variable>& var = undo[d_j];
                  for (std::set<data::variable>::iterator w = var.begin(); w != var.end(); ++w)
                  {
                    sigma[*w] = *w;
                  }
                  undo[d_j].clear();
                }
              }
            }
          }
        }
      } while (!dG.empty());
      
      // report the results
      if (m_verbose)
      {
        std::cout << "Removing the constant process parameters: ";
        for (std::set<data::variable>::iterator i = G.begin(); i != G.end(); ++i)
        {
          std::clog << data::pp(*i) << " ";
        }
        std::clog << std::endl;       

        if (!sigma.empty())
        {
          std::clog << "Applied the following free variable substitutions: " << data::to_string(sigma);
        }
      }

      // save the instantiated free variables for later use
      std::set<data::variable> instantiated_free_variables;
      for (variable_map::iterator k = sigma.begin(); k != sigma.end(); ++k)
      {
        instantiated_free_variables.insert(k->first);
      }
      
      // add the constant parameter substitutions to sigma
      for (std::set<data::variable>::iterator j = G.begin(); j != G.end(); ++j)
      {
        unsigned int index_j = m_index_of[*j];
        sigma[*j] = d1[index_j];
      }

      // rewrite the specification spec with the substitutions in sigma
      rewrite(data::make_map_substitution_adapter(sigma));
      
      // remove the constant parameters from the specification spec
      remove_formal_parameters(G);
      
      // remove the instantiated free variables from the specification spec
      remove_free_variables(instantiated_free_variables);
    }
};

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_CONSTELM_H
