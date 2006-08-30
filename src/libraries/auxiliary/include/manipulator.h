// Interface to classes ATerm_Manipulator, AM_Jitty and AM_Inner
// file: manipulator.h

#ifndef MANIPULATOR_H
#define MANIPULATOR_H

#include "librewrite.h"
#include "info.h"

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
    ATerm_Manipulator(Rewriter* a_rewriter, ATerm_Info* f_info);

    /// \brief Destructor with no particular functionality.
    virtual ~ATerm_Manipulator();

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
    virtual ATerm set_true_auxiliary(ATerm a_formula, ATerm a_guard);

    /// \brief Replaces all occurences of \c a_guard in \c a_formula by \c false.
    virtual ATerm set_false_auxiliary(ATerm a_formula, ATerm a_guard);

    /// \brief Returns an expression in the internal format of the rewriter with the Jitty strategy.
    /// \brief The main operator of this expression is an \c if \c then \c else function. Its guard is \c a_expr,
    /// \brief the true-branch is \c a_high and the false-branch is \c a_low.
    virtual ATerm make_if_then_else(ATerm a_expr, ATerm a_high, ATerm a_low);
  public:
    /// \brief Constructor initializing all fields.
    AM_Jitty(Rewriter* a_rewriter, ATerm_Info* f_info);

    /// \brief Destructor with no particular functionality.
    virtual ~AM_Jitty();

    /// \brief Returns an expression in the internal format of the rewriter with the jitty strategy.
    /// \brief The main operator of this expression is an \c if \c then \c else function. Its guard is \c a_expr,
    /// \brief the true-branch is \c a_high and the false-branch is \c a_low. If \c a_high equals \c a_low, the
    /// \brief method returns \c a_high instead.
    virtual ATerm make_reduced_if_then_else(ATerm a_expr, ATerm a_high, ATerm a_low);

    /// \brief Orients the term \c a_term such that all equations of the form t1 == t2 are
    /// \brief replaced by t2 == t1 if t1 > t2.
    virtual ATerm orient(ATerm a_term);

    /// \brief Initializes the table ATerm_Manipulator::f_set_true and calls the method
    /// \brief AM_Jitty::f_set_true_auxiliary.
    virtual ATerm set_true(ATerm a_formula, ATerm a_guard);

    /// \brief Initializes the table ATerm_Manipulator::f_set_false and calls the method
    /// \brief AM_Jitty::f_set_false_auxiliary.
    virtual ATerm set_false(ATerm a_formula, ATerm a_guard);
};

/// \brief Class that provides functionality to modify or create
/// \brief terms in the internal format of the rewriter with the
/// \brief innermost strategy.

class AM_Inner: public ATerm_Manipulator {
  protected:
    /// \brief Replaces all occurences of \c a_guard in \c a_formula by \c true. Additionally, if the variable
    /// \brief on the righthand side of the guard is encountered in \c a_formula, it is replaced by the variable
    /// \brief on the lefthand side.
    virtual ATerm set_true_auxiliary(ATerm a_formula, ATerm a_guard);

    /// \brief Replaces all occurences of \c a_guard in \c a_formula by \c false.
    virtual ATerm set_false_auxiliary(ATerm a_formula, ATerm a_guard);

    /// \brief Returns an expression in the internal format of the rewriter with the innermost strategy.
    /// \brief The main operator of this expression is an \c if \c then \c else function. Its guard is \c a_expr,
    /// \brief the true-branch is \c a_high and the false-branch is \c a_low.
    virtual ATerm make_if_then_else(ATerm a_expr, ATerm a_high, ATerm a_low);
  public:
    /// \brief Constructor initializing all fields.
    AM_Inner(Rewriter* a_rewriter, ATerm_Info* f_info);

    /// \brief Destructor with no particular functionality.
    virtual ~AM_Inner();

    /// \brief Returns an expression in the internal format of the rewriter with the innermost strategy.
    /// \brief The main operator of this expression is an \c if \c then \c else function. Its guard is \c a_expr,
    /// \brief the true-branch is \c a_high and the false-branch is \c a_low. If \c a_high equals \c a_low, the
    /// \brief method returns \c a_high instead.
    virtual ATerm make_reduced_if_then_else(ATerm a_expr, ATerm a_high, ATerm a_low);

    /// \brief Orients the term \c a_term such that all equations of the form t1 == t2 are
    /// \brief replaced by t2 == t1 if t1 > t2.
    virtual ATerm orient(ATerm a_term);

    /// \brief Initializes the table ATerm_Manipulator::f_set_true and calls the method
    /// \brief ATerm_Inner::f_set_true_auxiliary.
    virtual ATerm set_true(ATerm a_formula, ATerm a_guard);

    /// \brief Initializes the table ATerm_Manipulator::f_set_false and calls the method
    /// \brief AM_Inner::f_set_false_auxiliary.
    virtual ATerm set_false(ATerm a_formula, ATerm a_guard);
};

#endif
