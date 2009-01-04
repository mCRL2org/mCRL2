// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsbinary.cpp
/// \brief 

#include "boost.hpp" // precompiled headers

//Aterms
#include <mcrl2/atermpp/algorithm.h>
#include <mcrl2/atermpp/aterm.h>
#include <mcrl2/atermpp/table.h>

//LPS Framework
#include <mcrl2/data/data_operation.h>
#include <mcrl2/lps/linear_process.h>
#include <mcrl2/lps/specification.h>
#include <mcrl2/data/sort_utility.h>
#include <mcrl2/data/find.h>
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/data/detail/data_functional.h"

//Enumerator
#include <mcrl2/data/detail/enum/standard.h>
#include <mcrl2/lps/nextstate.h>

#include <mcrl2/lps/suminst.h>

//using namespace std;
// For Aterm library extension functions
using namespace mcrl2::core;
using namespace atermpp;
using namespace mcrl2::data;
using namespace mcrl2::lps;
using namespace mcrl2;

namespace mcrl2 {

namespace lps {

/////////////////////////////////////////////////////////////////
// Helper functions
/////

///\ret a list of all data_variables of sort s in vl
data_variable_list get_occurrences(const data_variable_list& vl, const sort_expression& s)
{
  data_variable_list result;
  for (data_variable_list::iterator i = vl.begin(); i != vl.end(); ++i)
  {
    if (i->sort() == s)
    {
      result = push_front(result, *i);
    }
  }
  result = reverse(result);
  return result;
}

///\ret the list of all data_variables in vl, which are unequal to v
data_variable_list filter(const data_variable_list& vl, const data_variable& v)
{
  data_variable_list result;
  for (data_variable_list::iterator i = vl.begin(); i != vl.end(); ++i)
  {
    if (!(*i == v))
    {
      result = push_front(result, *i);
    }
  }
  return result;
}

///\ret the list of all date_variables in vl, that are not in rl
data_variable_list filter(const data_variable_list& vl, const data_variable_list& rl)
{
  data_variable_list result;
  for (data_variable_list::iterator i = vl.begin(); i != vl.end(); ++i)
  {
    if (!find_data_variable(rl, *i))
    {
      result = push_front(result, *i);
    }
  }

  return result;
}

///\pre fl is a list of constructors
///\ret a list of finite sorts in sl
sort_expression_list get_finite_sorts(const data_operation_list& fl, const sort_expression_list& sl)
{
  sort_expression_list result;
  for(sort_expression_list::iterator i = sl.begin(); i != sl.end(); ++i)
  {
    if (is_finite(fl, *i))
    {
      result = push_front(result, *i);
    }
  }
  reverse(result);
  return result;
}

///\ret a list of all variables of a sort that occurs in sl
data_variable_list get_variables(const data_variable_list& vl, const sort_expression_list& sl)
{
  data_variable_list result;
  for (data_variable_list::iterator i = vl.begin(); i != vl.end(); ++i)
  {
    if (find_sort_expression(sl, i->sort()))
    {
      result = push_front(result, *i);
    }
  }
  result = reverse(result);
  return result;
}


////////////////////////////////////////////////////////////////
// Declustering
/////

///\pre specification is the specification belonging to summand
///\post the instantiated version of summand has been appended to result
///\ret none
void instantiate_summand(const lps::specification& specification, const lps::summand& summand_, lps::summand_list& result, EnumeratorStandard& enumerator, const t_suminst_options& o)
{
  int nr_summands = 0; // Counter for the nummer of new summands, used for verbose output

  gsVerboseMsg("initialization...");

  data_variable_list variables; // The variables we need to consider in instantiating
  if (o.finite_only)
  {
    // Only consider finite variables
    variables = get_variables(summand_.summation_variables(), get_finite_sorts(specification.data().constructors(), specification.data().sorts()));
  }
  else
  {
    variables = summand_.summation_variables();
  }

  if (aterm_get_length(variables) == 0)
  {
    // Nothing to be done, return original summand
    gsVerboseMsg("No summation variables in this summand\n");
    result = push_front(result, summand_);
  }
  else
  {
    // List of variables with the instantiated variables removed (can be done upfront, which is more efficient,
    // because we only need to calculate it once.
    data_variable_list new_vars = filter(summand_.summation_variables(), variables);

    ATermList vars = ATermList(variables);

    ATerm expr = enumerator.getRewriter()->toRewriteFormat(aterm_appl(summand_.condition()));

    // Solutions
    EnumeratorSolutions* sols = enumerator.findSolutions(vars, expr, false);

    gsVerboseMsg("processing...");
    // sol is a solution in internal rewriter format
    ATermList sol;
    bool error = false; // Flag enumerator error to break loop.
    while (sols->next(&sol) && !error)
    {
      if (sols->errorOccurred())
      {
        // If an error occurs in enumerating, remove all summands that
        // have been added to result thus far, and re-add the original.
        // This prevents problems e.g. in case of a sort without constructors.
        gsDebugMsg("An error occurred in enumeration, removing already added summands\n");
        error = true;

        for (int i = 0; i < nr_summands; ++i);
        {
          result = pop_front(result);
        }
        nr_summands = 0;
      }
      else
      {
        data_assignment_list substitutions; 
        // Convenience cast, so that the iterator, and the modifications from the atermpp library can be used
        aterm_list solution = aterm_list(sol);

        // Translate internal rewriter solution to lps data_assignment_list
        for (aterm_list::iterator i = solution.begin(); i != solution.end(); ++i)
        {
          // lefthandside of substitution
          data_variable var = data_variable(ATgetArgument(ATerm(*i), 0));

          // righthandside of substitution in internal rewriter format
          ATerm arg = ATgetArgument(ATerm(*i),1);

          // righthandside of substitution in lps format
          data_expression res = data_expression(aterm_appl(enumerator.getRewriter()->fromRewriteFormat(arg)));

          // Substitution to be performed
          data_assignment substitution = data_assignment(var, res);
          substitutions = push_front(substitutions, substitution);
        }
        gsDebugMsg("substitutions: %s\n", substitutions.to_string().c_str());

        summand s = summand(new_vars,
                                    summand_.condition().substitute(assignment_list_substitution(substitutions)),
                                    summand_.is_delta(),
                                    summand_.actions().substitute(assignment_list_substitution(substitutions)),
                                    summand_.time().substitute(assignment_list_substitution(substitutions)),
                                    summand_.assignments().substitute(assignment_list_substitution(substitutions))
                                    );
        
        result = push_front(result, s);
        ++nr_summands;
      }
    }

    gsVerboseMsg("done...\n");
    if (nr_summands == 0 && sols->errorOccurred())
    {
      gsVerboseMsg("Cannot expand this summand, keeping the original\n");
      result = push_front(result, summand_);
    }
    else if (nr_summands == 0)
    {
      gsVerboseMsg("All valuations for the variables in the condition of this summand reduce to false; removing this summand\n");
    }
    else
    {
      gsVerboseMsg("Replaced with %d summands\n", nr_summands);
    }
  }
}

///Takes the summand list sl, instantiates it,
///and returns the instantiated summand list
lps::summand_list instantiate_summands(const lps::specification& specification,
                                     const lps::summand_list& sl,
                                     EnumeratorStandard& enumerator, 
                                     const t_suminst_options& o)
{
  lps::summand_list result;

  // instantiate_summand(..) is called only in this function, therefore, it is safe to count the summands here for verbose output.
  lps::summand_list summands = reverse(sl); // This is not absolutely necessary, but it helps in comparing input and output of the suminst algorithm (that is, the relative order is preserved (because instantiate_summand plainly appends to result)
  int j = 1;
  for (summand_list::iterator i = summands.begin(); i != summands.end(); ++i, ++j)
  {
    gsVerboseMsg("Summand %d\n", j);
    // If tau_only is set, only instantiate tau summands, else instantiate all.
    // Summands we do not need to instantiate are simply added to the
    // specification.
    if ((o.tau_only && i->is_tau()) || !o.tau_only)
    {
      instantiate_summand(specification, *i, result, enumerator, o);
    }
    else
    {
      result = push_front(result, *i);
    }
  }

  return result;
}

///Takes the specification in specification, instantiates it,
///and returns the instantiateed specification.
lps::specification instantiate_sums(const lps::specification& specification, Rewriter& r, const t_suminst_options& o)
{
  gsVerboseMsg("Instantiating...\n");
  lps::linear_process lps = specification.process();

  gsVerboseMsg("Input: %d summands.\n", lps.summands().size());

  // Some use of internal format because we need it for the rewriter
  EnumeratorStandard enumerator = EnumeratorStandard(specification.data(), &r);

  lps::summand_list sl = instantiate_summands(specification, lps.summands(), enumerator, o);
  lps = set_summands(lps, sl);

  gsVerboseMsg("Output: %d summands.\n", lps.summands().size());

  return set_lps(specification, lps);
}

} // namespace lps

} // namespace mcrl2

