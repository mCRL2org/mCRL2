// Author(s): Alexander van Dam
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes_rewrite_jfg.h
/// \brief Add your file description here.

#include <string>
#include "librewrite.h"
#include "mcrl2/pbes/pbes.h"
#include "libprint_c.h"
#include "atermpp/indexed_set.h"
// #include "bes.h"

// data structure containing the options that pbes2bool can have.

typedef struct{
  std::string opt_outputformat;
  std::string opt_strategy;
  bool opt_precompile_pbes;
  RewriteStrategy rewrite_strategy;
  std::string infilename;
  std::string outfilename;
} t_tool_options;


// pbes_expression_rewrite_and_simplify rewrites a pbes_expression p as far
// as possible and removes free variables in quantifiers. If the precompile option
// is set in the tool_options, the data expressions in the resulting pbes are
// translated to internal rewrite format.

lps::pbes_expression pbes_expression_rewrite_and_simplifyDEPRECATED(
              lps::pbes_expression p,
              Rewriter *rewriter,
              const t_tool_options &tool_options);

// pbes_expression_rewrite rewrites a pbes_expression p as far as possible,
// while simultaneously applying substitutions. The substituted terms
// are assumed to be in normal form, and are not rewritten.
// If the precompile option is set, it is assumed that data expressions
// in p are in internal rewrite format. The data expressions in the
// resulting pbes are also in internal rewrite format.

lps::pbes_expression pbes_expression_substitute_and_rewrite(
              const lps::pbes_expression &p, 
              const lps::data_specification &data, 
              Rewriter *rewriter,
              const t_tool_options &tool_options);

// Pre: p is a pbes_expression
//      data is the data of a pbes (used to initialise the rewriter)
// Ret: A pbes_expression, which is the as far as possible rewritten p

