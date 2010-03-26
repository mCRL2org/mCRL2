// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/vartype.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_VARTYPE_H
#define MCRL2_FDR_VARTYPE_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"

namespace mcrl2 {

namespace fdr {

  /// \brief Var type
  class vartype: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      vartype()
        : atermpp::aterm_appl(fdr::detail::constructVarType())
      {}

      /// \brief Constructor.
      /// \param term A term
      vartype(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(fdr::detail::check_rule_VarType(m_term));
      }
  };

//--- start generated classes ---//
/// \brief A simple branch
class simplebranch
{
  public:
    /// \brief Default constructor.
    simplebranch()
      : atermpp::aterm_appl(fdr::detail::constructSimpleBranch())
    {}

    /// \brief Constructor.
    /// \param term A term
    simplebranch(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_SimpleBranch(m_term));
    }

    /// \brief Constructor.
    simplebranch(const name& name)
      : atermpp::aterm_appl(fdr::detail::gsMakeSimpleBranch(name))
    {}

    name name() const
    {
      return atermpp::arg1(*this);
    }
};/// \brief list of simplebranchs
    typedef atermpp::term_list<simplebranch> simplebranch_list;

    /// \brief vector of simplebranchs
    typedef atermpp::vector<simplebranch>    simplebranch_vector;


/// \brief A branch
class branch
{
  public:
    /// \brief Default constructor.
    branch()
      : atermpp::aterm_appl(fdr::detail::constructBranch())
    {}

    /// \brief Constructor.
    /// \param term A term
    branch(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Branch(m_term));
    }

    /// \brief Constructor.
    branch(const name& name, const type& type)
      : atermpp::aterm_appl(fdr::detail::gsMakeBranch(name, type))
    {}

    name name() const
    {
      return atermpp::arg1(*this);
    }

    type type() const
    {
      return atermpp::arg2(*this);
    }
};/// \brief list of branchs
    typedef atermpp::term_list<branch> branch_list;

    /// \brief vector of branchs
    typedef atermpp::vector<branch>    branch_vector;

//--- end generated classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a simplebranch expression
    /// \param t A term
    /// \return True if it is a simplebranch expression
    inline
    bool is_simplebranch(const vartype& t)
    {
      return fdr::detail::gsIsSimpleBranch(t);
    }

    /// \brief Test for a branch expression
    /// \param t A term
    /// \return True if it is a branch expression
    inline
    bool is_branch(const vartype& t)
    {
      return fdr::detail::gsIsBranch(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_VARTYPE_H
