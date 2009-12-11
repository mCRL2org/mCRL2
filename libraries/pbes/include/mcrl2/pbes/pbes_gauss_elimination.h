// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes_gauss_elimination.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PBES_GAUSS_ELIMINATION_H
#define MCRL2_PBES_PBES_GAUSS_ELIMINATION_H

#include "mcrl2/pbes/pbes_expression_with_variables.h"
#include "mcrl2/data/identifier_generator.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/gauss_elimination.h"

namespace mcrl2 {

namespace pbes_system {

  /// \brief Traits class for pbes expressions
  struct pbes_traits
  {
  public:
    typedef pbes_expression expression_type;
    typedef propositional_variable variable_type;
    typedef pbes_equation equation_type;
    
    /// \brief Applies the substitution X := phi to the PBES expression t.
    /// \param t A PBES expression
    /// \param X A propositional variable
    /// \param phi A PBES expression
    /// \return The substition result
    static inline
    expression_type substitute(const expression_type& t, const variable_type& X, const expression_type& phi)
    {
      return substitute_propositional_variable(t, X, phi);
    }

    /// \brief Applies the substitution X := phi to the pbes equation eq.
    /// \param eq A PBES equation
    /// \param X A propositional variable
    /// \param phi A PBES expression
    /// \return The substition result
    static inline
    void substitute(equation_type& eq, const variable_type& X, const expression_type& phi)
    {
      eq.formula() = pbes_traits::substitute(eq.formula(), X, phi);
    }
  
    /// \brief Applies the substitution from a solved pbes equation e2 to the pbes equation e1.
    /// \param e1 A PBES equation
    /// \param e2 A PBES equation
    static inline
    void substitute(equation_type& e1, const equation_type& e2)
    {
      pbes_traits::substitute(e1, e2.variable(), e2.formula());
    }

    /// \brief Pretty print an equation without generating a newline after the equal sign
    /// \param eq An equation
    /// \return A pretty printed string
    static inline
    std::string print(const equation_type& eq)
    {
      return core::pp(eq.symbol()) + " " + core::pp(eq.variable()) + " = " + core::pp(eq.formula());
    }
  };

  /// \brief Solves an equation
  /// \param e A pbes equation
  template <typename Rewriter>
  struct pbes_equation_solver
  {
    const Rewriter& m_rewriter;
    
    pbes_equation_solver(const Rewriter& rewriter)
      : m_rewriter(rewriter)
    {}

    /// \brief Returns true if e.symbol() == nu(), else false.
    /// \param e A pbes equation
    /// \return True if e.symbol() == nu(), else false.
    pbes_expression sigma(const pbes_equation& e)
    {
      typedef typename core::term_traits<pbes_expression> tr;
      return e.symbol().is_nu() ? tr::true_() : tr::false_();
    }
    
    /// \brief Solves the equation e
    void operator()(pbes_equation& e)
    {
      pbes_traits::substitute(e, e.variable(), sigma(e));
      e.formula() = m_rewriter(e.formula());
    }
  };

  /// \brief Utility function for creating a pbes_equation_solver
  template <typename Rewriter> 
  pbes_equation_solver<Rewriter> make_pbes_equation_solver(const Rewriter& rewriter)
  {
    return pbes_equation_solver<Rewriter>(rewriter);
  }

  /// \brief Solves a PBES equation system using Gauss elimination.
  /// \pre The pbes \p p is a bes.
  /// \param p A pbes
  /// \return 0 if the solution is false, 1 if the solution is true, 2 if the solution is unknown
  template <typename Container>
  int gauss_elimination(pbes<Container>& p)
  {
    typedef data::data_enumerator<data::number_postfix_generator> my_enumerator;
    typedef enumerate_quantifiers_rewriter<pbes_expression_with_variables, data::rewriter, my_enumerator> my_rewriter;
    typedef typename core::term_traits<pbes_expression> tr;
  
    data::rewriter datar(p.data());
    data::number_postfix_generator name_generator("x");
    my_enumerator datae(p.data(), datar, name_generator);
    my_rewriter pbesr(datar, datae);
  
    gauss_elimination_algorithm<pbes_traits> algorithm;
    algorithm.run(p.equations().begin(), p.equations().end(), pbes_equation_solver<my_rewriter>(pbesr));
  
    if (tr::is_false(p.equations().front().formula()))
    {
      return 0;
    }
    else if (tr::is_true(p.equations().front().formula()))
    {
      return 1;
    }
    else
    {
      return 2;
    }
  }

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBES_GAUSS_ELIMINATION_H
