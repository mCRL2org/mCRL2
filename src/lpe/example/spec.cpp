#include <iostream>
#include <string>
#include "mcrl2/specification.h"

using namespace std;
using namespace atermpp;
using namespace mcrl2;

int main()
{
  specification spec;
  if (!spec.load("data/abp_b.lpe"))
  {
    cerr << "could not load data/abp_b.lpe" << endl;
    return 1;
  }

  for (sort_list::iterator i = spec.sorts().begin(); i != spec.sorts().end(); ++i)
  {
    cout << "sort " << i->to_string() << endl;
  }

  for (function_list::iterator i = spec.mappings().begin(); i != spec.mappings().end(); ++i)
    cout << "mapping " << i->to_string() << endl;

  for (function_list::iterator i = spec.constructors().begin(); i != spec.constructors().end(); ++i)
    cout << "constructors " << i->to_string() << endl;

  data_expression_list d0 = spec.initial_state();
  cout << "<d0> = " << d0.to_string() << endl;

  LPE lpe = spec.lpe();
  for (data_variable_list::iterator i = lpe.free_variables().begin(); i != lpe.free_variables().end(); ++i)
  {
    cout << "<var>" << i->to_string() << endl;
  }

  LPE_summand s = lpe.summands().front();
  data_expression_list d1 = d0.substitute(s.assignments().begin(), s.assignments().end());
  cout << "<d1> = " << d0.to_string() << endl;

  // bool c = compare(d0.front(), d1.front());
  // cout << "compare(d0.front(), d1.front()) == " << c << endl;

  for (summand_list::iterator i = spec.lpe().summands().begin(); i != spec.lpe().summands().end(); ++i)
  {
    cout << "summands " << i->to_string() << endl;
  }

  cin.get();
  return 0;
}
