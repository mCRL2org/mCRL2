// Author(s): Jeroen Keiren, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/specification_utility.cpp
/// \brief Utility functionality for working with specficiations

#ifndef MCRL2_DATA_SPECIFICATION_UTILITY_H__
#define MCRL2_DATA_SPECIFICATION_UTILITY_H__

#include <set>
#include <string>
#include <vector>
#include <functional>

#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/atermpp/algorithm.h"

#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/map_substitution.h"
#include "mcrl2/data/replace.h"

namespace mcrl2 {
  namespace data {

    /// \cond INTERNAL_DOCS
    namespace detail {
      struct is_new_variable : std::unary_function< bool, atermpp::aterm_appl >
      {
        std::set< atermpp::aterm_appl > seen;

        bool operator()(atermpp::aterm_appl p)
        {
         if (data_expression(p).is_variable() && seen.find(p) == seen.end())
         {
           seen.insert(p);

           return true;
         }

         return false;
        }
      };

      data_equation normalise_equation(mcrl2::data::data_equation const& e)
      {
        atermpp::vector< variable > variables;

        atermpp::find_all_if(mcrl2::data::data_equation(e.condition(), e.lhs(), e.rhs()), is_new_variable(), std::back_inserter(variables));

        return mcrl2::data::data_equation(variables, e.condition(), e.lhs(), e.rhs());
      }

      bool alpha_equivalent(mcrl2::data::data_equation o1, mcrl2::data::data_equation o2)
      {
        using namespace mcrl2::data;

        if (o1.variables().size() == o2.variables().size()) {
          data_equation normalised_o1(normalise_equation(o1));
          data_equation normalised_o2(normalise_equation(o2));

          if (((normalised_o1.condition() == normalised_o2.condition()) &&
               (normalised_o1.lhs() == normalised_o2.lhs()) && (normalised_o1.rhs() == normalised_o2.rhs()))) {
            return true;
          }
          else {
            boost::iterator_range< data_equation::variables_const_range::const_iterator > o1variables(normalised_o1.variables());
            boost::iterator_range< data_equation::variables_const_range::const_iterator > o2variables(normalised_o2.variables());

            // check alpha equivalence
            if (!o1variables.empty()) {
              std::map< variable, variable > renamings;

              // Assumes that the equation variables are declared in the same order
              while (!o1variables.empty() && !o2variables.empty()) {
                if (o1variables.front() != o2variables.front()) {
                  if (o1variables.front().sort() != o2variables.front().sort()) {
                    return false;
                  }

                  renamings[o2variables.front()] = o1variables.front();
                }

                o1variables.advance_begin(1);
                o2variables.advance_begin(1);
              }

              if (normalised_o1 == data::make_map_substitution_adapter(renamings)(normalised_o2)) {
                return true;
              }
            }
          }
        }

        return false;
      }

      // imposes a strict ordering on the operands (<)
      struct equation_smaller {

        bool operator()(mcrl2::data::data_equation o1, mcrl2::data::data_equation o2) const {

          if (alpha_equivalent(o1, o2)) {
            return false;
          }

          return ((o1.variables().size() < o2.variables().size()) ||
           ((o1.variables().size() == o2.variables().size()) && ((o1.condition() < o2.condition()) ||
           ((o1.condition() == o2.condition()) && ((o1.rhs() < o2.rhs()) ||
           ((o1.rhs() == o2.rhs()) && o1.lhs() < o2.lhs()))))));
        }
      };
    } // namespace detail

    /// \brief compares function symbols of two lists, disregarding duplicates and function
    /// order
    template < typename Element >
    bool compare_modulo_order(
      atermpp::term_list< Element > olde,
      atermpp::term_list< Element > newe)
    {

      std::set< Element > oldset(olde.begin(), olde.end());
      std::set< Element > newset(newe.begin(), newe.end());

      if (oldset != newset) {
        std::vector< Element > olddiff;
        std::vector< Element > newdiff;

        std::set_difference(oldset.begin(), oldset.end(), newset.begin(), newset.end(),
          std::back_insert_iterator< std::vector< Element > >(olddiff));

        std::set_difference(newset.begin(), newset.end(), oldset.begin(), oldset.end(),
          std::back_insert_iterator< std::vector< Element > >(newdiff));

        std::string old_differences;

        // Because the order is not defined on term structure but on ATerm pointer values there always is a difference
        for (typename std::vector< Element >::const_iterator i = olddiff.begin(); i != olddiff.end(); ++i) {
          typename std::vector< Element >::const_iterator j =
             std::find(newdiff.begin(), newdiff.end(), *i);

          if (j == newdiff.end()) {
            old_differences.append(mcrl2::core::pp(*i)).append(", ");
          }
        }

//        if (!old_differences.empty()) {
//          std::clog << "IN LEFT BUT NOT RIGHT " << old_differences.substr(0, old_differences.size() - 2) << std::endl;
//        }

        std::string new_differences;

        // Because the order is not defined on term structure but on ATerm pointer values there always is a difference
        for (typename std::vector< Element >::const_iterator i = newdiff.begin(); i != newdiff.end(); ++i) {
          typename std::vector< Element >::const_iterator j =
             std::find(olddiff.begin(), olddiff.end(), *i);

          if (j == olddiff.end()) {
            new_differences.append(mcrl2::core::pp(*i)).append(", ");
          }
        }

//        if (!new_differences.empty()) {
//          std::clog << "IN LEFT BUT NOT RIGHT " << new_differences.substr(0, new_differences.size() - 2) << std::endl;
//        }

        return old_differences.empty() && new_differences.empty();
      }

      return true;
    }

    /// \brief compares equations of two lists of equations disregarding duplicates, equation order and variable naming
    bool compare_modulo_order_and_alpha(
      atermpp::term_list< mcrl2::data::data_equation > olde,
      atermpp::term_list< mcrl2::data::data_equation > newe)
    {

      using detail::equation_smaller;

      std::set< mcrl2::data::data_equation, equation_smaller > oldset(olde.begin(), olde.end());
      std::set< mcrl2::data::data_equation, equation_smaller > newset(newe.begin(), newe.end());

      if (oldset != newset) {
        std::vector< mcrl2::data::data_equation > olddiff;
        std::vector< mcrl2::data::data_equation > newdiff;

        std::set_difference(oldset.begin(), oldset.end(), newset.begin(), newset.end(),
          std::back_insert_iterator< std::vector< mcrl2::data::data_equation > >(olddiff),
          equation_smaller());

        std::set_difference(newset.begin(), newset.end(), oldset.begin(), oldset.end(),
          std::back_insert_iterator< std::vector< mcrl2::data::data_equation > >(newdiff),
          equation_smaller());

        std::string old_differences;

        // The relative order of equations in oldset and newset differs so olddiff.size() not necessarily newdiff.size()
        for (std::vector< mcrl2::data::data_equation >::const_iterator i = olddiff.begin(); i != olddiff.end(); ++i) {
          std::set< mcrl2::data::data_equation, equation_smaller >::const_iterator j =
             std::find_if(newset.begin(), newset.end(), std::bind1st(std::ptr_fun(&detail::alpha_equivalent), *i));

          if (j == newset.end()) {
            old_differences.append(mcrl2::core::pp(*i)).append(",\n");
            old_differences.append(i->to_string()).append(",\n");
          }
        }

//        if (!old_differences.empty()) {
//          std::clog << "IN LEFT BUT NOT RIGHT: " << std::endl
//                    << old_differences.substr(0, old_differences.size() - 2) << std::endl;
//        }

        std::string new_differences;

        // The relative order of equations in oldset and newset differs so olddiff.size() not necessarily newdiff.size()
        for (std::vector< mcrl2::data::data_equation >::const_iterator i = newdiff.begin(); i != newdiff.end(); ++i) {
          std::set< mcrl2::data::data_equation, equation_smaller >::const_iterator j =
             std::find_if(oldset.begin(), oldset.end(), std::bind1st(std::ptr_fun(&detail::alpha_equivalent), *i));

          if (j == oldset.end()) {
            new_differences.append(mcrl2::core::pp(*i)).append(",\n");
            new_differences.append(i->to_string()).append(",\n");
          }
        }

//        if (!new_differences.empty()) {
//          std::clog << "IN LEFT BUT NOT RIGHT: " << std::endl
//                    << new_differences.substr(0, new_differences.size() - 2) << std::endl;
//        }

        return old_differences.empty() && new_differences.empty();
      }

      return true;
    }
  } // namespace data
} // namespace mcrl2
#endif

