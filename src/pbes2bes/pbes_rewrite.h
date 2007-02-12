#include "librewrite.h"
#include "lpe/pbes_init.h"
#include "lpe/pbes.h"
#include "libprint_c.h"

/// pbes_expression_rewrite rewrites a pbes_expression p as far as possible.
lpe::pbes_expression pbes_expression_rewrite(lpe::pbes_expression p, lpe::data_specification data, Rewriter *rewriter);
// Pre: p is a pbes_expression
//      data is the data of a pbes (used to initialise the rewriter)
// Ret: A pbes_expression, which is the as far as possible rewritten p

lpe::pbes_expression_list get_all_possible_expressions(lpe::data_variable_list data_vars, lpe::pbes_expression pbexp, lpe::data_specification data);

lpe::pbes_expression make_and_from_list(lpe::pbes_expression_list and_list);

lpe::pbes_expression make_or_from_list(lpe::pbes_expression_list or_list);
