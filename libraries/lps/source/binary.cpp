// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file binary.cpp
/// \brief

#include <cmath>

#include "mcrl2/atermpp/table.h"
#include "mcrl2/lps/specification.h"

//Enumerator
#include "mcrl2/data/classic_enumerator.h"
#include "mcrl2/data/enumerator_factory.h"
#include "mcrl2/data/fresh_variable_generator.h"

#include "mcrl2/core/messaging.h"
#include "mcrl2/lps/binary.h"

using namespace mcrl2::data;

namespace mcrl2 {

namespace lps {

///\pre 0 <= n <= list.size() = m, list is [0..m), list == original list
///\post list contains elements [n..m)
///\return list containing elements [0..n)
static
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
///\return if then else tree from enumerated_elements in terms of new_parameters
static
data_expression make_if_tree(const variable_list& new_parameters,
                             const data_expression_list& enumerated_elements)
{
  //core::gsDebugMsg("New parameters: %s\n", new_parameters.to_string().c_str());
  //core::gsDebugMsg("Enumerated elements: %s\n", enumerated_elements.to_string().c_str());
  data_expression result;

  if (new_parameters.empty())
  {
    result = enumerated_elements.front();
  }
  else
  {
    int n = enumerated_elements.size();
	int m = static_cast<int>(pow(static_cast<double>(2), static_cast<double>(new_parameters.size() - 1)));

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

  //core::gsDebugMsg("If tree: %s\n", result.to_string().c_str());
  return result;
}

// Take a specification and calculate a vector of boolean variables for each process
// parameter of a finite sort. A mapping variable -> vector of booleans is stored in new_parameters_table
// a mapping variable -> enumerated elements is stored in enumerated_elements_table
/// \return data variable list with the new process parameters (i.e. with all variables of a
/// finite type != bool replaced by a vector of boolean variables.
static
variable_list replace_enumerated_parameters(const lps::specification& specification,
                                                 data::enumerator_factory< classic_enumerator< > > const& classic_enumerator_factory,
                                                 atermpp::table& new_parameters_table,
                                                 atermpp::table& enumerated_elements_table)
{
  variable_list result;
  variable_list process_parameters = specification.process().process_parameters();
  core::gsDebugMsg("Original process parameters: %s\n", process_parameters.to_string().c_str());

  fresh_variable_generator generator(specification_to_aterm(specification));
  // Transpose all process parameters, and replace those that are finite, and not bool with boolean variables.
  for (variable_list::iterator i = process_parameters.begin(); i != process_parameters.end(); ++i)
  {
    variable par = *i;

    if (!sort_bool::is_bool(par) && specification.data().is_certainly_finite(par.sort()))
    {
      //Get all constructors for par
      variable_list new_pars; // List to store new parameters
      data_expression_vector enumerated_elements; // List to store enumerated elements of a parameter

      for (classic_enumerator< > j(classic_enumerator_factory.make(par)); j != classic_enumerator< >(); ++j)
      {
        enumerated_elements.push_back((*j)(par));
      }

      enumerated_elements_table.put(par, make_data_expression_list(enumerated_elements)); // Store enumerated elements for easy retrieval later on.

      //Calculate the number of booleans needed to encode par
      int n = static_cast<int>(ceil(log(static_cast<double>(enumerated_elements.size())) / log(static_cast<double>(2))));//log2(enumerated_elements.size());

      // n = ceil(log_2(j)), so also 2^n <= j
      core::gsVerboseMsg("Parameter `%s' has been replaced by %d parameters of type bool\n", par.to_string().c_str(), n);

      //Set hint for fresh variable names
      generator.set_hint(par.name());

      //Create new parameters and add them to the parameter list.
      for (int i = 0; i<n; ++i)
      {
        new_pars = push_front(new_pars, generator(sort_bool::bool_()));
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
      core::gsVerboseMsg("Parameter `%s' has not been replaced by parameters of type Bool\n", par.to_string().c_str());
      result = push_front(result, par);
    }
  }

  result = reverse(result);
  core::gsDebugMsg("New process parameters: %s\n", result.to_string().c_str());
  return result;
}


//////////////////////////////////////////////////////
/// Binary
/////////////////////////////////////////////////////

///Replace all occurrences of variables of a finite sort != bool in expression with an if-then-else tree
///of boolean variables
static
data_expression replace_enumerated_parameters_in_data_expression(data_expression expression,
                                                                 atermpp::table& new_parameters_table,
                                                                 atermpp::table& enumerated_elements_table)
{
  core::gsDebugMsg("replace enumerated parameters in data expression\n");
  variable_list orig_parameters = variable_list(new_parameters_table.table_keys());
  for (variable_list::iterator i = orig_parameters.begin(); i != orig_parameters.end(); ++i)
  {
    core::gsDebugMsg("Replacing data expression %s with tree %s\n", expression.to_string().c_str(), make_if_tree(new_parameters_table.get(*i), enumerated_elements_table.get(*i)).to_string().c_str());
    expression = data_expression(bottom_up_replace(expression, *i, make_if_tree(new_parameters_table.get(*i), enumerated_elements_table.get(*i))));
  }
  return expression;
}

///Replace all occurrences of variables of a finite sort != bool with a vector of boolean variables
static
variable_list replace_enumerated_parameters_in_variables(const variable_list& list,
                                                                   atermpp::table& new_parameters_table,
                                                                   atermpp::table& enumerated_elements_table)
{
  core::gsDebugMsg("replace enumerated parameters in data variables\n");
  variable_list result;
  for (variable_list::iterator i = list.begin(); i != list.end(); ++i)
  {
    atermpp::aterm t = new_parameters_table.get(*i);
    if (t != NULL) // *i is not of a finite type, therefore it hasn't been stored in the tables.
    {
      variable_list new_variables = variable_list(t);
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

///Calculate the new assignments for parameter, this returns a assignment_list with || expressions as the
///righthandsides.
static
assignment_list replace_enumerated_parameter_in_assignment(const assignment& argument,
                                                                     const data_expression& parameter,
                                                                     variable_list new_parameters,
                                                                     const data_expression_list& enumerated_elements)
{
  core::gsDebugMsg("replace enumerated parameter %s in data assigment %s\n", parameter.to_string().c_str(), argument.to_string().c_str());
  assignment_list result;
  data_expression arg = argument.rhs();

  // Iterate over the parameters, i.e. the bools in which we encode
  for (int i = new_parameters.size(); i > 0; --i)
  {
    data_expression r = sort_bool::false_(); // We make a big || expression, so start with unit false
    data_expression_list elts = enumerated_elements; // Copy the enumerated elements, as these are needed for each iteration.

    // Make sure all elements get encoded.
    while (!elts.empty())
    {
      int count(static_cast<int>(pow(static_cast<double>(2), i-1)));

      // Iterate over the elements that get the boolean value new_parameters.front() == false
      for(int j = 0; j < count; ++j)
      {
        if (!elts.empty())
        {
          elts = pop_front(elts);
        }
      }

      // Iterate over the elements that get the boolean value new_parameters.front() == true
      for(int j = 0; j < count; ++j)
      {
        if (!elts.empty())
        {
          r = lazy::or_(r, data::equal_to(arg, elts.front()));
          elts = pop_front(elts);
        }
      }
    }

    result = push_front(result, assignment(new_parameters.front(), r));
    new_parameters = pop_front(new_parameters);
  }

  return result;
}

///Replace all assignments in which the left-hand side == parameter with a vector of boolean assignments.
static
assignment_list replace_enumerated_parameter_in_assignments(const assignment_list& list,
                                                                      const data_expression& parameter,
                                                                      const variable_list& new_parameters,
                                                                      const data_expression_list& enumerated_elements)
{
  core::gsDebugMsg("replace enumerated parameter %s in data assignments %s\n", parameter.to_string().c_str(), list.to_string().c_str());
  assignment_list result;

  for (assignment_list::iterator i = list.begin(); i != list.end(); ++i)
  {
    if (i->lhs() != parameter)
    {
      result = result + *i;
    }
    else
    {
      result = result + replace_enumerated_parameter_in_assignment(*i, parameter, new_parameters, enumerated_elements);
    }
  }

  return result;
}

///Replace all assignments of finite sorts != bool with a vector of boolean assignments.
static
assignment_list replace_enumerated_parameters_in_assignments(const assignment_list& list,
                                                                       atermpp::table& new_parameters_table,
                                                                       atermpp::table& enumerated_elements_table)
{
  core::gsDebugMsg("replace_enumerated_parameters_in_assignments %s\n", list.to_string().c_str());
  assignment_list result;
  // First replace right-hand-sides
  for (assignment_list::iterator i = list.begin(); i != list.end(); ++i)
  {
    result = push_front(result, assignment(i->lhs(),replace_enumerated_parameters_in_data_expression(i->rhs(), new_parameters_table, enumerated_elements_table)));
  }

  result = reverse(result);

  variable_list orig_parameters = variable_list(new_parameters_table.table_keys());
  for (variable_list::iterator i = orig_parameters.begin(); i != orig_parameters.end(); ++i)
  {
    result = replace_enumerated_parameter_in_assignments(result, *i, new_parameters_table.get(*i), enumerated_elements_table.get(*i));
  }

  return result;
}

///Replace all parameters of finite sorts != bool in list with an if tree of booleans.
static
action_list replace_enumerated_parameters_in_actions(action_list list,
                                                     atermpp::table& new_parameters_table,
                                                     atermpp::table& enumerated_elements_table)
{
  core::gsDebugMsg("replace enumerated parameters in action labels\n");
  variable_list orig_parms = variable_list(new_parameters_table.table_keys());
  for (variable_list::iterator i = orig_parms.begin(); i != orig_parms.end(); ++i)
  {
    list = bottom_up_replace(list, *i, make_if_tree(new_parameters_table.get(*i), enumerated_elements_table.get(*i)));
  }
  return list;
}

///Replace all parameters of finite sorts != bool in summand with a vector of booleans
static
summand replace_enumerated_parameters_in_summand(const summand& summand_,
                                                     atermpp::table& new_parameters_table,
                                                     atermpp::table& enumerated_elements_table)
{
  core::gsDebugMsg("replace enumerated parameters in summand %s\n", summand_.to_string().c_str());
  summand result;

  core::gsDebugMsg("\nOriginal condition: %s\n\n New condition: %s\n\n", summand_.condition().to_string().c_str(), replace_enumerated_parameters_in_data_expression(summand_.condition(), new_parameters_table, enumerated_elements_table).to_string().c_str());

  result = summand(summand_.summation_variables(),
                       replace_enumerated_parameters_in_data_expression(summand_.condition(), new_parameters_table, enumerated_elements_table),
                       summand_.is_delta(),
                       replace_enumerated_parameters_in_actions(summand_.actions(), new_parameters_table, enumerated_elements_table),
                       replace_enumerated_parameters_in_data_expression(summand_.time(), new_parameters_table, enumerated_elements_table),
                       replace_enumerated_parameters_in_assignments(summand_.assignments(), new_parameters_table, enumerated_elements_table));

  return result;
}

///Replace all parameters of finite sorts != bool in list with a vector of booleans
static
summand_list replace_enumerated_parameters_in_summands(const summand_list& list,
                                                       atermpp::table& new_parameters_table,
                                                       atermpp::table& enumerated_elements_table)
{
  core::gsDebugMsg("replace enumerated parameters in summands\n");
  summand_list result;
  for (summand_list::iterator i = list.begin(); i != list.end(); ++i)
  {
    result = push_front(result, replace_enumerated_parameters_in_summand(*i, new_parameters_table, enumerated_elements_table));
  }
  result = reverse(result);

  return result;
}

///Replace all parameters of finite sorts != bool in lps with a vector of booleans
static
linear_process replace_enumerated_parameters_in_lps(const lps::linear_process& lps,
                                         atermpp::table& new_parameters_table,
                                         atermpp::table& enumerated_elements_table)
{
  core::gsDebugMsg("replace enumerated parameters in linear process\n");
  linear_process result = lps;
  result.process_parameters() = replace_enumerated_parameters_in_variables(lps.process_parameters(), new_parameters_table, enumerated_elements_table);
  result.set_summands(replace_enumerated_parameters_in_summands(lps.summands(), new_parameters_table, enumerated_elements_table));
  return result;
}

/// Replace all parameters of finite sorts != bool in specification with a vector of booleans
static
specification replace_enumerated_parameters_in_specification(const lps::specification& specification,
                                                             atermpp::table& new_parameters_table,
                                                             atermpp::table& enumerated_elements_table)
{
  core::gsDebugMsg("replace enumerated parameters in specification\n");
  lps::specification result;

  // Compute new initial assignments
  assignment_list initial_assignments = replace_enumerated_parameters_in_assignments(specification.initial_process().assignments(), new_parameters_table, enumerated_elements_table);
  process_initializer initial_process(initial_assignments);

  // Compute new specification
  result = lps::specification(specification.data(),
                              specification.action_labels(),
                              specification.global_variables(),
                              replace_enumerated_parameters_in_lps(specification.process(), new_parameters_table, enumerated_elements_table),
                              initial_process);

  return result;
}

///Takes the specification in specification, applies binary to it,
///and returns the new specification.
specification binary(const lps::specification& spec,
                     rewriter& r)
{
  core::gsVerboseMsg("Executing binary...\n");
  specification result = spec;
  // table new_parameters_table = table(128, 50); Table is non copyable since 30/4/2007.
  // table enumerated_elements_table = table(128,50);
  atermpp::table new_parameters_table(128, 50);
  atermpp::table enumerated_elements_table(128,50);

  data::enumerator_factory< classic_enumerator< > > enumerator(spec.data(), r);

  // This needs to be done in a counter-intuitive order because of the well-typedness checks
  // (they make sure we can't build up an intermediate result!)
  variable_list new_process_parameters = replace_enumerated_parameters(result, enumerator, new_parameters_table, enumerated_elements_table);
  result = replace_enumerated_parameters_in_specification(result, new_parameters_table, enumerated_elements_table);
  result.process().process_parameters() = new_process_parameters;
  core::gsDebugMsg("Finished processing\n");

  return result;
}

} // namespace lps

} // namespace mcrl2

