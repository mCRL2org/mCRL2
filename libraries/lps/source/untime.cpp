//Aterms
#include <mcrl2/atermpp/aterm.h>

//LPS framework
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/linear_process.h"
//mCRL2 data
#include "mcrl2/data/data.h"
#include "mcrl2/data/utility.h"

#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"

#include "mcrl2/lps/untime.h"

// For Aterm library extension functions
#ifdef __cplusplus
using namespace ::mcrl2::utilities;
using namespace mcrl2::core;
#endif
using namespace mcrl2::data;
using namespace mcrl2::data::data_expr;
using namespace mcrl2::lps;
using namespace mcrl2;

namespace mcrl2 {

namespace lps {

///\ret specification, in which all delta summands have been removed, and replaced with a single true->delta
lps::specification remove_deltas(const lps::specification& spec) {
  lps::specification result;
  lps::summand_list summands;
  for (lps::summand_list::iterator i = spec.process().summands().begin(); i != spec.process().summands().end(); ++i)
  {
    if (!(i->is_delta()))
    {
      summands = push_front(summands, *i);
    }
  }

  lps::summand delta_summand = summand(data_variable_list(),
                                       true_(),
                                       true,
                                       action_list(),
                                       data_assignment_list()
                                      );

  summands = push_front(summands, delta_summand);
  summands = atermpp::reverse(summands);

  result = set_lps(spec, set_summands(spec.process(), summands));

  return result;
}

///Returns an LPS specification in which the timed arguments have been rewritten
lps::specification untime(const lps::specification& spec) {
  lps::specification untime_specification; // Updated specification
  lps::linear_process lps = spec.process(); // Original lps
  lps::linear_process untime_lps; // Updated lps
  lps::summand_list untime_summand_list; // Updated summand list
  data::data_variable_list untime_process_parameters; // Updated process parameters
  data::data_variable last_action_time; // Extra parameter to display the last action time
  data::data_assignment_list untime_initial_assignments; // Updated initial assignments

  gsVerboseMsg("Untiming %d summands\n", lps.summands().size());
  
  //If an lps has got no time at the initialization, return the original lps with all present delta's removed, and replaced with one true->delta.
  if (!lps.has_time())
  {
    gsVerboseMsg("LPS has no time, only removing deltas, and replacing with one true->delta summand\n");
    return remove_deltas(spec);
  }

  // Create extra parameter last_action_time and add it to the list of process parameters,
  // last_action_time is used later on in the code
  last_action_time = fresh_variable(spec, mcrl2::data::sort_expr::real(), "last_action_time");
  untime_process_parameters = push_back(lps.process_parameters(), last_action_time);
      
  // Transpose the original summand list, and see if there are summands with time
  // If a summand has time, remove it, create new conditions for time, and add it to the new summand list (untime_summand_list)
  // If a summand does not contain time, first introduce time, and then untime it.
  int j = 0; //Counter only used for verbose output (keep track of the summand number
  for (lps::summand_list::iterator i = lps.summands().begin(); i != lps.summands().end(); ++i,++j)
  { 
    gsVerboseMsg("Untiming summand %d\n", j);

    // Declarations within scope of for-loop
    data::data_variable_list untime_summation_variables; //Updated set of summation variables
    data::data_expression untime_condition; //Updated condition
    data::data_assignment_list untime_assignments; //Updated assignments (or next state)
    lps::summand untime_summand; //Updated summand

    // Only untime summands that are not delta summands; all delta summands are removed, and replaced by one true->delta summand
    if (!(i->is_delta())){

      if (i->has_time()) 
      { 
	// The summand is already timed, therefor there is no need to add an extra summation variable for time
	untime_summation_variables = i->summation_variables();

	// Extend the original condition with an additional argument t.i(d,e.i)>last_action_time
	untime_condition = optimized::and_(i->condition(), data::data_expr::greater(i->time(),data_expression(last_action_time)));

	// Extend original assignments to include t.i(d,e.i)
	untime_assignments = push_back(i->assignments(),data_assignment(last_action_time,i->time()));
      }
      else
      {

	// Add a new summation variable (this is allowed because according to an axiom the following equality holds):
	// c -> a . X == sum t:Real . c -> a@t . X
	mcrl2::data::data_variable time_var = fresh_variable(spec, data::sort_expr::real(), "time_var");
	untime_summation_variables = push_back(i->summation_variables(), time_var);

	// Extend the original condition with an additional argument
	untime_condition = optimized::and_(i->condition(), mcrl2::data::data_expr::greater(time_var, data_expression(last_action_time)));


	// Extend original assignments to include t.i(d,e.i)
	untime_assignments = push_back(i->assignments(),
                                      data_assignment(last_action_time, time_var)
                                     );
      } // i->has_time()

      // Create a new summand with the changed parameters
      untime_summand = lps::summand(untime_summation_variables,
					  untime_condition,
					  i->is_delta(),
					  i->actions(),
					  untime_assignments
					  );

      // Add the new summand to the list
      untime_summand_list = push_front(untime_summand_list, untime_summand);

    } // !(i->is_delta())

  }

  // Add delta summand
  lps::summand untime_summand;
  untime_summand = lps::summand(data_variable_list(),
                                data_expression(true_()),
                                true,
                                action_list(),
                                data_assignment_list()
                               );

  untime_summand_list = push_front(untime_summand_list, untime_summand);
  
  // Reverse summand list, because it is the wrong way round now.
  untime_summand_list = atermpp::reverse(untime_summand_list);
      
  // Create new LPS, this equals lps, except for the new summand list and the additional process parameter.
  untime_lps = lps::linear_process(lps.free_variables(), untime_process_parameters, untime_summand_list);

  // Create new initial_variables and initial_state in order to correctly initialize.
  untime_initial_assignments = push_back(spec.initial_process().assignments(), data_assignment(last_action_time, real(0)));

  // Create new specification, this equals original specification, except for the new LPS.
  untime_specification = lps::specification(spec.data(), 
			                    spec.action_labels(),
					    untime_lps, //new LPS
					    process_initializer(spec.initial_process().free_variables(),
                                            untime_initial_assignments)
					   );

  return untime_specification;
}

} // namespace lps

} // namespace mcrl2

