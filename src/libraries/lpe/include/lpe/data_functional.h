///////////////////////////////////////////////////////////////////////////////
/// \file data_functional.h

#ifndef LPE_DATA_FUNCTIONAL_H
#define LPE_DATA_FUNCTIONAL_H

#include <boost/config.hpp>
#include "lpe/data.h"

namespace lpe {

/*
inline bool is_negate       (aterm_appl t) { return ; }
inline bool is_plus         (aterm_appl t) { return ; }
inline bool is_minus        (aterm_appl t) { return ; }
inline bool is_multiplies   (aterm_appl t) { return ; }
inline bool is_divides      (aterm_appl t) { return ; }
inline bool is_modulus      (aterm_appl t) { return ; }
inline bool is_equal_to     (aterm_appl t) { return ; }
inline bool is_not_equal_to (aterm_appl t) { return ; }
inline bool is_less         (aterm_appl t) { return ; }
inline bool is_greater      (aterm_appl t) { return ; }
inline bool is_less_equal   (aterm_appl t) { return ; }
inline bool is_greater_equal(aterm_appl t) { return ; }
inline bool is_min          (aterm_appl t) { return ; }
inline bool is_max          (aterm_appl t) { return ; }
inline bool is_abs          (aterm_appl t) { return ; }
*/

inline
data_expression negate(data_expression d)
{
  return gsMakeDataExprNot(d);
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

inline
data_expression min BOOST_PREVENT_MACRO_SUBSTITUTION (data_expression d, data_expression e)
{
  return gsMakeDataExprMin(d, e);
}

inline
data_expression max BOOST_PREVENT_MACRO_SUBSTITUTION (data_expression d, data_expression e)
{
  return gsMakeDataExprMax(d, e);
}

inline
data_expression abs(data_expression d)
{
  return gsMakeDataExprAbs(d);
}

/*
//==============================================================//
//                       numbers
//==============================================================//

/// Returns '0' of sort Pos
inline
data_expression c0()
{
  return gsMakeDataExprC0();
}

/// Returns '1' of sort Pos
inline
data_expression c1()
{
  return gsMakeDataExprC1();
}

inline
data_expression if_(data_expression i, data_expression t, data_expression e)
{
  return gsMakeDataExprIf(i, t, e);
}


inline
data_expression cdub()
{
  return gsMakeDataExprCDub(bit, pos);
}


/// Conversion from pos to nat.
inline
data_expression cnat(data_expression d)
{
  return gsMakeDataExprCNat(d);
}
*/

} // namespace lpe

#endif // LPE_DATA_FUNCTIONAL_H
