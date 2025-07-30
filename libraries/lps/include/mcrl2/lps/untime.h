// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file untime.h
/// \brief Removes time from a linear process.

#ifndef MCRL2_LPS_UNTIME_H
#define MCRL2_LPS_UNTIME_H

#include "mcrl2/data/fourier_motzkin.h"
#include "mcrl2/lps/detail/lps_algorithm.h"

namespace mcrl2::lps
{

namespace detail
{

template <class INITIALIZER>
INITIALIZER make_process_initializer(const data::data_expression_list& expressions, const INITIALIZER& init);

template<>
inline process_initializer make_process_initializer(const data::data_expression_list& expressions,
    const process_initializer& /* init */)
{
  return lps::process_initializer(expressions);
}

template<>
inline stochastic_process_initializer make_process_initializer(const data::data_expression_list& expressions,
    const stochastic_process_initializer& init)
{
  return stochastic_process_initializer(expressions, init.distribution());
}

} // namespace detail

template <typename Specification>
class untime_algorithm: public detail::lps_algorithm<Specification>
{
  protected:
    using super = typename detail::lps_algorithm<Specification>;
    using process_type = typename Specification::process_type;
    using action_summand_type = typename process_type::action_summand_type;
    using super::m_spec;

    bool m_add_invariants;
    bool m_apply_fm;
    const data::rewriter& m_rewriter;

  protected:
    /// \brief Variable denoting the time at which the last action occurred
    data::variable m_last_action_time;

    /// \brief Data expression expressing the invariant for variables relating to time
    /// \details For all parameters x relating to time, the expression 0<=x && x<=m_last_action_time,
    ///         provided that in the initial vector the variable x gets the value 0, and in each summand the
    ///         new value for x is either x, or the value that is assigned to last action time, which is the time
    ///         tag of the action in that summand.
    data::data_expression m_time_invariant;

    /// \brief Identifier generator, for generating fresh identifiers.
    data::set_identifier_generator m_identifier_generator;

    /// \brief Data expression expressing the invariant for variables relating to time
    ///\details For all parameters x relating to time, the expression 0<=x && x<=m_last_action_time is returned,
    ///         provided that in the initial vector the variable x gets the value 0, and in each summand the
    ///         new value for x is either x, or the value that is assigned to last action time, which is the time
    ///         tag of the action in that summand.
    data::data_expression calculate_time_invariant()
    {
      const data::data_expression real_zero= data::sort_real::real_(0);

      // The vector below contains exactly one boolean for each parameter. As long as the value
      // for the elements in the vector is true, it is a candidate time variable.
      std::vector <bool> time_variable_candidates(m_spec.process().process_parameters().size(),true);
      std::vector <bool>::iterator j=time_variable_candidates.begin() ;
      mCRL2log(log::verbose) << "For untiming to function optimally, it is assumed that the input lps is rewritten to normal form" << std::endl;

      const data::data_expression_list& process_parameters = m_spec.initial_process().expressions();
      for (data::data_expression_list::const_iterator k = process_parameters.begin(); k != process_parameters.end(); ++j, ++k)
      {
        if (*k != real_zero)
        {
          (*j) = false;
        }
      }

      assert(j == time_variable_candidates.end());

      auto const& summands = m_spec.process().action_summands();
      for (auto i = summands.begin(); i != summands.end(); ++i)
      {
        const data::data_expression_list summand_arguments = i->next_state(m_spec.process().process_parameters());
        std::vector <bool>::iterator j = time_variable_candidates.begin();
        data::variable_list::const_iterator l=m_spec.process().process_parameters().begin();
        for (data::data_expression_list::const_iterator k=summand_arguments.begin() ;
             k!=summand_arguments.end(); ++j, ++k, l++)
        {
          if ((*k!=real_zero)&&(*k!=*l)&&(*k!=i->multi_action().time()))
          {
            (*j)=false;
          }
        }
        assert(j==time_variable_candidates.end());
      }

      data::data_expression time_invariant(data::sort_bool::true_());
      j=time_variable_candidates.begin();
      for (data::variable_list::const_iterator k=m_spec.process().process_parameters().begin();
           k!=m_spec.process().process_parameters().end() ; ++j, ++k)
      {
        if (*j)
        {
          data::variable kvar(*k);
          data::variable lat(m_last_action_time);
          time_invariant=data::lazy::and_(time_invariant,
                                          data::lazy::and_(data::less_equal(real_zero,kvar),
                                              data::less_equal(kvar,lat)));
        }
      }
      assert(j==time_variable_candidates.end());
      mCRL2log(log::verbose) << "Time invariant " << data::pp(time_invariant) << std::endl;
      return time_invariant;
    }

    /// \brief Apply untime to an action summand.
    void untime(action_summand_type& s)
    {
      using namespace mcrl2::data;
      if (s.has_time())
      {
        // Extend the original condition with an additional argument t.i(d,e.i)>m_last_action_time && t.i(d,e.i) > 0
        s.condition() = lazy::and_(s.condition(),
                                         lazy::and_(greater(s.multi_action().time(),m_last_action_time),
                                             greater(s.multi_action().time(), sort_real::real_(0))));

        // Extend original assignments to include m_last_action_time := t.i(d,e.i)
        s.assignments()=push_back(s.assignments(),assignment(m_last_action_time,s.multi_action().time()));

        // Remove time
        s.multi_action() = multi_action(s.multi_action().actions());

        // Try to apply Fourier-Motzkin elimination to simplify
        std::set< variable > variables_in_action = process::find_all_variables(s.multi_action());
        std::set< variable > variables_in_assignments = process::find_all_variables(s.assignments());
        // Split the variables that do/do not occur in actions and assignments.
        variable_list do_occur;
        variable_list do_not_occur;

        for(const variable& v: s.summation_variables())
        {
          if (variables_in_action.count(v)>0 || variables_in_assignments.count(v)>0)
          {
            do_occur.push_front(v);
          }
          else
          {
            do_not_occur.push_front(v);
          }
        }

        // Only apply Fourier Motzkin elimination to the new condition if the user
        // explicitly asked for it.
        // The application of Fourier Motzkin can change the structure of the
        // condition significantly. Therefore, its application hides the real
        // outcome of the untime algorithm.
        if(m_apply_fm)
        {
          try
          {
            variable_list remaining_variables;
            data_expression new_condition;
            fourier_motzkin(s.condition(), do_not_occur, new_condition, remaining_variables, m_rewriter);
            s.condition() = new_condition;
            s.summation_variables() = do_occur + remaining_variables;
          }
          catch(mcrl2::runtime_error& e)
          {
            // The application of Fourier Motzkin failed because of mixed Real and
            // non-Real variables. We leave the original condition, but show a
            // warning to the user
            mCRL2log(log::debug) << "Application of Fourier Motzkin failed with the message\n" << e.what() << std::endl;
          }
        }
      }
      else
      {
        // Add a new summation variable (this is allowed because according to an axiom the following equality holds):
        // c -> a . X == sum t:Real . c -> a@t . X
        variable time_var(m_identifier_generator("time_var"), sort_real::real_());
        s.summation_variables().push_front(time_var);

        // Extend the original condition with an additional argument time_var > m_last_action_time && time_var > 0
        s.condition() = lazy::and_(s.condition(),
                                         lazy::and_(greater(time_var, m_last_action_time),
                                             greater(time_var, sort_real::real_(0))));

        // Extend original assignments to include m_last_action_time := time_var
        s.assignments()=push_back(s.assignments(),assignment(m_last_action_time, time_var));
      } // s.has_time()

      // Add the condition m_last_action_time>=0, which holds, and which is generally a useful fact for further processing.
      s.condition() = lazy::and_(s.condition(),m_time_invariant);
    }




  public:
    untime_algorithm(Specification& spec, bool add_invariants, bool apply_fourier_motzkin, const data::rewriter& r)
      : detail::lps_algorithm<Specification>(spec),
        m_add_invariants(add_invariants),
        m_apply_fm(apply_fourier_motzkin),
        m_rewriter(r)
    {
      m_identifier_generator.add_identifiers(lps::find_identifiers(spec));
      m_identifier_generator.add_identifiers(data::function_and_mapping_identifiers(spec.data()));
    }

    void run()
    {
      m_spec.process().deadlock_summands() = deadlock_summand_vector();
      m_spec.process().deadlock_summands().push_back(
        deadlock_summand(data::variable_list(), data::sort_bool::true_(), deadlock()));

      if (m_spec.process().has_time())
      {
        mCRL2log(log::verbose) << "Untiming " << m_spec.process().summand_count() << " summands" << std::endl;

        // Create extra parameter m_last_action_time and add it to the list of process parameters,
        // m_last_action_time is used later on in the code
        m_last_action_time = data::variable(m_identifier_generator("last_action_time"), data::sort_real::real_());
        mCRL2log(log::verbose) << "Introduced variable " << data::pp(m_last_action_time) << " to denote time of last action" << std::endl;

        // Should happen before updating the process
        m_time_invariant = m_add_invariants ? calculate_time_invariant() : (data::data_expression) data::sort_bool::true_();

        m_spec.process().process_parameters()=push_back(m_spec.process().process_parameters(),m_last_action_time);
        data::data_expression_list init = m_spec.initial_process().expressions();
        init = push_back(init, data::sort_real::real_(0));
        m_spec.initial_process() = detail::make_process_initializer(init, m_spec.initial_process());

        for(action_summand_type& s: m_spec.process().action_summands())
        {
          untime(s);
        }
      }
    }
};

} // namespace mcrl2::lps

#endif // MCRL2_LPS_UNTIME_H
