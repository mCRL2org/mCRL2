// Interface to classes ATerm_Manipulator, AM_Jitty and AM_Inner
// file: manipulator.h

#ifndef MANIPULATOR_H
#define MANIPULATOR_H

#include "librewrite.h"
#include "info.h"

class ATerm_Manipulator {
  protected:
    Rewriter* f_rewriter;
    ATerm_Info* f_info;
    ATermTable f_set_true;
    ATermTable f_set_false;
    ATermTable f_orient;
    ATerm f_true;
    ATerm f_false;
    ATerm f_if_then_else;
    virtual ATerm set_true_auxiliary(ATerm a_formula, ATerm a_guard) = 0;
    virtual ATerm set_false_auxiliary(ATerm a_formula, ATerm a_guard) = 0;
    virtual ATerm make_if_then_else(ATerm a_expr, ATerm a_high, ATerm a_low) = 0;
  public:
    ATerm_Manipulator(Rewriter* a_rewriter, ATerm_Info* f_info);
    virtual ~ATerm_Manipulator();
    virtual ATerm make_reduced_if_then_else(ATerm a_expr, ATerm a_high, ATerm a_low) = 0;
    virtual ATerm orient(ATerm a_term) = 0;
    virtual ATerm set_true(ATerm a_formula, ATerm a_guard) = 0;
    virtual ATerm set_false(ATerm a_formula, ATerm a_guard) = 0;
};

class AM_Jitty: public ATerm_Manipulator {
  protected:
    virtual ATerm set_true_auxiliary(ATerm a_formula, ATerm a_guard);
    virtual ATerm set_false_auxiliary(ATerm a_formula, ATerm a_guard);
    virtual ATerm make_if_then_else(ATerm a_expr, ATerm a_high, ATerm a_low);
  public:
    AM_Jitty(Rewriter* a_rewriter, ATerm_Info* f_info);
    virtual ~AM_Jitty();
    virtual ATerm make_reduced_if_then_else(ATerm a_expr, ATerm a_high, ATerm a_low);
    virtual ATerm orient(ATerm a_term);
    virtual ATerm set_true(ATerm a_formula, ATerm a_guard);
    virtual ATerm set_false(ATerm a_formula, ATerm a_guard);
};

class AM_Inner: public ATerm_Manipulator {
  protected:
    virtual ATerm set_true_auxiliary(ATerm a_formula, ATerm a_guard);
    virtual ATerm set_false_auxiliary(ATerm a_formula, ATerm a_guard);
    virtual ATerm make_if_then_else(ATerm a_expr, ATerm a_high, ATerm a_low);
  public:
    AM_Inner(Rewriter* a_rewriter, ATerm_Info* f_info);
    virtual ~AM_Inner();
    virtual ATerm make_reduced_if_then_else(ATerm a_expr, ATerm a_high, ATerm a_low);
    virtual ATerm orient(ATerm a_term);
    virtual ATerm set_true(ATerm a_formula, ATerm a_guard);
    virtual ATerm set_false(ATerm a_formula, ATerm a_guard);
};

#endif
