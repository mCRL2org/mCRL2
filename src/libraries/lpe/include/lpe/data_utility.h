///////////////////////////////////////////////////////////////////////////////
/// \file data_utility.h

#ifndef LPE_DATA_UTILITY_H
#define LPE_DATA_UTILITY_H

#include <string>
#include <utility>
#include <set>
#include <iterator>

#include "boost/format.hpp"
#include "lpe/data.h"
#include "lpe/sort_init.h"
#include "atermpp/algorithm.h"
#include "atermpp/aterm.h"
#include "atermpp/aterm_list.h"
#include "atermpp/aterm_string.h"

namespace lpe {

/// Test if a term is an identifier.
struct is_identifier
{
  bool operator()(aterm t) const
  {
    return t.type() == AT_APPL && aterm_appl(t).size() == 0;
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

/// Returns an identifier that doesn't appear in the term context.
inline
aterm_string fresh_identifier(std::string hint, aterm context)
{
  std::set<aterm_string> ids = identifiers(context);
  aterm_string s(hint);
  int index = 0;
  while (ids.find(s) != ids.end())
  {   
    std::string name = str(boost::format(hint + "%02d") % index++);
    s = aterm_string(name);
  }
  return s;
}

/// Returns a variable that doesn't appear in context.
inline
data_variable fresh_variable(std::string hint, aterm context, lpe::sort s = sort_init::real())
{
  aterm_string id = fresh_identifier(hint, context);
  return data_variable(gsMakeDataVarId(id, s));
}

/// Fresh variable generator that generates data variables with
/// names that do not appear in the given context.
class fresh_variable_generator
{
  protected:
    std::set<aterm_string> m_identifiers;
    std::string m_hint;
    lpe::sort m_sort;
  
  public:
    fresh_variable_generator(aterm context, std::string hint = "t", lpe::sort s = sort_init::real())
    {
      m_identifiers = identifiers(context);
      m_hint = hint;
      m_sort = s;
    }

    /// Set a new hint.
    void set_hint(std::string hint)
    {
      m_hint = hint;
    }

    /// Returns the current hint.
    std::string hint() const
    {
      return m_hint;
    }

    /// Set a new context.
    void set_context(aterm context)
    {
      m_identifiers = identifiers(context);
    }

    /// Set a new sort.
    void set_sort(lpe::sort s)
    {
      m_sort = s;
    }

    /// Returns the current sort.
    lpe::sort sort() const
    {
      return m_sort;
    }

    /// Returns a unique variable of the given sort, with the given hint as prefix.
    data_variable operator()()
    {
      aterm_string id(m_hint);
      int index = 0;
      while (m_identifiers.find(id) != m_identifiers.end())
      {   
        std::string name = str(boost::format(m_hint + "%02d") % index++);
        id = aterm_string(name);
      }
      m_identifiers.insert(id);
      return data_variable(gsMakeDataVarId(id, m_sort));
    }
};

/// Returns a variable list that doesn't contain terms that appear in context.
inline
data_variable_list fresh_variable_list(unsigned int size, aterm context, std::string hint, lpe::sort s = sort_init::real())
{
  data_variable_list result;
  fresh_variable_generator generator(context, hint, s);
  for (unsigned int i = 0; i < size; i++)
  {
    result = push_front(result, generator());
  }
  return data_variable_list(atermpp::reverse(result));
}

} // namespace lpe

#endif // LPE_DATA_UTILITY_H
