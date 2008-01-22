// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/gauss_elimination.h
/// \brief Gauss elimination algorithm for pbes equation systems.

#ifndef MCRL2_PBES_GAUSS_ELIMINATION_H
#define MCRL2_PBES_GAUSS_ELIMINATION_H

#include "mcrl2/pbes/propositional_variable_replace.h"
#include "mcrl2/pbes/pbes.h"

namespace mcrl2 {

namespace pbes_system {

/// Contains an implementation of the Gauss elimination algorithm for solving
/// systems of pbes equations.
class gauss_elimination_algorithm
{
  public:
    /// Returns true if e.symbol() == nu(), else false.
    pbes_expression sigma(const pbes_equation& e) const
    {
      using namespace pbes_expr;
      return e.symbol().is_nu() ? true_() : false_();
    }
    
    /// Uses approximation to solve equation e.
    /// \return The solved equation, in which the right hand side has been replaced.
    pbes_equation approximate(pbes_equation e) const
    {
      return e;
    }
    
    /// Applies the substitution X := phi to the pbes equation eq.
    pbes_equation substitute(pbes_equation eq, propositional_variable X, pbes_expression phi) const
    {
      pbes_expression formula = substitute_propositional_variable(eq.formula(), X, phi);
      return pbes_equation(eq.symbol(), eq.variable(), formula);
    }
    
    /// Applies the substitution X := phi to the sequence of pbes equations [first, last[.
    template <typename Iter>
    void substitute(Iter first, Iter last, propositional_variable X, pbes_expression phi) const
    {
      for (Iter i = first; i != last; ++i)
      {
        *i = substitute(*i, X, phi);
      }
    }
    
    /// Applies Gauss elimination to the sequence of pbes equations [first, last[.
    template <typename Iter>
    void run(Iter first, Iter last) const
    {
      if (first == last)
      {
        return;
      }
    
      Iter i = last;
      while (i != first)
      {
        *i = approximate(*i);
        substitute(first, i, i->variable(), i->formula());
        --i;
      }
    }
};

/// Applies Gauss elimination to the pbes p.
template <typename Container>
void apply_gauss_elimination(pbes<Container>& p)
{
  gauss_elimination_algorithm().run(p.equations().begin(), p.equations().end());
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_GAUSS_ELIMINATION_H
