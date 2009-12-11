// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/gauss_elimination.h
/// \brief Gauss elimination algorithm for pbes equation systems.

#ifndef MCRL2_PBES_GAUSS_ELIMINATION_H
#define MCRL2_PBES_GAUSS_ELIMINATION_H

#include "mcrl2/pbes/replace.h"
#include "mcrl2/pbes/pbes.h"

namespace mcrl2 {

namespace pbes_system {

  /// \brief Algorithm class for the Gauss elimination algorithm for solving
  /// systems of pbes equations.
  template <typename ExpressionTraits>
  class gauss_elimination_algorithm
  {
    public:
      typedef typename ExpressionTraits::expression_type expression_type;
      typedef typename ExpressionTraits::variable_type variable_type;
      typedef typename ExpressionTraits::equation_type equation_type;
    
    protected:
      /// \brief Prints the sequence of pbes equations [first, last) to standard out.
      /// \param first Start of a range of pbes equations
      /// \param last End of a range of pbes equations
      template <typename Iter>
      void print(Iter first, Iter last)
      {
        std::cerr << "equations\n";
        for (Iter i = first; i != last; ++i)
        {
          std::cerr << "  " << ExpressionTraits::print(*i) << std::endl;
        }
      }
  
    public:
  
      /// \brief Runs the algorithm. Applies Gauss elimination to the sequence of pbes equations [first, last).
      /// \param first Start of a range of pbes equations
      /// \param last End of a range of pbes equations
      /// \param solver An equation solver
      template <typename Iter, typename EquationSolver>
      void run(Iter first, Iter last, EquationSolver solve)
      {    
  #ifdef MCRL2_GAUSS_ELIMINATION_DEBUG
    print(first, last);
  #endif
        if (first == last)
        {
          return;
        }
  
        Iter i = last;
        while (i != first)
        {
          --i;
  #ifdef MCRL2_GAUSS_ELIMINATION_DEBUG
    std::cerr << "solving equation\n";
    std::cerr << "  before: " << ExpressionTraits::print(*i) << std::endl;
  #endif
          solve(*i);
  #ifdef MCRL2_GAUSS_ELIMINATION_DEBUG
    std::cerr << "   after: " << ExpressionTraits::print(*i) << std::endl;
  #endif
          // propagate the substitutions
          for (Iter j = first; j != i; ++j)
          {
            ExpressionTraits::substitute(*j, *i);
          }
        }
        solve(*i); // TODO: clean the logic of this algorithm up
  #ifdef MCRL2_GAUSS_ELIMINATION_DEBUG
    print(first, last);
  #endif
      }
  };

  /// \brief Approximation algorithm
  template <typename ExpressionTraits, typename Compare>
  struct approximate
  {
    Compare m_compare;

    approximate(Compare compare)
      : m_compare(compare)
    {}

    void operator()(typename ExpressionTraits::equation_type& eq) const
    {
      typedef typename ExpressionTraits::expression_type expression_type;
      typedef typename ExpressionTraits::variable_type variable_type;
      typedef typename ExpressionTraits::equation_type equation_type;
      
      const expression_type& phi = eq.formula();
      const variable_type& X = eq.variable();
      expression_type next = phi;
      expression_type prev;
      do
      {
        prev = next;
        next = ExpressionTraits::substitute(phi, X, prev);
      }
      while (!m_compare(prev, next));
      eq.formula() = next;
    }
  };

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_GAUSS_ELIMINATION_H
