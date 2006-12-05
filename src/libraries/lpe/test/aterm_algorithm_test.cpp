#include <iostream>
#include <iterator>
#include <boost/test/minimal.hpp>
#include "atermpp/atermpp.h"
#include "atermpp/algorithm.h"
#include "lpe/data.h"
#include "lpe/data_init.h"
#include "lpe/data_utility.h"
#include "lpe/sort.h"
#include "lpe/specification.h"

using namespace std;
using namespace atermpp;
using namespace lpe;
using namespace lpe::data_init;

struct compare_variable
{
  aterm d;

  compare_variable(data_variable d_)
    : d(d_)
  {}

  bool operator()(aterm t) const
  {
    return d == t;
  }
};

bool occurs_in(data_expression d, data_variable v)
{
  return find_if(aterm_appl(d), compare_variable(v)) != aterm();
}

int test_main(int, char*[])
{
  aterm bottom_of_stack;
  aterm_init(bottom_of_stack);
  gsEnableConstructorFunctions();

  specification spec;
  if (!spec.load("data/abp_b.lpe"))
  {
    cerr << "could not load data/abp_b.lpe" << endl;
    return 1;
  }
  LPE lpe = spec.lpe();
 
  // find all action labels in lpe
  std::set<action_label> labels;
  find_all_if(lpe, is_action_label, inserter(labels, labels.end()));

  // find all data variables in lpe
  std::set<data_variable> variables;
  find_all_if(lpe, is_data_variable, inserter(variables, variables.end()));

  // find all functions in spec
  std::set<function> functions;
  find_all_if(spec.constructors(), is_function, std::inserter(functions, functions.end()));
  find_all_if(spec.mappings(), is_function, std::inserter(functions, functions.end()));

  // find all existential quantifications in lpe
  std::set<data_expression> existential_quantifications;
  find_all_if(lpe, is_exists, inserter(existential_quantifications, existential_quantifications.end()));

  return 0;
}
