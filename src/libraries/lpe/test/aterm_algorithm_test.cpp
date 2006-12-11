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
#include "lpe/detail/mcrl22lpe.h"

using namespace std;
using namespace atermpp;
using namespace lpe;
using namespace lpe::data_init;
using namespace lpe::detail;

std::string SPECIFICATION = 
"% This file contains the alternating bit protocol, as described in W.J.    \n"
"% Fokkink, J.F. Groote and M.A. Reniers, Modelling Reactive Systems.       \n"
"%                                                                          \n"
"% The only exception is that the domain D consists of two data elements to \n"
"% facilitate simulation.                                                   \n"
"                                                                           \n"
"sort                                                                       \n"
"  D     = struct d1 | d2;                                                  \n"
"  Error = struct e;                                                        \n"
"                                                                           \n"
"act                                                                        \n"
"  r1,s4: D;                                                                \n"
"  s2,r2,c2: D # Bool;                                                      \n"
"  s3,r3,c3: D # Bool;                                                      \n"
"  s3,r3,c3: Error;                                                         \n"
"  s5,r5,c5: Bool;                                                          \n"
"  s6,r6,c6: Bool;                                                          \n"
"  s6,r6,c6: Error;                                                         \n"
"  i;                                                                       \n"
"                                                                           \n"
"proc                                                                       \n"
"  S(b:Bool)     = sum d:D. r1(d).T(d,b);                                   \n"
"  T(d:D,b:Bool) = s2(d,b).(r6(b).S(!b)+(r6(!b)+r6(e)).T(d,b));             \n"
"                                                                           \n"
"  R(b:Bool)     = sum d:D. r3(d,b).s4(d).s5(b).R(!b)+                      \n"
"                  (sum d:D.r3(d,!b)+r3(e)).s5(!b).R(b);                    \n"
"                                                                           \n"
"  K             = sum d:D,b:Bool. r2(d,b).(i.s3(d,b)+i.s3(e)).K;           \n"
"                                                                           \n"
"  L             = sum b:Bool. r5(b).(i.s6(b)+i.s6(e)).L;                   \n"
"                                                                           \n"
"init                                                                       \n"
"  allow({r1,s4,c2,c3,c5,c6,i},                                             \n"
"    comm({r2|s2->c2, r3|s3->c3, r5|s5->c5, r6|s6->c6},                     \n"
"        S(true) || K || L || R(true)                                       \n"
"    )                                                                      \n"
"  );                                                                       \n";

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

  specification spec = mcrl22lpe(SPECIFICATION);
  LPE lpe = spec.lpe();
 
  // find all action labels in lpe
  std::set<action_label> labels;
  find_all_if(lpe, is_action_label, inserter(labels, labels.end()));

  // find all data variables in lpe
  std::set<data_variable> variables;
  find_all_if(lpe, is_data_variable, inserter(variables, variables.end()));

  // find all functions in spec
  std::set<function> functions;
  find_all_if(spec.data().constructors(), is_function, std::inserter(functions, functions.end()));
  find_all_if(spec.data().mappings(), is_function, std::inserter(functions, functions.end()));

  // find all existential quantifications in lpe
  std::set<data_expression> existential_quantifications;
  find_all_if(lpe, is_exists, inserter(existential_quantifications, existential_quantifications.end()));

  return 0;
}
