///////////////////////////////////////////////////////////////////////////////
/// \file lpe/utility.h

#ifndef LPE_UTILITY_H
#define LPE_UTILITY_H

#include <string>
#include <utility>
#include <set>
#include <iterator>
#include "boost/format.hpp"

#include "atermpp/algorithm.h"
#include "atermpp/aterm.h"
#include "atermpp/aterm_list.h"
#include "atermpp/aterm_string.h"
#include "lpe/data.h"
#include "lpe/data_functional.h"

#include "lpe/mucalculus.h"
#include "lpe/mucalculus_init.h"
#include "lpe/pbes.h"
#include "lpe/pbes_init.h"
#include "lpe/specification.h"

namespace lpe {

using atermpp::aterm_appl;
using atermpp::aterm_list;
using atermpp::aterm_string;
using atermpp::find_all_if;
using atermpp::front;
using atermpp::reverse;

/// Test if a term is an identifier.
struct is_identifier
{
  bool operator()(aterm t) const
  {
    return t.type() == AT_APPL && aterm_appl(t).argument_list().size() == 0;
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

/// Returns a variable that doesn't appear in l.
inline
data_variable fresh_variable(std::string hint, aterm_list l)
{
  std::set<aterm_string> ids = identifiers(l);
  aterm_string s(hint);
  int index = 0;
  while (ids.find(s) != ids.end())
  {   
    std::string name = str(boost::format(hint + "%02d") % index++);
    s = aterm_string(name);
  }
  return data_variable(gsMakeDataVarId(aterm_appl(s), gsMakeSortIdReal()));
}

/// Returns a variable list that doesn't contain terms that appear in l.
data_variable_list fresh_variable_list(std::string hint, unsigned int size, aterm_list l)
{
  data_variable_list result;
  std::set<aterm_string> ids = identifiers(l);
  int index = 0;
  for (unsigned int i = 0; i < size; i++)
  {
    for(;;) // forever
    {
      std::string name = str(boost::format(hint + "%02d") % index++);
      aterm_string s = aterm_string(name);
      if (ids.find(s) != ids.end()) // s does not exist in l
      {
        result = push_front(result, data_variable(gsMakeDataVarId(aterm_appl(s), gsMakeSortIdReal())));
        break;
      }
    }
  }
  return data_variable_list(reverse(result));
}

} // namespace lpe

#endif // LPE_UTILITY_H
