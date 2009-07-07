// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes_solver_test.h
/// \brief This file contains a simple solver for pbesses.
///        This solver is mainly intended for testing purposes.

#ifndef MCRL2_PBES_PBES_SOLVER_TEST_H
#define MCRL2_PBES_PBES_SOLVER_TEST_H

//Data framework
#include "mcrl2/data/enumerator.h"

//Boolean equation systems
// #include "mcrl2/pbes/utility.h"
#include "mcrl2/pbes/bes_deprecated.h"
#include "mcrl2/pbes/pbesrewr.h"

namespace mcrl2 {

namespace pbes_system {

/// \brief Straightforward solver for pbesses
/// \detail This solver gets a parameterised boolean equation system.
///         It subsequently transforms it to a boolean equation system
///         where quantifiers are eliminated as much as possible (using
///         the quantifier-all pbes rewriter). After generating the equations
///         a straightforward solver is called on the equations to solve
///         these. This method uses the same code as pbes2bool (except that
///         pbes2bool uses more advanced features).

template < typename Container > 
bool pbes2_bool_test(const typename pbes<Container>& pbes_spec)
{ // Generate an enumerator, a data rewriter and a pbes rewriter.
  data::rewriter datar= 
            data::rewriter(p.data(), mcrl2::data::used_data_equation_selector(p.data(), p.equations()), rewrite_strategy());
  data::number_postfix_generator generator("UNIQUE_PREFIX");
  data::rewriter_with_variables datarv(datar);
  data::data_enumerator<> datae(p.data(), datar, generator);
  const bool enumerate_infinite_sorts = true;
  enumerate_quantifiers_rewriter<pbes_expression, data::rewriter_with_variables,data::data_enumerator<> >
                          pbesr(datarv, datae, enumerate_infinite_sorts);
  ::bes::boolean_equation_system bes_equations(p, pbesr);
  return solve_bes(bes_equations);
} 

} // namespace pbes_system
} // namespace mcrl2

#endif // MCRL2_PBES_PBES_SOLVER_TEST_H
