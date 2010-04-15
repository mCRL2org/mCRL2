// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/boolean_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_BOOLEAN_EXPRESSION_H
#define MCRL2_FDR_BOOLEAN_EXPRESSION_H

#include "mcrl2/fdr/boolean_expression_decl.h"
#include "mcrl2/fdr/boolean_expression_impl.h"

//--- start generated is-functions ---//

    /// \brief Test for a true expression
    /// \param t A term
    /// \return True if it is a true expression
    inline
    bool is_true(const boolean_expression& t)
    {
      return fdr::detail::gsIstrue(t);
    }

    /// \brief Test for a false expression
    /// \param t A term
    /// \return True if it is a false expression
    inline
    bool is_false(const boolean_expression& t)
    {
      return fdr::detail::gsIsfalse(t);
    }

    /// \brief Test for a and expression
    /// \param t A term
    /// \return True if it is a and expression
    inline
    bool is_and(const boolean_expression& t)
    {
      return fdr::detail::gsIsAnd(t);
    }

    /// \brief Test for a or expression
    /// \param t A term
    /// \return True if it is a or expression
    inline
    bool is_or(const boolean_expression& t)
    {
      return fdr::detail::gsIsOr(t);
    }

    /// \brief Test for a not expression
    /// \param t A term
    /// \return True if it is a not expression
    inline
    bool is_not(const boolean_expression& t)
    {
      return fdr::detail::gsIsNot(t);
    }

    /// \brief Test for a null expression
    /// \param t A term
    /// \return True if it is a null expression
    inline
    bool is_null(const boolean_expression& t)
    {
      return fdr::detail::gsIsNull(t);
    }

    /// \brief Test for a elem expression
    /// \param t A term
    /// \return True if it is a elem expression
    inline
    bool is_elem(const boolean_expression& t)
    {
      return fdr::detail::gsIsElem(t);
    }

    /// \brief Test for a member expression
    /// \param t A term
    /// \return True if it is a member expression
    inline
    bool is_member(const boolean_expression& t)
    {
      return fdr::detail::gsIsMember(t);
    }

    /// \brief Test for a empty expression
    /// \param t A term
    /// \return True if it is a empty expression
    inline
    bool is_empty(const boolean_expression& t)
    {
      return fdr::detail::gsIsEmpty(t);
    }

    /// \brief Test for a equal expression
    /// \param t A term
    /// \return True if it is a equal expression
    inline
    bool is_equal(const boolean_expression& t)
    {
      return fdr::detail::gsIsEqual(t);
    }

    /// \brief Test for a notequal expression
    /// \param t A term
    /// \return True if it is a notequal expression
    inline
    bool is_notequal(const boolean_expression& t)
    {
      return fdr::detail::gsIsNotEqual(t);
    }

    /// \brief Test for a less expression
    /// \param t A term
    /// \return True if it is a less expression
    inline
    bool is_less(const boolean_expression& t)
    {
      return fdr::detail::gsIsLess(t);
    }

    /// \brief Test for a lessorequal expression
    /// \param t A term
    /// \return True if it is a lessorequal expression
    inline
    bool is_lessorequal(const boolean_expression& t)
    {
      return fdr::detail::gsIsLessOrEqual(t);
    }

    /// \brief Test for a greater expression
    /// \param t A term
    /// \return True if it is a greater expression
    inline
    bool is_greater(const boolean_expression& t)
    {
      return fdr::detail::gsIsGreater(t);
    }

    /// \brief Test for a greaterorequal expression
    /// \param t A term
    /// \return True if it is a greaterorequal expression
    inline
    bool is_greaterorequal(const boolean_expression& t)
    {
      return fdr::detail::gsIsGreaterOrEqual(t);
    }
//--- end generated is-functions ---//

#endif // MCRL2_FDR_BOOLEAN_EXPRESSION_H
