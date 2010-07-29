// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/prover/manipulator.h
/// \brief Interface to classes ATerm_Manipulator, AM_Jitty and AM_Inner

#ifndef MANIPULATOR_H
#define MANIPULATOR_H

#include "mcrl2/data/detail/prover/info.h"

namespace mcrl2 {
  namespace data {
    namespace detail {

/// \brief Base class for classes that provide functionality to modify or create
/// \brief terms in one of the internal formats of the rewriter.

class ATerm_Manipulator {
  protected:
    /// \brief The rewriter used to translate formulas to one of the internal formats of the rewriter.
    Rewriter* f_rewriter;

    /// \brief A class that provides information on the structure of expressions in one of the
    /// \brief internal formats of the rewriter.
    ATerm_Info* f_info;

    /// \brief A table used by the method ATerm_Manipulator::set_true_auxiliary.
    /// The method ATerm_Manipulator::set_true_auxiliary stores resulting terms in this
    /// table. If a term is encountered that has already been processed, it is
    /// not processed again, but retreived from this table.
    /// The table is cleared after each run of the method.
    ATermTable f_set_true;

    /// \brief A table used by the method ATerm_Manipulator::set_false_auxiliary.
    /// The method ATerm_Manipulator::set_false_auxiliary stores resulting terms in this
    /// table. If a term is encountered that has already been processed, it is
    /// not processed again, but retreived from this table.
    /// The table is cleared after each run of the method.
    ATermTable f_set_false;

    /// \brief A table used by the method ATerm_Manipulator::orient.
    /// The method ATerm_Manipulator::orient stores resulting terms in this
    /// table. If a term is encountered that has already been processed, it is
    /// not processed again, but retreived from this table.
    ATermTable f_orient;

    /// \brief ATerm representing \c true in one of the internal formats of the rewriter.
    ATerm f_true;

    /// \brief ATerm representing \c false in one of the internal formats of the rewriter.
    ATerm f_false;

    /// \brief ATerm representing the \c if \c then \c else function in one of the internal formats of the rewriter.
    ATerm f_if_then_else;

    /// \brief Replaces all occurences of \c a_guard in \c a_formula by \c true. Additionally, if the variable
    /// \brief on the righthand side of the guard is encountered in \c a_formula, it is replaced by the variable
    /// \brief on the lefthand side.
    virtual ATerm set_true_auxiliary(ATerm a_formula, ATerm a_guard) = 0;

    /// \brief Replaces all occurences of \c a_guard in \c a_formula by \c false.
    virtual ATerm set_false_auxiliary(ATerm a_formula, ATerm a_guard) = 0;

    /// \brief Returns an expression in one of the internal formats of the rewriter.
    /// \brief The main operator of this expression is an \c if \c then \c else function. Its guard is \c a_expr,
    /// \brief the true-branch is \c a_high and the false-branch is \c a_low.
    virtual ATerm make_if_then_else(ATerm a_expr, ATerm a_high, ATerm a_low) = 0;
  public:
    /// \brief Constructor initializing the rewriter and the field \c f_info.
    ATerm_Manipulator(Rewriter* a_rewriter, ATerm_Info* a_info)
    {
      f_rewriter = a_rewriter;
      f_info = a_info;
      f_orient = ATtableCreate(2000, 50);
    }

    /// \brief Destructor with no particular functionality.
    virtual ~ATerm_Manipulator()
    {
      ATtableDestroy(f_orient);
    }

    /// \brief Returns an expression in one of the internal formats of the rewriter.
    /// \brief The main operator of this expression is an \c if \c then \c else function. Its guard is \c a_expr,
    /// \brief the true-branch is \c a_high and the false-branch is \c a_low. If \c a_high equals \c a_low, the
    /// \brief method returns \c a_high instead.
    virtual ATerm make_reduced_if_then_else(ATerm a_expr, ATerm a_high, ATerm a_low) = 0;

    /// \brief Orients the term \c a_term such that all equations of the form t1 == t2 are
    /// \brief replaced by t2 == t1 if t1 > t2.
    virtual ATerm orient(ATerm a_term) = 0;

    /// \brief Initializes the table ATerm_Manipulator::f_set_true and calls the method
    /// \brief ATerm_Manipulator::f_set_true_auxiliary.
    virtual ATerm set_true(ATerm a_formula, ATerm a_guard) = 0;

    /// \brief Initializes the table ATerm_Manipulator::f_set_false and calls the method
    /// \brief ATerm_Manipulator::f_set_false_auxiliary.
    virtual ATerm set_false(ATerm a_formula, ATerm a_guard) = 0;
};

/// \brief Class that provides functionality to modify or create
/// \brief terms in the internal format of the rewriter with the
/// \brief Jitty strategy.

class AM_Jitty: public ATerm_Manipulator {
  protected:
    /// \brief Replaces all occurences of \c a_guard in \c a_formula by \c true. Additionally, if the variable
    /// \brief on the righthand side of the guard is encountered in \c a_formula, it is replaced by the variable
    /// \brief on the lefthand side.
    virtual ATerm set_true_auxiliary(ATerm a_formula, ATerm a_guard){
      if (a_formula == f_true || a_formula == f_false) {
        return a_formula;
      }
      if (a_formula == a_guard) {
        return f_true;
      }

      bool v_is_equality;

      v_is_equality = f_info->is_equality(a_guard);
      if (v_is_equality && ATgetArgument(a_guard, 2) == a_formula) {
        return ATgetArgument(a_guard, 1);
      }
      if (f_info->is_variable(a_formula)) {
        return a_formula;
      }

      ATerm v_result;

      v_result = ATtableGet(f_set_true, a_formula);
      if (v_result) {
        return v_result;
      }

      Symbol v_symbol;
      ATerm v_function;
      int v_arity;

      v_symbol = ATgetSymbol(a_formula);
      v_function = ATgetArgument(a_formula, 0);
      v_arity = ATgetArity(v_symbol);

      ATerm* v_parts;
      int i;

      v_parts = new ATerm[v_arity + 1];
      v_parts[0] = v_function;
      for (i = 1; i < v_arity; i++) {
        v_parts[i] = set_true_auxiliary(ATgetArgument(a_formula, i), a_guard);
      }
      v_result = (ATerm) ATmakeApplArray(v_symbol, v_parts);
      ATtablePut(f_set_true, a_formula, v_result);
      delete[] v_parts;
      v_parts = 0;

      return v_result;
    }

    /// \brief Replaces all occurences of \c a_guard in \c a_formula by \c false.
    virtual ATerm set_false_auxiliary(ATerm a_formula, ATerm a_guard){
      if (a_formula == f_true || a_formula == f_false) {
        return a_formula;
      }
      if (a_formula == a_guard) {
        return f_false;
      }
      if (f_info->is_variable(a_formula)) {
        return a_formula;
      }

      ATerm v_result;

      v_result = ATtableGet(f_set_false, a_formula);
      if (v_result) {
        return v_result;
      }

      Symbol v_symbol;
      ATerm v_function;
      int v_arity;

      v_symbol = ATgetSymbol(a_formula);
      v_function = ATgetArgument(a_formula, 0);
      v_arity = ATgetArity(v_symbol);

      ATerm* v_parts;
      int i;

      v_parts = new ATerm[v_arity + 1];
      v_parts[0] = v_function;
      for (i = 1; i < v_arity; i++) {
        v_parts[i] = set_false_auxiliary(ATgetArgument(a_formula, i), a_guard);
      }
      v_result = (ATerm) ATmakeApplArray(v_symbol, v_parts);
      ATtablePut(f_set_false, a_formula, v_result);
      delete[] v_parts;
      v_parts = 0;

      return v_result;
    }

    /// \brief Returns an expression in the internal format of the rewriter with the Jitty strategy.
    /// \brief The main operator of this expression is an \c if \c then \c else function. Its guard is \c a_expr,
    /// \brief the true-branch is \c a_high and the false-branch is \c a_low.
    virtual ATerm make_if_then_else(ATerm a_expr, ATerm a_high, ATerm a_low){
      return (ATerm) ATmakeAppl4(
        ATmakeAFun("@REWR@", 4, ATfalse), f_if_then_else, a_expr, a_high, a_low);
    }

  public:
    /// \brief Constructor initializing all fields.
    AM_Jitty(Rewriter* a_rewriter, ATerm_Info* a_info)
      : ATerm_Manipulator(a_rewriter, a_info)
    {
      using namespace mcrl2::core::detail;
      f_true =  a_rewriter->toRewriteFormat(sort_bool::true_());
      f_false = a_rewriter->toRewriteFormat(sort_bool::false_());
      f_if_then_else = ATgetArgument((ATermAppl) a_rewriter->toRewriteFormat(if_(sort_bool::bool_())), 0);
    }

    /// \brief Destructor with no particular functionality.
    virtual ~AM_Jitty()
    {}

    /// \brief Returns an expression in the internal format of the rewriter with the jitty strategy.
    /// \brief The main operator of this expression is an \c if \c then \c else function. Its guard is \c a_expr,
    /// \brief the true-branch is \c a_high and the false-branch is \c a_low. If \c a_high equals \c a_low, the
    /// \brief method returns \c a_high instead.
    virtual ATerm make_reduced_if_then_else(ATerm a_expr, ATerm a_high, ATerm a_low){
      if (a_high == a_low) {
        return a_high;
      } else {
        return make_if_then_else(a_expr, a_high, a_low);
      }
    }

    /// \brief Orients the term \c a_term such that all equations of the form t1 == t2 are
    /// \brief replaced by t2 == t1 if t1 > t2.
    virtual ATerm orient(ATerm a_term){
      ATerm v_result;

      v_result = ATtableGet(f_orient, a_term);
      if (v_result) {
        return v_result;
      }

      Symbol v_symbol;
      ATerm v_function;
      int v_arity;

      v_symbol = ATgetSymbol(a_term);
      v_function = ATgetArgument(a_term, 0);
      v_arity = ATgetArity(v_symbol);

      ATerm* v_parts;
      int i;

      v_parts = new ATerm[v_arity + 1];
      v_parts[0] = v_function;
      for (i = 1; i < v_arity; i++) {
        v_parts[i] = orient(ATgetArgument(a_term, i));
      }
      v_result = (ATerm) ATmakeApplArray(v_symbol, v_parts);
      delete[] v_parts;
      v_parts = 0;

      if (f_info->is_equality(v_result)) {
        ATerm v_term1;
        ATerm v_term2;

        v_term1 = ATgetArgument(v_result, 1);
        v_term2 = ATgetArgument(v_result, 2);
        if (f_info->compare_term(v_term1, v_term2) == compare_result_bigger) {
          v_result = (ATerm) ATmakeAppl3(v_symbol, v_function, v_term2, v_term1);
        }
      }
      ATtablePut(f_orient, a_term, v_result);

      return v_result;
    }

    /// \brief Initializes the table ATerm_Manipulator::f_set_true and calls the method
    /// \brief AM_Jitty::f_set_true_auxiliary.
    virtual ATerm set_true(ATerm a_formula, ATerm a_guard){
      ATerm v_result;
      f_set_true = ATtableCreate(2047, 50);
      v_result = set_true_auxiliary(a_formula, a_guard);
      ATtableDestroy(f_set_true);
      return v_result;
    }

    /// \brief Initializes the table ATerm_Manipulator::f_set_false and calls the method
    /// \brief AM_Jitty::f_set_false_auxiliary.
    virtual ATerm set_false(ATerm a_formula, ATerm a_guard){
      ATerm v_result;
      f_set_false = ATtableCreate(2047, 50);
      v_result = set_false_auxiliary(a_formula, a_guard);
      ATtableDestroy(f_set_false);
      return v_result;
    }
};

/// \brief Class that provides functionality to modify or create
/// \brief terms in the internal format of the rewriter with the
/// \brief innermost strategy.

class AM_Inner: public ATerm_Manipulator {
  protected:
    /// \brief Replaces all occurences of \c a_guard in \c a_formula by \c true. Additionally, if the variable
    /// \brief on the righthand side of the guard is encountered in \c a_formula, it is replaced by the variable
    /// \brief on the lefthand side.
    virtual ATerm set_true_auxiliary(ATerm a_formula, ATerm a_guard){
      if (a_formula == f_true || a_formula == f_false) {
        return a_formula;
      }
      if (a_formula == a_guard) {
        return f_true;
      }

      bool v_is_equality;

      v_is_equality = f_info->is_equality(a_guard);
      if (v_is_equality && ATelementAt((ATermList) a_guard, 2) == a_formula) {
        return ATelementAt((ATermList) a_guard, 1);
      }
      if (f_info->is_variable(a_formula)) {
        return a_formula;
      }

      ATerm v_result;

      v_result = ATtableGet(f_set_true, a_formula);
      if (v_result) {
        return v_result;
      }

      if (ATgetType(a_formula) == AT_LIST) {
        ATerm v_function;
        int v_length;

        v_function = ATelementAt((ATermList) a_formula, 0);
        v_length = ATgetLength((ATermList) a_formula);

        ATerm* v_parts;
        int i;

        v_parts = new ATerm[v_length];
        v_parts[0] = v_function;
        for (i = 1; i < v_length; i++) {
          v_parts[i] = set_true_auxiliary(ATelementAt((ATermList) a_formula, i), a_guard);
        }
        v_result = (ATerm) ATmakeList0();
        for (i = v_length - 1; i >= 0; i--) {
          v_result = (ATerm) ATinsert((ATermList) v_result, v_parts[i]);
        }
        ATtablePut(f_set_true, a_formula, v_result);
        delete[] v_parts;
        v_parts = 0;
      } else {
        v_result = a_formula;
      }
      return v_result;
    }

    /// \brief Replaces all occurences of \c a_guard in \c a_formula by \c false.
    virtual ATerm set_false_auxiliary(ATerm a_formula, ATerm a_guard){
      if (a_formula == f_true || a_formula == f_false) {
        return a_formula;
      }
      if (a_formula == a_guard) {
        return f_false;
      }
      if (f_info->is_variable(a_formula)) {
        return a_formula;
      }

      ATerm v_result;

      v_result = ATtableGet(f_set_false, a_formula);
      if (v_result) {
        return v_result;
      }

      if (ATgetType(a_formula) == AT_LIST) {
        ATerm v_function;
        int v_length;

        v_function = ATelementAt((ATermList) a_formula, 0);
        v_length = ATgetLength((ATermList) a_formula);

        ATerm* v_parts;
        int i;

        v_parts = new ATerm[v_length];
        v_parts[0] = v_function;
        for (i = 1; i < v_length; i++) {
          v_parts[i] = set_false_auxiliary(ATelementAt((ATermList) a_formula, i), a_guard);
        }
        v_result = (ATerm) ATmakeList0();
        for (i = v_length - 1; i >= 0; i--) {
          v_result = (ATerm) ATinsert((ATermList) v_result, v_parts[i]);
        }
        ATtablePut(f_set_false, a_formula, v_result);
        delete[] v_parts;
        v_parts = 0;
      } else {
        v_result = a_formula;
      }
      return v_result;
    }

    /// \brief Returns an expression in the internal format of the rewriter with the innermost strategy.
    /// \brief The main operator of this expression is an \c if \c then \c else function. Its guard is \c a_expr,
    /// \brief the true-branch is \c a_high and the false-branch is \c a_low.
    virtual ATerm make_if_then_else(ATerm a_expr, ATerm a_high, ATerm a_low){
      return (ATerm) ATmakeList4(f_if_then_else, a_expr, a_high, a_low);
    }

  public:
    /// \brief Constructor initializing all fields.
    AM_Inner(Rewriter* a_rewriter, ATerm_Info* a_info)
      : ATerm_Manipulator(a_rewriter, a_info)
    {
      using namespace mcrl2::core::detail;
      f_true =  a_rewriter->toRewriteFormat(sort_bool::true_());
      f_false = a_rewriter->toRewriteFormat(sort_bool::false_());
      f_if_then_else = a_rewriter->toRewriteFormat(if_(sort_bool::bool_()));
    }

    /// \brief Destructor with no particular functionality.
    virtual ~AM_Inner()
    {}

    /// \brief Returns an expression in the internal format of the rewriter with the innermost strategy.
    /// \brief The main operator of this expression is an \c if \c then \c else function. Its guard is \c a_expr,
    /// \brief the true-branch is \c a_high and the false-branch is \c a_low. If \c a_high equals \c a_low, the
    /// \brief method returns \c a_high instead.
    virtual ATerm make_reduced_if_then_else(ATerm a_expr, ATerm a_high, ATerm a_low){
      if (a_high == a_low) {
        return a_high;
      } else {
        return make_if_then_else(a_expr, a_high, a_low);
      }
    }

    /// \brief Orients the term \c a_term such that all equations of the form t1 == t2 are
    /// \brief replaced by t2 == t1 if t1 > t2.
    virtual ATerm orient(ATerm a_term){
      ATerm v_result;

      v_result = ATtableGet(f_orient, a_term);
      if (v_result) {
        return v_result;
      }

      if (ATgetType(a_term) == AT_LIST) {
        ATerm v_function;
        int v_length;

        v_function = ATelementAt((ATermList) a_term, 0);
        v_length = ATgetLength((ATermList) a_term);

        ATerm* v_parts;
        int i;

        v_parts = new ATerm[v_length];
        v_parts[0] = v_function;
        for (i = 1; i < v_length; i++) {
          v_parts[i] = orient(ATelementAt((ATermList) a_term, i));
        }
        v_result = (ATerm) ATmakeList0();
        for (i = v_length - 1; i >= 0; i--) {
          v_result = (ATerm) ATinsert((ATermList) v_result, v_parts[i]);
        }
        delete[] v_parts;
        v_parts = 0;

        if (f_info->is_equality(v_result)) {
          ATerm v_term1;
          ATerm v_term2;

          v_term1 = ATelementAt((ATermList) v_result, 1);
          v_term2 = ATelementAt((ATermList) v_result, 2);
          if (f_info->compare_term(v_term1, v_term2) == compare_result_bigger) {
            v_result = (ATerm) ATmakeList3(v_function, v_term2, v_term1);
          }
        }
      } else {
        v_result = a_term;
      }
      ATtablePut(f_orient, a_term, v_result);

      return v_result;
    }

    /// \brief Initializes the table ATerm_Manipulator::f_set_true and calls the method
    /// \brief ATerm_Inner::f_set_true_auxiliary.
    virtual ATerm set_true(ATerm a_formula, ATerm a_guard){
      ATerm v_result;
      f_set_true = ATtableCreate(2047, 50);
      v_result = set_true_auxiliary(a_formula, a_guard);
      ATtableDestroy(f_set_true);
      return v_result;
    }

    /// \brief Initializes the table ATerm_Manipulator::f_set_false and calls the method
    /// \brief AM_Inner::f_set_false_auxiliary.
    virtual ATerm set_false(ATerm a_formula, ATerm a_guard){
      ATerm v_result;
      f_set_false = ATtableCreate(2047, 50);
      v_result = set_false_auxiliary(a_formula, a_guard);
      ATtableDestroy(f_set_false);
      return v_result;
    }
};

    }
  }
}

#endif
