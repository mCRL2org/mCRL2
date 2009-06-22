// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// \file: mcrl2/lps/untime.cpp

#include "mcrl2/lps/specification.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/lps/untime.h"

using namespace mcrl2::core;
using namespace mcrl2::data;

namespace mcrl2 {

namespace lps {

///\return specification, in which all delta summands have been removed, and replaced with a single true->delta
static
lps::specification remove_deltas(const lps::specification& spec) {
  lps::specification result;
  lps::summand_list summands;
  lps::summand_list psummands = spec.process().summands();
  for (lps::summand_list::iterator i = psummands.begin(); i != psummands.end(); ++i)
  {
    if (!(i->is_delta()))
    {
      summands = push_front(summands, *i);
    }
  }

  lps::summand delta_summand = summand(variable_list(),
                                       sort_bool::true_(),
                                       true,
                                       action_list(),
                                       assignment_list()
                                      );

  summands = push_front(summands, delta_summand);
  summands = atermpp::reverse(summands);

  result = spec;
  result.process().set_summands(summands);

  return result;
}


///\brief This function returns an expression containing invariants for variables relating to time.
///\details For all parameters x relating to time, the expression 0<=x && x<=last_action_time is returned,
///         provided that in the initial vector the variable x gets the value 0, and in each summand the
///         new value for x is either x, or the value that is assigned to last action time, which is the time
///         tag of the action in that summand.

static
mcrl2::data::data_expression calculate_time_invariant(
                   const lps::specification& spec,
                   const mcrl2::data::variable &last_action_time)
{
  const data::variable_list parameters=spec.process().process_parameters();
  const data::data_expression_list initial_values =spec.initial_process().state();
  const data::data_expression real_zero= sort_real::real_(0);

  // The vector below contains exactly one boolean for each parameter. As long as the value
  // for the elements in the vector is true, it is a candidate time variable.
  std::vector <bool> time_variable_candidates(parameters.size(),true);
  std::vector <bool>::iterator j=time_variable_candidates.begin() ;
  if (core::gsVerbose)
  { std::cerr << "For lpsuntime to function optimally, it is assumed that the input lps is rewritten to normal form\n";
  }
  for(data::data_expression_list::const_iterator k=initial_values.begin();
              k!=initial_values.end() ; ++j, ++k)
  {
    if (*k!=real_zero)
    {
      (*j) = false;
    }
  }
  assert(j==time_variable_candidates.end());

  lps::summand_list summands = spec.process().summands();
  for (lps::summand_list::iterator i = summands.begin(); i != summands.end(); ++i)
  {
    if (!(i->is_delta()))
    { const data::data_expression_list summand_arguments=i->next_state(parameters);
      std::vector <bool>::iterator j=time_variable_candidates.begin();
      data::variable_list::const_iterator l=parameters.begin();
      for( data::data_expression_list::const_iterator k=summand_arguments.begin() ;
                  k!=summand_arguments.end(); ++j, ++k, l++)
      {
        if ((*k!=real_zero)&&(*k!=*l)&&(*k!=i->time()))
        {
          (*j)=false;
        }
      }
      assert(j==time_variable_candidates.end());
    }
  }

  mcrl2::data::data_expression time_invariant(sort_bool::true_());
  j=time_variable_candidates.begin();
  for( data::variable_list::const_iterator k=parameters.begin();
              k!=parameters.end() ; ++j, ++k)
  {
    if (*j)
    {
      data::variable kvar(*k);
      data::variable lat(last_action_time);
      time_invariant=lazy::and_(time_invariant,
                         lazy::and_(data::less_equal(real_zero,kvar),
                                         data::less_equal(kvar,lat)));
    }
  }
  assert(j==time_variable_candidates.end());
  if (core::gsVerbose)
  { std::cerr << "Time invariant " << core::pp(time_invariant) << "\n";
  }
  return time_invariant;
}




///Returns an LPS specification in which the timed arguments have been rewritten
lps::specification untime(const lps::specification& spec) {
  lps::specification untime_specification; // Updated specification
  lps::linear_process lps = spec.process(); // Original lps
  lps::linear_process untime_lps; // Updated lps
  lps::summand_list untime_summand_list; // Updated summand list
  data::variable_vector untime_process_parameters; // Updated process parameters
  data::variable last_action_time; // Extra parameter to display the last action time

  gsVerboseMsg("Untiming %d summands\n", spec.process().summand_count());

  //If an lps has got no time at the initialization, return the original lps with all present delta's removed, and replaced with one true->delta.
  if (!lps.has_time())
  {
    gsVerboseMsg("LPS has no time, only removing deltas, and replacing with one true->delta summand\n");
    return remove_deltas(spec);
  }

  // Create extra parameter last_action_time and add it to the list of process parameters,
  // last_action_time is used later on in the code
  last_action_time = fresh_variable(lps::specification_to_aterm(spec), mcrl2::data::sort_real::real_(), "last_action_time");
  data::data_expression time_invariant=calculate_time_invariant(spec,last_action_time);
  untime_process_parameters = data::make_variable_vector(lps.process_parameters());
  untime_process_parameters.push_back(last_action_time);

  // Transpose the original summand list, and see if there are summands with time
  // If a summand has time, remove it, create new conditions for time, and add it to the new summand list (untime_summand_list)
  // If a summand does not contain time, first introduce time, and then untime it.
  int j = 0; //Counter only used for verbose output (keep track of the summand number
  lps::summand_list summands = lps.summands();
  for (lps::summand_list::iterator i = summands.begin(); i != summands.end(); ++i,++j)
  {
    gsVerboseMsg("Untiming summand %d\n", j);

    // Declarations within scope of for-loop
    data::variable_vector untime_summation_variables; //Updated set of summation variables
    data::data_expression untime_condition; //Updated condition
    lps::summand untime_summand; //Updated summand

    // Only untime summands that are not delta summands; all delta summands are removed, and replaced by one true->delta summand
    if (!(i->is_delta())){
      data::assignment_vector untime_assignments = data::convert< data::assignment_vector >(i->assignments()); //Updated assignments (or next state)

      if (i->has_time())
      {
  // The summand is already timed, therefor there is no need to add an extra summation variable for time
  untime_summation_variables = data::make_variable_vector(i->summation_variables());

  // Extend the original condition with an additional argument t.i(d,e.i)>last_action_time
  untime_condition = lazy::and_(i->condition(),
                           lazy::and_(data::greater(i->time(),data_expression(last_action_time)),
                                   data::greater(data_expression(i->time()), data::sort_real::real_(0))));

  // Extend original assignments to include t.i(d,e.i)
        untime_assignments.push_back(assignment(last_action_time,i->time()));
      }
      else
      {
  // Add a new summation variable (this is allowed because according to an axiom the following equality holds):
  // c -> a . X == sum t:Real . c -> a@t . X
  mcrl2::data::variable time_var = fresh_variable(lps::specification_to_aterm(spec), data::sort_real::real_(), "time_var");
  untime_summation_variables = data::make_variable_vector(i->summation_variables());
        untime_summation_variables.push_back(time_var);

  // Extend the original condition with an additional argument
  untime_condition = lazy::and_(i->condition(),
                           lazy::and_(data::greater(time_var, data_expression(last_action_time)),
                                   data::greater(data_expression(time_var), data::sort_real::real_(0))));


  // Extend original assignments to include t.i(d,e.i)
        untime_assignments.push_back(assignment(last_action_time, time_var));
      } // i->has_time()

      // Add the condition last_action_time>=0, which holds, and which is generally a useful fact for further processing.
      untime_condition = lazy::and_(untime_condition,time_invariant);

      // Create a new summand with the changed parameters
      untime_summand = lps::summand(data::make_variable_list(untime_summation_variables),
            untime_condition,
            i->is_delta(),
            i->actions(),
            data::make_assignment_list(untime_assignments)
            );

      // Add the new summand to the list
      untime_summand_list = push_front(untime_summand_list, untime_summand);

    } // !(i->is_delta())

  }

  // Add delta summand
  lps::summand untime_summand;
  untime_summand = lps::summand(variable_list(),
                                data_expression(sort_bool::true_()),
                                true,
                                action_list(),
                                assignment_list()
                               );

  untime_summand_list = push_front(untime_summand_list, untime_summand);

  // Reverse summand list, because it is the wrong way round now.
  untime_summand_list = atermpp::reverse(untime_summand_list);

  // Create new LPS, this equals lps, except for the new summand list and the additional process parameter.
  untime_lps = lps;
  untime_lps.process_parameters() = data::make_variable_list(untime_process_parameters);
  untime_lps.set_summands(untime_summand_list);

  // Create new initial_variables and initial_state in order to correctly initialize.
  data::assignment_list untime_initial_assignments = spec.initial_process().assignments();
  untime_initial_assignments = push_back(untime_initial_assignments,assignment(last_action_time, sort_real::real_(0)));

  // Create new specification, this equals original specification, except for the new LPS.
  untime_specification = lps::specification(spec.data(),
                          spec.action_labels(),
              untime_lps, //new LPS
              process_initializer(spec.initial_process().global_variables(),
                                            untime_initial_assignments)
             );

  return untime_specification;
}

} // namespace lps

} // namespace mcrl2

