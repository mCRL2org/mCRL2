#include <iostream>

#include "lpe/data.h"
#include "lpe/sort.h"
#include "lpe/substitute.h"
#include "lpe/specification.h"

using namespace std;
using namespace atermpp;
using namespace lpe;

int main()
{
  aterm bottom_of_stack;
  aterm_init(bottom_of_stack);
  gsEnableConstructorFunctions();

  // create a list of assignments
  lpe::sort D("D");
  data_variable d0("d0", D);
  data_variable d1("d1", D);
  data_variable x0("x0", D);
  data_variable x1("x1", D);
  data_assignment_list a;
  a = push_front(a, data_assignment(d0, x0));
  a = push_front(a, data_assignment(d1, x1));

  cout << "a = " << pp(a) << endl;

  specification spec;
  if (!spec.load("data/abp_b.lpe"))
  {
    cerr << "could not load data/abp_b.lpe" << endl;
    return 1;
  }
  data_expression_list p = spec.initial_state();
  cout << "p = " << pp(p) << endl;
  
  // apply multiple substitutions
  data_expression_list q = substitute(p, assignment_list_substitution(a));
  cout << "q = " << pp(q) << endl;

  cin.get();
  return 0;
}
