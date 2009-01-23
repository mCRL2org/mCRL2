#include <algorithm>
#include <iostream>
#include <cstdio>
#include <string.h>
#include "lpsparunfoldlib.h"
#include <iostream>
#include <string>
#include "mcrl2/core/messaging.h"


using namespace std;
using namespace mcrl2::core;
using namespace mcrl2::data;

Sorts::Sorts(boost::iterator_range<mcrl2::data::sort_expression_list::const_iterator> s)
{
  sortSet = s;
};

void Sorts::generateFreshSort()
{
  std::set<identifier_string> s;

  for( mcrl2::data::sort_expression_list::const_iterator i = sortSet.begin();
                                                         i != sortSet.end();
                                                         ++i)
  {
    if (i->is_basic_sort())
    {
      s.insert( (static_cast<basic_sort>(*i)).name() );
    }
  };

  mcrl2::data::postfix_identifier_generator generator("");
  generator.add_identifiers(s);
  identifier_string new_name = generator( "Pos");

  cout << new_name << endl;
  
  assert(false);
  return;
}
