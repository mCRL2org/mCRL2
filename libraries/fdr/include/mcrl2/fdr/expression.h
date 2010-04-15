// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_EXPRESSION_H
#define MCRL2_FDR_EXPRESSION_H

#include "mcrl2/fdr/expression_decl.h"
#include "mcrl2/fdr/expression_impl.h"

//--- start generated is-functions ---//

    /// \brief Test for a numb expression
    /// \param t A term
    /// \return True if it is a numb expression
    inline
    bool is_numb(const expression& t)
    {
      return fdr::detail::gsIsNumb(t);
    }

    /// \brief Test for a bool expression
    /// \param t A term
    /// \return True if it is a bool expression
    inline
    bool is_bool(const expression& t)
    {
      return fdr::detail::gsIsBool(t);
    }

    /// \brief Test for a set expression
    /// \param t A term
    /// \return True if it is a set expression
    inline
    bool is_set(const expression& t)
    {
      return fdr::detail::gsIsSet(t);
    }

    /// \brief Test for a seq expression
    /// \param t A term
    /// \return True if it is a seq expression
    inline
    bool is_seq(const expression& t)
    {
      return fdr::detail::gsIsSeq(t);
    }

    /// \brief Test for a tuple expression
    /// \param t A term
    /// \return True if it is a tuple expression
    inline
    bool is_tuple(const expression& t)
    {
      return fdr::detail::gsIsTuple(t);
    }

    /// \brief Test for a dotted expression
    /// \param t A term
    /// \return True if it is a dotted expression
    inline
    bool is_dotted(const expression& t)
    {
      return fdr::detail::gsIsDotted(t);
    }

    /// \brief Test for a lambda expression
    /// \param t A term
    /// \return True if it is a lambda expression
    inline
    bool is_lambda(const expression& t)
    {
      return fdr::detail::gsIsLambda(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_EXPRESSION_H
