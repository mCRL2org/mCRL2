// Author(s): Simona Orzan.
//
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file sort_instantiator.h



#include <string>

#include "mcrl2/new_data/data_expression.h"
#include "mcrl2/new_data/sort_expression.h"
#include "mcrl2/new_data/function_symbol.h"

// enumerates finite sorts
// i.e., produces all possible instantiations
// for a given sort, or set of sorts
struct t_sdel {
  mcrl2::new_data::sort_expression s;
  mcrl2::new_data::data_expression_list del;
};

class sort_instantiator {
 private:
  std::vector<t_sdel> instantiated_sorts;
  mcrl2::new_data::function_symbol_list fl;
 public:
  sort_instantiator() {};
  void set_function_symbol_list(mcrl2::new_data::function_symbol_list const& flist);
  void instantiate_sorts(mcrl2::new_data::sort_expression_list const& sl);
  mcrl2::new_data::data_expression_list get_enumeration(mcrl2::new_data::sort_expression s);
  bool is_finite(mcrl2::new_data::sort_expression s);
} ;
