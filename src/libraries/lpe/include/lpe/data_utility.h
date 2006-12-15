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
#include "atermpp/utility.h"

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

/// Returns all data variables that occur in the term t.
template <typename Term>
std::set<data_variable> find_variables(Term t)
{
  // find all data variables in t
  std::set<data_variable> variables;
  atermpp::find_all_if(t, is_data_variable, std::inserter(variables, variables.end()));

  // find all data variable_init's in t (since they contain data variables implicitly!)
  std::set<data_variable_init> variable_inits;
  atermpp::find_all_if(t, lpe::is_data_variable_init, std::inserter(variable_inits, variable_inits.end()));
  for (std::set<data_variable_init>::iterator i = variable_inits.begin(); i != variable_inits.end(); ++i)
  {
    variables.insert(i->to_variable());
  }
  return variables;
}

/// Fresh variable generator that generates data variables with
/// names that do not appear in the given context.
class fresh_variable_generator
{
  protected:
    std::set<data_variable> m_variables; // context
    std::string m_hint;                  // used as a hint for operator()()
    lpe::sort m_sort;                    // used for operator()()

  public:
    fresh_variable_generator()
     : m_hint("t"), m_sort(sort_init::real())
    { }

    template <typename Term>
    fresh_variable_generator(Term context, std::string hint = "t", lpe::sort s = sort_init::real())
    {
      m_variables = find_variables(context);
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
    template <typename Term>
    void set_context(Term context)
    {
      m_variables = find_variables(context);
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

    /// Add variables to context.
    template <typename Iter>
    void add_context_variables(Iter first, Iter last)
    {
      for (Iter i = first; i != last; ++i)
        m_variables.insert(*i);
    }

    /// Returns a unique variable of the given sort, with the given hint as prefix.
    /// The returned variable is added to the context.
    data_variable operator()()
    {
      aterm_string id(m_hint);
      data_variable v(gsMakeDataVarId(id, m_sort));
      int index = 0;
      while (m_variables.find(v) != m_variables.end())
      {   
        id = aterm_string(str(boost::format(m_hint + "%02d") % index++));
        v = data_variable(gsMakeDataVarId(id, m_sort));
      }
      m_variables.insert(v);
      return v;
    }

    /// Returns a unique variable with the same sort as the variable v, and with
    /// the same prefix. The returned variable is added to the context.
    data_variable operator()(data_variable v)
    {
      int index = 0;
      std::string name = atermpp::unquote(v.name());
      while (m_variables.find(v) != m_variables.end())
      {   
        aterm_string id(str(boost::format(name + "%02d") % index++));
        v = data_variable(gsMakeDataVarId(id, v.sort()));
      }
      m_variables.insert(v);
      return v;
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
