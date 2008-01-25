// Author(s): Yaroslav Usenko
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file typecheck.h
///
/// \brief A library for type checking (parts of) the internal mCRL2 data structure.

#ifndef PARSER_TYPECHECK_H
#define PARSER_TYPECHECK_H

#include <aterm2.h>
#include "mcrl2/lps/specification.h"

namespace mcrl2 {
  namespace core {
    namespace detail{
 
//Global preconditions:
//- the ATerm library has been initialised

/** \pre  spec represents an mCRL2 specification that adheres to the initial
 *      internal ATerm structure.
 * \post spec is type checked.
 * \return  if type checking went well, an equivalent version of spec is returned
 *      that adheres to the internal ATerm structure after type checking.
 *      if something went wrong, an appropriate error message is printed and
 *      NULL is returned.
 **/
ATermAppl type_check_spec(ATermAppl spec);

/** \pre  spec represents a (partial) mCRL2 specification that adheres to the initial
 *      internal ATerm structure (without init).
 * \post spec is type checked.
 * \return  if type checking went well, an equivalent version of spec is returned
 *      that adheres to the internal ATerm structure after type checking.
 *      if something went wrong, an appropriate error message is printed and
 *      NULL is returned.
 **/
ATermAppl type_check_spec_part(ATermAppl spec);

/** \pre  sort_expr represents an mCRL2 sort expression that adheres to the
 *      initial internal ATerm structure.
 *      lps_spec represents an LPS specification
 * \post sort_expr is type checked using the declaration from lps_spec
 * \return  if type checking went well, an equivalent version of sort_expr is returned
 *      that adheres to the internal ATerm structure after type checking.
 *      if something went wrong, an appropriate error message is printed and
 *      NULL is returned.
 **/
ATermAppl type_check_sort_expr(ATermAppl sort_expr, lps::specification &lps_spec);

/** \pre  sort_expr represents an mCRL2 sort expression that adheres to the
 *      initial internal ATerm structure.
 *      spec represents a (partial) mCRL2 specification that adheres to the initial
 *      internal ATerm structure (without init).
 * \post sort_expr is type checked using the declaration from spec
 * \return  if type checking went well, an equivalent version of sort_expr is returned
 *      that adheres to the internal ATerm structure after type checking.
 *      if something went wrong, an appropriate error message is printed and
 *      NULL is returned.
 **/
ATermAppl type_check_sort_expr_part(ATermAppl sort_expr, ATermAppl spec=NULL);

/** \pre  data_expr represents an mCRL2 data expression that adheres to the
 *      initial internal ATerm structure.
 *      sort_expr represents an mCRL2 sort expression that adheres to the
 *      internal ATerm structure after type checking or is NULL.
 *      lps_spec represents an LPS specification
 *      use_vars_from_lps says if the parameters of the LPS can be used in the data expression
 *      Vars contains any extra variables that can be used in the data expression
 * \post data_expr is type checked using the declaration from lps_spec and, if
 *      sort_expr is not NULL, it is type checked as being of type sort_expr
 * \return  if type checking went well, an equivalent version of data_expr is returned
 *      that adheres to the internal ATerm structure after type checking.
 *      if something went wrong, an appropriate error message is printed and
 *      NULL is returned.
 **/
ATermAppl type_check_data_expr(ATermAppl data_expr, ATermAppl sort_expr, lps::specification &lps_spec, bool use_vars_from_lps=false, ATermTable Vars=NULL);

/** \pre  data_expr represents an mCRL2 data expression that adheres to the
 *      initial internal ATerm structure.
 *      sort_expr represents an mCRL2 sort expression that adheres to the
 *      internal ATerm structure after type checking.
 *      spec represents a (partial) mCRL2 specification that adheres to the initial
 *      internal ATerm structure (without init).
 * \post data_expr is type checked as being of type sort_expr using the
 *      declaration from spec
 * \return  if type checking went well, an equivalent version of data_expr is returned
 *      that adheres to the internal ATerm structure after type checking.
 *      if something went wrong, an appropriate error message is printed and
 *      NULL is returned.
 **/
ATermAppl type_check_data_expr_part(ATermAppl data_expr, ATermAppl sort_expr, ATermAppl spec=NULL, ATermTable Vars=NULL);

/** \pre  mult_act represents an mCRL2 multi-action that adheres to the initial
 *      internal ATerm structure.
 *      lps_spec represents an LPS specification
 *      use_vars_from_lps says if the parameters of the LPS can be used in the data expression
 * \post mult_act is type checked using the declarations from lps_spec
 * \return  if type checking went well, an equivalent version of mult_act is returned
 *      that adheres to the internal ATerm structure after type checking.
 *      if something went wrong, an appropriate error message is printed and
 *      NULL is returned.
 **/
ATermAppl type_check_mult_act(ATermAppl mult_act, lps::specification &lps_spec, bool use_vars_from_lps=false);

/** \pre  proc_expr represents an mCRL2 proc expression that adheres to the
 *      initial internal ATerm structure.
 *      lps_spec represents an LPS specification
 *      use_vars_from_lps says if the parameters of the LPS can be used in the data expression
 * \post proc_expr is type checked using the declaration from lps_spec
 * \return  if type checking went well, an equivalent version of proc_expr is returned
 *      that adheres to the internal ATerm structure after type checking.
 *      if something went wrong, an appropriate error message is printed and
 *      NULL is returned.
 **/
ATermAppl type_check_proc_expr(ATermAppl proc_expr, lps::specification &lps_spec, bool use_vars_from_lps=false);

/** \pre  state_formula represents an mCRL2 state formula that adheres to the
 *      initial internal ATerm structure.
 *      spec represents an LPS specification before data implementation
 *      use_vars_from_lps says if the parameters of the LPS can be used in the data expression
 * \post state_formula is type checked using the declarations from lps_spec
 * \return  if type checking went well, an equivalent version of state_formula is
 *      returned that adheres to the internal ATerm structure after type
 *      checking.
 *      if something went wrong, an appropriate error message is printed and
 *      NULL is returned.
 **/
ATermAppl type_check_state_frm(ATermAppl state_formula, ATermAppl spec, bool use_vars_from_lps=false);

/** \pre  ar_spec represents an action rename specification that adheres to the
 *      initial internal ATerm structure.
 *      lps_spec represents an LPS specification
 * \post ar_spec is type checked using the declarations from lps_spec and its
 *      own declarations
 * \return  if type checking went well, an equivalent version of ar_spec is returned
 *     that adheres to the internal ATerm structure after type checking.
 *      if something went wrong, an appropriate error message is printed and
 *      NULL is returned.
 **/
ATermAppl type_check_action_rename_spec(ATermAppl ar_spec, lps::specification &lps_spec);

    } 
  }
}

#endif // PARSER_TYPECHECK_H
