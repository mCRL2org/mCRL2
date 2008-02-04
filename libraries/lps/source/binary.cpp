//Aterms
#include <mcrl2/atermpp/algorithm.h>
#include <mcrl2/atermpp/aterm.h>
#include <mcrl2/atermpp/table.h>

//LPS Framework
#include <mcrl2/data/data.h>
#include <mcrl2/data/utility.h>
#include <mcrl2/data/data_operation.h>
#include <mcrl2/lps/linear_process.h>
#include <mcrl2/data/sort_utility.h>
#include <mcrl2/lps/specification.h>
#include <mcrl2/lps/detail/specification_utility.h>
#include <mcrl2/core/messaging.h>
#include <mcrl2/utilities/aterm_ext.h>
//Enumerator
#include <mcrl2/data/enum/standard.h>
#include <mcrl2/lps/nextstate.h>

#include <mcrl2/lps/binary.h>

// For Aterm library extension functions
#ifdef __cplusplus
using namespace ::mcrl2::utilities;
using namespace mcrl2::core;
#endif
using namespace atermpp;
using namespace mcrl2::data;
using namespace mcrl2::lps;
using namespace mcrl2::data::data_expr;

namespace mcrl2 {

namespace lps {

//////////////////////////////////////////////////////
/// General helper functions
//////////////////////////////////////////////////////

///\pre n>0
///\ret ceil(log_2(n))
int log2(int n)
{
  int result = 0;
  if (n == 0)
  {
    gsErrorMsg("Domain cannot be empty\n");
  }

  --n;
  //n is the maximal value to be represented
  for ( ; n>0; n = n/2)
  {
    result ++;
  }
  return result;
}

///\ret 2^n
int powerof2_(int n)
{
  int result = 1;
  for ( ; n>0; --n)
  {
    result = result * 2;
  }

  return result;
}

///\pre cl is a list of constructors
///\ret all sorts s in sl that are finite and not bool
sort_expression_list get_finite_sorts_not_bool(const data_operation_list& cl, const sort_expression_list& sl)
{
  sort_expression_list result;
  for(sort_expression_list::iterator i = sl.begin(); i != sl.end(); ++i)
  {
    if (!is_bool(*i) && is_finite(cl, *i))
    {
      result = push_front(result, *i);
    }
  }
  reverse(result);
  return result;
}

///\pre 0 <= n <= list.size() = m, list is [0..m), list == original list
///\post list contains elements [n..m)
///\ret list containing elements [0..n)
data_expression_list split_at(data_expression_list& list, unsigned int n)
{
  assert (n <= list.size());

  data_expression_list result;
  ///invariant: list.size() + result.size() == m
  for (unsigned int j = 0; j < n; ++j)
  {
    result = push_front(result, list.front());
    list = pop_front(list);
  }
  result = reverse(result);
  // result ++ list == original list

  return result;
}

///////////////////////////////////////////////////////////////////////////////
/// Binary specific helper functions
///////////////////////////////////////////////////////////////////////////////

///\pre enumerated_elements.size() <= 2^new_parameters.size()
///\ret if then else tree from enumerated_elements in terms of new_parameters
data_expression make_if_tree(const data_variable_list& new_parameters,
                             const data_expression_list& enumerated_elements)
{
  //gsDebugMsg("New parameters: %s\n", new_parameters.to_string().c_str());
  //gsDebugMsg("Enumerated elements: %s\n", enumerated_elements.to_string().c_str());
  int n,m;
  data_expression result;

  if (new_parameters.empty())
  {
    result = enumerated_elements.front();
  }
  else
  {
    n = enumerated_elements.size();
    m = powerof2_(new_parameters.size() - 1);
    //m == 2^(new_parameters.size() - 1)

    if (m > n)
    {
      m = n;
    }

    // Splitting a list in two parts is quite cumbersome at the moment, Wieger
    // will implement something better for this.
    data_expression_list left_list, right_list;
    right_list = enumerated_elements;
    left_list = split_at(right_list, m);

    if (m == n) // Make sure there is at least one element in right_list (the last element in the list)
    {
      right_list = enumerated_elements;
      split_at(right_list, m-1);
    }
    // Build a recursive if tree
    result = if_(new_parameters.front(),
                 make_if_tree(pop_front(new_parameters),
                              right_list),
                 make_if_tree(pop_front(new_parameters),
                              left_list)
                 );
  }

  //gsDebugMsg("If tree: %s\n", result.to_string().c_str());
  return result;
}

// Take a specification and calculate a vector of boolean variables for each process
// parameter of a finite sort. A mapping variable -> vector of booleans is stored in new_parameters_table
// a mapping variable -> enumerated elements is stored in enumerated_elements_table
/// \ret data variable list with the new process parameters (i.e. with all variables of a
/// finite type != bool replaced by a vector of boolean variables.
data_variable_list replace_enumerated_parameters(const lps::specification& specification,
                                                 EnumeratorStandard& enumerator,
                                                 table& new_parameters_table,
                                                 table& enumerated_elements_table)
{
  data_variable_list result;
  data_variable_list process_parameters = specification.process().process_parameters();
  gsDebugMsg("Original process parameters: %s\n", process_parameters.to_string().c_str());

  fresh_variable_generator generator = fresh_variable_generator(aterm(specification));
  generator.set_sort(sort_expr::bool_());

  // Transpose all process parameters, and replace those that are finite, and not bool with boolean variables.
  for (data_variable_list::iterator i = process_parameters.begin(); i != process_parameters.end(); ++i)
  {
    data_variable par = *i;
    if (!is_bool(par) && is_finite(specification.data().constructors(), par.sort()))
    {
      //Get all constructors for par
      aterm_list pl;
      pl = push_front(pl, aterm(par)); //Needed because findSolutions wants a list
      EnumeratorSolutions* sols = enumerator.findSolutions(pl, aterm(par), false, NULL);
      ATermList sol;


      data_variable_list new_pars; // List to store new parameters
      data_expression_list enumerated_elements; // List to store enumerated elements of a parameter
      int j = 0;
      while (sols->next(&sol))
      {
        aterm_list solution = aterm_list(sol);

        for (aterm_list::iterator i = solution.begin(); i != solution.end(); ++i)
        {
          ++j;
          data_expression res = data_expression(aterm_appl(enumerator.getRewriter()->fromRewriteFormat(ATgetArgument(ATerm(*i),1))));
          enumerated_elements = push_front(enumerated_elements, res);
        }

      }
      // j = enumerated_elements.size()
      enumerated_elements = reverse(enumerated_elements);

      enumerated_elements_table.put(par , enumerated_elements); // Store enumerated elements for easy retrieval later on.

      //Calculate the number of booleans needed to encode par
      int n = log2(j);
      // n = ceil(log_2(j)), so also 2^n <= j
      gsVerboseMsg("Parameter `%s' has been replaced by %d parameters of type bool\n", par.to_string().c_str(), n);

      //Set hint for fresh variable names
      generator.set_hint(par.name());

      //Create new parameters and add them to the parameter list.
      for (int i = 0; i<n; ++i)
      {
        new_pars = push_front(new_pars, generator());
      }
      // n = new_pars.size() && new_pars.size() = ceil(log_2(j)) && new_pars.size() = ceil(log_2(enumerated_elements.size()))

      new_pars = reverse(new_pars);

      //Store new parameters in a hastable
      new_parameters_table.put(par, new_pars);

      //Add new parameters to the result
      result = new_pars + result;
    }
    else
    {
      gsVerboseMsg("Parameter `%s' has not been replaced by parameters of type Bool\n", par.to_string().c_str());
      result = push_front(result, par);
    }
  }

  result = reverse(result);
  gsDebugMsg("New process parameters: %s\n", result.to_string().c_str());
  return result;
}


//////////////////////////////////////////////////////
/// Binary
/////////////////////////////////////////////////////

///Replace all occurrences of variables of a finite sort != bool in expression with an if-then-else tree
///of boolean variables
data_expression replace_enumerated_parameters_in_data_expression(data_expression expression,
                                                                 table& new_parameters_table,
                                                                 table& enumerated_elements_table)
{
  gsDebugMsg("replace enumerated parameters in data expression\n");
  data_variable_list orig_parameters = data_variable_list(new_parameters_table.table_keys());
  for (data_variable_list::iterator i = orig_parameters.begin(); i != orig_parameters.end(); ++i)
  {
    gsDebugMsg("Replacing data expression %s with tree %s\n", expression.to_string().c_str(), make_if_tree(new_parameters_table.get(*i), enumerated_elements_table.get(*i)).to_string().c_str());
    expression = data_expression(bottom_up_replace(expression, *i, make_if_tree(new_parameters_table.get(*i), enumerated_elements_table.get(*i))));
  }
  return expression;
}

///Replace all occurrences of variables of a finite sort != bool with a vector of boolean variables
data_variable_list replace_enumerated_parameters_in_data_variables(const data_variable_list& list,
                                                                   table& new_parameters_table,
                                                                   table& enumerated_elements_table)
{
  gsDebugMsg("replace enumerated parameters in data variables\n");
  data_variable_list result;
  for (data_variable_list::iterator i = list.begin(); i != list.end(); ++i)
  {
    aterm t = new_parameters_table.get(*i);
    if (t != NULL) // *i is not of a finite type, therefore it hasn't been stored in the tables.
    {
      data_variable_list new_variables = data_variable_list(t);
      result = new_variables + result;
    }
    else
    {
      result = push_front(result, *i);
    }
  }
  result = reverse(result);
  return result;
}

///Calculate the new assignments for parameter, this returns a data_assignment_list with || expressions as the
///righthandsides.
data_assignment_list replace_enumerated_parameter_in_data_assignment(const data_assignment& argument,
                                                                     const data_expression& parameter,
                                                                     data_variable_list new_parameters,
                                                                     const data_expression_list& enumerated_elements)
{
  gsDebugMsg("replace enumerated parameter %s in data assigment %s\n", parameter.to_string().c_str(), argument.to_string().c_str());
  data_assignment_list result;
  data_expression arg = argument.rhs();

  // Iterate over the parameters, i.e. the bools in which we encode
  for (int i = new_parameters.size(); i > 0; --i)
  {
    data_expression r = false_(); // We make a big || expression, so start with unit false
    data_expression_list elts = enumerated_elements; // Copy the enumerated elements, as these are needed for each iteration.

    // Make sure all elements get encoded.
    while (!elts.empty())
    {
      // Iterate over the elements that get the boolean value new_parameters.front() == false
      for(int j = 0; j < powerof2_(i-1); ++j)
      {
        if (!elts.empty())
        {
          elts = pop_front(elts);
        }
      }

      // Iterate over the elements that get the boolean value new_parameters.front() == true
      for(int j = 0; j < powerof2_(i-1); ++j)
      {
        if (!elts.empty())
        {
          r = or_(r, data::data_expr::equal_to(arg, elts.front()));
          elts = pop_front(elts);
        }
      }
    }

    result = push_front(result, data_assignment(new_parameters.front(), r));
    new_parameters = pop_front(new_parameters);
  }

  return result;
}

///Replace all assignments in which the lefthandside == parameter with a vector of boolean assignments.
data_assignment_list replace_enumerated_parameter_in_data_assignments(const data_assignment_list& list,
                                                                      const data_expression& parameter,
                                                                      const data_variable_list& new_parameters,
                                                                      const data_expression_list& enumerated_elements)
{
  gsDebugMsg("replace enumerated parameter %s in data assignments %s\n", parameter.to_string().c_str(), list.to_string().c_str());
  data_assignment_list result;

  for (data_assignment_list::iterator i = list.begin(); i != list.end(); ++i)
  {
    if (i->lhs() != parameter)
    {
      result = result + *i;
    }
    else
    {
      result = result + replace_enumerated_parameter_in_data_assignment(*i, parameter, new_parameters, enumerated_elements);
    }
  }

  return result;
}

///Replace all assignments of finite sorts != bool with a vector of boolean assignments.
data_assignment_list replace_enumerated_parameters_in_data_assignments(const data_assignment_list& list,
                                                                       table& new_parameters_table,
                                                                       table& enumerated_elements_table)
{
  gsDebugMsg("replace_enumerated_parameters_in_data_assignments %s\n", list.to_string().c_str());
  data_assignment_list result, temp;
  // First replace right-hand-sides
  for (data_assignment_list::iterator i = list.begin(); i != list.end(); ++i)
  {
    result = push_front(result, data_assignment(i->lhs(),replace_enumerated_parameters_in_data_expression(i->rhs(), new_parameters_table, enumerated_elements_table)));
  }

  result = reverse(result);

  data_variable_list orig_parameters = data_variable_list(new_parameters_table.table_keys());
  for (data_variable_list::iterator i = orig_parameters.begin(); i != orig_parameters.end(); ++i)
  {
    result = replace_enumerated_parameter_in_data_assignments(result, *i, new_parameters_table.get(*i), enumerated_elements_table.get(*i));
  }

  return result;
}

///Replace all parameters of finite sorts != bool in list with an if tree of booleans.
action_list replace_enumerated_parameters_in_actions(action_list list,
                                                     table& new_parameters_table,
                                                     table& enumerated_elements_table)
{
  gsDebugMsg("replace enumerated parameters in action labels\n");
  data_variable_list orig_parms = data_variable_list(new_parameters_table.table_keys());
  for (data_variable_list::iterator i = orig_parms.begin(); i != orig_parms.end(); ++i)
  {
    list = bottom_up_replace(list, *i, make_if_tree(new_parameters_table.get(*i), enumerated_elements_table.get(*i)));
  }
  return list;
}

///Replace all parameters of finite sorts != bool in summand with a vector of booleans
summand replace_enumerated_parameters_in_summand(const summand& summand_,
                                                     table& new_parameters_table,
                                                     table& enumerated_elements_table)
{
  gsDebugMsg("replace enumerated parameters in summand %s\n", summand_.to_string().c_str());
  summand result;

  gsDebugMsg("\nOriginal condition: %s\n\n New condition: %s\n\n", summand_.condition().to_string().c_str(), replace_enumerated_parameters_in_data_expression(summand_.condition(), new_parameters_table, enumerated_elements_table).to_string().c_str());

  result = summand(summand_.summation_variables(),
                       replace_enumerated_parameters_in_data_expression(summand_.condition(), new_parameters_table, enumerated_elements_table),
                       summand_.is_delta(),
                       replace_enumerated_parameters_in_actions(summand_.actions(), new_parameters_table, enumerated_elements_table),
                       replace_enumerated_parameters_in_data_expression(summand_.time(), new_parameters_table, enumerated_elements_table),
                       replace_enumerated_parameters_in_data_assignments(summand_.assignments(), new_parameters_table, enumerated_elements_table));

  return result;
}

///Replace all parameters of finite sorts != bool in list with a vector of booleans
summand_list replace_enumerated_parameters_in_summands(const summand_list& list,
                                                       table& new_parameters_table,
                                                       table& enumerated_elements_table)
{
  gsDebugMsg("replace enumerated parameters in summands\n");
  summand_list result;
  for (summand_list::iterator i = list.begin(); i != list.end(); ++i)
  {
    result = push_front(result, replace_enumerated_parameters_in_summand(*i, new_parameters_table, enumerated_elements_table));
  }
  result = reverse(result);

  return result;
}

///Replace all parameters of finite sorts != bool in lps with a vector of booleans
linear_process replace_enumerated_parameters_in_lps(const lps::linear_process& lps,
                                         table& new_parameters_table,
                                         table& enumerated_elements_table)
{
  gsDebugMsg("replace enumerated parameters in linear process\n");
  linear_process result;

  result = linear_process(lps.free_variables(),
               replace_enumerated_parameters_in_data_variables(lps.process_parameters(), new_parameters_table, enumerated_elements_table),
               replace_enumerated_parameters_in_summands(lps.summands(), new_parameters_table, enumerated_elements_table));

  return result;
}

/// Replace all parameters of finite sorts != bool in specification with a vector of booleans
specification replace_enumerated_parameters_in_specification(const lps::specification& specification,
                                                             table& new_parameters_table,
                                                             table& enumerated_elements_table)
{
  gsDebugMsg("replace enumerated parameters in specification\n");
  lps::specification result;

  // Compute new initial assignments
  data_assignment_list initial_assignments = replace_enumerated_parameters_in_data_assignments(specification.initial_process().assignments(), new_parameters_table, enumerated_elements_table);
  process_initializer initial_process(specification.initial_process().free_variables(), initial_assignments);

  // Compute new specification
  result = lps::specification(specification.data(),
                              specification.action_labels(),
                              replace_enumerated_parameters_in_lps(specification.process(), new_parameters_table, enumerated_elements_table),
                              initial_process);

  return result;
}

///Takes the specification in specification, applies binary to it,
///and returns the new specification.
specification binary(const lps::specification& spec,
                     Rewriter& r)
{
  gsVerboseMsg("Executing binary...\n");
  specification result = spec;
  // table new_parameters_table = table(128, 50); Table is non copyable since 30/4/2007.
  // table enumerated_elements_table = table(128,50);
  table new_parameters_table(128, 50);
  table enumerated_elements_table(128,50);

  EnumeratorStandard enumerator = EnumeratorStandard(spec.data(), &r);

  // This needs to be done in a counter-intuitive order because of the well-typedness checks
  // (they make sure we can't build up an intermediate result!)
  data_variable_list new_process_parameters = replace_enumerated_parameters(result, enumerator, new_parameters_table, enumerated_elements_table);
  result = replace_enumerated_parameters_in_specification(result, new_parameters_table, enumerated_elements_table);
  result = set_lps(result, set_process_parameters(result.process(), new_process_parameters));
  gsDebugMsg("Finished processing\n");

  return result;
}

} // namespace lps

} // namespace mcrl2

