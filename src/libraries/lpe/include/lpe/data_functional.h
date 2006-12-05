///////////////////////////////////////////////////////////////////////////////
/// \file data_functional.h

#ifndef LPE_DATA_FUNCTIONAL_H
#define LPE_DATA_FUNCTIONAL_H

#include <boost/config.hpp>
#include "lpe/data.h"
#include "lpe/detail/utility.h"

namespace lpe {

// namespace data {
  using detail::has_expression_type_level_0;
  using detail::has_expression_type_level_1;
  using detail::has_expression_type_level_2;
  
  inline bool is_negate       (data_expression t) { return has_expression_type_level_1(t, gsMakeOpIdNameNeg); }
  inline bool is_plus         (data_expression t) { return has_expression_type_level_2(t, gsMakeOpIdNameAdd); }
  inline bool is_minus        (data_expression t) { return has_expression_type_level_2(t, gsMakeOpIdNameSubt); }
  inline bool is_multiplies   (data_expression t) { return has_expression_type_level_2(t, gsMakeOpIdNameMult); }
  inline bool is_divides      (data_expression t) { return has_expression_type_level_2(t, gsMakeOpIdNameDiv); }
  inline bool is_modulus      (data_expression t) { return has_expression_type_level_2(t, gsMakeOpIdNameMod); }
  inline bool is_equal_to     (data_expression t) { return has_expression_type_level_2(t, gsMakeOpIdNameEq); }
  inline bool is_not_equal_to (data_expression t) { return has_expression_type_level_2(t, gsMakeOpIdNameNeq); }
  inline bool is_less         (data_expression t) { return has_expression_type_level_2(t, gsMakeOpIdNameLT); }
  inline bool is_greater      (data_expression t) { return has_expression_type_level_2(t, gsMakeOpIdNameGT); }
  inline bool is_less_equal   (data_expression t) { return has_expression_type_level_2(t, gsMakeOpIdNameLTE); }
  inline bool is_greater_equal(data_expression t) { return has_expression_type_level_2(t, gsMakeOpIdNameGTE); }
  inline bool is_min          (data_expression t) { return has_expression_type_level_2(t, gsMakeOpIdNameMin); }
  inline bool is_max          (data_expression t) { return has_expression_type_level_2(t, gsMakeOpIdNameMax); }
  inline bool is_abs          (data_expression t) { return has_expression_type_level_1(t, gsMakeOpIdNameAbs); }
  
  inline
  data_expression negate(data_expression d)
  {
    return gsMakeDataExprNeg(d);
  }
  
  inline
  data_expression plus(data_expression d, data_expression e)
  {
    return gsMakeDataExprAdd(d, e);
  }
  
  inline
  data_expression minus(data_expression d, data_expression e)
  {
    return gsMakeDataExprSubt(d, e);
  }
  
  inline
  data_expression multiplies(data_expression d, data_expression e)
  {
    return gsMakeDataExprMult(d, e);
  }
  
  inline
  data_expression divides(data_expression d, data_expression e)
  {
    return gsMakeDataExprDiv(d, e);
  }
  
  inline
  data_expression modulus(data_expression d, data_expression e)
  {
    return gsMakeDataExprMod(d, e);
  }
  
  inline
  data_expression equal_to(data_expression d, data_expression e)
  {
    return gsMakeDataExprEq(d, e);
  }
  
  inline
  data_expression not_equal_to(data_expression d, data_expression e)
  {
    return gsMakeDataExprNeq(d, e);
  }
  
  inline
  data_expression less(data_expression d, data_expression e)
  {
    return gsMakeDataExprLT(d, e);
  }
  
  inline
  data_expression greater(data_expression d, data_expression e)
  {
    return gsMakeDataExprGT(d, e);
  }
  
  inline
  data_expression less_equal(data_expression d, data_expression e)
  {
    return gsMakeDataExprLTE(d, e);
  }
  
  inline
  data_expression greater_equal(data_expression d, data_expression e)
  {
    return gsMakeDataExprGTE(d, e);
  }
  
  // MSVC is broken with respect to using 'min' and 'max' as identifiers.
  inline
  data_expression min_(data_expression d, data_expression e)
  {
    return gsMakeDataExprMin(d, e);
  }
  
  // MSVC is broken with respect to using 'min' and 'max' as identifiers.
  inline
  data_expression max_(data_expression d, data_expression e)
  {
    return gsMakeDataExprMax(d, e);
  }
  
  inline
  data_expression abs(data_expression d)
  {
    return gsMakeDataExprAbs(d);
  }

  inline
  data_expression if_(data_expression i, data_expression t, data_expression e)
  {
    return gsMakeDataExprIf(i, t, e);
  }

// } // namespace data

} // namespace lpe

#endif // LPE_DATA_FUNCTIONAL_H
