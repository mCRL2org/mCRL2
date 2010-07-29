// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/linkpar_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_LINKPAR_EXPRESSION_H
#define MCRL2_FDR_LINKPAR_EXPRESSION_H

#include "mcrl2/fdr/term_include_files.h"
#include "mcrl2/fdr/comprehension_expression_fwd.h"
#include "mcrl2/fdr/link.h"

namespace mcrl2 {

namespace fdr {

//--- start generated linkpar expression class declarations ---//
/// \brief class linkpar_expression
class linkpar_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    linkpar_expression()
      : atermpp::aterm_appl(fdr::detail::constructLinkPar())
    {}

    /// \brief Constructor.
    /// \param term A term
    linkpar_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_rule_LinkPar(m_term));
    }
};

/// \brief list of linkpar_expressions
typedef atermpp::term_list<linkpar_expression> linkpar_expression_list;

/// \brief vector of linkpar_expressions
typedef atermpp::vector<linkpar_expression>    linkpar_expression_vector;

/// \brief A link list
class links: public linkpar_expression
{
  public:
    /// \brief Default constructor.
    links();

    /// \brief Constructor.
    /// \param term A term
    links(atermpp::aterm_appl term);

    /// \brief Constructor.
    links(const link_list& linkpars);

    link_list linkpars() const;
};

/// \brief A link/generator list
class linksgens: public linkpar_expression
{
  public:
    /// \brief Default constructor.
    linksgens();

    /// \brief Constructor.
    /// \param term A term
    linksgens(atermpp::aterm_appl term);

    /// \brief Constructor.
    linksgens(const link_list& linkpars, const comprehension_expression_list& comprs);

    link_list linkpars() const;

    comprehension_expression_list comprs() const;
};
//--- end generated linkpar expression class declarations ---//

//--- start generated is-functions ---//

    /// \brief Test for a links expression
    /// \param t A term
    /// \return True if it is a links expression
    inline
    bool is_links(const linkpar_expression& t)
    {
      return fdr::detail::gsIsLinks(t);
    }

    /// \brief Test for a linksgens expression
    /// \param t A term
    /// \return True if it is a linksgens expression
    inline
    bool is_linksgens(const linkpar_expression& t)
    {
      return fdr::detail::gsIsLinksGens(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_LINKPAR_EXPRESSION_H
