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

#include <cmath>
#include <iterator>

#include "mcrl2/core/messaging.h"
#include "mcrl2/data/detail/convert.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/fresh_variable_generator.h"
#include "mcrl2/data/substitution.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/classic_enumerator.h"
#include "mcrl2/data/enumerator_factory.h"
#include "mcrl2/lps/detail/lps_algorithm.h"

namespace mcrl2 {

  namespace lps {

    /// \brief Algorithm class that can be used to apply the binary algorithm
    ///
    /// All parameters of finite data types are replaced with a vector of
    /// booleans.
    template<typename DataRewriter>
    class binary_algorithm: public lps::detail::lps_algorithm
    {
      protected:
        /// Rewriter
        DataRewriter m_rewriter;

        /// Enumerator factory
        data::enumerator_factory< data::classic_enumerator< > > m_enumerator_factory;

        /// Mapping of finite variables to boolean vectors
        atermpp::map<data::variable, atermpp::vector<data::variable> > m_new_parameters;

        /// Mapping of variables to all values they can be assigned
        atermpp::map<data::variable, atermpp::vector<data::data_expression> > m_enumerated_elements;

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
            int n = enumerated_elements.size();
            int m = pow( static_cast<int>(2), static_cast<int>(new_parameters.size()) - 1);

            //m == 2^(new_parameters.size() - 1)

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

            data::data_expression condition = new_parameters.front();
            new_parameters.erase(new_parameters.begin());
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

          if (core::gsDebug)
          {
            std::cerr << "Original process parameters: " << pp(process_parameters) << std::endl;
          }

          data::fresh_variable_generator generator(specification_to_aterm(m_spec));
          // Transpose all process parameters, and replace those that are finite, and not bool with boolean variables.
          for (data::variable_list::const_iterator i = process_parameters.begin(); i != process_parameters.end(); ++i)
          {
            data::variable par = *i;

            if (!data::sort_bool::is_bool(par) && m_spec.data().is_certainly_finite(par.sort()))
            {
              //Get all constructors for par
              data::data_expression_vector enumerated_elements; // List to store enumerated elements of a parameter

              for (data::classic_enumerator< > j(m_enumerator_factory.make(par)); j != data::classic_enumerator<>(); ++j)
              {
                enumerated_elements.push_back((*j)(par));
              }

              m_enumerated_elements[par] = enumerated_elements;

              //Calculate the number of booleans needed to encode par
              int n = ceil(log(enumerated_elements.size()) / log(2));

              //Set hint for fresh variable names
              generator.set_hint(par.name());

              // Temp list for storage
              data::variable_vector new_pars;
              //Create new parameters and add them to the parameter list.
              for (int i = 0; i<n; ++i)
              {
                data::variable v(generator(data::sort_bool::bool_()));
                new_parameters.push_back(v);
                new_pars.push_back(v);
              }
              // n = new_pars.size() && new_pars.size() = ceil(log_2(j)) && new_pars.size() = ceil(log_2(enumerated_elements.size()))
              
              if (core::gsVerbose)
              {
                std::cerr << "Parameter `" << par << "' has been replaced by parameters " << pp(new_pars) << " of sort Bool" << std::endl;
              }

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

          if (core::gsDebug)
          {
            std::cerr << "New process parameters: " << pp(new_parameters) << std::endl;
          }
          m_spec.process().process_parameters() = data::convert<data::variable_list>(new_parameters);
        }

        /// \brief Replace assignments in v that are of a finite sort with a
        ///        vector of assignments to Boolean variables.
        data::assignment_list replace_enumerated_parameters_in_assignments(data::assignment_list v)
        {
          v = data::replace_free_variables(v, m_if_trees);

          data::assignment_vector result;
          for (data::assignment_list::const_iterator i = v.begin(); i != v.end(); ++i)
          {
            if(m_new_parameters.find(i->lhs()) == m_new_parameters.end())
            {
              result.push_back(*i);
            }
            else
            {
              data::variable_vector new_parameters = m_new_parameters[i->lhs()];
              data::data_expression_vector elements = m_enumerated_elements[i->lhs()];

              for(unsigned int j = 0; j < new_parameters.size(); ++j)
              {
                data::data_expression_vector disjuncts;

                data::data_expression_vector::iterator k = elements.begin();
                while(k != elements.end())
                {
                  // Elements that get boolean value false
                  int count(round(pow(2, j)));
                  // TODO: Why doesn't just std::advance(k,count) work?, i.e. if
                  // distance(k, elements.end()) > count, advance(k, count)
                  // entails k != elements.end().
                  if (std::distance(k, elements.end()) > count)
                  {
                    k = elements.end();
                  }
                  else
                  {
                    std::advance(k, count);
                  }

                  // Elements that get value true
                  for(int l = 0; l < count && k != elements.end(); ++l)
                  {
                      disjuncts.push_back(data::equal_to(i->rhs(), *k++));
                  }
                }
                result.push_back(data::assignment(new_parameters[j], data::lazy::join_or(disjuncts.begin(), disjuncts.end())));
              }
            }
          }
          return data::convert<data::assignment_list>(result);
        }

        /// \brief Update an action summand with the new Boolean parameters
        void update_action_summand(action_summand& s)
        {
          s.condition() = m_if_trees(s.condition());
          s.multi_action().actions() = data::replace_free_variables(s.multi_action().actions(), m_if_trees);
          s.multi_action().time() = m_if_trees(s.multi_action().time());
          s.assignments() = replace_enumerated_parameters_in_assignments(s.assignments());
        }

        /// \brief Update a deadlock summand with the new Boolean parameters
        void update_deadlock_summand(deadlock_summand& s)
        {
          s.condition() = m_if_trees(s.condition());
          s.deadlock().time() = m_if_trees(s.deadlock().time());
        }

      public:
        /// \brief Constructor for binary algorithm
        /// \param spec Specification to which the algorithm should be applied
        /// \param r a rewriter for data
        binary_algorithm(specification& spec,
                         DataRewriter& r)
          : lps_algorithm(spec,core::gsVerbose),
            m_rewriter(r),
            m_enumerator_factory(spec.data(), r)
        {}

        /// \brief Apply the algorithm to the specification passed in the
        ///        constructor
        void run()
        {
          replace_enumerated_parameters();

          // Initial process
          m_spec.initial_process() = process_initializer(replace_enumerated_parameters_in_assignments(m_spec.initial_process().assignments()));

          // Summands
          std::for_each(m_spec.process().action_summands().begin(),
                        m_spec.process().action_summands().end(),
                        boost::bind(&binary_algorithm::update_action_summand, this, _1));

          std::for_each(m_spec.process().deadlock_summands().begin(),
                        m_spec.process().deadlock_summands().end(),
                        boost::bind(&binary_algorithm::update_deadlock_summand, this, _1));
        }
    };

  } // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_BINARY_H
