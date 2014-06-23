// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file binary.h
/// \brief The binary algorithm.

#ifndef MCRL2_LPS_BINARY_H
#define MCRL2_LPS_BINARY_H

// #include <cmath>
#include <iterator>

#include "mcrl2/utilities/logger.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/classic_enumerator.h"
#include "mcrl2/data/substitutions/mutable_indexed_substitution.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/lps/detail/lps_algorithm.h"
#include "mcrl2/lps/replace.h"

namespace mcrl2
{

namespace lps
{

// Compute base 2 logarithm of n, by checking which is the leftmost
// bit that has been set.
inline
size_t ceil_log2(size_t n)
{
  assert(n>0);
  size_t result = 0;
  while(n != 0)
  {
    n = n >> 1;
    ++result;
  }
  return result;
}

// Compute the number of booleans needed to represent a set of size n.
inline
size_t nr_of_booleans_for_elements(size_t n)
{
  if(n == 1)
  {
    return 1;
  }
  else
  {
    return ceil_log2(n-1);
  }
}

/// \brief Algorithm class that can be used to apply the binary algorithm
///
/// All parameters of finite data types are replaced with a vector of
/// booleans.
template<typename DataRewriter>
class binary_algorithm: public lps::detail::lps_algorithm
{
    typedef data::classic_enumerator< data::rewriter > enumerator_type;

  protected:
    /// Rewriter
    DataRewriter m_rewriter;

    /// Mapping of finite variables to boolean vectors
    std::map<data::variable, std::vector<data::variable> > m_new_parameters;

    /// Mapping of variables to all values they can be assigned
    std::map<data::variable, std::vector<data::data_expression> > m_enumerated_elements;

    /// Mapping of variables to corresponding if-tree
    data::mutable_map_substitution<> m_if_trees;

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
        size_t n = enumerated_elements.size();
        size_t m = 1 << (new_parameters.size() - 1);  //m == 2^(new_parameters.size() - 1)

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

    // Take a specification and calculate a vector of boolean variables for each process
    // parameter of a finite sort. A mapping variable -> vector of booleans is stored in new_parameters_table
    // a mapping variable -> enumerated elements is stored in enumerated_elements_table
    /// \return data variable list with the new process parameters (i.e. with all variables of a
    /// finite type != bool replaced by a vector of boolean variables.
    void replace_enumerated_parameters()
    {
      data::variable_list process_parameters = m_spec.process().process_parameters();
      data::variable_vector new_parameters;

      mCRL2log(log::debug) << "Original process parameters: " << data::pp(process_parameters) << std::endl;

      data::set_identifier_generator generator;
      generator.add_identifiers(lps::find_identifiers(m_spec));
      enumerator_type enumerator(m_rewriter,m_spec.data());

      // Transpose all process parameters, and replace those that are finite, and not bool with boolean variables.
      for (data::variable_list::const_iterator i = process_parameters.begin(); i != process_parameters.end(); ++i)
      {
        data::variable par = *i;

        if (!data::sort_bool::is_bool(par.sort()) && m_spec.data().is_certainly_finite(par.sort()))
        {
          //Get all constructors for par
          data::data_expression_vector enumerated_elements; // List to store enumerated elements of a parameter

          data::mutable_indexed_substitution<> local_sigma;
          const data::variable_list vl=atermpp::make_list<data::variable>(par);
          std::deque<data::enumerator_list_element_with_substitution<data::data_expression> >
                     enumerator_deque(1,data::enumerator_list_element_with_substitution<data::data_expression>(vl, data::sort_bool::true_()));
          for (enumerator_type::iterator j=enumerator.begin(
                           local_sigma,
                           enumerator_deque);
                j != enumerator.end() ; ++j)
          {
            j->add_assignments(vl,local_sigma,m_rewriter);
            enumerated_elements.push_back(local_sigma(par));
          }

          m_enumerated_elements[par] = enumerated_elements;

          //Calculate the number of booleans needed to encode par
          size_t n = nr_of_booleans_for_elements(enumerated_elements.size());

          //Set hint for fresh variable names
          std::string par_name = par.name();

          // Temp list for storage
          data::variable_vector new_pars;
          //Create new parameters and add them to the parameter list.
          for (size_t i = 0; i<n; ++i)
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
      for (data::assignment_list::const_iterator i = v.begin(); i != v.end(); ++i)
      {
        if (m_new_parameters.find(i->lhs()) == m_new_parameters.end())
        {
          result.push_back(*i);
        }
        else
        {
          data::variable_vector new_parameters = m_new_parameters[i->lhs()];
          data::data_expression_vector elements = m_enumerated_elements[i->lhs()];

          mCRL2log(log::debug) << "Found " << new_parameters.size() << " new parameter(s) for parameter " << data::pp(i->lhs()) << std::endl;

          for (size_t j = 0; j < new_parameters.size(); ++j)
          {
            data::data_expression_vector disjuncts;

            data::data_expression_vector::iterator k = elements.begin();
            while (k != elements.end())
            {
              // Elements that get boolean value false
              ssize_t count(1 << j);
              if (std::distance(k, elements.end()) < count)
              {
                k = elements.end();
              }
              else
              {
                std::advance(k, count);
              }

              // Elements that get value true
              for (ssize_t l = 0; l < count && k != elements.end(); ++l)
              {
                disjuncts.push_back(data::equal_to(i->rhs(), *k++));
              }
            }
            result.push_back(data::assignment(new_parameters[j], data::lazy::join_or(disjuncts.begin(), disjuncts.end())));
          }

        }
      }

      mCRL2log(log::debug) << "Replaced assignment(s) " << data::pp(v) << " with assignment(s) " << data::pp(result) << std::endl;

      return data::assignment_list(result.begin(),result.end());
    }

    /// \brief Update an action summand with the new Boolean parameters
    void update_action_summand(action_summand& s, const std::set<data::variable>& if_trees_variables)
    {
      s.condition() = data::replace_variables_capture_avoiding(s.condition(), m_if_trees, if_trees_variables);
      s.multi_action().actions() = lps::replace_variables_capture_avoiding(s.multi_action().actions(), m_if_trees, data::substitution_variables(m_if_trees));
      if (s.multi_action().has_time())
      {
        s.multi_action().time() = data::replace_variables_capture_avoiding(s.multi_action().time(), m_if_trees, if_trees_variables);
      }
      s.assignments() = replace_enumerated_parameters_in_assignments(s.assignments());
    }

    /// \brief Update a deadlock summand with the new Boolean parameters
    void update_deadlock_summand(deadlock_summand& s, const std::set<data::variable>& if_trees_variables)
    {
      s.condition() = data::replace_variables_capture_avoiding(s.condition(), m_if_trees, data::substitution_variables(m_if_trees));
      if (s.deadlock().has_time())
      {
        s.deadlock().time() = data::replace_variables_capture_avoiding(s.deadlock().time(), m_if_trees, if_trees_variables);
      }
    }

  public:
    /// \brief Constructor for binary algorithm
    /// \param spec Specification to which the algorithm should be applied
    /// \param r a rewriter for data
    binary_algorithm(specification& spec,
                     DataRewriter& r)
      : lps_algorithm(spec),
        m_rewriter(r)
    {}

    /// \brief Apply the algorithm to the specification passed in the
    ///        constructor
    void run()
    {
      replace_enumerated_parameters();

      // Initial process
      mCRL2log(log::debug) << "Updating process initializer" << std::endl;

      m_spec.initial_process() = process_initializer(replace_enumerated_parameters_in_assignments(m_spec.initial_process().assignments()));

      // Summands
      mCRL2log(log::debug) << "Updating summands" << std::endl;
      std::set<data::variable> if_trees_variables = data::substitution_variables(m_if_trees);

      std::for_each(m_spec.process().action_summands().begin(),
                    m_spec.process().action_summands().end(),
                    boost::bind(&binary_algorithm::update_action_summand, this, _1, if_trees_variables));

      std::for_each(m_spec.process().deadlock_summands().begin(),
                    m_spec.process().deadlock_summands().end(),
                    boost::bind(&binary_algorithm::update_deadlock_summand, this, _1, if_trees_variables));
    }
};

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_BINARY_H
