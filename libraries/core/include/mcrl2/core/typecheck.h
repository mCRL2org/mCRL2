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

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/data/data_expression.h" // Bad inclusion. Headers should be moved to data/include.

namespace mcrl2
{
namespace core
{

using namespace atermpp;

/** \brief     Type check a parsed mCRL2 data specification.
 *  \param[in] data_spec An aterm representation of an mCRL2 data
 *             specification that adheres to the initial internal aterm
 *             structure.
 *  \post      data_spec is type checked.
 *  \return    If type checking went well, an equivalent version of
 *             data_spec is returned that adheres to the internal aterm
 *             structure after type checking. If something went wrong,
 *             an mcrl2::runtime_error exception is raised.
 **/
// aterm_appl type_check_data_spec(aterm_appl data_spec);

/** \brief     Type check a parsed mCRL2 process specification.
 *  \param[in] proc_spec An aterm representation of an mCRL2 process
 *             specification that adheres to the initial internal aterm
 *             structure.
 *  \post      proc_spec is type checked.
 *  \return    If type checking went well, an equivalent version of
 *             proc_spec is returned that adheres to the internal aterm
 *             structure after type checking. If something went wrong,
 *             an mcrl2::runtime_error exception is raised.
 **/
// aterm_appl type_check_proc_spec(aterm_appl proc_spec);


/** \brief     Type check a parsed mCRL2 parameterised boolean equation
 *             system (PBES) specification.
 *  \param[in] pbes_spec An aterm representation of an mCRL2 PBES
 *             specification that adheres to the initial internal aterm
 *             structure.
 *  \post      pbes_spec is type checked.
 *  \return    If type checking went well, an equivalent version of
 *             pbes_spec is returned that adheres to the internal aterm
 *             structure after type checking. If something went wrong,
 *             an mcrl2::runtime_error exception is raised.
 **/
aterm_appl type_check_pbes_spec(aterm_appl pbes_spec);


/** \brief     Type check a parsed mCRL2 sort expression with respect to a
 *             type checked mCRL2 specification.
 *  \param[in] sort_expr An aterm representation of an mCRL2 sort
 *             expression that adheres to the initial internal aterm
 *             structure.
 *  \param[in] spec An aterm representation of an mCRL2 process specification,
 *             LPS, PBES or data specification that adheres to the
 *             internal aterm structure after type checking.
 *  \post      sort_expr is type checked using the declaration from spec.
 *  \return    If type checking went well, an equivalent version of
 *             sort_expr is returned that adheres to the internal aterm
 *             structure after type checking. if something went wrong,
 *             an mcrl2::runtime_error exception is raised.
 **/
// data::sort_expression type_check_sort_expr(const data::sort_expression &sort_expr, aterm_appl spec);


/** \brief     Type check a parsed mCRL2 data expression with respect to a
 *             type checked mCRL2 specification.
 *  \param[in] data_expr An aterm representation of an mCRL2 data
 *             expression that adheres to the initial internal aterm
 *             structure.
 *  \param[in] spec An aterm representation of an mCRL2 process specification,
 *             LPS, PBES or data specification that adheres to the
 *             internal aterm structure after type checking.
 *  \param[in] Vars A map of variables that may occur in the data expression, where:
 *             \li each lhs is an mCRL2 identifier
 *             \li each rhs is an mCRL2 sort expression that adheres to
 *                 the internal aterm structure after type checking
 *  \post      data_expr is type checked using the declaration from spec
 *             and, if sort_expr is returned, it is type checked as
 *             being of type sort_expr.
 *  \return    If type checking went well, an equivalent version of
 *             data_expr is returned that adheres to the internal aterm
 *             structure after type checking.  If something went wrong,
 *             an mcrl2::runtime_error exception is raised.
 **/
// data::data_expression type_check_data_expr(const data::data_expression& data_expr, aterm_appl spec, 
//                                const std::map<atermpp::aterm_appl,data::sort_expression> &Vars=std::map<atermpp::aterm_appl,data::sort_expression>());


/** \brief     Type check a parsed mCRL2 multiaction with respect to a
 *             type checked mCRL2 specification.
 *  \param[in] mult_act An aterm representation of an mCRL2 multiaction
 *             that adheres to the initial internal aterm structure.
 *  \param[in] data_spec An aterm representation of an mCRL2 data_spec that adheres
 *             to the internal aterm structure.
 *  \param[in] action_labels An action declaration list.
 *  \post      mult_act is type checked using the declarations from spec.
 *  \return    If type checking went well, an equivalent version of
 *             mult_act is returned that adheres to the internal aterm
 *             structure after type checking.  If something went wrong,
 *             an mcrl2::runtime_error exception is raised.
 **/
/* aterm_appl type_check_mult_act(
  aterm_appl mult_act,
  aterm_appl data_spec,
  aterm_list action_labels); */


/** \brief     Type check a parsed mCRL2 multiaction list with respect to a
 *             type checked mCRL2 specification.
 *  \param[in] mult_actions An aterm_list representation of an mCRL2 multiaction list, i.e.
 *             a list of multi actions,
 *             that adheres to the initial internal aterm structure.
 *  \param[in] data_spec An aterm representation of an mCRL2 data_spec that adheres
 *             to the internal aterm structure.
 *  \param[in] action_labels An action declaration list.
 *  \post      mult_actions is type checked using the declarations from spec.
 *  \return    If type checking went well, an equivalent version of
 *             mult_action list is returned that adheres to the internal aterm
 *             structure after type checking.  If something went wrong,
 *             an exception is raised.
 **/
/* aterm_list type_check_mult_actions(
  aterm_list mult_actions,
  aterm_appl data_spec,
  aterm_list action_labels); */


/** \brief     Type check a parsed mCRL2 process expression with respect to a
 *             type checked mCRL2 specification.
 *  \param[in] proc_expr An aterm representation of an mCRL2 process
 *             expression that adheres to the initial internal aterm structure.
 *  \param[in] spec An aterm representation of an mCRL2 LPS that adheres
 *             to the internal aterm structure after type checking.
 *  \post      proc_expr is type checked using the declarations from spec.
 *  \return    If type checking went well, an equivalent version of
 *             proc_expr is returned that adheres to the internal aterm
 *             structure after type checking.  If something went wrong,
 *             an mcrl2::runtime_error exception is raised.
 **/
// aterm_appl type_check_proc_expr(aterm_appl proc_expr, aterm_appl spec);


/** \brief     Type check a parsed mCRL2 state formula with respect to a
 *             type checked mCRL2 process specification or LPS.
 *  \param[in] state_formula An aterm representation of an mCRL2 state
 *             formula that adheres to the initial internal aterm structure.
 *  \param[in] spec An aterm representation of an mCRL2 process specification
 *             or LPS that adheres to the internal aterm structure after
 *             type checking.
 *  \post      state_formula is type checked using the declarations from spec.
 *  \return    If type checking went well, an equivalent version of
 *             state_formula is returned that adheres to the internal
 *             aterm structure after type checking.  If something went
 *             wrong, an mcrl2::runtime_error exception is raised.
 **/
aterm_appl type_check_state_frm(aterm_appl state_formula, aterm_appl spec);


/** \brief     Type check a parsed mCRL2 action rename specification with respect to a
 *             type checked mCRL2 specification.
 *  \param[in] ar_spec An aterm representation of an mCRL2 action rename
 *             specification formula that adheres to the initial
 *             internal aterm structure.
 *  \param[in] spec An aterm representation of an mCRL2 LPS that adheres
 *             to the internal aterm structure after type checking.
 *  \post      ar_spec is type checked using the declarations from spec and its
 *             own declarations.
 *  \return    If type checking went well, an equivalent version of
 *             ar_spec is returned that adheres to the internal aterm
 *             structure after type checking.  If something went wrong,
 *             an mcrl2::runtime_error exception is raised.
 **/
aterm_appl type_check_action_rename_spec(aterm_appl ar_spec, aterm_appl spec);


/** \brief     Type check parsed mCRL2 data variables
 *             with respect to a type checked mCRL2 specification.
 *  \param[in] data_vars An aterm representation of a list of
 *             data variables that adheres to the initial internal
 *             aterm structure.
 *  \param[in] spec An aterm representation of an mCRL2 process specification,
 *             LPS, PBES or data specification that adheres to the
 *             internal aterm structure after type checking.
 *  \post      data_var_decl is type checked using the declaration from spec.
 *  \return    If type checking went well, an equivalent version of
 *             data_vars is returned that adheres to the internal aterm
 *             structure after type checking. if something went wrong,
 *             an mcrl2::runtime_error exception is raised.
 **/
// aterm_list type_check_data_vars(aterm_list data_vars, aterm_appl spec);

}
}

#endif // PARSER_TYPECHECK_H
