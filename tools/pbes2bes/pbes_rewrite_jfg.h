// Author(s): Alexander van Dam
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes_rewrite_jfg.h
/// \brief Add your file description here.

#include "librewrite.h"
#include "mcrl2/pbes/pbes.h"
#include "libprint_c.h"
#include "atermpp/indexed_set.h"
#include "bes.h"

/// pbes_expression_rewrite_and_simplify rewrites a pbes_expression p as far
// as possible and removes free variables in quantifiers. 
lps::pbes_expression pbes_expression_rewrite_and_simplify(
              lps::pbes_expression p,
              Rewriter *rewriter);

/// pbes_expression_rewrite rewrites a pbes_expression p as far as possible,
/// while simultaneously applying substitutions. The substituted terms
//  are assumed to be in normal form, and are not rewritten.
lps::pbes_expression pbes_expression_substitute_and_rewrite(
              const lps::pbes_expression &p, 
              const lps::data_specification &data, 
              Rewriter *rewriter);

// Pre: p is a pbes_expression
//      data is the data of a pbes (used to initialise the rewriter)
// Ret: A pbes_expression, which is the as far as possible rewritten p

// get_all_possible_expressions combines a data_variable_list and a pbes_expression to a list of all possible expressions which are possible.
//lps::pbes_expression_list get_all_possible_expressions(lps::data_variable_list data_vars, lps::pbes_expression pbexp, lps::data_specification data);

// get_and_expressions returns a list which contains:
// - [false] if one of the expressions is false
// - a list with only non-true elements out of the and_list if all expressions are !false
//lps::pbes_expression_list get_and_expressions(lps::pbes_expression_list and_list, lps::data_specification data, Rewriter *rewriter);

// get_or_expressions returns a list which contains:
// - [true] if one of the expressions is true
// - a list with only non-false elements out of the or_list if all expressions are !true
//lps::pbes_expression_list get_or_expressions(lps::pbes_expression_list or_list, lps::data_specification data, Rewriter *rewriter);

//bool element_in_propvarinstlist(lps::data_variable_list vars, std::set< lps::propositional_variable_instantiation > pvilist);

//bool occurs_in_var(atermpp::aterm_appl l, lps::data_variable v);
