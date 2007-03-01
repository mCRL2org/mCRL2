///////////////////////////////////////////////////////////////////////////////
/// \file data_operators.h

#ifndef LPS_DATA_OPERATORS_H
#define LPS_DATA_OPERATORS_H

#include "lps/data.h"

namespace lps {

///
/// Return the concatenation of the lists l and m.
///
inline
data_expression_list operator+(data_expression_list l, data_variable_list m)
{ return data_expression_list(ATconcat(l, m)); }

///
/// Return the concatenation of the lists l and m.
///
inline
data_expression_list operator+(data_variable_list l, data_expression_list m)
{ return data_expression_list(ATconcat(l, m)); }

///
/// Return the concatenation of [v] and the list l.
///
inline
data_expression_list operator+(data_variable v, data_expression_list l)
{
  return data_expression(aterm_appl(v)) + l;
}

///
/// Return the concatenation of the list l and [v].
///
inline
data_expression_list operator+(data_expression_list l, data_variable v)
{
  return l + data_expression(aterm_appl(v));
}

} // namespace lps

#endif // LPS_DATA_OPERATORS_H
