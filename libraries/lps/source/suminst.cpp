// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file suminst.cpp
/// \brief

#include "mcrl2/lps/specification.h"

//Enumerator
#include "mcrl2/data/classic_enumerator.h"
#include "mcrl2/data/enumerator_factory.h"

#include "mcrl2/core/messaging.h"
#include "mcrl2/lps/suminst.h"

using namespace mcrl2::core;
using namespace mcrl2::data;

namespace mcrl2 {

namespace lps {

/////////////////////////////////////////////////////////////////
// Helper functions
/////

///\return the list of all date_variables in vl, that are not in rl
static
variable_list filter(const variable_list& vl, const std::set< variable >& rl)
{
  variable_list result;
  for (variable_list::const_iterator i = vl.begin(); i != vl.end(); ++i)
  {
    if (rl.find(*i) == rl.end())
    {
      result = push_front(result, *i);
    }
  }

  return result;
}

///\pre fl is a list of constructors
///\return a list of finite sorts in sl
static
std::set< sort_expression > get_finite_sorts(const data::data_specification& d, const sort_expression_list& sl)
{
  std::set< sort_expression > result;
  for(sort_expression_list::iterator i = sl.begin(); i != sl.end(); ++i)
  {
    if (i->is_alias()) {
      alias ai(*i);

      if (d.is_certainly_finite(ai.reference()))
      {
        result.insert(ai.name());
        result.insert(ai.reference());
      }
    }
    else if (d.is_certainly_finite(*i))
    {
      result.insert(*i);
    }
  }
  return result;
}

///\return a list of all variables of a sort that occurs in sl
static
std::set< variable > get_variables(const variable_list& vl, std::set< sort_expression > const& sl)
{
  std::set< variable > result;
  for (variable_list::iterator i = vl.begin(); i != vl.end(); ++i)
  {
    if (sl.find(i->sort()) != sl.end())
    {
      result.insert(*i);
    }
  }
  return result;
}


////////////////////////////////////////////////////////////////
// Declustering
/////

///\pre specification is the specification belonging to summand
///\post the instantiated version of summand has been appended to result
///\return none
static
void instantiate_summand(const lps::specification& specification, const lps::summand& summand_, lps::summand_list& result, enumerator_factory< classic_enumerator< > >& enumerator_factory, const t_suminst_options& o)
{
  int nr_summands = 0; // Counter for the number of new summands, used for verbose output

  gsVerboseMsg("initialization...");

  std::set< variable > variables; // The variables we need to consider in instantiating
  if (o.finite_only)
  {
    // Only consider finite variables
    variables = get_variables(summand_.summation_variables(),
                get_finite_sorts(specification.data(), make_sort_expression_list(specification.data().sorts())));
  }
  else
  {
    variables = data::convert< std::set< variable > >(summand_.summation_variables());
  }

  if (variables.size() == 0)
  {
    // Nothing to be done, return original summand
    gsVerboseMsg("No summation variables in this summand\n");
    result = push_front(result, summand_);
  }
  else
  {
    // List of variables with the instantiated variables removed (can be done upfront, which is more efficient,
    // because we only need to calculate it once.
    variable_list new_vars = filter(summand_.summation_variables(), variables);

    // Solutions
    gsVerboseMsg("processing...");

    try {
      for (classic_enumerator< > i(enumerator_factory.make(variables, summand_.condition())); i != classic_enumerator<>(); ++i)
      {
        assignment_list substitutions;

        // Translate internal rewriter solution to lps assignment_list
        for (classic_enumerator<>::substitution_type::const_iterator s(i->begin()); s != i->end(); ++s)
        {
          // Substitution to be performed
          substitutions = push_front(substitutions, assignment(s->first, s->second));
        }
        gsDebugMsg("substitutions: %s\n", substitutions.to_string().c_str());

        summand s = summand(new_vars,
                substitute(assignment_list_substitution(substitutions), summand_.condition()),
                summand_.is_delta(),
                substitute(assignment_list_substitution(substitutions), summand_.actions()),
                substitute(assignment_list_substitution(substitutions), summand_.time()),
                substitute(assignment_list_substitution(substitutions), summand_.assignments())
                );

        result = push_front(result, s);
        ++nr_summands;
      }

      gsVerboseMsg("done...\n");
      if (nr_summands == 0)
      {
        gsVerboseMsg("All valuations for the variables in the condition of this summand reduce to false; removing this summand\n");
      }
      else
      {
        gsVerboseMsg("Replaced with %d summands\n", nr_summands);
      }
    }
    catch (mcrl2::runtime_error const&)
    {
      // If an error occurs in enumerating, remove all summands that
      // have been added to result thus far, and re-add the original.
      // This prevents problems e.g. in case of a sort without constructors.
      gsDebugMsg("An error occurred in enumeration, removing already added summands\n");

      while (nr_summands-- != 0)
      {
        result = pop_front(result);
      }

      gsVerboseMsg("Cannot expand this summand, keeping the original\n");
      result = push_front(result, summand_);
    }
  }
}

///Takes the summand list sl, instantiates it,
///and returns the instantiated summand list
static
lps::summand_list instantiate_summands(const lps::specification& specification,
                                     const lps::summand_list& sl,
                                     enumerator_factory< classic_enumerator< > >& enumerator,
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
lps::specification instantiate_sums(const lps::specification& specification, rewriter& r, const t_suminst_options& o)
{
  gsVerboseMsg("Instantiating...\n");
  lps::linear_process lps = specification.process();

  gsVerboseMsg("Input: %d summands.\n", lps.summands().size());

  // Some use of internal format because we need it for the rewriter
  enumerator_factory< classic_enumerator< > > enumerator(specification.data(), r);

  lps::summand_list sl = instantiate_summands(specification, lps.summands(), enumerator, o);
  lps = set_summands(lps, sl);

  gsVerboseMsg("Output: %d summands.\n", lps.summands().size());

  return lps::specification(specification.data(), specification.action_labels(), lps, specification.initial_process());
}

} // namespace lps

} // namespace mcrl2

