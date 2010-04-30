// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/renaming_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_RENAMING_EXPRESSION_H
#define MCRL2_FDR_RENAMING_EXPRESSION_H

#include "mcrl2/fdr/term_include_files.h"
#include "mcrl2/fdr/comprehension_expression_fwd.h"
#include "mcrl2/fdr/map.h"

namespace mcrl2 {

namespace fdr {

//--- start generated renaming expression class declarations ---//
/// \brief class renaming_expression
class renaming_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    renaming_expression()
      : atermpp::aterm_appl(fdr::detail::constructRenaming())
    {}

    /// \brief Constructor.
    /// \param term A term
    renaming_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_rule_Renaming(m_term));
    }
};

/// \brief list of renaming_expressions
typedef atermpp::term_list<renaming_expression> renaming_expression_list;

/// \brief vector of renaming_expressions
typedef atermpp::vector<renaming_expression>    renaming_expression_vector;

/// \brief A map list
class maps: public renaming_expression
{
  public:
    /// \brief Default constructor.
    maps();

    /// \brief Constructor.
    /// \param term A term
    maps(atermpp::aterm_appl term);

    /// \brief Constructor.
    maps(const map_list& renamings);

    map_list renamings() const;
};

/// \brief A map/generator list
class mapsgens: public renaming_expression
{
  public:
    /// \brief Default constructor.
    mapsgens();

    /// \brief Constructor.
    /// \param term A term
    mapsgens(atermpp::aterm_appl term);

    /// \brief Constructor.
    mapsgens(const map_list& renamings, const comprehension_expression_list& comprs);

    map_list renamings() const;

    comprehension_expression_list comprs() const;
};
//--- end generated renaming expression class declarations ---//

//--- start generated is-functions ---//

    /// \brief Test for a maps expression
    /// \param t A term
    /// \return True if it is a maps expression
    inline
    bool is_maps(const renaming_expression& t)
    {
      return fdr::detail::gsIsMaps(t);
    }

    /// \brief Test for a mapsgens expression
    /// \param t A term
    /// \return True if it is a mapsgens expression
    inline
    bool is_mapsgens(const renaming_expression& t)
    {
      return fdr::detail::gsIsMapsGens(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_RENAMING_EXPRESSION_H
