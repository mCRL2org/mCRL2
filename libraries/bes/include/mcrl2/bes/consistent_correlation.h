// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/consistent_correlation.h
/// \brief add your file description here.

#ifndef MCRL2_BES_CONSISTENT_CORRELATION_H
#define MCRL2_BES_CONSISTENT_CORRELATION_H

#include <map>
#include <set>
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/pbes/equation_index.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2 {

namespace bes {

namespace detail {

template <typename SortedSequence1, typename SortedSequence2>
bool empty_intersection(const SortedSequence1& x, const SortedSequence2& y)
{
  auto i = x.begin();
  auto j = y.begin();
  while (i != x.end() && j != y.end())
  {
    if (*i == *j)
    {
      return false;
    }
    else if (*i < *j)
    {
      ++i;
    }
    else
    {
      ++j;
    }
  }
  return true;
}

} // namespace detail

// Pseudo code supplied by Tim Willemse
//
// Notatie:
// * E,E',F,F' : BES
// * X,X',Y,Y' : propositionele variabele
// * P : partitie van propositionele variabelen (set van set van prop vars)
// * B : set van prop vars
// * rank(X,E): de rank van vergelijking X in E
// * bnd(E): bound propositionele variabelen
// * rhs(X): right-hand side van de vergelijking X
// * init(E): de propositionele variabele die in de init van BES E staat
//
//
// input: E, F ranging over disjoint sets of propositional variables
// output: true iff init(E) and init(F) consistently correlate
//
// 1. P := { { Y in bnd(F) U bnd(E) | rank(Y,F) = rank(X,E) \/ rank(Y,E) = rank(X,E) } | X in bnd(E) }
// 2. repeat
// 3.   stable := true
// 4.   for each B in P do
// 5.      if exists X,Y in B such that differ(X,Y,P) then
// 6.         choose X,Y in B such that differ(X,Y,P)
// 7.         stable := false
// 8.         (B0,B1) := split(P,B,X)
// 9.         P := (P \ {B}) U {B0, B1}
// 10. until stable
// 11. return exists B in P: {init(E), init(F)} subseteq B
//
// where:
//
// * differ(X,Y,P) = exists theta in Theta_P: |[ rhs(X) ]|theta =/= |[ rhs(Y) ]|theta
// * Theta_P := { theta | forall B in P: forall X',Y' in B: theta(X') = theta(Y') };
//   in words: theta in Theta_P iff theta assigns the same truth value to all prop vars that are in the same partition
// * split(P,B,X) := ( { X' | differ(X,X',P) } , B \ { X' | differ(X,X',P) } )

struct rank
{
  const boolean_equation_system& E;
  std::map<boolean_variable, std::size_t> r; // stores the rank

  rank(const boolean_equation_system& E_)
    : E(E_)
  {
    std::size_t index = 0;
    auto const& eqn = E.equations();
    for (auto i = eqn.begin(); i != eqn.end(); ++i)
    {
      if (i->symbol().is_mu())
      {
        index++;
      }
      r[i->variable()] = index;
    }
  }

  std::size_t operator()(const boolean_variable& x) const
  {
    return utilities::detail::map_element(r, x);
  }
};

struct consistent_correlation_checker
{
  const boolean_equation_system& E;
  std::set<boolean_variable> bndE;
  rank rankE;
  pbes_system::equation_index<boolean_equation_system> indexE;

  const boolean_equation_system& F;
  std::set<boolean_variable> bndF;
  rank rankF;
  pbes_system::equation_index<boolean_equation_system> indexF;

  std::set<std::set<boolean_variable> > P;

  // preconditions:
  consistent_correlation_checker(const boolean_equation_system& E_, const boolean_equation_system& F_)
  : E(E_),
    bndE(E_.binding_variables()),
    rankE(E_),
    indexE(E_),
    F(F_),
    bndF(F_.binding_variables()),
    rankF(F_),
    indexF(F_)
  {
    assert(!E.equations().empty());
    assert(!F.equations().empty());
    assert(detail::empty_intersection(bndE, bndF));
  }

  const boolean_expression& rhsE(const boolean_variable& x) const
  {
    return indexE.equation(x).formula();
  }

  const boolean_expression& rhsF(const boolean_variable& x) const
  {
    return indexF.equation(x).formula();
  }

  const boolean_expression& initE() const
  {
    return E.initial_state();
  }

  const boolean_expression& initF() const
  {
    return F.initial_state();
  }

  void run()
  {
    auto const& eqnE = E.equations();
    auto const& eqnF = F.equations();

    // compute the initial partition P
    auto e = eqnE.begin();
    auto f = eqnF.begin();

    for (auto i = eqnE.begin(); i != eqnE.end(); ++i)
    {
      auto const& X = i->variable();
      auto r = rankE(X);
    }
  }
};

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_CONSISTENT_CORRELATION_H
