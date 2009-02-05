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

#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/data_operation.h"
#include "mcrl2/data/sort_utility.h"

// enumerates finite sorts
// i.e., produces all possible instantiations
// for a given sort, or set of sorts
typedef struct {
  mcrl2::data::sort_expression s;
  mcrl2::data::data_expression_list del;
} t_sdel;


class sort_instantiator {
 private:
  std::vector<t_sdel> instantiated_sorts;
  mcrl2::data::data_operation_list fl;
 public:
  sort_instantiator() {};
  void set_data_operation_list(mcrl2::data::data_operation_list flist);
  void instantiate_sorts(mcrl2::data::sort_expression_list sl);
  mcrl2::data::data_expression_list get_enumeration(mcrl2::data::sort_expression s);
  bool is_finite(mcrl2::data::sort_expression s);
} ;
