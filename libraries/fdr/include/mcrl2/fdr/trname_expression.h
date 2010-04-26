// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/trname_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_TRNAME_EXPRESSION_H
#define MCRL2_FDR_TRNAME_EXPRESSION_H

#include "mcrl2/fdr/term_include_files.h"

namespace mcrl2 {

namespace fdr {

//--- start generated trname expression class declarations ---//
/// \brief class trname_expression
class trname_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    trname_expression()
      : atermpp::aterm_appl(fdr::detail::constructTrName())
    {}

    /// \brief Constructor.
    /// \param term A term
    trname_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_rule_TrName(m_term));
    }
};

/// \brief list of trname_expressions
typedef atermpp::term_list<trname_expression> trname_expression_list;

/// \brief vector of trname_expressions
typedef atermpp::vector<trname_expression>    trname_expression_vector;

/// \brief A normal
class normal: public trname_expression
{
  public:
    /// \brief Default constructor.
    normal();

    /// \brief Constructor.
    /// \param term A term
    normal(atermpp::aterm_appl term);
};

/// \brief A normal
class normalise: public trname_expression
{
  public:
    /// \brief Default constructor.
    normalise();

    /// \brief Constructor.
    /// \param term A term
    normalise(atermpp::aterm_appl term);
};

/// \brief A normal
class normalize: public trname_expression
{
  public:
    /// \brief Default constructor.
    normalize();

    /// \brief Constructor.
    /// \param term A term
    normalize(atermpp::aterm_appl term);
};

/// \brief A sbsim
class sbsim: public trname_expression
{
  public:
    /// \brief Default constructor.
    sbsim();

    /// \brief Constructor.
    /// \param term A term
    sbsim(atermpp::aterm_appl term);
};

/// \brief A tau_loop_factor
class tau_loop_factor: public trname_expression
{
  public:
    /// \brief Default constructor.
    tau_loop_factor();

    /// \brief Constructor.
    /// \param term A term
    tau_loop_factor(atermpp::aterm_appl term);
};

/// \brief A diamond
class diamond: public trname_expression
{
  public:
    /// \brief Default constructor.
    diamond();

    /// \brief Constructor.
    /// \param term A term
    diamond(atermpp::aterm_appl term);
};

/// \brief A model compress
class model_compress: public trname_expression
{
  public:
    /// \brief Default constructor.
    model_compress();

    /// \brief Constructor.
    /// \param term A term
    model_compress(atermpp::aterm_appl term);
};

/// \brief An explicate
class explicate: public trname_expression
{
  public:
    /// \brief Default constructor.
    explicate();

    /// \brief Constructor.
    /// \param term A term
    explicate(atermpp::aterm_appl term);
};
//--- end generated trname expression class declarations ---//

//--- start generated is-functions ---//

    /// \brief Test for a normal expression
    /// \param t A term
    /// \return True if it is a normal expression
    inline
    bool is_normal(const trname_expression& t)
    {
      return fdr::detail::gsIsnormal(t);
    }

    /// \brief Test for a normalise expression
    /// \param t A term
    /// \return True if it is a normalise expression
    inline
    bool is_normalise(const trname_expression& t)
    {
      return fdr::detail::gsIsnormalise(t);
    }

    /// \brief Test for a normalize expression
    /// \param t A term
    /// \return True if it is a normalize expression
    inline
    bool is_normalize(const trname_expression& t)
    {
      return fdr::detail::gsIsnormalize(t);
    }

    /// \brief Test for a sbsim expression
    /// \param t A term
    /// \return True if it is a sbsim expression
    inline
    bool is_sbsim(const trname_expression& t)
    {
      return fdr::detail::gsIssbsim(t);
    }

    /// \brief Test for a tau_loop_factor expression
    /// \param t A term
    /// \return True if it is a tau_loop_factor expression
    inline
    bool is_tau_loop_factor(const trname_expression& t)
    {
      return fdr::detail::gsIstau_loop_factor(t);
    }

    /// \brief Test for a diamond expression
    /// \param t A term
    /// \return True if it is a diamond expression
    inline
    bool is_diamond(const trname_expression& t)
    {
      return fdr::detail::gsIsdiamond(t);
    }

    /// \brief Test for a model_compress expression
    /// \param t A term
    /// \return True if it is a model_compress expression
    inline
    bool is_model_compress(const trname_expression& t)
    {
      return fdr::detail::gsIsmodel_compress(t);
    }

    /// \brief Test for a explicate expression
    /// \param t A term
    /// \return True if it is a explicate expression
    inline
    bool is_explicate(const trname_expression& t)
    {
      return fdr::detail::gsIsexplicate(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_TRNAME_EXPRESSION_H
