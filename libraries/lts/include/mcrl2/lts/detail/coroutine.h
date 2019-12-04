// Author(s): David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands
//
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \file lts/detail/coroutine.h
///
/// \brief helper macros for coroutines
///
/// \details The following macros are intended to be used for two (stackless)
/// **coroutines** that are executed in _lockstep,_ i. e. both routines do
/// (approximately) the same amount of work until one of them terminates
/// successfully.  The two coroutines are inserted into a code block after each
/// other.  Under the hood of the macros, code is produced to switch regularly
/// between the two coroutines.
///
/// The macros are to be used within a code block (a function) as follows:
///
///     int function(...)
///     {
///         ...
///         COROUTINES_SECTION
///             variable declarations of local variables of both coroutines;
///
///             COROUTINE_LABELS(labels of both coroutines)
///
///             COROUTINE
///                 code of the first coroutine, with special macros for loops.
///             END_COROUTINE
///
///             COROUTINE
///                 code of the second coroutine, also with special macros for
///                 loops.
///             END_COROUTINE
///         END_COROUTINES_SECTION
///         ...
///         return ...;
///     }
///
/// Note that the scope of the local variables will be both coroutines; the
/// user has to ensure that one coroutine does not access variables of the
/// other coroutine.  If there are some shared parameters where access from
/// both coroutines is required, they may also be declared in the variable
/// declaration section.
///
/// The labels of the coroutines are given as a so-called boost sequence:
/// `(location1) (location2) (location3)`  etc.  These labels are _interrupt_
/// _locations,_ i. e. places where a unit of work is counted and the coroutine
/// may be interrupted (to allow the other coroutine to work).  As code without
/// loops always incurs the same time complexity, only loop statements need to
/// be counted.  There are three commands to create a loop whose iterations are
/// counted, closely corresponding to ``normal'' loops:
///
///     COROUTINE_WHILE(interrupt location, condition)  while (condition)
///     {                                               {
///         loop body;                                      loop body;
///     }                                               }
///     END_COROUTINE_WHILE;
///
///     COROUTINE_FOR(interrupt location,               for (initialisation;
///             initialisation, condition, update)          condition; update)
///     {                                               {
///         loop body;                                      loop body;
///     }                                               }
///     END_COROUTINE_FOR;
///
///     COROUTINE_DO_WHILE(interrupt locatn,condition)  do
///     {                                               {
///         loop body;                                      loop body;
///     }                                               }
///     END_COROUTINE_DO_WHILE;                         while (condition);
///
/// These macros hide some code that counts how many iterations have been
/// executed in a loop at the _end_ of each iteration;  if one coroutine has
/// done enough work, it is interrupted to let the other catch up.  It is
/// assumed that a coroutine does at most `SIZE_MAX` units of work.  Otherwise,
/// the error "corrupted coroutine state" may occur.
///
/// A coroutine may also terminate explicitly by executing the statement
/// `TERMINATE_COROUTINE_SUCCESSFULLY();`.  If it should no longer execute but
/// has not terminated successfully, it may call `ABORT_THIS_COROUTINE();`.
/// If it does no longer allow the other coroutine to run up, it may indicate
/// so by calling `ABORT_OTHER_COROUTINE();`.
///
/// ``Approximately the same amount of work'' is defined as:  As long as both
/// coroutines run, the difference between the number of steps taken by the two
/// (i. e. the balance of work) always is between -1 and +1.
///
/// \author David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands

#ifndef _COROUTINE_H
#define _COROUTINE_H

#include <cstdlib>       // for std::size_t
#include <cassert>
#include <boost/preprocessor.hpp>





/* ************************************************************************* */
/*                                                                           */
/*                             I N T E R N A L S                             */
/*                                                                           */
/* ************************************************************************* */





/// _coroutine_ENUMDEF is an internal macro to define an appropriate
/// enumeration type for interrupt locations in a coroutine.
#define _coroutine_ENUMDEF(lblseq)                                            \
                    BOOST_PP_SEQ_FOR_EACH(_coroutine_ENUMDEF_1, , lblseq)
#define _coroutine_ENUMDEF_1(r,data,label)                                    \
                    BOOST_PP_CAT(BOOST_PP_CAT(_coroutine_, label), _enum),





/* ************************************************************************* */
/*                                                                           */
/*                    E X T E R N A L   I N T E R F A C E                    */
/*                                                                           */
/* ************************************************************************* */





/// \brief begin a section with two coroutines
/// \details The macro indicates that in the following, a number of coroutines
/// will be defined, which should run in lockstep.
///
/// A coroutine section is closed with `END_COROUTINES_SECTION`.
#define COROUTINES_SECTION                                                    \
        do {{ if (1) {{{ do {

/// \brief Declare the interrupt locations for the coroutines
/// \details The call to COROUTINE_LABELS is needed to declare the interrupt
/// locations used in a coroutine.  For every loop, there is exactly one
/// interrupt location, which will also be given when the loop is defined in
/// the coroutine.
///
/// If additional labels are given to COROUTINE_LABELS, the compiler may
/// complain that a `switch() {}` statement with an `enum` type does not
/// mention all possible values.  The user of the macro should then delete the
/// unused locations from the parameter list of the macro.
///
/// Immediately after `COROUTINE_LABELS()`, the user should place the first
/// coroutine, indicated by `COROUTINE`.
/// \param locations   locations where the coroutine can be interrupted, as a
///                    boost sequence: `(location1) (location2) (location3)`
///                    etc.
#define COROUTINE_LABELS(locations)                                           \
            enum class _coroutine_labels {                                    \
                _coroutine_1_BEGIN_enum_,                                     \
                _coroutine_ENUMDEF(locations)                                 \
                ONLY_IF_DEBUG( _coroutine_ILLEGAL_enum_, )                    \
                _coroutine_2_BEGIN_enum_                                      \
            } _coroutine_location[2] =                                        \
                              { _coroutine_labels::_coroutine_2_BEGIN_enum_,  \
                                _coroutine_labels::_coroutine_1_BEGIN_enum_ };\
            std::size_t _coroutine_allowance = 1;                             \
    _coroutine_label_switch:                                                  \
            _coroutine_labels _coroutine_temp_location=_coroutine_location[1];\
            _coroutine_location[1] = _coroutine_location[0];                  \
            ONLY_IF_DEBUG( _coroutine_location[0] =                           \
                                _coroutine_labels::_coroutine_ILLEGAL_enum_; )\
            switch (_coroutine_temp_location)                                 \
            {                                                                 \
        case _coroutine_labels::_coroutine_1_BEGIN_enum_:                     \
                do { do {                                                     \
                    switch (_coroutine_labels::_coroutine_2_BEGIN_enum_) {
                        //< to neutralize the case label of the first coroutine


/// \brief Define the code for a coroutine
/// \details Between COROUTINE and END_COROUTINE, the code for the coroutine
/// is placed.  It is required to have exactly two coroutines.
///
/// A coroutine is closed with `END_COROUTINE`.
///
/// (If this is the first coroutine, the case label is ignored because an
/// additional switch statement is included in COROUTINE_LABELS.)
#define COROUTINE                                                             \
        case _coroutine_labels::_coroutine_2_BEGIN_enum_: ;                   \
                }} while (0); } while (0);                                    \
                do { do {{                                                    \
                    {{ if (1) {


/// \brief Ends the definition of code for a coroutine
#define END_COROUTINE                                                         \
                        TERMINATE_COROUTINE_SUCCESSFULLY();                   \
                    } else assert(0 && "mismatched END_COROUTINE"); }}        \
                }} while (0); } while (0);                                    \
                do { do {{


/// \brief Close a section containing coroutines
#define END_COROUTINES_SECTION                                                \
        ONLY_IF_DEBUG( default: ; )                                           \
                }} while (0); } while (0);                                    \
            }                                                                 \
            assert(0 && "corrupted coroutine state");                         \
    _coroutine_label_terminate: ;                                             \
        } while(0); }}} else assert(0 && "mismatched END_COROUTINES_SECTION");\
        }} while (0);


/// \brief a `while` loop where every iteration incurs one unit of work
/// \details A `COROUTINE_WHILE` may be interrupted at the end of an iteration
/// to allow the other coroutine to catch up.
/// \param location  a unique interrupt location (one of the locations given as
///                  parameter to `DEFINE_COROUTINE`)
/// \param condition the while condition
/* The pattern of nested do { } while loops and if ... else statements used in
the macro implementation should generate error messages if there is an
unmatched COROUTINE_WHILE or END_COROUTINE_WHILE. */
#define COROUTINE_WHILE(location, condition)                                  \
    do                                                                        \
    {                                                                         \
        if (1)                                                                \
        {                                                                     \
            if (1)                                                            \
            {                                                                 \
                goto _coroutine_ ## location ## _label;                       \
                for ( ;; )                                                    \
                {                                                             \
                    assert(_coroutine_labels::_coroutine_ILLEGAL_enum_ ==     \
                                                      _coroutine_location[0]);\
                    if (0 == --_coroutine_allowance)                          \
                    {                                                         \
                        _coroutine_location[0] = _coroutine_labels::          \
                                             _coroutine_ ## location ## _enum;\
                        _coroutine_allowance = 2;                             \
                        goto _coroutine_label_switch;                         \
        case _coroutine_labels::_coroutine_ ## location ## _enum: ;           \
                    }                                                         \
                    _coroutine_ ## location ## _label:                        \
                    if (!(condition))  break;


/// \brief ends a loop started with `COROUTINE_WHILE`
#define END_COROUTINE_WHILE                                                   \
                }                                                             \
            }                                                                 \
            else  assert(0 && "mismatched END_COROUTINE_WHILE");              \
        }                                                                     \
        else  assert(0 && "mismatched END_COROUTINE_WHILE");                  \
    }                                                                         \
    while (0)


/// \brief a `for` loop where every iteration incurs one unit of work
/// \details A `COROUTINE_FOR` may be interrupted at the end of an iteration
/// to allow the other coroutine to catch up.
/// \param location  a unique interrupt location (one of the locations given as
///                  parameter to `DEFINE_COROUTINE`)
/// \param init      the for initialiser expression
/// \param condition the for condition
/// \param update    the for update expression (executed near the end of each
///                  iteration)
#define COROUTINE_FOR(location, init, condition, update)                      \
    do                                                                        \
    {                                                                         \
        (init);                                                               \
        if (1)                                                                \
            do                                                                \
            {{                                                                \
                goto _coroutine_ ## location ## _label;                       \
                for( ;; )                                                     \
                {                                                             \
                    (update);                                                 \
                    assert(_coroutine_labels::_coroutine_ILLEGAL_enum_ ==     \
                                                      _coroutine_location[0]);\
                    if (0 == --_coroutine_allowance)                          \
                    {                                                         \
                        _coroutine_location[0] = _coroutine_labels::          \
                                             _coroutine_ ## location ## _enum;\
                        _coroutine_allowance = 2;                             \
                        goto _coroutine_label_switch;                         \
        case _coroutine_labels::_coroutine_ ## location ## _enum: ;           \
                    }                                                         \
                    _coroutine_ ## location ## _label:                        \
                    if (!(condition))  break;


/// \brief ends a loop started with `COROUTINE_FOR`
#define END_COROUTINE_FOR                                                     \
                }                                                             \
            }}                                                                \
            while (0);                                                        \
        else  assert(0 && "mismatched END_COROUTINE_FOR");                    \
    }                                                                         \
    while (0)


/// \brief a `do { } while` loop where every iteration incurs one unit of work
/// \details A `COROUTINE_DO_WHILE` may be interrupted at the end of an
/// iteration to allow the other coroutine to catch up.
/// Note that one has to specify the condition at the beginnin of the loop,
/// even though it will not be tested before the first iteration.
/// \param location  a unique interrupt location (one of the locations given as
///                  parameter to `DEFINE_COROUTINE`)
/// \param condition the while condition
#define COROUTINE_DO_WHILE(location,condition)                                \
    do                                                                        \
        if (1)                                                                \
        {{                                                                    \
            goto _coroutine_ ## location ## _do_while_begin;                  \
            do                                                                \
            {                                                                 \
                assert(_coroutine_labels::_coroutine_ILLEGAL_enum_ ==         \
                                                      _coroutine_location[0]);\
                if (0 == --_coroutine_allowance)                              \
                {                                                             \
                    _coroutine_location[0] = _coroutine_labels::              \
                                             _coroutine_ ## location ## _enum;\
                    _coroutine_allowance = 2;                                 \
                    goto _coroutine_label_switch;                             \
        case _coroutine_labels::_coroutine_ ## location ## _enum: ;           \
                }                                                             \
                if (!(condition))  break;                                     \
                _coroutine_ ## location ## _do_while_begin:


/// \brief ends a loop started with `COROUTINE_DO_WHILE`
#define END_COROUTINE_DO_WHILE                                                \
            }                                                                 \
            while (1);                                                        \
        }}                                                                    \
        else  assert(0 && "mismatched END_COROUTINE_DO_WHILE");               \
    while (0)


/// \brief terminate the pair of coroutines successfully
/// \details If called in a coroutine, both coroutines will immediately
/// terminate.  The final statement (in `RUN_COROUTINES`) of the one that
/// called `TERMINATE_COROUTINE_SUCCESSFULLY` will be executed.
#define TERMINATE_COROUTINE_SUCCESSFULLY()                                    \
                        do                                                    \
                        {                                                     \
                            assert(_coroutine_labels::_coroutine_ILLEGAL_enum_\
                                                   == _coroutine_location[0]);\
                            goto _coroutine_label_terminate;                  \
                            (void) _coroutine_allowance;                      \
                            (void) _coroutine_location[1];                    \
                        }                                                     \
                        while (0)


/// \brief indicates that this coroutine gives up control to the other one
/// \details In a situation where one coroutine finds that the other should run
/// to completion, it calls `ABORT_THIS_COROUTINE`.
#define ABORT_THIS_COROUTINE()                                                \
                        do                                                    \
                        {                                                     \
                            assert(_coroutine_labels::_coroutine_ILLEGAL_enum_\
                                                   == _coroutine_location[0]);\
                            _coroutine_allowance = 0;                         \
                            goto _coroutine_label_switch;                     \
                            (void) _coroutine_location[1];                    \
                        }                                                     \
                        while (0)


/// \brief indicates that the other coroutine should give up control
/// \details In a situation where one coroutine finds that the other should
/// **no longer** run, it calls `ABORT_OTHER_COROUTINE`.
#define ABORT_OTHER_COROUTINE()                                               \
                        do                                                    \
                        {                                                     \
                            assert(_coroutine_labels::_coroutine_ILLEGAL_enum_\
                                                   == _coroutine_location[0]);\
                            ONLY_IF_DEBUG( _coroutine_location[1] =           \
                                _coroutine_labels::_coroutine_ILLEGAL_enum_; )\
                            _coroutine_allowance = 0;                         \
                        }                                                     \
                        while (0)

#endif // ifndef _COROUTINE_H
