#ifndef LPS_DETAIL_DATA_EXPR_UTILITY_H
#define LPS_DETAIL_DATA_EXPR_UTILITY_H

#include <vector>
#include <string>
#include <utility>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include "atermpp/aterm_appl.h"
#include "atermpp/aterm_access.h"
#include "libstruct.h"

namespace lps {

namespace detail {

// OpId(f())
template <typename Function>
inline
bool has_expression_type_level_0(aterm_appl t, const Function f)
{
  return gsIsOpId(t) && (arg1(t) == f());
}

// DataAppl(OpId(f()))
template <typename Function>
inline
bool has_expression_type_level_1(aterm_appl t, const Function f)
{
  if (!gsIsDataAppl(t))
    return false;   
  aterm_appl t1 = arg1(t);
  return gsIsOpId(t1) && (arg1(t1) == f());
}

// DataAppl(DataAppl(OpId(f())))
template <typename Function>
inline
bool has_expression_type_level_2(aterm_appl t, const Function f)
{
  if (!gsIsDataAppl(t))
    return false;   
  aterm_appl t1 = arg1(t);
  if (!gsIsDataAppl(t1))
    return false;
  aterm_appl t11 = arg1(t1);
  return gsIsOpId(t11) && (arg1(t11) == f());
}

} // namespace detail

} // namespace lps

#endif // LPS_DETAIL_DATA_EXPR_UTILITY_H
