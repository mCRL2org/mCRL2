// Interface to classes ATerm_Info, AI_Jitty and AI_Inner
// file: info.h

#ifndef INFO_H
#define INFO_H

#include "librewrite.h"
#include "utilities.h"

class ATerm_Info {
  protected:
    Rewriter* f_rewriter;
    ATerm f_true;
    ATerm f_false;
    ATerm f_if_then_else_bool;
    ATerm f_eq;
    bool f_full;
    bool f_reverse;
    int get_guard_structure(ATerm a_guard);
    Compare_Result compare_guard_structure(ATerm a_guard1, ATerm a_guard2);
    Compare_Result compare_guard_equality(ATerm a_guard1, ATerm a_guard2);
    Compare_Result compare_term_type(ATerm a_term1, ATerm a_term2);
    Compare_Result compare_term_occurs(ATerm a_term1, ATerm a_term2);
  public:
    ATerm_Info(Rewriter* a_rewriter);
    virtual ~ATerm_Info();
    void set_reverse(bool a_reverse);
    void set_full(bool a_bool);
    Compare_Result compare_guard(ATerm a_guard1, ATerm a_guard2);
    Compare_Result compare_term(ATerm a_term1, ATerm a_term2);
    virtual bool has_type_bool(ATerm a_term) = 0;
    virtual int get_number_of_arguments(ATerm a_term) = 0;
    virtual ATerm get_argument(ATerm a_term, int a_number) = 0;
    virtual bool is_true(ATerm a_term) = 0;
    virtual bool is_false(ATerm a_term) = 0;
    virtual bool is_if_then_else_bool(ATerm a_term) = 0;
    virtual bool is_variable(ATerm a_term) = 0;
    virtual bool is_equality(ATerm a_term) = 0;
};

class AI_Jitty: public ATerm_Info {
  public:
    AI_Jitty(Rewriter* a_rewriter);
    virtual ~AI_Jitty();
    virtual bool has_type_bool(ATerm a_term);
    virtual int get_number_of_arguments(ATerm a_term);
    virtual ATerm get_argument(ATerm a_term, int a_number);
    virtual bool is_true(ATerm a_term);
    virtual bool is_false(ATerm a_term);
    virtual bool is_if_then_else_bool(ATerm a_term);
    virtual bool is_variable(ATerm a_term);
    virtual bool is_equality(ATerm a_term);
};

class AI_Inner: public ATerm_Info {
  public:
    AI_Inner(Rewriter* a_rewriter);
    virtual ~AI_Inner();
    virtual bool has_type_bool(ATerm a_term);
    virtual int get_number_of_arguments(ATerm a_term);
    virtual ATerm get_argument(ATerm a_term, int a_number);
    virtual bool is_true(ATerm a_term);
    virtual bool is_false(ATerm a_term);
    virtual bool is_if_then_else_bool(ATerm a_term);
    virtual bool is_variable(ATerm a_term);
    virtual bool is_equality(ATerm a_term);
};

#endif
