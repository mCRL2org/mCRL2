#include <iostream>
#include <vector>
#include <algorithm>
#include "atermpp/aterm.h"
#include "mcrl2/lpe.h"

using namespace std;
using namespace mcrl2;
using namespace atermpp;

// function object for printing a variable
struct PrintVariable
{
  void operator()(const DataVariable &v) const
  {
    cout << "   " << v.name() << ":" << v.sort().to_string() << endl;
  }
};

int main()
{
  string filename("data/abp_b.lpe");
  aterm_appl t = read_from_named_file(filename).to_appl();
  if (!t)
    cerr << "could not read file!" << endl;

  LPE lpe(t);
  cout << "--- LPE -----------------" << endl;
  cout << "- free variables:" << endl;
  for_each(lpe.free_variables_begin(), lpe.free_variables_end(), PrintVariable()); 

  int index = 0;
  for (LPE::summand_iterator i = lpe.summands_begin(); i != lpe.summands_end(); ++i)
  {
    cout << "<LPE summand " << index++ << ">\n";
    for (LPESummand::action_iterator j = i->actions_begin(); j != i->actions_end(); ++j)
      cout << "  <Action>" << j->to_string() << endl;
    cout << "<Condition>" << i->condition().to_string() << endl;
  }

  DataDeclaration decl = lpe.data_declaration();
  for (DataDeclaration::equation_iterator i = decl.equations_begin(); i != decl.equations_end(); ++i)
  {
    cout << "<DataEquation>" << endl;
    DataEquation eq(*i);
    DataExpression lhs = eq.lhs();
    DataExpression rhs = eq.rhs();
    cout << "  <lhs>" << lhs.to_string() << endl;
    cout << "  <rhs>" << rhs.to_string() << endl;
  }

  return 0;
}
