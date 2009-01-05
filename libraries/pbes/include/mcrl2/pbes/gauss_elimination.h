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
class gauss_elimination_algorithm
{
  protected:
    /// \brief Pretty print an equation without generating a newline after the equal sign
    /// \param eq A bes/pbes equation
    /// \return A pretty printed string
    template <typename Equation>
    std::string mypp(Equation eq)
    {
      typedef core::term_traits<typename Equation::term_type> tr;
      //return core::pp(eq.symbol()) + " " + tr::pp(eq.variable()) + " = " + tr::pp(eq.formula());
      return core::pp(eq.symbol()) + " " + eq.variable().to_string() + " = " + eq.formula().to_string();
    }

    /// \brief Prints the sequence of pbes equations [first, last) to standard out.
    /// \param first Start of a range of pbes equations
    /// \param last End of a range of pbes equations
    template <typename Iter>
    void print(Iter first, Iter last)
    {
      std::cerr << "pbes\n";
      for (Iter i = first; i != last; ++i)
      {
        std::cerr << "  " << mypp(*i) << std::endl;
      }
    }

  public:
    
    /// \brief Runs the algorithm. Applies Gauss elimination to the sequence of pbes equations [first, last).
    /// \param first Start of a range of pbes equations
    /// \param last End of a range of pbes equations
    /// \param solver An equation solver
    template <typename Iter, typename EquationSolver>
    void run(Iter first, Iter last, EquationSolver solver)
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
  std::cerr << "  before: " << mypp(*i) << std::endl;
#endif
        solver.solve(*i);
#ifdef MCRL2_GAUSS_ELIMINATION_DEBUG
  std::cerr << "   after: " << mypp(*i) << std::endl;
#endif
        // propagate the substitutions
        for (Iter j = first; j != i; ++j)
        {
          solver.substitute(*j, *i);
        }
      }
      solver.solve(*i); // TODO: clean the logic of this algorithm up
#ifdef MCRL2_GAUSS_ELIMINATION_DEBUG
  print(first, last);
#endif
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_GAUSS_ELIMINATION_H
