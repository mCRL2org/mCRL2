// Author(s): David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands
//
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \file lts/detail/coroutine.h
///
/// \brief helper macros for coroutines
///
/// \details The following macros are intended to be used for **coroutines**
/// that are executed in _lockstep,_ i. e. both routines do (approximately) the
/// same amount of work until one of them terminates successfully.  The macros
/// are suitable for coroutines that are member methods of a class.
///
/// First, the coroutine has to be _declared_ (if it is a class method, the
/// declaration is in the body of the class).  One writes:
///
///     DECLARE_COROUTINE(function name, (formal parameters),
///                 (local variables), shared data type, shared variable name,
///                 (interrupt locations));
///
/// Later (outside the class body), one can _define_ the coroutine, i. e. give
/// its function body.  This is written as:
///
///     DEFINE_COROUTINE(namespace, function name, (formal parameters),
///                 (local variables), shared data type, shared variable name,
///                 (interrupt locations))
///     {
///         function body;
///     } END_COROUTINE
///
/// The declaration and definition must have the same parameter list (except
/// for the namespace).  The formal parameters, the local variables and the
/// interrupt locations must be enclosed in parentheses.  The variables are
/// given as (type, name, type, name, ...).  In the coroutine, parameters and
/// variables can be accessed normally.  At least one parameter is required.
///
/// Local variables also must be declared/defined using the macro because if
/// one coroutine is interrupted, its local variables have to be stored
/// temporarily until it resumes.
///
/// There is one shared parameter for communication between the coroutines.  If
/// one wants more than one shared parameter, a class or struct holding all
/// shared parameters has to be defined.  Both coroutines will access the same
/// shared variable.
///
/// Use the `namespace` parameter in the coroutine definition to indicate the
/// name of the class of which it is part. Include a final `::`.
///
/// Interrupt locations are places where a unit of work is counted and the
/// coroutine may be interrupted (to allow the other coroutine to work).  As
/// code without loops always incurs the same time complexity, only loop
/// statements need to be counted.  There are two commands to create a loop
/// whose iterations are counted, closely corresponding to ``normal'' loops:
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
///     COROUTINE_DO_WHILE(interrupt location)          do
///     {                                               {
///         loop body;                                      loop body;
///     }                                               }
///     END_COROUTINE_DO_WHILE(condition);              while (condition);
///
/// These macros hide some code that counts how many iterations have been
/// executed in a loop at the end of each iteration;  if one coroutine has done
/// enough work, it is interrupted to let the other catch up (except at the end
/// of the last iteration).  That means that no work is counted if the loop
/// body is executed at most once;  so if there are multiple nested loops it is
/// not enough to let only the innermost loop count work.  It is assumed that a
/// coroutine does at most `SIZE_MAX + 1` units of work.
///
/// A coroutine may also terminate explicitly by executing the statement
/// `TERMINATE_COROUTINE_SUCCESSFULLY();`.  If it should no longer execute but
/// has not terminated successfully, it may call `ABORT_THIS_COROUTINE();`.
/// If it does no longer allow the other coroutine to run up, it may indicate
/// so by calling `ABORT_OTHER_COROUTINE();`.
///
/// A pair of coroutines is called using the macro `RUN_COROUTINES`.  This
/// macro takes two coroutine names, two actual parameter lists, and two
/// ``final'' statements:
///
///     RUN_COROUTINES(function name 1, (actual parameters), final statement 1,
///                    function name 2, (actual parameters), final statement 2,
///                    shared data type, (shared data initialiser));
///
/// It initialises the shared data with the indicated initialiser (in
/// parentheses); then, it starts the two coroutines and makes sure that they
/// do (approximately) the same amount of work.  As soon as the first one
/// terminates, its associated final statement is executed and `RUN_COROUTINES`
/// terminates.
///
/// ``Approximately the same amount of work'' is defined as:  The second
/// coroutine will not do no more than 10/7 = ~1.4286 times the amount of work
/// of the first, and the first coroutine will not do more than 16 steps in
/// addition to 16/11 = ~1.4545 times the amount of work of the second.  (These
/// numbers are approximations of sqrt(2) = ~1.4142.)
///
/// \author David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands

#ifndef _COROUTINE_H
#define _COROUTINE_H

#ifndef NDEBUG
    #include <iostream>  // for cerr
    #include <limits>    // used in ilog2() for Clang and g++
#endif
#include <cstdlib>       // for EXIT_FAILURE and size_t





/*****************************************************************************/
/*                                                                           */
/*                             I N T E R N A L S                             */
/*                                                                           */
/*****************************************************************************/





namespace mcrl2
{
namespace lts
{
namespace detail
{
namespace coroutine
{

/* type to indicate what to do with the coroutine */
typedef enum { _coroutine_CONTINUE, _coroutine_TERMINATE, _coroutine_ABORT }
                                                        _coroutine_result_t;

} // end namespace bisim_gjkw
} // end namespace detail
} // end namespace lts
} // end namespace mcrl2

/* _coroutine_N_ARGS(...) counts the number of arguments it receives */
#define _coroutine_N_ARGS(...) _coroutine_N_ARGS_helper(__VA_ARGS__,          \
                             20, 19, 18, 17, 16, 15, 14, 13, 12, 11,          \
                             10,  9,  8,  7,  6,  5,  4,  3,  2,  1, )
#define _coroutine_N_ARGS_helper(                                             \
                             _1, _2, _3, _4, _5, _6, _7, _8, _9,_10,          \
                            _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, N, ...) N

/* _coroutine_CAT(a, b) expands and then concatenates its arguments */
#define _coroutine_CAT(a,b)   _coroutine_CAT_1(a,b)
#define _coroutine_CAT_1(a,b) _coroutine_CAT_2(a,b)
#define _coroutine_CAT_2(a,b) a ## b

/* _coroutine_SELECT is an internal macro that chooses the correct helper
macro, depending on the number of variadic arguments. */
#define _coroutine_SELECT(macro, ...)                                         \
        _coroutine_CAT(macro, _coroutine_N_ARGS(__VA_ARGS__))(__VA_ARGS__)

/* _coroutine_NO_PARENS is an internal macro that helps to remove parentheses
from its argument. */
#define _coroutine_NO_PARENS(...) __VA_ARGS__

/* _coroutine_VARDEF is a helper macro to define structure members for
variables (parameters or locals) in the reentrant version. */
#define _coroutine_VARDEF(...)                                                \
                    _coroutine_SELECT(_coroutine_VARDEF_, __VA_ARGS__)
#define _coroutine_VARDEF_1(_)
#define _coroutine_VARDEF_2(t1,v1) t1 _coroutine_ ## v1 ## _var;
#define _coroutine_VARDEF_4(t1,v1,t2,v2)                                      \
                                _coroutine_VARDEF_2(t1,v1)                    \
                                _coroutine_VARDEF_2(t2,v2)
#define _coroutine_VARDEF_6(t1,v1,t2,v2,t3,v3)                                \
                                _coroutine_VARDEF_4(t1,v1,t2,v2)              \
                                _coroutine_VARDEF_2(t3,v3)
#define _coroutine_VARDEF_8(t1,v1,t2,v2,t3,v3,t4,v4)                          \
                                _coroutine_VARDEF_4(t1,v1,t2,v2)              \
                                _coroutine_VARDEF_4(t3,v3,t4,v4)
#define _coroutine_VARDEF_10(t1,v1,t2,v2,t3,v3,t4,v4,t5,v5)                   \
                                _coroutine_VARDEF_8(t1,v1,t2,v2,t3,v3,t4,v4)  \
                                _coroutine_VARDEF_2(t5,v5)
#define _coroutine_VARDEF_12(t1,v1,t2,v2,t3,v3,t4,v4,t5,v5,t6,v6)             \
                                _coroutine_VARDEF_8(t1,v1,t2,v2,t3,v3,t4,v4)  \
                                _coroutine_VARDEF_4(t5,v5,t6,v6)
#define _coroutine_VARDEF_14(t1,v1,t2,v2,t3,v3,t4,v4,t5,v5,t6,v6,t7,v7)       \
                                _coroutine_VARDEF_8(t1,v1,t2,v2,t3,v3,t4,v4)  \
                                _coroutine_VARDEF_6(t5,v5,t6,v6,t7,v7)
#define _coroutine_VARDEF_16(t1,v1,t2,v2,t3,v3,t4,v4,t5,v5,t6,v6,t7,v7,t8,v8) \
                                _coroutine_VARDEF_8(t1,v1,t2,v2,t3,v3,t4,v4)  \
                                _coroutine_VARDEF_8(t5,v5,t6,v6,t7,v7,t8,v8)

/* _coroutine_PARLST is a helper macro to define a parameter list for the
constructor. */
#define _coroutine_PARLST(...)                                                \
                    _coroutine_SELECT(_coroutine_PARLST_, __VA_ARGS__)
#define _coroutine_PARLST_1(_)
#define _coroutine_PARLST_2(t1,v1) t1 _coroutine_ ## v1 ## _new
#define _coroutine_PARLST_4(t1,v1,t2,v2)                                      \
                                _coroutine_PARLST_2(t1,v1),                   \
                                _coroutine_PARLST_2(t2,v2)
#define _coroutine_PARLST_6(t1,v1,t2,v2,t3,v3)                                \
                                _coroutine_PARLST_4(t1,v1,t2,v2),             \
                                _coroutine_PARLST_2(t3,v3)
#define _coroutine_PARLST_8(t1,v1,t2,v2,t3,v3,t4,v4)                          \
                                _coroutine_PARLST_4(t1,v1,t2,v2),             \
                                _coroutine_PARLST_4(t3,v3,t4,v4)
#define _coroutine_PARLST_10(t1,v1,t2,v2,t3,v3,t4,v4,t5,v5)                   \
                                _coroutine_PARLST_8(t1,v1,t2,v2,t3,v3,t4,v4), \
                                _coroutine_PARLST_2(t5,v5)
#define _coroutine_PARLST_12(t1,v1,t2,v2,t3,v3,t4,v4,t5,v5,t6,v6)             \
                                _coroutine_PARLST_8(t1,v1,t2,v2,t3,v3,t4,v4), \
                                _coroutine_PARLST_4(t5,v5,t6,v6)
#define _coroutine_PARLST_14(t1,v1,t2,v2,t3,v3,t4,v4,t5,v5,t6,v6,t7,v7)       \
                                _coroutine_PARLST_8(t1,v1,t2,v2,t3,v3,t4,v4), \
                                _coroutine_PARLST_6(t5,v5,t6,v6,t7,v7)
#define _coroutine_PARLST_16(t1,v1,t2,v2,t3,v3,t4,v4,t5,v5,t6,v6,t7,v7,t8,v8) \
                                _coroutine_PARLST_8(t1,v1,t2,v2,t3,v3,t4,v4), \
                                _coroutine_PARLST_8(t5,v5,t6,v6,t7,v7,t8,v8)

/* _coroutine_INITLST is a helper macro to define a member initializer list for
the constructor. */
#define _coroutine_INITLST(...)                                               \
                    _coroutine_SELECT(_coroutine_INITLST_, __VA_ARGS__)
#define _coroutine_INITLST_1(_)
#define _coroutine_INITLST_2(t1,v1)                                           \
                    _coroutine_ ## v1 ## _var(_coroutine_ ## v1 ## _new),
#define _coroutine_INITLST_4(t1,v1,t2,v2)                                     \
                                _coroutine_INITLST_2(t1,v1)                   \
                                _coroutine_INITLST_2(t2,v2)
#define _coroutine_INITLST_6(t1,v1,t2,v2,t3,v3)                               \
                                _coroutine_INITLST_4(t1,v1,t2,v2)             \
                                _coroutine_INITLST_2(t3,v3)
#define _coroutine_INITLST_8(t1,v1,t2,v2,t3,v3,t4,v4)                         \
                                _coroutine_INITLST_4(t1,v1,t2,v2)             \
                                _coroutine_INITLST_4(t3,v3,t4,v4)
#define _coroutine_INITLST_10(t1,v1,t2,v2,t3,v3,t4,v4,t5,v5)                  \
                                _coroutine_INITLST_8(t1,v1,t2,v2,t3,v3,t4,v4) \
                                _coroutine_INITLST_2(t5,v5)
#define _coroutine_INITLST_12(t1,v1,t2,v2,t3,v3,t4,v4,t5,v5,t6,v6)            \
                                _coroutine_INITLST_8(t1,v1,t2,v2,t3,v3,t4,v4) \
                                _coroutine_INITLST_4(t5,v5,t6,v6)
#define _coroutine_INITLST_14(t1,v1,t2,v2,t3,v3,t4,v4,t5,v5,t6,v6,t7,v7)      \
                                _coroutine_INITLST_8(t1,v1,t2,v2,t3,v3,t4,v4) \
                                _coroutine_INITLST_6(t5,v5,t6,v6,t7,v7)
#define _coroutine_INITLST_16(t1,v1,t2,v2,t3,v3,t4,v4,t5,v5,t6,v6,t7,v7,t8,v8)\
                                _coroutine_INITLST_8(t1,v1,t2,v2,t3,v3,t4,v4) \
                                _coroutine_INITLST_8(t5,v5,t6,v6,t7,v7,t8,v8)

/* _coroutine_ALIAS is a helper macro to define aliases for parameters and
local variables, so that the programmer can access them easily. */
#define _coroutine_ALIAS(...) _coroutine_SELECT(_coroutine_ALIAS_, __VA_ARGS__)
#define _coroutine_ALIAS_1(_)
#define _coroutine_ALIAS_2(t1,v1)                                             \
                        t1& v1 = _coroutine_param._coroutine_ ## v1 ## _var;
#define _coroutine_ALIAS_4(t1,v1,t2,v2)                                       \
                                _coroutine_ALIAS_2(t1,v1)                     \
                                _coroutine_ALIAS_2(t2,v2)
#define _coroutine_ALIAS_6(t1,v1,t2,v2,t3,v3)                                 \
                                _coroutine_ALIAS_4(t1,v1,t2,v2)               \
                                _coroutine_ALIAS_2(t3,v3)
#define _coroutine_ALIAS_8(t1,v1,t2,v2,t3,v3,t4,v4)                           \
                                _coroutine_ALIAS_4(t1,v1,t2,v2)               \
                                _coroutine_ALIAS_4(t3,v3,t4,v4)
#define _coroutine_ALIAS_10(t1,v1,t2,v2,t3,v3,t4,v4,t5,v5)                    \
                                _coroutine_ALIAS_8(t1,v1,t2,v2,t3,v3,t4,v4)   \
                                _coroutine_ALIAS_2(t5,v5)
#define _coroutine_ALIAS_12(t1,v1,t2,v2,t3,v3,t4,v4,t5,v5,t6,v6)              \
                                _coroutine_ALIAS_8(t1,v1,t2,v2,t3,v3,t4,v4)   \
                                _coroutine_ALIAS_4(t5,v5,t6,v6)
#define _coroutine_ALIAS_14(t1,v1,t2,v2,t3,v3,t4,v4,t5,v5,t6,v6,t7,v7)        \
                                _coroutine_ALIAS_8(t1,v1,t2,v2,t3,v3,t4,v4)   \
                                _coroutine_ALIAS_6(t5,v5,t6,v6,t7,v7)
#define _coroutine_ALIAS_16(t1,v1,t2,v2,t3,v3,t4,v4,t5,v5,t6,v6,t7,v7,t8,v8)  \
                                _coroutine_ALIAS_8(t1,v1,t2,v2,t3,v3,t4,v4)   \
                                _coroutine_ALIAS_8(t5,v5,t6,v6,t7,v7,t8,v8)

/* _coroutine_ENUMDEF is a helper macro to define an appropriate enumeration
type for interrupt locations in a coroutine. */
#define _coroutine_ENUMDEF(...)                                               \
                        _coroutine_SELECT(_coroutine_ENUMDEF_, __VA_ARGS__)
#define _coroutine_ENUMDEF_1(l1) , _coroutine_ ## l1 ## _enum
#define _coroutine_ENUMDEF_2(l1,l2) _coroutine_ENUMDEF_1(l1)                  \
                                    _coroutine_ENUMDEF_1(l2)
#define _coroutine_ENUMDEF_3(l1,l2,l3)                                        \
                                    _coroutine_ENUMDEF_2(l1,l2)               \
                                    _coroutine_ENUMDEF_1(l3)
#define _coroutine_ENUMDEF_4(l1,l2,l3,l4)                                     \
                                    _coroutine_ENUMDEF_2(l1,l2)               \
                                    _coroutine_ENUMDEF_2(l3,l4)
#define _coroutine_ENUMDEF_5(l1,l2,l3,l4,l5)                                  \
                                    _coroutine_ENUMDEF_4(l1,l2,l3,l4)         \
                                    _coroutine_ENUMDEF_1(l5)
#define _coroutine_ENUMDEF_6(l1,l2,l3,l4,l5,l6)                               \
                                    _coroutine_ENUMDEF_4(l1,l2,l3,l4)         \
                                    _coroutine_ENUMDEF_2(l5,l6)
#define _coroutine_ENUMDEF_7(l1,l2,l3,l4,l5,l6,l7)                            \
                                    _coroutine_ENUMDEF_4(l1,l2,l3,l4)         \
                                    _coroutine_ENUMDEF_3(l5,l6,l7)
#define _coroutine_ENUMDEF_8(l1,l2,l3,l4,l5,l6,l7,l8)                         \
                                    _coroutine_ENUMDEF_4(l1,l2,l3,l4)         \
                                    _coroutine_ENUMDEF_4(l5,l6,l7,l8)

/* _coroutine_SWITCHCASE is a helper macro to jump to the interrupt location
where the coroutine was interrupted. */
#define _coroutine_SWITCHCASE(...)                                            \
                        _coroutine_SELECT(_coroutine_SWITCHCASE_, __VA_ARGS__)
#define _coroutine_SWITCHCASE_1(l1) case _coroutine_ ## l1 ## _enum:          \
                                        goto _coroutine_ ## l1 ## _label;
#define _coroutine_SWITCHCASE_2(l1,l2)                                        \
                                    _coroutine_SWITCHCASE_1(l1)               \
                                    _coroutine_SWITCHCASE_1(l2)
#define _coroutine_SWITCHCASE_3(l1,l2,l3)                                     \
                                    _coroutine_SWITCHCASE_2(l1,l2)            \
                                    _coroutine_SWITCHCASE_1(l3)
#define _coroutine_SWITCHCASE_4(l1,l2,l3,l4)                                  \
                                    _coroutine_SWITCHCASE_2(l1,l2)            \
                                    _coroutine_SWITCHCASE_2(l3,l4)
#define _coroutine_SWITCHCASE_5(l1,l2,l3,l4,l5)                               \
                                    _coroutine_SWITCHCASE_4(l1,l2,l3,l4)      \
                                    _coroutine_SWITCHCASE_1(l5)
#define _coroutine_SWITCHCASE_6(l1,l2,l3,l4,l5,l6)                            \
                                    _coroutine_SWITCHCASE_4(l1,l2,l3,l4)      \
                                    _coroutine_SWITCHCASE_2(l5,l6)
#define _coroutine_SWITCHCASE_7(l1,l2,l3,l4,l5,l6,l7)                         \
                                    _coroutine_SWITCHCASE_4(l1,l2,l3,l4)      \
                                    _coroutine_SWITCHCASE_3(l5,l6,l7)
#define _coroutine_SWITCHCASE_8(l1,l2,l3,l4,l5,l6,l7,l8)                      \
                                    _coroutine_SWITCHCASE_4(l1,l2,l3,l4)      \
                                    _coroutine_SWITCHCASE_4(l5,l6,l7,l8)





/*****************************************************************************/
/*                                                                           */
/*                    E X T E R N A L   I N T E R F A C E                    */
/*                                                                           */
/*****************************************************************************/





/// \def DECLARE_COROUTINE
/// \brief declare a member method or a function as a coroutine
/// \param routine     name of the coroutine
/// \param param       formal parameter list of the coroutine, in parentheses
/// \param local       local variables used by the coroutine, in parentheses
/// \param shared_type type of data shared between the two coroutines
/// \param shared_var  name of the formal parameter that contains the shared
///                    data
/// \param locations   locations where the coroutine can be interrupted, in
///                    parentheses
#define DECLARE_COROUTINE(routine, param, local, shared_type, shared_var,     \
                                                                    locations)\
    enum _coroutine_ ## routine ## _location                                  \
    {                                                                         \
         _coroutine_BEGIN_ ## routine ## _enum = __LINE__ /* no comma here */ \
         _coroutine_ENUMDEF locations                                         \
    };                                                                        \
                                                                              \
    class _coroutine_ ## routine ## _struct                                   \
    {                                                                         \
      public:                                                                 \
         _coroutine_VARDEF param                                              \
         enum _coroutine_ ## routine ## _location _coroutine_location;        \
         _coroutine_VARDEF local                                              \
         /* constructor: */                                                   \
         _coroutine_ ## routine ## _struct(_coroutine_PARLST param)           \
           : _coroutine_INITLST param /* no comma here */                     \
             _coroutine_location(_coroutine_BEGIN_ ## routine ## _enum)       \
        {  }                                                                  \
    };                                                                        \
                                                                              \
    inline coroutine::_coroutine_result_t _coroutine_ ## routine ## _func(    \
                          size_t _coroutine_allowance,                        \
                          _coroutine_ ## routine ## _struct& _coroutine_param,\
                          shared_type& shared_var);

/// \def DEFINE_COROUTINE
/// \brief define a member method or a function as a coroutine
/// \param namespace   namespace of the coroutine.  If it is a member method,
///                    use `class name::`.
/// \param routine     name of the coroutine
/// \param param       formal parameter list of the coroutine, in parentheses
/// \param local       local variables used by the coroutine, in parentheses
/// \param shared_type type of data shared between the two coroutines
/// \param shared_var  name of the formal parameter that contains the shared
///                    data
/// \param locations   locations where the coroutine can be interrupted, in
///                    parentheses
#ifndef NDEBUG

#define DEFINE_COROUTINE(namespace, routine, param, local, shared_type,       \
                                                        shared_var, locations)\
coroutine::_coroutine_result_t namespace _coroutine_ ## routine ## _func(     \
                   size_t _coroutine_allowance, namespace                     \
                          _coroutine_ ## routine ## _struct& _coroutine_param,\
                   shared_type& shared_var)                                   \
{                                                                             \
    _coroutine_ALIAS param                                                    \
    _coroutine_ALIAS local                                                    \
    switch (_coroutine_param._coroutine_location)                             \
    {                                                                         \
        _coroutine_SWITCHCASE locations                                       \
        case _coroutine_BEGIN_ ## routine ## _enum:                           \
            break;                                                            \
        default:                                                              \
            std::cerr << __FILE__ << "(" << __LINE__ << "): Corrupted "       \
                        "internal state in coroutine " << __func__ << "\n";   \
            exit(EXIT_FAILURE);                                               \
    }

#else // #ifndef NDEBUG

#define DEFINE_COROUTINE(namespace, routine, param, local, shared_type,       \
                                                        shared_var, locations)\
coroutine::_coroutine_result_t namespace _coroutine_ ## routine ## _func(     \
                size_t _coroutine_allowance, namespace                        \
                          _coroutine_ ## routine ## _struct& _coroutine_param,\
                shared_type& shared_var)                                      \
{                                                                             \
    _coroutine_ALIAS param                                                    \
    _coroutine_ALIAS local                                                    \
    switch (_coroutine_param._coroutine_location)                             \
    {                                                                         \
        _coroutine_SWITCHCASE locations                                       \
        case _coroutine_BEGIN_ ## routine ## _enum:                           \
            break;                                                            \
    }

#endif // #ifndef NDEBUG

/// \def END_COROUTINE
/// \brief end a coroutine that was started with `DEFINE_COROUTINE`
#define END_COROUTINE                                                         \
     TERMINATE_COROUTINE_SUCCESSFULLY();                                      \
}

/// \def RUN_COROUTINES
/// \brief starts two coroutines more or less in lockstep
/// If the coroutines are member methods, also `RUN_COROUTINES` has to be
/// called within a member method (otherwise, a namespace error will be
/// generated).
/// \param routine1    the first coroutine to be started (defined with
///                    `DEFINE_COROUTINE`)
/// \param param1      actual parameter list of routine1, in parentheses
/// \param final1      statement to be executed if routine1 terminates first
/// \param routine2    the second coroutine to be started (defined with
///                    `DEFINE_COROUTINE`)
/// \param param2      actual parameter list of routine2, in parentheses
/// \param final2      statement to be executed if routine2 terminates first
/// \param shared_type type of the data shared between the two coroutines
/// \param shared_init initial value of the shared data, in parentheses
#ifndef NDEBUG

#define RUN_COROUTINES(routine1, param1, final1, routine2, param2, final2,    \
                                                    shared_type, shared_init) \
        do                                                                    \
        {                                                                     \
            _coroutine_ ## routine1 ## _struct _coroutine_local1 =            \
                                    _coroutine_ ## routine1 ## _struct param1;\
            _coroutine_ ## routine2 ## _struct _coroutine_local2 =            \
                                    _coroutine_ ## routine2 ## _struct param2;\
            shared_type _coroutine_shared_data =                              \
                                             _coroutine_NO_PARENS shared_init;\
            for (size_t _coroutine_allowance = 1;; )                          \
            {                                                                 \
                coroutine::_coroutine_result_t _coroutine_result =            \
                        _coroutine_ ## routine1 ## _func(_coroutine_allowance,\
                                                     _coroutine_local1,       \
                                                     _coroutine_shared_data); \
                if (coroutine::_coroutine_CONTINUE != _coroutine_result)      \
                {                                                             \
                    if (coroutine::_coroutine_TERMINATE == _coroutine_result) \
                    {                                                         \
                        {  final1;  }                                         \
                        break;                                                \
                    }                                                         \
                    else if(coroutine::_coroutine_ABORT != _coroutine_result) \
                    {                                                         \
                        std::cerr << __FILE__ << "(" << __LINE__              \
                                << "): Illegal return value of coroutine "    \
                                << "_coroutine_" #routine1 "_func" << "\n";   \
                        exit(EXIT_FAILURE);                                   \
                    }                                                         \
                    _coroutine_allowance = 0;                                 \
                }                                                             \
                else if (0 == _coroutine_allowance)                           \
                {                                                             \
                    std::cerr << __FILE__ << "(" << __LINE__ <<"): Coroutine "\
                                            << "_coroutine_" #routine1 "_func"\
                                            << " takes too many steps\n";     \
                    exit(EXIT_FAILURE);                                       \
                }                                                             \
                else                                                          \
                {                                                             \
                    _coroutine_allowance = 2;                                 \
                }                                                             \
                _coroutine_result = _coroutine_ ## routine2 ## _func(         \
                                     _coroutine_allowance, _coroutine_local2, \
                                                     _coroutine_shared_data); \
                if (coroutine::_coroutine_CONTINUE != _coroutine_result)      \
                {                                                             \
                    if (coroutine::_coroutine_TERMINATE == _coroutine_result) \
                    {                                                         \
                        {  final2;  }                                         \
                        break;                                                \
                    }                                                         \
                    else if(coroutine::_coroutine_ABORT != _coroutine_result) \
                    {                                                         \
                        std::cerr << __FILE__ << "(" << __LINE__              \
                                << "): Illegal return value of coroutine "    \
                                << "_coroutine_" #routine2 "_func" << "\n";   \
                        exit(EXIT_FAILURE);                                   \
                    }                                                         \
                    _coroutine_allowance = 0;                                 \
                }                                                             \
                else if (0 == _coroutine_allowance)                           \
                {                                                             \
                    std::cerr << __FILE__ << "(" << __LINE__ <<"): Coroutine "\
                                            << "_coroutine_" #routine2 "_func"\
                                            << " takes too many steps\n";     \
                    exit(EXIT_FAILURE);                                       \
                }                                                             \
            }                                                                 \
        }                                                                     \
        while (0)

#else // #ifndef NDEBUG

#define RUN_COROUTINES(routine1, param1, final1, routine2, param2, final2,    \
                                                    shared_type, shared_init) \
        do                                                                    \
        {                                                                     \
            _coroutine_ ## routine1 ## _struct _coroutine_local1 =            \
                                    _coroutine_ ## routine1 ## _struct param1;\
            _coroutine_ ## routine2 ## _struct _coroutine_local2 =            \
                                    _coroutine_ ## routine2 ## _struct param2;\
            shared_type _coroutine_shared_data =                              \
                                             _coroutine_NO_PARENS shared_init;\
            for (size_t _coroutine_allowance = 16;; _coroutine_allowance *= 2)\
            {                                                                 \
                coroutine::_coroutine_result_t _coroutine_result =            \
                        _coroutine_ ## routine1 ## _func(_coroutine_allowance,\
                                                     _coroutine_local1,       \
                                                     _coroutine_shared_data); \
                if (coroutine::_coroutine_CONTINUE != _coroutine_result)      \
                {                                                             \
                    if (coroutine::_coroutine_TERMINATE == _coroutine_result) \
                    {                                                         \
                        {  final1;  }                                         \
                        break;                                                \
                    }                                                         \
                    _coroutine_allowance = 0;                                 \
                }                                                             \
                _coroutine_result = _coroutine_ ## routine2 ## _func(         \
                            (_coroutine_allowance+3)/5*7, _coroutine_local2,  \
                            _coroutine_shared_data);                          \
                if (coroutine::_coroutine_CONTINUE != _coroutine_result)      \
                {                                                             \
                    if (coroutine::_coroutine_TERMINATE == _coroutine_result) \
                    {                                                         \
                        {  final2;  }                                         \
                        break;                                                \
                    }                                                         \
                    _coroutine_allowance = 0;                                 \
                }                                                             \
            }                                                                 \
        }                                                                     \
        while (0)

#endif // #ifndef NDEBUG

/// \def COROUTINE_WHILE
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
                    if (0 == --_coroutine_allowance)                          \
                    {                                                         \
                        _coroutine_param._coroutine_location =                \
                                            _coroutine_ ## location ## _enum; \
                        return coroutine::_coroutine_CONTINUE;                \
                    }                                                         \
                    _coroutine_ ## location ## _label:                        \
                    if (!(condition))  break;

/// \def END_COROUTINE_WHILE
/// \brief ends a loop started with `COROUTINE_WHILE`
#define END_COROUTINE_WHILE                                                   \
                }                                                             \
            }                                                                 \
            else  {  }                                                        \
        }                                                                     \
        else  {  }                                                            \
    }                                                                         \
    while (0)

/// \def COROUTINE_FOR
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
                    if (0 == --_coroutine_allowance)                          \
                    {                                                         \
                        _coroutine_param._coroutine_location =                \
                                            _coroutine_ ## location ## _enum; \
                        return coroutine::_coroutine_CONTINUE;                \
                    }                                                         \
                    _coroutine_ ## location ## _label:                        \
                    if (!(condition))  break;

/// \def END_COROUTINE_FOR
/// \brief ends a loop started with `COROUTINE_FOR`
#define END_COROUTINE_FOR                                                     \
                }                                                             \
            }}                                                                \
            while (0);                                                        \
        else  {  }                                                            \
    }                                                                         \
    while (0)

/// \def COROUTINE_DO_WHILE
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
                if (0 == --_coroutine_allowance)                              \
                {                                                             \
                    _coroutine_param._coroutine_location =                    \
                                            _coroutine_ ## location ## _enum; \
                    return coroutine::_coroutine_CONTINUE;                    \
                }                                                             \
                _coroutine_ ## location ## _label:                            \
                if (!(condition))  break;                                     \
                _coroutine_ ## location ## _do_while_begin:

/// \def END_COROUTINE_DO_WHILE
/// \brief ends a loop started with `COROUTINE_DO_WHILE`
#define END_COROUTINE_DO_WHILE                                                \
            }                                                                 \
            while (1);                                                        \
        }}                                                                    \
        else  {  }                                                            \
    while (0)

/// \def TERMINATE_COROUTINE_SUCCESSFULLY
/// \brief terminate the pair of coroutines successfully
/// \details If called in a coroutine, both coroutines will immediately
/// terminate.  The final statement (in `RUN_COROUTINES`) of the one that
/// called `TERMINATE_COROUTINE_SUCCESSFULLY` will be executed.
#define TERMINATE_COROUTINE_SUCCESSFULLY()                                    \
        do                                                                    \
        {                                                                     \
            return coroutine::_coroutine_TERMINATE;                           \
            (void) _coroutine_allowance;                                      \
            (void) _coroutine_param._coroutine_location;                      \
        }                                                                     \
        while (0)

/// \def ABORT_THIS_COROUTINE
/// \brief indicates that this coroutine gives up control to the other one
/// \details In a situation where one coroutine finds that the other should run
/// to completion, it calls `ABORT_THIS_COROUTINE`.
#define ABORT_THIS_COROUTINE()                                                \
        do                                                                    \
        {                                                                     \
            return coroutine::_coroutine_ABORT;                               \
            (void) _coroutine_allowance;                                      \
            (void) _coroutine_param._coroutine_location;                      \
        }                                                                     \
        while (0)

/// \def ABORT_OTHER_COROUTINE
/// \brief indicates that the other coroutine should give up control
/// \details In a situation where one coroutine finds that the other should
/// **no longer** run, it calls `ABORT_OTHER_COROUTINE`.
#define ABORT_OTHER_COROUTINE()                                               \
        do                                                                    \
        {                                                                     \
            _coroutine_allowance = 0;                                         \
            (void) _coroutine_param._coroutine_location;                      \
        }                                                                     \
        while (0)

#endif // ifndef _COROUTINE_H
