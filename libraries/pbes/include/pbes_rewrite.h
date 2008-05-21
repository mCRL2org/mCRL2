// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes_rewrite.h
/// \brief Add your file description here.

#include "mcrl2/data/rewrite.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/core/print.h"

using namespace mcrl2::data;
using namespace mcrl2::pbes_system;

/// pbes_expression_rewrite rewrites a pbes_expression p as far as possible.
pbes_expression pbes_expression_rewrite(pbes_expression p, data_specification data, Rewriter *rewriter);

// get_all_possible_expressions combines a data_variable_list and a pbes_expression to a list of all possible expressions which are possible.
pbes_expression_list get_all_possible_expressions(data_variable_list data_vars, pbes_expression pbexp, data_specification data);

// get_and_expressions returns a list which contains:
// - [false] if one of the expressions is false
// - a list with only non-true elements out of the and_list if all expressions are !false
pbes_expression_list get_and_expressions(pbes_expression_list and_list, data_specification data, Rewriter *rewriter);

// get_or_expressions returns a list which contains:
// - [true] if one of the expressions is true
// - a list with only non-false elements out of the or_list if all expressions are !true
pbes_expression_list get_or_expressions(pbes_expression_list or_list, data_specification data, Rewriter *rewriter);

// element_in_propvarinstlist returns a boolean value which:
// - is true if one of the data variables occurs in pvilist
// - is false otherwise
bool element_in_propvarinstlist(data_variable_list vars, std::set< propositional_variable_instantiation > pvilist);

// occurs_inL returns if a data variable occurs in an aterm_appl l
bool occurs_inL(atermpp::aterm_appl l, data_variable v);

bool has_propvarinsts(std::set< propositional_variable_instantiation > propvars, data_variable_list data_vars);
