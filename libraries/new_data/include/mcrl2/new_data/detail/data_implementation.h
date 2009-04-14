// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_implementation.h
///
/// \brief Implement new_data types in type checked mCRL2 specifications and expressions.

#ifndef MCRL2_NEW_DATA_DATAIMPL_H
#define MCRL2_NEW_DATA_DATAIMPL_H

#include <aterm2.h>
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/new_data/detail/data_implementation_concrete.h"
#include "mcrl2/core/detail/struct.h"

namespace mcrl2 {
  namespace new_data {
    namespace detail {


/** \brief     Implement data types of a type checked mCRL2 data
 *             specification.
 *  \param[in] spec An ATerm representation of an mCRL2 data
 *             specification that adheres to the internal ATerm structure
 *             after the type checking phase.
 *  \post      The datatypes of spec are implemented as higher-order
 *             abstract data types.
 *  \return    If the data implementation went well, an equivalent
 *             version of spec is returned that adheres to the internal
 *             ATerm structure after data implementation.  If something
 *             went wrong, an appropriate error message is printed and
 *             NULL is returned.
 **/
inline ATermAppl implement_data_spec(ATermAppl spec)
{
  assert(core::detail::gsIsDataSpec(spec));

  ATermList substitution_context = ATmakeList0();

  return implement_data_spec(spec, &substitution_context);
}

/** \brief     Implement data types of a type checked mCRL2 process
 *             specification.
 *  \param[in] spec An ATerm representation of an mCRL2 process
 *             specification that adheres to the internal ATerm structure
 *             after the type checking phase.
 *  \post      The datatypes of spec are implemented as higher-order
 *             abstract data types.
 *  \return    If the data implementation went well, an equivalent
 *             version of spec is returned that adheres to the internal
 *             ATerm structure after data implementation.  If something
 *             went wrong, an appropriate error message is printed and
 *             NULL is returned.
**/
inline ATermAppl implement_data_proc_spec(ATermAppl spec)
{
  assert(core::detail::gsIsProcSpec(spec));

  ATermList substitution_context = ATmakeList0();

  return implement_data_spec(spec, &substitution_context);
}

/** \brief     Implement data types of a type checked mCRL2 linear process
 *             specification.
 *  \param[in] spec An ATerm representation of an mCRL2 linear process
 *             specification that adheres to the internal ATerm structure
 *             after the type checking phase.
 *  \post      The datatypes of spec are implemented as higher-order
 *             abstract data types.
 *  \return    If the data implementation went well, an equivalent
 *             version of spec is returned that adheres to the internal
 *             ATerm structure after data implementation.  If something
 *             went wrong, an appropriate error message is printed and
 *             NULL is returned.
**/
inline ATermAppl implement_data_lin_proc_spec(ATermAppl spec)
{
  assert(core::detail::gsIsLinProcSpec(spec));

  ATermList substitution_context = ATmakeList0();

  return detail::implement_data_spec(spec, &substitution_context); // TODO
}

/** \brief     Implement data types of a type checked mCRL2 parameterised
 *             boolean equation system (PBES) specification.
 *  \param[in] spec An ATerm representation of an mCRL2 PBES
 *             specification that adheres to the internal ATerm structure
 *             after the type checking phase.
 *  \post      The datatypes of spec are implemented as higher-order
 *             abstract data types.
 *  \return    If the data implementation went well, an equivalent
 *             version of spec is returned that adheres to the internal
 *             ATerm structure after data implementation.  If something
 *             went wrong, an appropriate error message is printed and
 *             NULL is returned.
**/
inline ATermAppl implement_data_pbes_spec(ATermAppl spec)
{
  assert(core::detail::gsIsPBES(spec));

  ATermList substitution_context = ATmakeList0();

  return implement_data_spec(spec, &substitution_context); // TODO
}

/** \brief     Implement data types of a type checked mCRL2 sort
 *             expression with respect to a type checked mCRL2
 *             specification.
 *  \param[in] sort_expr An ATerm representation of an mCRL2 sort
 *             expression that adheres to the internal ATerm structure
 *             after the type checking phase.
 *  \param[in] spec An ATerm representation of an mCRL2 process
 *             specification, LPS, PBES or data specification that
 *             adheres to the internal ATerm structure after the type
 *             checking phase.
 *  \post      The data types of sort_expr are implemented as
 *             higher-order abstract data types in spec.
 *             spec is updated to be the equivalent of spec in the
 *             internal format after data implementation.
 *  \return    If the data implementation went well, an equivalent
 *             version of sort_expr is returned that adheres to the
 *             internal ATerm structure after data implementation.  If
 *             something went wrong, an appropriate error message is
 *             printed and NULL is returned.
**/
inline ATermAppl implement_data_sort_expr(ATermAppl sort_expr, ATermAppl& spec)
{
  assert(core::detail::gsIsSortExpr(sort_expr));
  return impl_exprs_with_spec(sort_expr, spec);
}

/** \brief     Implement data types of a type checked mCRL2 data expression
 *             with respect to a type checked mCRL2 specification.
 *  \param[in] data_expr An ATerm representation of an mCRL2 data
 *             expression that adheres to the internal ATerm structure
 *             after the type checking phase.
 *  \param[in] spec An ATerm representation of an mCRL2 process
 *             specification, LPS, PBES or data specification that
 *             adheres to the internal ATerm structure after the type
 *             checking phase.
 *  \post      The data types of data_expr are implemented as
 *             higher-order abstract data types in spec.
 *             spec is updated to be the equivalent of spec in the
 *             internal format after data implementation.
 *  \return    If the data implementation went well, an equivalent
 *             version of data_expr is returned that adheres to the
 *             internal ATerm structure after data implementation.  If
 *             something went wrong, an appropriate error message is
 *             printed and NULL is returned.
**/
inline ATermAppl implement_data_expr(ATermAppl data_expr, ATermAppl& spec)
{
  assert(core::detail::gsIsDataExpr(data_expr));
  return impl_exprs_with_spec(data_expr, spec);
}

/** \brief     Implement data types of a type checked mCRL2 multiaction
 *             with respect to a type checked mCRL2 specification.
 *  \param[in] mult_act An ATerm representation of an mCRL2 multiaction
 *             that adheres to the internal ATerm structure after the
 *             type checking phase.
 *  \param[in] spec An ATerm representation of an mCRL2 process
 *             specification, LPS, PBES or data specification that
 *             adheres to the internal ATerm structure after the type
 *             checking phase.
 *  \post      The data types of mult_act are implemented as
 *             higher-order abstract data types in spec.
 *             spec is updated to be the equivalent of spec in the
 *             internal format after data implementation.
 *  \return    If the data implementation went well, an equivalent
 *             version of mult_act is returned that adheres to the
 *             internal ATerm structure after data implementation.  If
 *             something went wrong, an appropriate error message is
 *             printed and NULL is returned.
**/
inline ATermAppl implement_data_mult_act(ATermAppl mult_act, ATermAppl& spec)
{
  assert(core::detail::gsIsMultAct(mult_act));
  return impl_exprs_with_spec(mult_act, spec);
}

/** \brief     Implement data types of a type checked mCRL2 process
 *             expression with respect to a type checked mCRL2
 *             specification.
 *  \param[in] proc_expr An ATerm representation of an mCRL2 process
 *             expression that adheres to the internal ATerm structure
 *             after the type checking phase.
 *  \param[in] spec An ATerm representation of an mCRL2 process
 *             specification, LPS, PBES or data specification that
 *             adheres to the internal ATerm structure after the type
 *             checking phase.
 *  \post      The data types of proc_expr are implemented as
 *             higher-order abstract data types in spec.
 *             spec is updated to be the equivalent of spec in the
 *             internal format after data implementation.
 *  \return    If the data implementation went well, an equivalent
 *             version of proc_expr is returned that adheres to the
 *             internal ATerm structure after data implementation.  If
 *             something went wrong, an appropriate error message is
 *             printed and NULL is returned.
**/
inline ATermAppl implement_data_proc_expr(ATermAppl proc_expr, ATermAppl& spec)
{
  assert(core::detail::gsIsProcExpr(proc_expr));
  return impl_exprs_with_spec(proc_expr, spec);
}

/** \brief     Implement data types of a type checked mCRL2 state formula
 *             with respect to a type checked mCRL2 specification.
 *  \param[in] state_frm An ATerm representation of an mCRL2 state
 *             formula that adheres to the internal ATerm structure
 *             after the type checking phase.
 *  \param[in] spec An ATerm representation of an mCRL2 process
 *             specification, LPS, PBES or data specification that
 *             adheres to the internal ATerm structure after the type
 *             checking phase.
 *  \post      The data types of state_frm are implemented as
 *             higher-order abstract data types in spec.
 *             spec is updated to be the equivalent of spec in the
 *             internal format after data implementation.
 *  \return    If the data implementation went well, an equivalent
 *             version of state_frm is returned that adheres to the
 *             internal ATerm structure after data implementation.  If
 *             something went wrong, an appropriate error message is
 *             printed and NULL is returned.
**/
inline ATermAppl implement_data_state_frm(ATermAppl state_frm, ATermAppl& spec)
{
  assert(core::detail::gsIsStateFrm(state_frm));
  return impl_exprs_with_spec(state_frm, spec);
}

/** \brief     Implement data types of a type checked mCRL2 action rename
 *             specification with respect to a type checked mCRL2 linear
 *             process specification (LPS).
 *  \param[in] ar_spec An ATerm representation of an mCRL2 action rename
 *             specification that adheres to the internal ATerm
 *             structure after the type checking phase.
 *  \param[in] lps_spec An ATerm representation of an mCRL2 LPS that adheres
 *             to the internal ATerm structure after the type checking phase.
 *  \post      The data types in action_rename_spec are implemented as
 *             higher-order abstract data types types and the data types
 *             of lps_spec are added to the data types of ar_spec.  The
 *             datatypes of lps_spec are implemented as higher-order
 *             abstract data types in lps_spec
 *  \return    If the data implementation went well, an equivalent
 *             version of ar_spec is returned that adheres to the
 *             internal ATerm structure after new_data implementation, also
 *             containing the new_data specification of lps_spec If
 *             something went wrong, an appropriate error message is
 *             printed and NULL is returned.
**/
inline ATermAppl implement_data_action_rename_spec(ATermAppl ar_spec, ATermAppl& lps_spec)
{
  assert(core::detail::gsIsActionRenameSpec(ar_spec));
  assert(core::detail::gsIsLinProcSpec(lps_spec));
  return impl_data_action_rename_spec_detail(ar_spec, lps_spec);
}

    } // namespace detail
  } // namespace new_data
} // namespace mcrl2
#endif // MCRL2_NEW_DATA_DATAIMPL_H
