#include "librewrite.h"
#include "lpe/pbes.h"
#include "libprint_c.h"

/// pbes_expression_rewrite rewrites a pbes_expression p as far as possible.
lpe::pbes_expression pbes_expression_rewrite(lpe::pbes_expression p, lpe::data_specification data, Rewriter *rewriter);
// Pre: p is a pbes_expression
//      data is the data of a pbes (used to initialise the rewriter)
// Ret: A pbes_expression, which is the as far as possible rewritten p

// get_all_possible_expressions combines a data_variable_list and a pbes_expression to a list of all possible expressions which are possible.
lpe::pbes_expression_list get_all_possible_expressions(lpe::data_variable_list data_vars, lpe::pbes_expression pbexp, lpe::data_specification data);

// get_and_expressions returns a list which contains:
// - [false] if one of the expressions is false
// - a list with only non-true elements out of the and_list if all expressions are !false
lpe::pbes_expression_list get_and_expressions(lpe::pbes_expression_list and_list, lpe::data_specification data, Rewriter *rewriter);

// get_or_expressions returns a list which contains:
// - [true] if one of the expressions is true
// - a list with only non-false elements out of the or_list if all expressions are !true
lpe::pbes_expression_list get_or_expressions(lpe::pbes_expression_list or_list, lpe::data_specification data, Rewriter *rewriter);

// make_and_from_list creates a PBESAnd structure from a list of pbes_expressions 
// So: [a, b, c] will be rewritten to: and(a, and(b,c))
lpe::pbes_expression make_and_from_list(lpe::pbes_expression_list and_list);

// make_or_from_list creates a PBESOr structure from a list of pbes_expressions 
// So: [a, b, c] will be rewritten to: or(a, or(b,c))
lpe::pbes_expression make_or_from_list(lpe::pbes_expression_list or_list);
