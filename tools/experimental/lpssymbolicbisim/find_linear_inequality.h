// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file find_linear_inequality.h

#ifndef MCRL2_LPSSYMBOLICBISIM_FIND_LINEAR_INEQUALITY_H
#define MCRL2_LPSSYMBOLICBISIM_FIND_LINEAR_INEQUALITY_H

#include "mcrl2/data/standard.h"
#include "mcrl2/data/traverser.h"
#include "mcrl2/data/variable.h"

#include <set>

namespace mcrl2::data
{

template <template <class> class Traverser, class OutputIterator>
struct find_linear_inequality_traverser: public Traverser<find_linear_inequality_traverser<Traverser, OutputIterator> >
{
  using super = Traverser<find_linear_inequality_traverser<Traverser, OutputIterator>>;
  using super::enter;
  using super::leave;
  using super::apply;

  OutputIterator out;
  std::set< variable > real_free_vars;
  bool search_free_var = false;
  bool found_free_var = false;

  find_linear_inequality_traverser(OutputIterator out_, std::set< variable > real_free_vars_)
    : out(out_)
    , real_free_vars(real_free_vars_)
  {}

  void apply(const data::variable& v)
  {
    if(search_free_var && !found_free_var && real_free_vars.find(v) != real_free_vars.end())
    {
      found_free_var = true;
    }
  }

  void apply(const data::application& x)
  {
    if (is_equal_to_application(x) ||
        is_not_equal_to_application(x) ||
        is_less_application(x) ||
        is_less_equal_application(x) ||
        is_greater_application(x) ||
        is_greater_equal_application(x))
    {
      // Switch to 'searching for free variables of type Real' mode
      search_free_var = true;
      found_free_var = false;
      super::apply(x);
      if(found_free_var)
      {
        // This expression contains a free variable of type Real, so add it to output
        *out = x;
      }
      search_free_var = false;
    }
    else if(!search_free_var || !found_free_var)
    {
      // traverse sub-expressions
      super::apply(x);
    }
  }
};

template <template <class> class Traverser, class OutputIterator>
find_linear_inequality_traverser<Traverser, OutputIterator>
make_find_linear_inequality_traverser(OutputIterator out, std::set< variable > real_free_vars)
{
  return find_linear_inequality_traverser<Traverser, OutputIterator>(out, real_free_vars);
}

template <typename T, typename OutputIterator>
void find_linear_inequalities(const T& x, OutputIterator o, std::set< variable > real_free_vars)
{
  make_find_linear_inequality_traverser<data::data_expression_traverser>(o, real_free_vars).apply(x);
}

template <typename T>
std::set<data::data_expression> find_linear_inequalities(const T& x, std::set< variable > real_free_vars)
{
  std::set<data::data_expression> result;
  find_linear_inequalities(x, std::inserter(result, result.end()), real_free_vars);
  return result;
}

} // namespace mcrl2::data

#endif // MCRL2_LPSSYMBOLICBISIM_FIND_LINEAR_INEQUALITY_H