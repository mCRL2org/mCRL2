#include <iostream>

#include "atermpp/algorithm.h"
#include "lpe/data.h"
#include "lpe/sort.h"
#include "lpe/substitute.h"
#include "lpe/specification.h"

using namespace std;
using namespace atermpp;
using namespace lpe;

struct is_identifier
{
  bool operator()(aterm t) const
  {
    return t.type() == AT_APPL && aterm_appl(t).argument_list().size() == 0;
  }
};

struct is_data_variable
{
  aterm d;

  is_data_variable(data_variable d_)
    : d(aterm_appl(d_))
  {}

  bool operator()(aterm t) const
  {
    return d == t;
  }
};

/// Returns the set of all identifiers occurring in the term t.
inline
std::set<aterm_string> identifiers(aterm t)
{
  std::set<aterm_string> result;
  find_all_if(t, is_identifier(), std::inserter(result, result.end()));
  return result;
}

bool occurs_in(data_expression d, data_variable v)
{
  return find_if(aterm_appl(d), is_data_variable(v)) != aterm();
}

int main()
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
  std::set<aterm_string> ids = identifiers(aterm(lpe));
  for (std::set<aterm_string>::iterator i = ids.begin(); i != ids.end(); ++i)
  {
    cout << "- " << *i << endl;
  }
  cin.get();
  
  LPE_summand summand = *lpe.summands().begin();
  data_expression d = summand.condition();
  cout << "d = " << d << endl;
  for (data_variable_list::iterator j = summand.summation_variables().begin(); j != summand.summation_variables().end(); ++j)
  {
    data_variable v = *j;
    bool b = occurs_in(d, v);
    cout << "v = " << v << endl;
    cout << "occurs: " << b << endl;
  }
  
  return 0;
}
