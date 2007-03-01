#include <iostream>
#include <iterator>

#include "atermpp/atermpp.h"
#include "atermpp/algorithm.h"
#include "lps/data.h"
#include "lps/data_utility.h"
#include "lps/sort.h"
#include "lps/specification.h"
#include "lps/detail/tools.h"
#include "test_specifications.h"

using namespace std;
using namespace atermpp;
using namespace lps;
using namespace lps::data_expr;
using namespace lps::detail;

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

int main()
{
  aterm bottom_of_stack;
  aterm_init(bottom_of_stack);
  gsEnableConstructorFunctions();

  specification spec = mcrl22lps(ABP_SPECIFICATION);
  linear_process lps = spec.lps();
  std::set<identifier_string> ids = identifiers(aterm(lps));
  for (std::set<identifier_string>::iterator i = ids.begin(); i != ids.end(); ++i)
  {
    cout << "- " << *i << endl;
  }
  cin.get();
  
  summand summand_ = *lps.summands().begin();
  data_expression d = summand_.condition();
  cout << "d = " << d << endl;
  for (data_variable_list::iterator j = summand_.summation_variables().begin(); j != summand_.summation_variables().end(); ++j)
  {
    data_variable v = *j;
    bool b = occurs_in(d, v);
    cout << "v = " << v << endl;
    cout << "occurs: " << b << endl;
  }

  // find all labels in an LPS
  std::set<action_label> labels;
  find_all_if(lps, is_action_label, inserter(labels, labels.end()));
  cout << "--- labels ---" << endl;
  for (std::set<action_label>::iterator i = labels.begin(); i != labels.end(); ++i)
  {
    cout << i->name() << endl;
  }

/*
  // find all "op id's" in an LPS
  cout << "--- op id's ---" << endl;
  std::set<aterm> s;
  find_all_if(lps, is_op_id, std::inserter(s, s.end()));
  for (std::set<aterm>::iterator i = s.begin(); i != s.end(); ++i)
  {
    cout << *i << endl;
  }
*/

  return 0;
}
