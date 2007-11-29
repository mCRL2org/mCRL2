// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_expression_replace.h
/// \brief Contains some functions for replacing data expressions in a term.

#ifndef MCRL2_DATA_DATA_EXPRESSION_REPLACE_H
#define MCRL2_DATA_DATA_EXPRESSION_REPLACE_H

#include <map>
#include "atermpp/algorithm.h"
#include "mcrl2/data/data.h"

namespace lps {

/// \internal
struct data_expression_map_replace_helper
{
  const std::map<data_expression, data_expression>& m_replacements;
    
  data_expression_map_replace_helper(const std::map<data_expression, data_expression>& replacements)
    : m_replacements(replacements)
  {}
  
  data_expression operator()(const data_expression& t) const
  {
    std::map<data_expression, data_expression>::const_iterator i = m_replacements.find(t);
    if (i == m_replacements.end())
    {
      return t;
    }
    else
    {
      return i->second;
    }
  }
};

/// Replaces all data_variables in the term t using the specified map of replacements.
///
template <typename Term>
Term data_expression_map_replace(Term t, const std::map<data_expression, data_expression>& replacements)
{
  return atermpp::checked_replace(t, is_data_expression, data_expression_map_replace_helper(replacements));
}

} // namespace lps

#endif // MCRL2_DATA_DATA_EXPRESSION_REPLACE_H
