// Author(s): Yaroslav Usenko
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file typecheck_part.h
/// \brief Type check parsed expressions against a partial
///        mCRL2 specification.

#ifndef PARSER_TYPECHECK_PART_H
#define PARSER_TYPECHECK_PART_H

#include <aterm2.h>

namespace mcrl2 {
  namespace core {
    namespace detail {
 
/** \brief     Type check a parsed mCRL2 sort expression with respect to a
 *             type checked partial mCRL2 specification.
 *  \param[in] sort_expr An ATerm representation of an mCRL2 sort
 *             expression that adheres to the initial internal ATerm
 *             structure.
 *  \param[in] spec An ATerm representation of a (partial) mCRL2
 *             specification that adheres to the initial internal ATerm structure
 *             (without init).
 *  \post      sort_expr is type checked using the declaration from spec.
 *  \return    If type checking went well, an equivalent version of
 *             sort_expr is returned that adheres to the internal ATerm
 *             structure after type checking.  If something went wrong,
 *             an appropriate error message is printed and NULL is
 *             returned.
 **/
ATermAppl type_check_sort_expr_part(ATermAppl sort_expr, ATermAppl spec=NULL);


/** \brief     Type check a parsed mCRL2 data expression with respect to a
 *             type checked partial mCRL2 specification.
 *  \param[in] data_expr An ATerm representation of an mCRL2 data
 *             expression that adheres to the initial internal ATerm
 *             structure.
 *  \param[in] sort_expr An ATerm representation of an mCRL2 sort
 *             expression that adheres to the internal ATerm structure
 *             after type checking, or NULL.
 *  \param[in] spec An ATerm representation of a partial (without init)
 *             mCRL2 LPS, PBES, or data specification that adheres to
 *             the internal ATerm structure after type checking.
 *  \param[in] Vars A table of variables that may occur in the data expression.
 *  \post      data_expr is type checked as being of type sort_expr using the
 *             declaration from spec.
 *  \return    If type checking went well, an equivalent version of
 *             data_expr is returned that adheres to the internal ATerm
 *             structure after type checking.  If something went wrong,
 *             an appropriate error message is printed and NULL is
 *             returned.
 **/
ATermAppl type_check_data_expr_part(ATermAppl data_expr, ATermAppl sort_expr, ATermAppl spec=NULL, ATermTable Vars=NULL);

    }
  }
}

#endif // PARSER_TYPECHECK_PART_H
