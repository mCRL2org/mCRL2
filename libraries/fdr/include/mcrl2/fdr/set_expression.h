// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/set_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_SET_EXPRESSION_H
#define MCRL2_FDR_SET_EXPRESSION_H

#include "mcrl2/fdr/set_expression_decl.h"
#include "mcrl2/fdr/set_expression_impl.h"
                                      
namespace mcrl2 {

namespace fdr {

//--- start generated is-functions ---//

    /// \brief Test for a chanset expression
    /// \param t A term
    /// \return True if it is a chanset expression
    inline
    bool is_chanset(const set_expression& t)
    {
      return fdr::detail::gsIsChanSet(t);
    }

    /// \brief Test for a union expression
    /// \param t A term
    /// \return True if it is a union expression
    inline
    bool is_union(const set_expression& t)
    {
      return fdr::detail::gsIsunion(t);
    }

    /// \brief Test for a inter expression
    /// \param t A term
    /// \return True if it is a inter expression
    inline
    bool is_inter(const set_expression& t)
    {
      return fdr::detail::gsIsinter(t);
    }

    /// \brief Test for a diff expression
    /// \param t A term
    /// \return True if it is a diff expression
    inline
    bool is_diff(const set_expression& t)
    {
      return fdr::detail::gsIsdiff(t);
    }

    /// \brief Test for a union expression
    /// \param t A term
    /// \return True if it is a union expression
    inline
    bool is_union(const set_expression& t)
    {
      return fdr::detail::gsIsUnion(t);
    }

    /// \brief Test for a inter expression
    /// \param t A term
    /// \return True if it is a inter expression
    inline
    bool is_inter(const set_expression& t)
    {
      return fdr::detail::gsIsInter(t);
    }

    /// \brief Test for a set expression
    /// \param t A term
    /// \return True if it is a set expression
    inline
    bool is_set(const set_expression& t)
    {
      return fdr::detail::gsIsset(t);
    }

    /// \brief Test for a set expression
    /// \param t A term
    /// \return True if it is a set expression
    inline
    bool is_set(const set_expression& t)
    {
      return fdr::detail::gsIsSet(t);
    }

    /// \brief Test for a seq expression
    /// \param t A term
    /// \return True if it is a seq expression
    inline
    bool is_seq(const set_expression& t)
    {
      return fdr::detail::gsIsSeq(t);
    }

    /// \brief Test for a extensions expression
    /// \param t A term
    /// \return True if it is a extensions expression
    inline
    bool is_extensions(const set_expression& t)
    {
      return fdr::detail::gsIsextensions(t);
    }

    /// \brief Test for a productions expression
    /// \param t A term
    /// \return True if it is a productions expression
    inline
    bool is_productions(const set_expression& t)
    {
      return fdr::detail::gsIsproductions(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_SET_EXPRESSION_H
