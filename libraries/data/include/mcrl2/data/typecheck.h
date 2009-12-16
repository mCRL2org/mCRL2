// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/typecheck.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_TYPECHECK_H
#define MCRL2_DATA_TYPECHECK_H

#include "mcrl2/core/typecheck.h"
#include "mcrl2/data/data_specification.h"

namespace mcrl2 {

  namespace data {

    /** \brief     Type check a sort expression.
     *  Throws an exception if something went wrong.
     *  \param[in] sort_expr A sort expression that has not been type checked.
     *  \post      sort_expr is type checked.
     **/
    inline
    void type_check(sort_expression& sort_expr, const data_specification& data_spec)
    {
      // TODO: replace all this nonsense code by a proper type check implementation
      ATermAppl t = detail::data_specification_to_aterm_data_spec(sort_expr);
      t = core::type_check_sort_expr(t, detail::data_specification_to_aterm_data_spec(data_spec));
      if (!t)
      {
        throw mcrl2::runtime_error("could not type check " + core::pp(data_spec));
      }
      sort_expr = sort_expression(t);
    }

    /** \brief     Type check a data expression.
     *  Throws an exception if something went wrong.
     *  \param[in] data_expr A data expression that has not been type checked.
     *  \post      data_expr is type checked.
     **/
    inline
    void type_check(data_expression& data_expr, const data_specification& data_spec)
    {
      // TODO: replace all this nonsense code by a proper type check implementation
      ATermAppl t = detail::data_specification_to_aterm_data_spec(data_expr);
      t = core::type_check_data_expr(t, detail::data_specification_to_aterm_data_spec(data_spec));
      if (!t)
      {
        throw mcrl2::runtime_error("could not type check " + core::pp(data_expr));
      }
      data_expr = data_expression(t);
    }

    /** \brief     Type check a parsed mCRL2 data specification.
     *  Throws an exception if something went wrong.
     *  \param[in] data_spec A data specification that has not been type checked.
     *  \post      data_spec is type checked.
     **/
    inline
    void type_check(data_specification& data_spec)
    {
      // TODO: replace all this nonsense code by a proper type check implementation
      ATermAppl t = detail::data_specification_to_aterm_data_spec(data_spec);
      t = core::type_check_data_spec(t);
      if (!t)
      {
        throw mcrl2::runtime_error("could not type check " + core::pp(data_spec));
      }
      data_specification result(t);
      detail::internal_format_conversion(result);
      data_spec = result;
    }

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_TYPECHECK_H
