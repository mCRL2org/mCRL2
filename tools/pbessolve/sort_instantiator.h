

//  Copyright 2007 Simona Orzan. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./sort_instantiator.h



#include <string>

#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/data_operation.h"
#include "mcrl2/data/sort_utility.h"

using namespace lps;

// enumerates finite sorts
// i.e., produces all possible instantiations
// for a given sort, or set of sorts


typedef struct {
  sort_expression s;
  data_expression_list del;
} t_sdel;
  
  
class sort_instantiator {
 private:
  std::vector<t_sdel> instantiated_sorts;
  data_operation_list fl;
 public:
  sort_instantiator() {};
  void set_data_operation_list(data_operation_list flist);
  void instantiate_sorts(lps::sort_expression_list sl);
  data_expression_list get_enumeration(sort_expression s);    
  bool is_finite(sort_expression s);
} ;
