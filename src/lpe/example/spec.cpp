#include <iostream>
#include <string>
#include <iterator>
#include <fstream>
#include <boost/format.hpp>

#include "mcrl2/specification.h"
#include "mcrl2/predefined_symbols.h"
#include "mcrl2/sort.h"
#include "gsfunc.h"

using namespace std;
using namespace atermpp;
using namespace mcrl2;
using boost::format;

/// Rewrites the data expressions x an y, and then compares if they are equal.
///
bool compare(data_expression x, data_expression y, data_equation_list equations)
{
  ATermAppl x1 = rewrite(x.to_ATermAppl(), gsMakeDataEqnSpec(equations.to_ATermList()));
  ATermAppl y1 = rewrite(y.to_ATermAppl(), gsMakeDataEqnSpec(equations.to_ATermList()));
  
  return atermpp::aterm(x1) == atermpp::aterm(y1);
}

int main()
{
  specification spec;
  if (!spec.load("data/abp_b.lpe"))
  {
    cerr << "could not load data/abp_b.lpe" << endl;
    return 1;
  }

  cout << "--- sort -----------" << endl;
  for (sort_list::iterator i = spec.sorts().begin(); i != spec.sorts().end(); ++i)
  {
    cout << str(format("%5s        %s") % i->pp() % i->to_string()) << endl;
  }

  cout << "--- map ------------" << endl;
  for (function_list::iterator i = spec.mappings().begin(); i != spec.mappings().end(); ++i)
  {
    cout << str(format("%5s        %s") % i->pp() % i->to_string()) << endl;
  }

  cout << "--- cons -----------" << endl;
  for (function_list::iterator i = spec.constructors().begin(); i != spec.constructors().end(); ++i)
  {
    cout << str(format("%5s        %s") % i->pp() % i->to_string()) << endl;
  }
  cout << endl;

  LPE lpe = spec.lpe();

  cout << "free variables: " << lpe.free_variables().pp() << " " << lpe.free_variables().to_string() << endl;
  cout << endl;

  cout << "--- process parameters: ---" << endl;
  for (data_variable_list::iterator i = lpe.process_parameters().begin(); i != lpe.process_parameters().end(); ++i)
  {
    cout << str(format("%8s : %8s  %s") % i->name() % i->type().pp() % i->to_string()) << endl;
  }
  cout << endl;

  mcrl2::sort D("D");
  data_variable v("d1", D);
  cout << "v  = " << v.pp() << " " << v.to_string() << endl;

  data_variable w("YES", D);
  data_assignment a(v, w.to_expr());
  cout << "a = " << a.pp() << " " << a.to_string() << endl;

  // test substitution
  data_expression_list d0 = spec.initial_state();
  cout << "d0 = " << d0.pp() << " " << d0.to_string() << endl;

  data_expression_list d1 = d0.substitute(a);
  cout << "d1 = " << d1.pp() << " " << d1.to_string() << endl;

  // test comparison (using rewrite)
  cout << endl;
  bool c = compare(d0.front(), d1.front(), spec.equations());
  cout << "compare(d0.front(), d1.front(), spec) == " << c << endl;

  for (summand_list::iterator i = spec.lpe().summands().begin(); i != spec.lpe().summands().end(); ++i)
  {
//    cout << "summand " << i->pp() << endl;
  }
  
  LPE_summand s = spec.lpe().summands().front();
  cout << "actions:" << endl;
  for (action_list::iterator i = s.actions().begin(); i != s.actions().end(); ++i)
  {
    cout << i->to_string() << endl;
  }

  ofstream o1("lpe1.txt");
  o1 << lpe.to_string();

  ofstream o2("lpe2.txt");
  o2 << lpe.substitute(a).to_string(); 

  cin.get();
  return 0;
}
