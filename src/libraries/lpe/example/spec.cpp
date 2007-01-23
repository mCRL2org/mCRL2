#include <iostream>
#include <string>
#include <iterator>
#include <fstream>
#include <boost/format.hpp>

#include "atermpp/utility.h"
#include "lpe/specification.h"
#include "lpe/sort.h"
#include "lpe/detail/mcrl22lpe.h"
#include "lpe/soundness_checks.h"
#include "test_specifications.h"

using namespace std;
using namespace atermpp;
using namespace lpe;
using namespace lpe::detail;
using boost::format;

int main()
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  gsEnableConstructorFunctions(); 
  
  specification spec = mcrl22lpe(ABP_SPECIFICATION);
  cout << "check_rule_Spec(spec) = " << check_rule_Spec(spec) << endl;

  aterm_appl f1 = make_term("OpId(\"!=\",SortArrow(SortArrow(SortId(\"Bool\"),SortArrow(SortId(\"Pos\"),SortId(\"Pos\"))),SortArrow(SortArrow(SortId(\"Bool\"),SortArrow(SortId(\"Pos\"),SortId(\"Pos\"))),SortId(\"Bool\"))))");
  cout << "check_rule_OpId(f1) = " << check_rule_OpId(f1) << endl;
  aterm_appl f2 = make_term("OpId(\"!=\",SortArrow(sortarrow(SortId(\"Bool\"),SortArrow(SortId(\"Pos\"),SortId(\"Pos\"))),SortArrow(SortArrow(SortId(\"Bool\"),SortArrow(SortId(\"Pos\"),SortId(\"Pos\"))),SortId(\"Bool\"))))");
  cout << "check_rule_OpId(f2) = " << check_rule_OpId(f2) << endl;
  aterm_appl f3 = make_term("OpId(\"!=\",SortArrow(SortArrow(bogus(\"Bool\"),SortArrow(SortId(\"Pos\"),SortId(\"Pos\"))),SortArrow(SortArrow(SortId(\"Bool\"),SortArrow(SortId(\"Pos\"),SortId(\"Pos\"))),SortId(\"Bool\"))))");
  cout << "check_rule_OpId(f3) = " << check_rule_OpId(f3) << endl;

  cout << "--- sort -----------" << endl;
  for (sort_list::iterator i = spec.data().sorts().begin(); i != spec.data().sorts().end(); ++i)
  {
    lpe::sort s = *i;
    cout << str(format("%5s        %s") % pp(*i) % i->to_string()) << endl;
  }

  cout << "--- map ------------" << endl;
  for (function_list::iterator i = spec.data().mappings().begin(); i != spec.data().mappings().end(); ++i)
  {
    function f = *i;
    cout << str(format("%5s        %s") % pp(*i) % i->to_string()) << endl;
  }

  cout << "--- cons -----------" << endl;
  for (function_list::iterator i = spec.data().constructors().begin(); i != spec.data().constructors().end(); ++i)
  {
    function f = *i;
    cout << str(format("%5s        %s") % pp(*i) % i->to_string()) << endl;
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
    cout << str(format("%8s : %8s  %s") % unquote(i->name()) % pp(i->sort()) % i->to_string()) << endl;
  }
  cout << endl;

  lpe::sort D("D");
  data_variable v("d1", D);
  cout << "v  = " << pp(v) << " " << v.to_string() << endl;

  data_variable w("YES", D);
  data_assignment a(v, w);
  cout << "a = " << pp(a) << " " << a.to_string() << endl;

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
//    cout << "summand " << pp(*i) << endl;
  }
  
  LPE_summand s = spec.lpe().summands().front();
  cout << "actions:" << endl;
  for (action_list::iterator i = s.actions().begin(); i != s.actions().end(); ++i)
  {
    action a = *i;
    action b(i->name(), i->arguments());
    cout << i->to_string() << endl;
    cout << "b == " << b << endl;
  }

  ofstream o1("lpe1.txt");
  o1 << lpe.to_string();

  ofstream o2("lpe2.txt");
  o2 << lpe.substitute(a).to_string(); 

  return 0;
}
