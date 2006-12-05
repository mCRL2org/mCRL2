#include <iostream>
#include <string>
#include <iterator>
#include <fstream>
#include <boost/format.hpp>

#include "lpe/specification.h"
#include "lpe/predefined_symbols.h"
#include "lpe/sort.h"

using namespace std;
using namespace atermpp;
using namespace lpe;
using boost::format;

/// Rewrites the data expressions x an y, and then compares if they are equal.
///
//bool compare(data_expression x, data_expression y, data_equation_list equations)
//{
//  rewriter r(equations);
//  ATermAppl x1 = r.rewrite(x);
//  ATermAppl y1 = r.rewrite(y); 
//  return atermpp::aterm(x1) == atermpp::aterm(y1);
//}

int main()
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  gsEnableConstructorFunctions(); 
  
  specification spec;
  if (!spec.load("data/abp_b.lpe"))
  {
    cerr << "could not load data/abp_b.lpe" << endl;
    return 1;
  }

  cout << "--- sort -----------" << endl;
  for (sort_list::iterator i = spec.sorts().begin(); i != spec.sorts().end(); ++i)
  {
    lpe::sort s = *i;
    cout << str(format("%5s        %s") % i->pp() % i->to_string()) << endl;
  }

  cout << "--- map ------------" << endl;
  for (function_list::iterator i = spec.mappings().begin(); i != spec.mappings().end(); ++i)
  {
    function f = *i;
    cout << str(format("%5s        %s") % i->pp() % i->to_string()) << endl;
  }

  cout << "--- cons -----------" << endl;
  for (function_list::iterator i = spec.constructors().begin(); i != spec.constructors().end(); ++i)
  {
    function f = *i;
    cout << str(format("%5s        %s") % i->pp() % i->to_string()) << endl;
  }
  cout << endl;

  cout << "--- act ------------" << endl;
  for (action_label_list::iterator i = spec.action_labels().begin(); i != spec.action_labels().end(); ++i)
  {
    aterm a = *i;
    cout << str(format("%5s") % i->to_string()) << endl;
  }
  cout << endl;

  LPE lpe = spec.lpe();
  cout << "lpe = " << lpe.to_string() << endl;

  cout << "free variables: " << pp(lpe.free_variables()) << " " << lpe.free_variables().to_string() << endl;
  cout << endl;

  cout << "--- process parameters: ---" << endl;
  for (data_variable_list::iterator i = lpe.process_parameters().begin(); i != lpe.process_parameters().end(); ++i)
  {
    cout << str(format("%8s : %8s  %s") % i->name() % i->type().pp() % i->to_string()) << endl;
  }
  cout << endl;

  lpe::sort D("D");
  data_variable v("d1", D);
  cout << "v  = " << v.pp() << " " << v.to_string() << endl;

  data_variable w("YES", D);
  data_assignment a(v, w);
  cout << "a = " << a.pp() << " " << a.to_string() << endl;

  // test substitution
  data_expression_list d0 = spec.initial_state();
  cout << "d0 = " << pp(d0) << " " << d0.to_string() << endl;

  data_expression_list d1 = d0.substitute(a);
  cout << "d1 = " << pp(d1) << " " << d1.to_string() << endl;

  data_assignment_list aa;
  aa = push_front(aa, a);
  aa = push_front(aa, a);
  data_expression_list d2 = d0.substitute(assignment_list_substitution(aa));
  cout << "d2 = " << pp(d2) << " " << d2.to_string() << endl;
  cin.get();

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

  return 0;
}
