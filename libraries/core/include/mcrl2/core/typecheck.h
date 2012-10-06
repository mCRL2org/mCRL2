// Author(s): Yaroslav Usenko
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/typecheck.h
/// \brief Type check parsed mCRL2 specifications and expressions.

#ifndef PARSER_TYPECHECK_H
#define PARSER_TYPECHECK_H

#include "mcrl2/aterm/aterm.h"

namespace mcrl2
{
namespace core
{

using namespace aterm_deprecated;

/** \brief     Type check a parsed mCRL2 data specification.
 *  \param[in] data_spec An ATerm representation of an mCRL2 data
 *             specification that adheres to the initial internal ATerm
 *             structure.
 *  \post      data_spec is type checked.
 *  \return    If type checking went well, an equivalent version of
 *             data_spec is returned that adheres to the internal ATerm
 *             structure after type checking. If something went wrong,
 *             an appropriate error message is printed and NULL is
 *             returned.
 **/
ATermAppl type_check_data_spec(ATermAppl data_spec);

/** \brief     Type check a parsed mCRL2 process specification.
 *  \param[in] proc_spec An ATerm representation of an mCRL2 process
 *             specification that adheres to the initial internal ATerm
 *             structure.
 *  \post      proc_spec is type checked.
 *  \return    If type checking went well, an equivalent version of
 *             proc_spec is returned that adheres to the internal ATerm
 *             structure after type checking. If something went wrong,
 *             an appropriate error message is printed and NULL is
 *             returned.
 **/
ATermAppl type_check_proc_spec(ATermAppl proc_spec);


/** \brief     Type check a parsed mCRL2 parameterised boolean equation
 *             system (PBES) specification.
 *  \param[in] pbes_spec An ATerm representation of an mCRL2 PBES
 *             specification that adheres to the initial internal ATerm
 *             structure.
 *  \post      pbes_spec is type checked.
 *  \return    If type checking went well, an equivalent version of
 *             pbes_spec is returned that adheres to the internal ATerm
 *             structure after type checking. If something went wrong,
 *             an appropriate error message is printed and NULL is
 *             returned.
 **/
ATermAppl type_check_pbes_spec(ATermAppl pbes_spec);


/** \brief     Type check a parsed mCRL2 sort expression with respect to a
 *             type checked mCRL2 specification.
 *  \param[in] sort_expr An ATerm representation of an mCRL2 sort
 *             expression that adheres to the initial internal ATerm
 *             structure.
 *  \param[in] spec An ATerm representation of an mCRL2 process specification,
 *             LPS, PBES or data specification that adheres to the
 *             internal ATerm structure after type checking.
 *  \post      sort_expr is type checked using the declaration from spec.
 *  \return    If type checking went well, an equivalent version of
 *             sort_expr is returned that adheres to the internal ATerm
 *             structure after type checking. if something went wrong,
 *             an appropriate error message is printed and NULL is
 *             returned.
 **/
ATermAppl type_check_sort_expr(ATermAppl sort_expr, ATermAppl spec);


/** \brief     Type check a parsed mCRL2 data expression with respect to a
 *             type checked mCRL2 specification.
 *  \param[in] data_expr An ATerm representation of an mCRL2 data
 *             expression that adheres to the initial internal ATerm
 *             structure.
 *  \param[in] sort_expr An ATerm representation of an mCRL2 sort
 *             expression that adheres to the internal ATerm structure
 *             after type checking, or NULL.
 *  \param[in] spec An ATerm representation of an mCRL2 process specification,
 *             LPS, PBES or data specification that adheres to the
 *             internal ATerm structure after type checking.
 *  \param[in] Vars A table of variables that may occur in the data expression, where:
 *             \li each key is an mCRL2 identifier
 *             \li each value is an mCRL2 sort expression that adheres to
 *                 the internal ATerm structure after type checking
 *  \post      data_expr is type checked using the declaration from spec
 *             and, if sort_expr is not NULL, it is type checked as
 *             being of type sort_expr.
 *  \return    If type checking went well, an equivalent version of
 *             data_expr is returned that adheres to the internal ATerm
 *             structure after type checking.  If something went wrong,
 *             an appropriate error message is printed and NULL is
 *             returned.
 **/
ATermAppl type_check_data_expr(ATermAppl data_expr, ATermAppl sort_expr, ATermAppl spec, const table &Vars=table());


/** \brief     Type check a parsed mCRL2 multiaction with respect to a
 *             type checked mCRL2 specification.
 *  \param[in] mult_act An ATerm representation of an mCRL2 multiaction
 *             that adheres to the initial internal ATerm structure.
 *  \param[in] data_spec An ATerm representation of an mCRL2 data_spec that adheres
 *             to the internal ATerm structure.
 *  \param[in] action_labels An action declaration list.
 *  \post      mult_act is type checked using the declarations from spec.
 *  \return    If type checking went well, an equivalent version of
 *             mult_act is returned that adheres to the internal ATerm
 *             structure after type checking.  If something went wrong,
 *             an appropriate error message is printed and NULL is
 *             returned.
 **/
ATermAppl type_check_mult_act(
  ATermAppl mult_act,
  ATermAppl data_spec,
  ATermList action_labels);


/** \brief     Type check a parsed mCRL2 multiaction list with respect to a
 *             type checked mCRL2 specification.
 *  \param[in] mult_actions An ATermList representation of an mCRL2 multiaction list, i.e.
 *             a list of multi actions,
 *             that adheres to the initial internal ATerm structure.
 *  \param[in] data_spec An ATerm representation of an mCRL2 data_spec that adheres
 *             to the internal ATerm structure.
 *  \param[in] action_labels An action declaration list.
 *  \post      mult_actions is type checked using the declarations from spec.
 *  \return    If type checking went well, an equivalent version of
 *             mult_action list is returned that adheres to the internal ATerm
 *             structure after type checking.  If something went wrong,
 *             an exception is raised.
 **/
ATermList type_check_mult_actions(
  ATermList mult_actions,
  ATermAppl data_spec,
  ATermList action_labels);


/** \brief     Type check a parsed mCRL2 process expression with respect to a
 *             type checked mCRL2 specification.
 *  \param[in] proc_expr An ATerm representation of an mCRL2 process
 *             expression that adheres to the initial internal ATerm structure.
 *  \param[in] spec An ATerm representation of an mCRL2 LPS that adheres
 *             to the internal ATerm structure after type checking.
 *  \post      proc_expr is type checked using the declarations from spec.
 *  \return    If type checking went well, an equivalent version of
 *             proc_expr is returned that adheres to the internal ATerm
 *             structure after type checking.  If something went wrong,
 *             an appropriate error message is printed and NULL is
 *             returned.
 **/
ATermAppl type_check_proc_expr(ATermAppl proc_expr, ATermAppl spec);


/** \brief     Type check a parsed mCRL2 state formula with respect to a
 *             type checked mCRL2 process specification or LPS.
 *  \param[in] state_formula An ATerm representation of an mCRL2 state
 *             formula that adheres to the initial internal ATerm structure.
 *  \param[in] spec An ATerm representation of an mCRL2 process specification
 *             or LPS that adheres to the internal ATerm structure after
 *             type checking.
 *  \post      state_formula is type checked using the declarations from spec.
 *  \return    If type checking went well, an equivalent version of
 *             state_formula is returned that adheres to the internal
 *             ATerm structure after type checking.  If something went
 *             wrong, an appropriate error message is printed and NULL
 *             is returned.
 **/
ATermAppl type_check_state_frm(ATermAppl state_formula, ATermAppl spec);


/** \brief     Type check a parsed mCRL2 action rename specification with respect to a
 *             type checked mCRL2 specification.
 *  \param[in] ar_spec An ATerm representation of an mCRL2 action rename
 *             specification formula that adheres to the initial
 *             internal ATerm structure.
 *  \param[in] spec An ATerm representation of an mCRL2 LPS that adheres
 *             to the internal ATerm structure after type checking.
 *  \post      ar_spec is type checked using the declarations from spec and its
 *             own declarations.
 *  \return    If type checking went well, an equivalent version of
 *             ar_spec is returned that adheres to the internal ATerm
 *             structure after type checking.  If something went wrong,
 *             an appropriate error message is printed and NULL is
 *             returned.
 **/
ATermAppl type_check_action_rename_spec(ATermAppl ar_spec, ATermAppl spec);


/** \brief     Type check parsed mCRL2 data variables
 *             with respect to a type checked mCRL2 specification.
 *  \param[in] data_vars An ATerm representation of a list of
 *             data variables that adheres to the initial internal
 *             ATerm structure.
 *  \param[in] spec An ATerm representation of an mCRL2 process specification,
 *             LPS, PBES or data specification that adheres to the
 *             internal ATerm structure after type checking.
 *  \post      data_var_decl is type checked using the declaration from spec.
 *  \return    If type checking went well, an equivalent version of
 *             data_vars is returned that adheres to the internal ATerm
 *             structure after type checking. if something went wrong,
 *             an appropriate error message is printed and NULL is
 *             returned.
 **/
ATermList type_check_data_vars(ATermList data_vars, ATermAppl spec);

}
}

#endif // PARSER_TYPECHECK_H
