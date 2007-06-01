///////////////////////////////////////////////////////////////////////////////
/// \file data_functional.h

#ifndef LPS_DATA_FUNCTIONAL_H
#define LPS_DATA_FUNCTIONAL_H

#include <boost/config.hpp>
#include "lps/data.h"
#include "lps/detail/utility.h"

namespace lps {

  /// \brief Returns true if t is a negate expression
  inline bool is_negate(data_expression t) { return gsIsDataExprNeg(t); }

  /// \brief Returns true if t is a plus expression
  inline bool is_plus(data_expression t) { return gsIsDataExprAdd(t); }

  /// \brief Returns true if t is a minus expression
  inline bool is_minus(data_expression t) { return gsIsDataExprSubt(t); }

  /// \brief Returns true if t is a multiply expression
  inline bool is_multiplies(data_expression t) { return gsIsDataExprMult(t); }

  /// \brief Returns true if t is a division expression
  inline bool is_divides(data_expression t) { return gsIsDataExprDiv(t); }

  /// \brief Returns true if t is a modulus expression
  inline bool is_modulus(data_expression t) { return gsIsDataExprMod(t); }

  /// \brief Returns true if t is an equality expression
  inline bool is_equal_to(data_expression t) { return gsIsDataExprEq(t); }

  /// \brief Returns true if t is an inequality expression
  inline bool is_not_equal_to(data_expression t) { return gsIsDataExprNeq(t); }

  /// \brief Returns true if t is a less expression
  inline bool is_less(data_expression t) { return gsIsDataExprLT(t); }

  /// \brief Returns true if t is a greater expression
  inline bool is_greater(data_expression t) { return gsIsDataExprGT(t); }

  /// \brief Returns true if t is a less-equal expression
  inline bool is_less_equal(data_expression t) { return gsIsDataExprLTE(t); }

  /// \brief Returns true if t is a greater-equal expression
  inline bool is_greater_equal(data_expression t) { return gsIsDataExprGTE(t); }

  /// \brief Returns true if t is a min expression
  inline bool is_min(data_expression t) { return gsIsDataExprMin(t); }

  /// \brief Returns true if t is a max expression
  inline bool is_max(data_expression t) { return gsIsDataExprMax(t); }

  /// \brief Returns true if t is an abs expression
  inline bool is_abs(data_expression t) { return gsIsDataExprAbs(t); }

  /// \brief Returns the expression d - e
  inline
  data_expression negate(data_expression d)
  {
    return gsMakeDataExprNeg(d);
  }
  
  /// \brief Returns the expression d + e
  inline
  data_expression plus(data_expression d, data_expression e)
  {
    return gsMakeDataExprAdd(d, e);
  }
  
  /// \brief Returns the expression d - e
  inline
  data_expression minus(data_expression d, data_expression e)
  {
    return gsMakeDataExprSubt(d, e);
  }
  
  /// \brief Returns the expression d * e
  inline
  data_expression multiplies(data_expression d, data_expression e)
  {
    return gsMakeDataExprMult(d, e);
  }
  
  /// \brief Returns the expression d / e
  inline
  data_expression divides(data_expression d, data_expression e)
  {
    return gsMakeDataExprDiv(d, e);
  }
  
  /// \brief Returns the expression d % e
  inline
  data_expression modulus(data_expression d, data_expression e)
  {
    return gsMakeDataExprMod(d, e);
  }
  
  /// \brief Returns the expression d = e
  inline
  data_expression equal_to(data_expression d, data_expression e)
  {
    return gsMakeDataExprEq(d, e);
  }
  
  /// \brief Returns the expression d != e
  inline
  data_expression not_equal_to(data_expression d, data_expression e)
  {
    return gsMakeDataExprNeq(d, e);
  }
  
  /// \brief Returns the expression d < e
  inline
  data_expression less(data_expression d, data_expression e)
  {
    return gsMakeDataExprLT(d, e);
  }
  
  /// \brief Returns the expression d > e
  inline
  data_expression greater(data_expression d, data_expression e)
  {
    return gsMakeDataExprGT(d, e);
  }
  
  /// \brief Returns the expression d <= e
  inline
  data_expression less_equal(data_expression d, data_expression e)
  {
    return gsMakeDataExprLTE(d, e);
  }
  
  /// \brief Returns the expression d >= e
  inline
  data_expression greater_equal(data_expression d, data_expression e)
  {
    return gsMakeDataExprGTE(d, e);
  }
  
  /// \brief Returns an expression for the minimum of d and e
  inline
  data_expression min_(data_expression d, data_expression e)
  {
    return gsMakeDataExprMin(d, e);
  }
  
  /// \brief Returns an expression for the maximum of d and e
  inline
  data_expression max_(data_expression d, data_expression e)
  {
    return gsMakeDataExprMax(d, e);
  }
  
  /// \brief Returns an expression for the absolute value of d
  inline
  data_expression abs(data_expression d)
  {
    return gsMakeDataExprAbs(d);
  }

  /// \brief Returns the expression 'if i then t else e'
  inline
  data_expression if_(data_expression i, data_expression t, data_expression e)
  {
    return gsMakeDataExprIf(i, t, e);
  }

} // namespace lps

#endif // LPS_DATA_FUNCTIONAL_H
