///////////////////////////////////////////////////////////////////////////////
/// \file data_init.h
//
/// Convenience functions for creating DataExpr formulas.

#ifndef LPE_DATA_INIT_H
#define LPE_DATA_INIT_H

#include "lpe/data.h"
#include "lpe/detail/utility.h"

namespace lpe {

namespace data_init {

// //data expression
// <DataExpr>     ::= Id(<String>)                                          (- tc)
//                  | <DataVarId>                                           (+ tc)
//                  | <OpId>                                                (+ tc)
//                  | DataApplProd(<DataExpr>, <DataExpr>+)                 (- di)
//                  | DataAppl(<DataExpr>, <DataExpr>)                      (+ di)
//                  | Number(<NumberString>, <SortExprOrUnknown>)           (- di)
//                  | ListEnum(<DataExpr>+, <SortExprOrUnknown>)            (- di)
//                  | SetEnum(<DataExpr>+, <SortExprOrUnknown>)             (- di)
//                  | BagEnum(<BagEnumElt>+, <SortExprOrUnknown>)           (- di)
//                  | SetBagComp(<DataVarId>, <DataExpr>)                   (- di)
//                  | Forall(<DataVarId>+, <DataExpr>)                      (- di)
//                  | Exists(<DataVarId>+, <DataExpr>)                      (- di)
//                  | Lambda(<DataVarId>+, <DataExpr>)                      (- di)
//                  | Whr(<DataExpr>, <WhrDecl>+)                           (- di)

using detail::has_expression_type_level_0;
using detail::has_expression_type_level_1;
using detail::has_expression_type_level_2;

// TODO: the names for these functions need to be improved.
  inline bool is_id            (data_expression t) { return gsIsId           (t); }
  inline bool is_data_var_id   (data_expression t) { return gsIsDataVarId    (t); }
  inline bool is_op_id         (data_expression t) { return gsIsOpId         (t); }
  inline bool is_data_appl_prod(data_expression t) { return gsIsDataApplProd (t); }
  inline bool is_data_appl     (data_expression t) { return gsIsDataAppl     (t); }
  inline bool is_number        (data_expression t) { return gsIsNumber       (t); }
  inline bool is_list_enum     (data_expression t) { return gsIsListEnum     (t); }
  inline bool is_set_enum      (data_expression t) { return gsIsSetEnum      (t); }
  inline bool is_bag_enum      (data_expression t) { return gsIsBagEnum      (t); }
  inline bool is_set_bag_comp  (data_expression t) { return gsIsSetBagComp   (t); }
  inline bool is_forall        (data_expression t) { return gsIsForall       (t); }
  inline bool is_exists        (data_expression t) { return gsIsExists       (t); }
  inline bool is_lambda        (data_expression t) { return gsIsLambda       (t); }
  inline bool is_where         (data_expression t) { return gsIsWhr          (t); }

  inline bool is_true          (data_expression t) { return has_expression_type_level_0(t, gsMakeOpIdNameTrue ); }
  inline bool is_false         (data_expression t) { return has_expression_type_level_0(t, gsMakeOpIdNameFalse); }
  inline bool is_not           (data_expression t) { return has_expression_type_level_1(t, gsMakeOpIdNameNot  ); }
  inline bool is_and           (data_expression t) { return has_expression_type_level_2(t, gsMakeOpIdNameAnd  ); }
  inline bool is_or            (data_expression t) { return has_expression_type_level_2(t, gsMakeOpIdNameOr   ); }

  inline
  data_expression true_()
  {
    return data_expression(gsMakeDataExprTrue());
  }
  
  inline
  data_expression false_()
  {
    return data_expression(gsMakeDataExprFalse());
  }
  
  inline
  data_expression not_(data_expression p)
  {
    if (is_true(p))
      return false_();
    else if (is_false(p))
      return true_();
    else
      return data_expression(gsMakeDataExprNot(p));
  }
  
  inline
  data_expression and_(data_expression p, data_expression q)
  {
    if (is_true(p))
      return q;
    else if (is_false(p))
      return false_();
    if (is_true(q))
      return p;
    else if (is_false(q))
      return false_();
    else
      return data_expression(gsMakeDataExprAnd(p,q));
  }
  
  inline
  data_expression or_(data_expression p, data_expression q)
  {
    if (is_true(p))
      return true_();
    else if (is_false(p))
      return q;
    if (is_true(q))
      return true_();
    else if (is_false(q))
      return p;
    else
      return data_expression(gsMakeDataExprOr(p,q));
  }

  /// Returns or_ applied to the sequence of data expressions [first, last[
  template <typename FwdIt>
  inline data_expression multi_or(FwdIt first, FwdIt last)
  {
    using namespace data_init;
  
    if (first == last)
      return data_init::false_();
    data_expression result = *first++;
    while (first != last)
    {
      result = or_(result, *first++);
    }
    return result;
  }
  
  /// Returns and_ applied to the sequence of data expressions [first, last[
  template <typename FwdIt>
  inline data_expression multi_and(FwdIt first, FwdIt last)
  {
    using namespace data_init;
  
    if (first == last)
      return data_init::true_();
    data_expression result = *first++;
    while (first != last)
    {
      result = and_(result, *first++);
    }
    return result;
  }

} // namespace data_init

} // namespace lpe

#endif // LPE_DATA_INIT_H
