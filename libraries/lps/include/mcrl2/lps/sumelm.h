// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file sumelm.h
/// \brief Provides an implemenation of the sum elimination lemma,
///        as well as the removal of unused summation variables.
///        The sum elimination lemma is the following:
///          sum d:D . d == e -> X(d) = X(e).
///        Removal of unused summation variables is according to the
///        following lemma:
///          d not in x implies sum d:D . x = x

#ifndef MCRL2_LPS_SUMELM_H
#define MCRL2_LPS_SUMELM_H

#include "mcrl2/data/map_substitution_adapter.h"
#include "mcrl2/lps/detail/lps_algorithm.h"

namespace mcrl2 {
  namespace lps {

    /// \brief Class implementing the sum elimination lemma.
    class sumelm_algorithm: public lps::detail::lps_algorithm
    {
      protected:
        /// Adds replacement lhs := rhs to the specified map of replacements.
        /// All replacements that have lhs as a right hand side will be changed to
        /// have rhs as a right hand side.
        void sumelm_add_replacement(std::map<data::variable, data::data_expression>& replacements,
                                    const data::variable& lhs,
                                    const data::data_expression& rhs)
        {
          using namespace mcrl2::data;
          // First apply already present substitutions to rhs
          data_expression new_rhs(replace_free_variables(rhs, make_map_substitution_adapter(replacements)));
          for (std::map<variable, data_expression>::iterator i = replacements.begin(); i != replacements.end(); ++i)
          {
            i->second = assignment(lhs, new_rhs)(i->second);
          }
          replacements[lhs] = new_rhs;
        }

        /// Recursively apply sum elimination on a summand.
        /// We build up a list of substitutions that need to be made in substitutions
        /// the caller of this function needs to apply substitutions to the summand
        /// once we exit recursion
        /// working_condition is a parameter that we use to split up the problem,
        /// at the first call of this function working_condition == summand_->condition()
        /// should hold.
        /// The new condition is built up on the return path of the recursion, so
        /// the last exit of the recursion is the new condition of the summand.
        data::data_expression recursive_substitute_equalities(const summand_base& summand_,
                                                              data::data_expression working_condition,
                                                              std::map<data::variable, data::data_expression>& substitutions)
        {
          using namespace mcrl2::data;

          // In all cases not explicitly handled we return the original working_condition
          data_expression result = working_condition;

          if (sort_bool::is_and_application(working_condition))
          {
            //Recursively apply sum elimination on lhs and rhs
            //Note that recursive application provides for progress because lhs and rhs split the working condition.
            data_expression a,b;
            a = recursive_substitute_equalities(summand_, application(working_condition).left(), substitutions);
            b = recursive_substitute_equalities(summand_, application(working_condition).right(), substitutions);
            result = lazy::and_(a,b);
          }

          else if (is_equal_to_application(working_condition))
          {
            //Check if rhs is a variable, if so, swap lhs and rhs, so that the following code
            //is always the same.
            if (!application(working_condition).left().is_variable() && application(working_condition).right().is_variable() &&
                data::search_variable(summand_.summation_variables(), application(working_condition).right()))
            {
              working_condition = data::equal_to(application(working_condition).right(), application(working_condition).left());
            }

            //If lhs is a variable, check if it occurs in the summation variables, if so
            //apply substitution lhs := rhs in actions, time and assignments.
            //substitution in condition is accounted for on return path of recursion,
            //substitution in summation_variables is done in calling function.
            if (application(working_condition).left().is_variable())
            {
              if (data::search_variable(summand_.summation_variables(), variable(application(working_condition).left())) &&
                  !search_data_expression(application(working_condition).right(), application(working_condition).left()))
              {
                if (substitutions.count(application(working_condition).left()) == 0)
                {
                  // apply all previously added substitutions to the rhs.
                  sumelm_add_replacement(substitutions, application(working_condition).left(), application(working_condition).right());
                  result = sort_bool::true_();
                } else if (application(working_condition).right().is_variable() &&
                           data::search_variable(summand_.summation_variables(), variable(application(working_condition).right()))) {
                  // check whether the converse is possible
                  if (substitutions.count(application(working_condition).right()) == 0) {
                    sumelm_add_replacement(substitutions, application(working_condition).right(), substitutions[application(working_condition).left()]);
                    result = sort_bool::true_();
                  }
                } else if (substitutions.count(substitutions[application(working_condition).left()]) == 0 &&
                             substitutions[application(working_condition).left()].is_variable() &&
                             data::search_variable(summand_.summation_variables(), variable(substitutions[application(working_condition).left()]))) {
                  sumelm_add_replacement(substitutions, substitutions[application(working_condition).left()], application(working_condition).right());
                  sumelm_add_replacement(substitutions, application(working_condition).left(), application(working_condition).right());
                  result = sort_bool::true_();
                }
              }
            }
          }
          return result;
        }

      public:
        /// \brief Constructor.
        /// \param spec The specification to which sum elimination should be
        ///             applied.
        /// \param verbose Control whether verbose output should be given.
        sumelm_algorithm(specification& spec, bool verbose = false)
          : lps::detail::lps_algorithm(spec, verbose)
        {}

        /// \brief Apply the sum elimination lemma to all summands in the
        ///        specification.
        void run()
        {
          std::for_each(m_spec.process().action_summands().begin(), m_spec.process().action_summands().end(), (*this));
          std::for_each(m_spec.process().deadlock_summands().begin(), m_spec.process().deadlock_summands().end(), (*this));
        }

        /// \brief Apply the sum elimination lemma to summand s.
        /// \param s an action_summand.
        void operator()(action_summand& s)
        {
          using namespace data;

          std::map<variable, data_expression> substitutions;
          data_expression new_condition = recursive_substitute_equalities(s, s.condition(), substitutions);

          s.condition() = make_map_substitution_adapter(substitutions)(new_condition);
          s.multi_action().actions() = replace_free_variables(s.multi_action().actions(), make_map_substitution_adapter(substitutions));
          s.multi_action().time() = make_map_substitution_adapter(substitutions)(s.multi_action().time());
          s.assignments() = replace_free_variables(s.assignments(), make_map_substitution_adapter(substitutions));

          remove_unused_summand_variables(s);
        }

        /// \brief Apply the sum elimination lemma to summand s.
        /// \param s a deadlock_summand.
        void operator()(deadlock_summand& s)
        {
          using namespace data;

          std::map<variable, data_expression> substitutions;
          data_expression new_condition = recursive_substitute_equalities(s, s.condition(), substitutions);

          s.condition() = make_map_substitution_adapter(substitutions)(new_condition);
          s.deadlock().time() = make_map_substitution_adapter(substitutions)(s.deadlock().time());

          remove_unused_summand_variables(s);
        }
    };

    /// \brief Apply the sum elimination lemma to summand s.
    /// \param s an action summand
    /// \return s to which the sum elimination lemma has been applied.
    inline
    void sumelm(action_summand& s)
    {
      specification spec;
      sumelm_algorithm algorithm(spec);
      algorithm(s);
    }

    /// \brief Apply the sum elimination lemma to summand s.
    /// \param s a deadlock summand
    /// \return s to which the sum elimination lemma has been applied.
    inline
    void sumelm(deadlock_summand& s)
    {
      specification spec;
      sumelm_algorithm algorithm(spec);
      algorithm(s);
    }

  } // namespace lps
} // namespace mcrl2

#endif // MCRL2_LPS_SUMELM_H

