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


} // namespace detail

} // namespace lpe

#endif // LPE_DETAIL_UTILITY_H
