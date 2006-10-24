///////////////////////////////////////////////////////////////////////////////
/// \file lpe/detail/utility.h

#ifndef LPE_DETAIL_UTILITY_H
#define LPE_DETAIL_UTILITY_H

#include <string>
#include <utility>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include "atermpp/aterm.h"
#include "lpe/data.h"
#include "libstruct.h"

namespace lpe {

namespace detail {

// example: "X(d:D,e:E)"
std::pair<std::string, data_expression_list> parse_variable(std::string s)
{
  using boost::algorithm::split;
  using boost::algorithm::is_any_of;

  std::string name;
  data_expression_list variables;

  std::string::size_type idx = s.find('(');
  if (idx == std::string::npos)
  {
    name = s;
  }
  else
  {
    name = s.substr(0, idx);
    assert(*s.rbegin() == ')');
    std::vector<std::string> v;
    std::string w = s.substr(idx + 1, s.size() - idx - 2);
    split(v, w, is_any_of(","));
    for (std::vector<std::string>::reverse_iterator i = v.rbegin(); i != v.rend(); ++i)
    {
      data_expression d = data_variable(*i);
      variables = push_front(variables, d);
    }
  }
  return std::make_pair(name, variables);
}

// OpId(f())
template <typename Function>
inline
bool has_expression_type_level_0(data_expression t, const Function f)
{
  return gsIsOpId(t) && (arg1(t) == aterm_appl(f()));
}

// DataAppl(OpId(f()))
template <typename Function>
inline
bool has_expression_type_level_1(data_expression t, const Function f)
{
  if (!gsIsDataAppl(t))
    return false;   
  aterm_appl t1 = arg1(t);
  return gsIsOpId(t1) && (arg1(t1) == aterm_appl(f()));
}

// DataAppl(DataAppl(OpId(f())))
template <typename Function>
inline
bool has_expression_type_level_2(data_expression t, const Function f)
{
  if (!gsIsDataAppl(t))
    return false;   
  aterm_appl t1 = arg1(t);
  if (!gsIsDataAppl(t1))
    return false;
  aterm_appl t11 = arg1(t1);
  return gsIsOpId(t11) && (arg1(t11) == aterm_appl(f()));
}

} // namespace detail

} // namespace lpe

#endif // LPE_DETAIL_UTILITY_H
