// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file untime.h
/// \brief Removes time from a linear process.

#ifndef MCRL2_LPS_UNTIME_H
#define MCRL2_LPS_UNTIME_H

#include "mcrl2/lps/detail/lps_algorithm.h"
#include "mcrl2/data/set_identifier_generator.h"

namespace mcrl2
{

namespace lps
{

template <typename Specification>
class untime_algorithm: public detail::lps_algorithm<Specification>
{
  typedef typename detail::lps_algorithm<Specification> super;
  typedef typename Specification::process_type process_type;
  typedef typename process_type::action_summand_type action_summand_type;
  using super::m_spec;

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

      for (data::data_expression_list::const_iterator k = m_spec.initial_process().state(m_spec.process().process_parameters()).begin();
           k != m_spec.initial_process().state(m_spec.process().process_parameters()).end(); ++j, ++k)
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

    /// \brief Apply untime to an action summand
    void untime(action_summand_type& s)
    {
      if (s.has_time())
      {
        // Extend the original condition with an additional argument t.i(d,e.i)>m_last_action_time && t.i(d,e.i) > 0
        s.condition() = data::lazy::and_(s.condition(),
                                         data::lazy::and_(data::greater(s.multi_action().time(),m_last_action_time),
                                             data::greater(s.multi_action().time(), data::sort_real::real_(0))));

        // Extend original assignments to include m_last_action_time := t.i(d,e.i)
        s.assignments()=push_back(s.assignments(),data::assignment(m_last_action_time,s.multi_action().time()));

        // Remove time
        s.multi_action() = multi_action(s.multi_action().actions()); // TODO: if Nil is removed, just remove time
      }
      else
      {
        // Add a new summation variable (this is allowed because according to an axiom the following equality holds):
        // c -> a . X == sum t:Real . c -> a@t . X
        data::variable time_var(m_identifier_generator("time_var"), data::sort_real::real_());
        s.summation_variables().push_front(time_var);

        // Extend the original condition with an additional argument time_var > m_last_action_time && time_var > 0
        s.condition() = data::lazy::and_(s.condition(),
                                         data::lazy::and_(data::greater(time_var, m_last_action_time),
                                             data::greater(time_var, data::sort_real::real_(0))));

        // Extend original assignments to include m_last_action_time := time_var
        s.assignments()=push_back(s.assignments(),data::assignment(m_last_action_time, time_var));
      } // i->has_time()

      // Add the condition m_last_action_time>=0, which holds, and which is generally a useful fact for further processing.
      s.condition() = data::lazy::and_(s.condition(),m_time_invariant);
    }

  public:
    untime_algorithm(Specification& spec)
      : detail::lps_algorithm<Specification>(spec)
    {
      m_identifier_generator.add_identifiers(lps::find_identifiers(spec));
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
        m_time_invariant = calculate_time_invariant();

        m_spec.process().process_parameters()=push_back(m_spec.process().process_parameters(),m_last_action_time);
        data::assignment_list init = m_spec.initial_process().assignments();
        init=push_back(init,data::assignment(m_last_action_time, data::sort_real::real_(0)));
        m_spec.initial_process() = process_initializer(init);

        std::for_each(m_spec.process().action_summands().begin(),
                      m_spec.process().action_summands().end(),
                      std::bind(&untime_algorithm::untime, this, std::placeholders::_1));
      }
    }
};

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_UNTIME_H
