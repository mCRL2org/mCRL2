///////////////////////////////////////////////////////////////////////////////
/// \file mucalculus_init.h
//
#ifndef LPE_MUCALCULUS_INIT_H
#define LPE_MUCALCULUS_INIT_H

#include "libstruct.h"
#include "lpe/mucalculus.h"

namespace lpe {

using atermpp::aterm_string;

namespace act_init {

// <ActFrm>       ::= <MultAct>
//                  | <DataExpr>
//                  | ActTrue
//                  | ActFalse
//                  | ActNot(<ActFrm>)
//                  | ActAnd(<ActFrm>, <ActFrm>)
//                  | ActOr(<ActFrm>, <ActFrm>)
//                  | ActImp(<ActFrm>, <ActFrm>)
//                  | ActForall(<DataVarId>+, <ActFrm>)
//                  | ActExists(<DataVarId>+, <ActFrm>)
//                  | ActAt(<ActFrm>, <DataExpr>)

  inline
  action_formula data(data_expression d)
  {
    return action_formula(aterm_appl(d));
  }
  
  inline
  action_formula true_()
  {
    return action_formula(gsMakeActTrue());
  }
  
  inline
  action_formula false_()
  {
    return action_formula(gsMakeActFalse());
  }
  
  inline
  action_formula not_(action_formula p)
  {
    return action_formula(gsMakeActNot(p));
  }
  
  inline
  action_formula and_(action_formula p, action_formula q)
  {
    return action_formula(gsMakeActAnd(p,q));
  }
  
  inline
  action_formula or_(action_formula p, action_formula q)
  {
    return action_formula(gsMakeActOr(p,q));
  }
  
  inline
  action_formula imp(action_formula p, action_formula q)
  {
    return action_formula(gsMakeActImp(p,q));
  }
  
  inline
  action_formula forall(data_variable_list l, action_formula p)
  {
    return action_formula(gsMakeActExists(l, p));
  }

  inline
  action_formula exists(data_variable_list l, action_formula p)
  {
    return action_formula(gsMakeActExists(l, p));
  }

  inline
  action_formula at(action_formula p, data_expression d)
  {
    return action_formula(gsMakeActAt(p,d));
  }

  inline bool is_mult_act   (action_formula t) { return gsIsMultAct  (t); }
  inline bool is_data       (action_formula t) { return gsIsDataExpr (t); }
  inline bool is_true       (action_formula t) { return gsIsActTrue  (t); }
  inline bool is_false      (action_formula t) { return gsIsActFalse (t); }
  inline bool is_not        (action_formula t) { return gsIsActNot   (t); }
  inline bool is_and        (action_formula t) { return gsIsActAnd   (t); }
  inline bool is_or         (action_formula t) { return gsIsActOr    (t); }
  inline bool is_imp        (action_formula t) { return gsIsActImp   (t); }
  inline bool is_forall     (action_formula t) { return gsIsActForall(t); }
  inline bool is_exists     (action_formula t) { return gsIsActExists(t); }
  inline bool is_at         (action_formula t) { return gsIsActAt    (t); }

} // namespace act_init

namespace reg_init {

// <RegFrm>       ::= <ActFrm>
//                  | RegNil                                                (-rft)
//                  | RegSeq(<RegFrm>, <RegFrm>)                            (-rft)
//                  | RegAlt(<RegFrm>, <RegFrm>)                            (-rft)
//                  | RegTrans(<RegFrm>)                                    (-rft)
//                  | RegTransOrNil(<RegFrm>)                               (-rft)

  inline
  regular_formula nil_()
  {
    return regular_formula(gsMakeRegNil());
  }
  
  inline
  regular_formula seq(regular_formula p, regular_formula q)
  {
    return regular_formula(gsMakeRegSeq(p,q));
  }
  
  inline
  regular_formula alt(regular_formula p, regular_formula q)
  {
    return regular_formula(gsMakeRegAlt(p,q));
  }
  
  inline
  regular_formula trans(regular_formula p)
  {
    return regular_formula(gsMakeRegTrans(p));
  }
  
  inline
  regular_formula trans_or_nil(regular_formula p)
  {
    return regular_formula(gsMakeRegTransOrNil(p));
  }

  inline bool is_act         (regular_formula t) { return gsIsActFrm       (t); }
  inline bool is_nil         (regular_formula t) { return gsIsRegNil       (t); }
  inline bool is_seq         (regular_formula t) { return gsIsRegSeq       (t); }
  inline bool is_alt         (regular_formula t) { return gsIsRegAlt       (t); }
  inline bool is_trans       (regular_formula t) { return gsIsRegTrans     (t); }
  inline bool is_trans_or_nil(regular_formula t) { return gsIsRegTransOrNil(t); }

} // namespace reg_init

namespace state_init {

// <StateFrm>     ::= <DataExpr>
//                  | StateTrue
//                  | StateFalse
//                  | StateNot(<StateFrm>)
//                  | StateAnd(<StateFrm>, <StateFrm>)
//                  | StateOr(<StateFrm>, <StateFrm>)
//                  | StateImp(<StateFrm>, <StateFrm>)
//                  | StateForall(<DataVarId>+, <StateFrm>)
//                  | StateExists(<DataVarId>+, <StateFrm>)
//                  | StateMust(<RegFrm>, <StateFrm>)
//                  | StateMay(<RegFrm>, <StateFrm>)
//                  | StateDelay
//                  | StateDelayTimed(<DataExpr>)
//                  | StateVar(<String>, <DataExpr>*)
//                  | StateNu(<String>, <DataVarIdInit>*, <StateFrm>)
//                  | StateMu(<String>, <DataVarIdInit>*, <StateFrm>)

  inline
  state_formula false_()
  {
    return state_formula(gsMakeStateFalse());
  }
  
  inline
  state_formula true_()
  {
    return state_formula(gsMakeStateTrue());
  }
  
  inline
  state_formula not_(state_formula p)
  {
    return state_formula(gsMakeStateNot(p));
  }
  
  inline
  state_formula and_(state_formula p, state_formula q)
  {
    return state_formula(gsMakeStateAnd(p,q));
  }
  
  inline
  state_formula or_(state_formula p, state_formula q)
  {
    return state_formula(gsMakeStateOr(p,q));
  }
  
  inline
  state_formula imp(state_formula p, state_formula q)
  {
    return state_formula(gsMakeStateImp(p,q));
  }
  
  inline
  state_formula exists(data_variable_list l, state_formula p)
  {
    return state_formula(gsMakeStateExists(l, p));
  }
  
  inline
  state_formula forall(data_variable_list l, state_formula p)
  {
    return state_formula(gsMakeStateExists(l, p));
  }

  inline
  state_formula must(regular_formula r, state_formula p)
  {
    return state_formula(gsMakeStateMust(r, p));
  }

  inline
  state_formula may(regular_formula r, state_formula p)
  {
    return state_formula(gsMakeStateMay(r, p));
  }

  inline
  state_formula delay()
  {
    return state_formula(gsMakeStateDelay());
  }

  inline
  state_formula delay_timed(data_expression t)
  {
    return state_formula(gsMakeStateDelayTimed(t));
  }

  inline
  state_formula var(std::string name, data_expression_list l)
  {
    return state_formula(gsMakeStateVar(aterm_string(name), l));
  }

  inline
  state_formula mu(std::string name, data_variable_init_list l, state_formula p)
  {
    return state_formula(gsMakeStateMu(aterm_string(name), l, p));
  }

  inline
  state_formula nu(std::string name, data_variable_init_list l, state_formula p)
  {
    return state_formula(gsMakeStateNu(aterm_string(name), l, p));
  }

  inline bool is_data       (state_formula t) { return gsIsDataExpr       (t); }
  inline bool is_true       (state_formula t) { return gsIsStateTrue      (t); }
  inline bool is_false      (state_formula t) { return gsIsStateFalse     (t); }
  inline bool is_not        (state_formula t) { return gsIsStateNot       (t); }
  inline bool is_and        (state_formula t) { return gsIsStateAnd       (t); }
  inline bool is_or         (state_formula t) { return gsIsStateOr        (t); }
  inline bool is_imp        (state_formula t) { return gsIsStateImp       (t); }
  inline bool is_forall     (state_formula t) { return gsIsStateForall    (t); }
  inline bool is_exists     (state_formula t) { return gsIsStateExists    (t); }
  inline bool is_must       (state_formula t) { return gsIsStateMust      (t); }
  inline bool is_may        (state_formula t) { return gsIsStateMay       (t); }
  inline bool is_delay      (state_formula t) { return gsIsStateDelay     (t); }
  inline bool is_delay_timed(state_formula t) { return gsIsStateDelayTimed(t); }
  inline bool is_yaled      (state_formula t) { return gsIsStateYaled     (t); }
  inline bool is_yaled_timed(state_formula t) { return gsIsStateYaledTimed(t); }
  inline bool is_var        (state_formula t) { return gsIsStateVar       (t); }
  inline bool is_nu         (state_formula t) { return gsIsStateNu        (t); }
  inline bool is_mu         (state_formula t) { return gsIsStateMu        (t); }

} // namespace state_init

} // namespace lpe

#endif // LPE_MUCALCULUS_INIT_H
