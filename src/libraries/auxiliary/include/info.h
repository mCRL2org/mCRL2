// Interface to classes ATerm_Info, AI_Jitty and AI_Inner
// file: info.h

#ifndef INFO_H
#define INFO_H

#include "librewrite.h"
#include "utilities.h"

/// \brief Base class for classes that provide information about the structure of
/// \brief data expressions in one of the internal formats of the rewriter.

class ATerm_Info {
  protected:
    /// \brief The rewriter used to translate formulas to the internal format of rewriters.
    Rewriter* f_rewriter;

    /// \brief ATermAppl representing the constant \c true.
    ATerm f_true;

    /// \brief ATermAppl representing the constant \c false.
    ATerm f_false;

    /// \brief ATermAppl representing the \c if \c then \c else function with type Bool -> Bool -> Bool -> Bool.
    ATerm f_if_then_else_bool;

    /// \brief ATermAppl representing the \c equality function.
    ATerm f_eq;

    /// \brief Flag indicating whether or not the arguments of equality functions are taken into account
    /// \brief when determining the order of expressions.
    bool f_full;

    /// \brief Flag indicating whether or not the result of the comparison between the first two arguments
    /// \brief weighs stronger than the result of the comparison between the second pair of arguments of an
    /// \brief equation, when determining the order of expressions.
    bool f_reverse;

    bool alpha1(ATerm a_term1, ATerm a_term2, int a_number);

    bool beta1(ATerm a_term1, ATerm a_term2);

    bool gamma1(ATerm a_term1, ATerm a_term2);

    bool delta1(ATerm a_term1, ATerm a_term2);

    bool majo1(ATerm a_term1, ATerm a_term2, int a_number);

    bool lex1(ATerm a_term1, ATerm a_term2, int a_number);

    /// \brief Returns an integer corresponding to the structure of the guard passed as argument \c a_guard.
    int get_guard_structure(ATerm a_guard);

    /// \brief Compares the structure of two guards.
    Compare_Result compare_guard_structure(ATerm a_guard1, ATerm a_guard2);

    /// \brief Compares two guards by their arguments.
    Compare_Result compare_guard_equality(ATerm a_guard1, ATerm a_guard2);

    /// \brief Compares terms by their type.
    Compare_Result compare_term_type(ATerm a_term1, ATerm a_term2);

    /// \brief Compares terms by checking whether one is a part of the other.
    Compare_Result compare_term_occurs(ATerm a_term1, ATerm a_term2);
  public:
    /// \brief Constructor that initializes the rewriter.
    ATerm_Info(Rewriter* a_rewriter);

    /// \brief Destructor with no particular functionality.
    virtual ~ATerm_Info();

    /// \brief Sets the flag ATerm_Info::f_reverse.
    void set_reverse(bool a_reverse);

    /// \brief Sets the flag ATerm_Info::f_full.
    void set_full(bool a_bool);

    /// \brief Compares two guards.
    Compare_Result compare_guard(ATerm a_guard1, ATerm a_guard2);

    /// \brief Compares two terms.
    Compare_Result compare_term(ATerm a_term1, ATerm a_term2);

    /// \brief Compares two terms using lpo.
    bool lpo1(ATerm a_term1, ATerm a_term2);

    /// \brief Indicates whether or not a term has type bool.
    virtual bool has_type_bool(ATerm a_term) = 0;

    /// \brief Returns the number of arguments of the main operator of a term.
    virtual int get_number_of_arguments(ATerm a_term) = 0;

    /// \brief Returns the main operator of the term \c a_term;
    virtual ATerm get_operator(ATerm a_term) = 0;

    /// \brief Returns the argument with number \c a_number of the main operator of term \c a_term.
    virtual ATerm get_argument(ATerm a_term, int a_number) = 0;

    /// \brief Indicates whether or not a term is equal to \c true.
    virtual bool is_true(ATerm a_term) = 0;

    /// \brief Indicates whether or not a term is equal to \c false.
    virtual bool is_false(ATerm a_term) = 0;

    /// \brief Indicates whether or not a term is equal to the \c if \c then \c else function
    /// \brief with type Bool -> Bool -> Bool -> Bool.
    virtual bool is_if_then_else_bool(ATerm a_term) = 0;

    /// \brief Indicates whether or not a term is a single variable.
    virtual bool is_variable(ATerm a_term) = 0;

    /// \brief Indicates whether or not a term is an equality.
    virtual bool is_equality(ATerm a_term) = 0;
};

/// \brief Class that provides information about the structure of
/// \brief data expressions in the internal format of the rewriter
/// \brief with the jitty strategy.

class AI_Jitty: public ATerm_Info {
  public:
    /// \brief Constructor that initializes all fields.
    AI_Jitty(Rewriter* a_rewriter);

    /// \brief Destructor with no particular functionality.
    virtual ~AI_Jitty();

    /// \brief Indicates whether or not a term has type bool.
    virtual bool has_type_bool(ATerm a_term);

    /// \brief Returns the number of arguments of the main operator of a term.
    virtual int get_number_of_arguments(ATerm a_term);

    /// \brief Returns the main operator of the term \c a_term;
    virtual ATerm get_operator(ATerm a_term);

    /// \brief Returns the argument with number \c a_number of the main operator of term \c a_term.
    virtual ATerm get_argument(ATerm a_term, int a_number);

    /// \brief Indicates whether or not a term is equal to \c true.
    virtual bool is_true(ATerm a_term);

    /// \brief Indicates whether or not a term is equal to \c false.
    virtual bool is_false(ATerm a_term);

    /// \brief Indicates whether or not a term is equal to the \c if \c then \c else function
    /// \brief with type Bool -> Bool -> Bool -> Bool.
    virtual bool is_if_then_else_bool(ATerm a_term);

    /// \brief Indicates whether or not a term is a single variable.
    virtual bool is_variable(ATerm a_term);

    /// \brief Indicates whether or not a term is an equality.
    virtual bool is_equality(ATerm a_term);
};

/// \brief Class that provides information about the structure of
/// \brief data expressions in the internal format of the rewriter
/// \brief with the innermost strategy.

class AI_Inner: public ATerm_Info {
  public:
    /// \brief Constructor that initializes all fields.
    AI_Inner(Rewriter* a_rewriter);

    /// \brief Destructor with no particular functionality.
    virtual ~AI_Inner();

    /// \brief Indicates whether or not a term has type bool.
    virtual bool has_type_bool(ATerm a_term);

    /// \brief Returns the number of arguments of the main operator of a term.
    virtual int get_number_of_arguments(ATerm a_term);

    /// \brief Returns the main operator of the term \c a_term;
    virtual ATerm get_operator(ATerm a_term);

    /// \brief Returns the argument with number \c a_number of the main operator of term \c a_term.
    virtual ATerm get_argument(ATerm a_term, int a_number);

    /// \brief Indicates whether or not a term is equal to \c true.
    virtual bool is_true(ATerm a_term);

    /// \brief Indicates whether or not a term is equal to \c false.
    virtual bool is_false(ATerm a_term);

    /// \brief Indicates whether or not a term is equal to the \c if \c then \c else function
    /// \brief with type Bool -> Bool -> Bool -> Bool.
    virtual bool is_if_then_else_bool(ATerm a_term);

    /// \brief Indicates whether or not a term is a single variable.
    virtual bool is_variable(ATerm a_term);

    /// \brief Indicates whether or not a term is an equality.
    virtual bool is_equality(ATerm a_term);
};

#endif
