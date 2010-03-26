// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/trname.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_TRNAME_H
#define MCRL2_FDR_TRNAME_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"
#include "mcrl2/fdr/detail/syntax_checks.h"

namespace mcrl2 {

namespace fdr {

//--- start generated classes ---//
/// \brief A normal
class normal
{
  public:
    /// \brief Default constructor.
    normal()
      : atermpp::aterm_appl(fdr::detail::constructnormal())
    {}

    /// \brief Constructor.
    /// \param term A term
    normal(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_normal(m_term));
    }
};/// \brief list of normals
    typedef atermpp::term_list<normal> normal_list;

    /// \brief vector of normals
    typedef atermpp::vector<normal>    normal_vector;


/// \brief A normal
class normalise
{
  public:
    /// \brief Default constructor.
    normalise()
      : atermpp::aterm_appl(fdr::detail::constructnormalise())
    {}

    /// \brief Constructor.
    /// \param term A term
    normalise(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_normalise(m_term));
    }
};/// \brief list of normalises
    typedef atermpp::term_list<normalise> normalise_list;

    /// \brief vector of normalises
    typedef atermpp::vector<normalise>    normalise_vector;


/// \brief A normal
class normalize
{
  public:
    /// \brief Default constructor.
    normalize()
      : atermpp::aterm_appl(fdr::detail::constructnormalize())
    {}

    /// \brief Constructor.
    /// \param term A term
    normalize(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_normalize(m_term));
    }
};/// \brief list of normalizes
    typedef atermpp::term_list<normalize> normalize_list;

    /// \brief vector of normalizes
    typedef atermpp::vector<normalize>    normalize_vector;


/// \brief A sbsim
class sbsim
{
  public:
    /// \brief Default constructor.
    sbsim()
      : atermpp::aterm_appl(fdr::detail::constructsbsim())
    {}

    /// \brief Constructor.
    /// \param term A term
    sbsim(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_sbsim(m_term));
    }
};/// \brief list of sbsims
    typedef atermpp::term_list<sbsim> sbsim_list;

    /// \brief vector of sbsims
    typedef atermpp::vector<sbsim>    sbsim_vector;


/// \brief A tau_loop_factor
class tau_loop_factor
{
  public:
    /// \brief Default constructor.
    tau_loop_factor()
      : atermpp::aterm_appl(fdr::detail::constructtau_loop_factor())
    {}

    /// \brief Constructor.
    /// \param term A term
    tau_loop_factor(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_tau_loop_factor(m_term));
    }
};/// \brief list of tau_loop_factors
    typedef atermpp::term_list<tau_loop_factor> tau_loop_factor_list;

    /// \brief vector of tau_loop_factors
    typedef atermpp::vector<tau_loop_factor>    tau_loop_factor_vector;


/// \brief A diamond
class diamond
{
  public:
    /// \brief Default constructor.
    diamond()
      : atermpp::aterm_appl(fdr::detail::constructdiamond())
    {}

    /// \brief Constructor.
    /// \param term A term
    diamond(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_diamond(m_term));
    }
};/// \brief list of diamonds
    typedef atermpp::term_list<diamond> diamond_list;

    /// \brief vector of diamonds
    typedef atermpp::vector<diamond>    diamond_vector;


/// \brief A model compress
class model_compress
{
  public:
    /// \brief Default constructor.
    model_compress()
      : atermpp::aterm_appl(fdr::detail::constructmodel_compress())
    {}

    /// \brief Constructor.
    /// \param term A term
    model_compress(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_model_compress(m_term));
    }
};/// \brief list of model_compresss
    typedef atermpp::term_list<model_compress> model_compress_list;

    /// \brief vector of model_compresss
    typedef atermpp::vector<model_compress>    model_compress_vector;


/// \brief An explicate
class explicate
{
  public:
    /// \brief Default constructor.
    explicate()
      : atermpp::aterm_appl(fdr::detail::constructexplicate())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicate(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_explicate(m_term));
    }
};/// \brief list of explicates
    typedef atermpp::term_list<explicate> explicate_list;

    /// \brief vector of explicates
    typedef atermpp::vector<explicate>    explicate_vector;

//--- end generated classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a normal expression
    /// \param t A term
    /// \return True if it is a normal expression
    inline
    bool is_normal(const trname& t)
    {
      return fdr::detail::gsIsnormal(t);
    }

    /// \brief Test for a normalise expression
    /// \param t A term
    /// \return True if it is a normalise expression
    inline
    bool is_normalise(const trname& t)
    {
      return fdr::detail::gsIsnormalise(t);
    }

    /// \brief Test for a normalize expression
    /// \param t A term
    /// \return True if it is a normalize expression
    inline
    bool is_normalize(const trname& t)
    {
      return fdr::detail::gsIsnormalize(t);
    }

    /// \brief Test for a sbsim expression
    /// \param t A term
    /// \return True if it is a sbsim expression
    inline
    bool is_sbsim(const trname& t)
    {
      return fdr::detail::gsIssbsim(t);
    }

    /// \brief Test for a tau_loop_factor expression
    /// \param t A term
    /// \return True if it is a tau_loop_factor expression
    inline
    bool is_tau_loop_factor(const trname& t)
    {
      return fdr::detail::gsIstau_loop_factor(t);
    }

    /// \brief Test for a diamond expression
    /// \param t A term
    /// \return True if it is a diamond expression
    inline
    bool is_diamond(const trname& t)
    {
      return fdr::detail::gsIsdiamond(t);
    }

    /// \brief Test for a model_compress expression
    /// \param t A term
    /// \return True if it is a model_compress expression
    inline
    bool is_model_compress(const trname& t)
    {
      return fdr::detail::gsIsmodel_compress(t);
    }

    /// \brief Test for a explicate expression
    /// \param t A term
    /// \return True if it is a explicate expression
    inline
    bool is_explicate(const trname& t)
    {
      return fdr::detail::gsIsexplicate(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_TRNAME_H
