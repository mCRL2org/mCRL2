// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file binary.h
/// \brief The binary algorithm.

#ifndef MCRL2_LPS_BINARY_H
#define MCRL2_LPS_BINARY_H

#include "mcrl2/data/enumerator.h"
#include "mcrl2/lps/detail/lps_algorithm.h"
#include "mcrl2/lps/detail/parameter_selection.h"

namespace mcrl2::lps
{

// Compute the number of booleans needed to represent a set of size n.
inline
std::size_t nr_of_booleans_for_elements(std::size_t n)
{
  if(n == 1)
  {
    return 1;
  }
  else
  {
    return utilities::ceil_log2(n-1);
  }
}

/// \brief Algorithm class that can be used to apply the binary algorithm
///
/// All parameters of finite data types are replaced with a vector of
/// booleans.
template<typename DataRewriter, typename Specification>
class binary_algorithm: public detail::lps_algorithm<Specification>
{
  using enumerator_element = data::enumerator_list_element_with_substitution<>;
  using super = typename detail::lps_algorithm<Specification>;
  using process_type = typename Specification::process_type;
  using action_summand_type = typename process_type::action_summand_type;
  using super::m_spec;

  protected:
    /// Rewriter
    DataRewriter m_rewriter;

    const std::string m_parameter_selection;

    /// Mapping of finite variables to boolean vectors
    std::map<data::variable, std::vector<data::variable> > m_new_parameters;

    /// Mapping of variables to all values they can be assigned
    std::map<data::variable, std::vector<data::data_expression> > m_enumerated_elements;

    /// Mapping of variables to corresponding if-tree
    data::mutable_map_substitution<> m_if_trees;

    /// Contains the names of variables appearing in rhs of m_if_trees
    data::set_identifier_generator m_if_trees_generator;

    /// Identifier generator for the enumerator
    data::enumerator_identifier_generator m_id_generator;

    /// \brief Build an if-then-else tree of enumerated elements in terms
    ///        of new parameters.
    /// \pre enumerated_elements.size() <= 2^new_parameters.size()
    /// \return if then else tree from enumerated_elements in terms of new_parameters
    data::data_expression make_if_tree(data::variable_vector new_parameters,
                                       const data::data_expression_vector& enumerated_elements)
    {
      data::data_expression result;

      if (new_parameters.empty())
      {
        result = enumerated_elements.front();
      }
      else
      {
        std::size_t n = enumerated_elements.size();
        std::size_t m = static_cast<std::size_t>(1) << (new_parameters.size() - 1);  //m == 2^(new_parameters.size() - 1)

        if (m > n)
        {
          m = n;
        }

        data::data_expression_vector left_list(enumerated_elements.begin(), enumerated_elements.begin() + m);
        data::data_expression_vector right_list;
        if (m == n)
        {
          right_list = data::data_expression_vector(enumerated_elements.begin() + m - 1, enumerated_elements.end());
        }
        else
        {
          right_list = data::data_expression_vector(enumerated_elements.begin() + m, enumerated_elements.end());
        }

        data::data_expression condition = new_parameters.back();
        new_parameters.pop_back();
        result = if_(condition,
                     make_if_tree(new_parameters, right_list),
                     make_if_tree(new_parameters, left_list));
      }

      return result;
    }

    /// \brief Determine which variables should be replaced, based on parameter_selection
    /// \return A subset of the process parameters, with a finite sort that is not Bool
    std::set<data::variable> select_parameters(const std::string parameter_selection) const
    {
      const data::variable_list& process_parameters = m_spec.process().process_parameters();
      bool use_selection = !parameter_selection.empty();

      std::list<data::variable> selected_params;
      if (use_selection)
      {
        const data::variable_vector sel = 
                   detail::parse_lps_parameter_selection(process_parameters, m_spec.data(), parameter_selection);
        selected_params = std::list<data::variable>(sel.begin(), sel.end());
      }
      else
      {
        selected_params = std::list<data::variable>(process_parameters.begin(), process_parameters.end());
      }
      selected_params.remove_if([&](const data::variable& v)
        {
          bool cannot_replace = v.sort() == data::sort_bool::bool_() || !m_spec.data().is_certainly_finite(v.sort());
          if (cannot_replace && use_selection)
          {
            mCRL2log(log::info) << "Not selecting " << v  << ":" << v.sort() << " since it is already Bool, or its type is not finite." << std::endl;
          }
          return cannot_replace;
        }
      );

      if (use_selection && selected_params.empty())
      {
        mCRL2log(log::info) << "No parameters were selected to be replaced." << std::endl;
      }

      return std::set<data::variable>(selected_params.begin(), selected_params.end());
    }

    /// \brief Take a specification and calculate a vector of boolean variables for each process
    /// parameter in selected_params. A mapping variable -> vector of booleans is stored in new_parameters_table
    /// a mapping variable -> enumerated elements is stored in enumerated_elements_table
    /// \pre all elements in selected_params should not be of type Bool and should have a finite type
    /// \return data variable list with the new process parameters (i.e. with all variables of a
    /// finite type != bool replaced by a vector of boolean variables.
    void replace_enumerated_parameters(const std::set<data::variable>& selected_params)
    {
      data::variable_list process_parameters = m_spec.process().process_parameters();
      data::variable_vector new_parameters;

      mCRL2log(log::debug) << "Original process parameters: " << data::pp(process_parameters) << std::endl;

      data::set_identifier_generator generator;
      generator.add_identifiers(lps::find_identifiers(m_spec));
      generator.add_identifiers(data::function_and_mapping_identifiers(m_spec.data()));
      bool accept_solutions_with_variables = false;
      data::enumerator_algorithm<> enumerator(m_rewriter, m_spec.data(), m_rewriter, m_id_generator, accept_solutions_with_variables);

      // Transpose all process parameters, and replace those that are finite, and not bool with boolean variables.
      for (const data::variable& par: process_parameters)
      {
        if (selected_params.find(par) != selected_params.end())
        {
          //Get all constructors for par
          data::data_expression_vector enumerated_elements; // List to store enumerated elements of a parameter

          data::mutable_indexed_substitution<> local_sigma;
          const data::variable_list vl{ par };
          enumerator.enumerate(enumerator_element(vl, data::sort_bool::true_()),
                               local_sigma,
                               [&](const enumerator_element& p)
                               {
                                 p.add_assignments(vl, local_sigma, m_rewriter);
                                 enumerated_elements.push_back(local_sigma(par));
                                 return false;
                               }
          );
          m_enumerated_elements[par] = enumerated_elements;

          //Calculate the number of booleans needed to encode par
          std::size_t n = nr_of_booleans_for_elements(enumerated_elements.size());

          //Set hint for fresh variable names
          std::string par_name = par.name();

          // Temp list for storage
          data::variable_vector new_pars;
          //Create new parameters and add them to the parameter list.
          for (std::size_t i = 0; i<n; ++i)
          {
            data::variable v(generator(par_name), data::sort_bool::bool_());
            new_parameters.push_back(v);
            new_pars.push_back(v);
          }
          // n = new_pars.size() && new_pars.size() = ceil(log_2(j)) && new_pars.size() = ceil(log_2(enumerated_elements.size()))

          mCRL2log(log::verbose) << "Parameter " << data::pp(par) << ":" << data::pp(par.sort()) << " has been replaced by " << new_pars.size() << " parameter(s) " << data::pp(new_pars) << " of sort Bool" << std::endl;

          //Store new parameters in a hastable
          m_new_parameters[par]=new_pars;

          m_if_trees[par]=make_if_tree(new_pars,
                                       enumerated_elements);
        }
        else
        {
          new_parameters.push_back(par);
        }
      }

      mCRL2log(log::debug) << "New process parameter(s): " << data::pp(new_parameters) << std::endl;

      m_spec.process().process_parameters() = data::variable_list(new_parameters.begin(),new_parameters.end());
      for (const data::variable& v: data::substitution_variables(m_if_trees))
      {
        m_if_trees_generator.add_identifier(v.name());
      }
    }

    /// \brief Replace expressions in v that are of a finite sort with a
    ///        vector of assignments to Boolean variables.
    data::data_expression_list replace_enumerated_parameters_in_initial_expressions(
                                 const data::variable_list& vl,
                                 const data::data_expression_list& el)
    {
      // We use replace_variables, to make sure that the binding variables of assignments are ignored.
      // Note that this operation is safe because the generated fresh variables can not clash with other
      // binding variables.
      const data::data_expression_list el_ = data::replace_variables(el, m_if_trees);

      data::data_expression_vector result;
      data::variable_list::const_iterator i=vl.begin();
      for (const data::data_expression& a: el_)
      {
        const data::variable par= *i;
        i++;
        if (m_new_parameters.find(par) == m_new_parameters.end())   // This parameter is not replaced by a boolean parameters.
        {
          result.push_back(a);
        }
        else
        {
          data::variable_vector new_parameters = m_new_parameters[par];
          data::data_expression_vector elements = m_enumerated_elements[par];

          mCRL2log(log::debug) << "Found " << new_parameters.size() << " new parameter(s) for parameter " << data::pp(par) << std::endl;

          for (std::size_t j = 0; j < new_parameters.size(); ++j)
          {
            data::data_expression_vector disjuncts;

            data::data_expression_vector::iterator k = elements.begin();
            while (k != elements.end())
            {
              // Elements that get boolean value false
              std::ptrdiff_t count(static_cast<std::ptrdiff_t>(1) << j);
              if (std::distance(k, elements.end()) < count)
              {
                k = elements.end();
              }
              else
              {
                std::advance(k, count);
              }

              // Elements that get value true
              for (std::ptrdiff_t l = 0; l < count && k != elements.end(); ++l)
              {
                disjuncts.push_back(data::equal_to(a, *k++));
              }
            }
            result.push_back(data::lazy::join_or(disjuncts.begin(), disjuncts.end()));
          }
        }
      }

      mCRL2log(log::debug) << "Replaced expression(s) " << data::pp(el_) << " in the initial state with expression(s) " << data::pp(result) << std::endl;

      return data::data_expression_list(result.begin(),result.end());
    }

    /// \brief Replace assignments in v that are of a finite sort with a
    ///        vector of assignments to Boolean variables.
    data::assignment_list replace_enumerated_parameters_in_assignments(data::assignment_list v)
    {
      // We use replace_variables, to make sure that the binding variables of assignments are ignored.
      // Note that this operation is safe because the generated fresh variables can not clash with other
      // binding variables.
      v = data::replace_variables(v, m_if_trees);

      data::assignment_vector result;
      for (const data::assignment& a: v)
      {
        if (m_new_parameters.find(a.lhs()) == m_new_parameters.end())
        {
          result.push_back(a);
        }
        else
        {
          data::variable_vector new_parameters = m_new_parameters[a.lhs()];
          data::data_expression_vector elements = m_enumerated_elements[a.lhs()];

          mCRL2log(log::debug) << "Found " << new_parameters.size() << " new parameter(s) for parameter " << data::pp(a.lhs()) << std::endl;

          for (std::size_t j = 0; j < new_parameters.size(); ++j)
          {
            data::data_expression_vector disjuncts;

            data::data_expression_vector::iterator k = elements.begin();
            while (k != elements.end())
            {
              // Elements that get boolean value false
              std::ptrdiff_t count(static_cast<std::ptrdiff_t>(1) << j);
              if (std::distance(k, elements.end()) < count)
              {
                k = elements.end();
              }
              else
              {
                std::advance(k, count);
              }

              // Elements that get value true
              for (std::ptrdiff_t l = 0; l < count && k != elements.end(); ++l)
              {
                disjuncts.push_back(data::equal_to(a.rhs(), *k++));
              }
            }
            result.emplace_back(new_parameters[j], data::lazy::join_or(disjuncts.begin(), disjuncts.end()));
          }

        }
      }

      mCRL2log(log::debug) << "Replaced assignment(s) " << data::pp(v) << " with assignment(s) " << data::pp(result) << std::endl;

      return data::assignment_list(result.begin(),result.end());
    }

    /// \brief Update an action summand with the new Boolean parameters
    void update_action_summand(action_summand& s)
    {
      s.condition() = data::replace_variables_capture_avoiding(s.condition(), m_if_trees, m_if_trees_generator);
      s.multi_action()=lps::replace_variables_capture_avoiding(s.multi_action(), m_if_trees, m_if_trees_generator);
      s.assignments() = replace_enumerated_parameters_in_assignments(s.assignments());
    }

    /// \brief Update an action summand with the new Boolean parameters
    void update_action_summand(stochastic_action_summand& s)
    {
      update_action_summand(static_cast<action_summand&>(s));
      s.distribution() = lps::replace_variables_capture_avoiding(s.distribution(), m_if_trees, m_if_trees_generator);
    }

    /// \brief Update a deadlock summand with the new Boolean parameters
    void update_deadlock_summand(deadlock_summand& s)
    {
      s.condition() = data::replace_variables_capture_avoiding(s.condition(), m_if_trees, m_if_trees_generator);
      lps::replace_variables_capture_avoiding(s.deadlock(), m_if_trees, m_if_trees_generator);
    }

    process_initializer update_initial_process(const data::variable_list& parameters, const process_initializer& init)
    {
      return process_initializer(replace_enumerated_parameters_in_initial_expressions(parameters, init.expressions()));
    }

    stochastic_process_initializer update_initial_process(const data::variable_list& parameters, const stochastic_process_initializer& init)
    {
      /* return stochastic_process_initializer(replace_enumerated_parameters_in_initial_expressions(parameters, init.expressions()),
                                            lps::replace_variables_capture_avoiding(init.distribution(), m_if_trees, m_if_trees_generator)
                                           ); */
      data::data_expression_list d = replace_enumerated_parameters_in_initial_expressions(parameters, init.expressions());
      const stochastic_distribution dist = lps::replace_variables_capture_avoiding(init.distribution(), d, m_if_trees, m_if_trees_generator);
      return stochastic_process_initializer(d, dist);
    }

  public:
    /// \brief Constructor for binary algorithm
    /// \param spec Specification to which the algorithm should be applied
    /// \param r a rewriter for data
    binary_algorithm(Specification& spec,
                     DataRewriter& r,
                     const std::string parameter_selection = "")
      : detail::lps_algorithm<Specification>(spec),
        m_rewriter(r),
        m_parameter_selection(parameter_selection)
    {}

    /// \brief Apply the algorithm to the specification passed in the
    ///        constructor
    void run()
    {
      data::variable_list old_parameters = m_spec.process().process_parameters();
      const std::set<data::variable> to_replace = select_parameters(m_parameter_selection);
      replace_enumerated_parameters(to_replace);

      // Initial process
      mCRL2log(log::debug) << "Updating process initializer" << std::endl;
      m_spec.initial_process() = update_initial_process(old_parameters, m_spec.initial_process());

      // Summands
      mCRL2log(log::debug) << "Updating summands" << std::endl;

      for (action_summand& a: m_spec.process().action_summands())
      {
        update_action_summand(a);
      }

      for (deadlock_summand& d: m_spec.process().deadlock_summands())
      {
        update_deadlock_summand(d);
      }
    }
};

} // namespace mcrl2::lps

#endif // MCRL2_LPS_BINARY_H
