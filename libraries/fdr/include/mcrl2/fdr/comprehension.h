// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/comprehension.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_COMPREHENSION_H
#define MCRL2_FDR_COMPREHENSION_H

#include "mcrl2/fdr/term_include_files.h"

namespace mcrl2 {

namespace fdr {

//--- start generated classes ---//
/// \brief An empty
class nil: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    nil()
      : atermpp::aterm_appl(fdr::detail::constructNil())
    {}

    /// \brief Constructor.
    /// \param term A term
    nil(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Nil(m_term));
    }
};

/// \brief list of nils
typedef atermpp::term_list<nil> nil_list;

/// \brief vector of nils
typedef atermpp::vector<nil>    nil_vector;


/// \brief A boolean
class bcomprehension: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    bcomprehension()
      : atermpp::aterm_appl(fdr::detail::constructBComprehension())
    {}

    /// \brief Constructor.
    /// \param term A term
    bcomprehension(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_BComprehension(m_term));
    }

    /// \brief Constructor.
    bcomprehension(const boolean_expression& operand)
      : atermpp::aterm_appl(fdr::detail::gsMakeBComprehension(operand))
    {}

    boolean_expression operand() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief list of bcomprehensions
typedef atermpp::term_list<bcomprehension> bcomprehension_list;

/// \brief vector of bcomprehensions
typedef atermpp::vector<bcomprehension>    bcomprehension_vector;


/// \brief A comprehension
class ecomprehension: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    ecomprehension()
      : atermpp::aterm_appl(fdr::detail::constructEComprehension())
    {}

    /// \brief Constructor.
    /// \param term A term
    ecomprehension(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_EComprehension(m_term));
    }

    /// \brief Constructor.
    ecomprehension(const expression& left, const expression& right)
      : atermpp::aterm_appl(fdr::detail::gsMakeEComprehension(left, right))
    {}

    expression left() const
    {
      return atermpp::arg1(*this);
    }

    expression right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief list of ecomprehensions
typedef atermpp::term_list<ecomprehension> ecomprehension_list;

/// \brief vector of ecomprehensions
typedef atermpp::vector<ecomprehension>    ecomprehension_vector;

//--- end generated classes ---//

//--- start generated is-functions ---//
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_COMPREHENSION_H
