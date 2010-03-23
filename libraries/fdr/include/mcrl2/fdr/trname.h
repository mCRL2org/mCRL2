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

namespace mcrl2 {

namespace fdr {

  /// \brief Transparent name
  class trname: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      trname()
        : atermpp::aterm_appl(fdr::detail::constructTrName())
      {}

      /// \brief Constructor.
      /// \param term A term
      trname(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(fdr::detail::check_rule_TrName(m_term));
      }
  };

//--- start generated classes ---//
/// \brief A normal
class normal: public trname
{
  public:
    /// \brief Default constructor.
    normal()
      : trname(fdr::detail::constructnormal())
    {}

    /// \brief Constructor.
    /// \param term A term
    normal(atermpp::aterm_appl term)
      : trname(term)
    {
      assert(fdr::detail::check_term_normal(m_term));
    }
};

/// \brief A normal
class normalise: public trname
{
  public:
    /// \brief Default constructor.
    normalise()
      : trname(fdr::detail::constructnormalise())
    {}

    /// \brief Constructor.
    /// \param term A term
    normalise(atermpp::aterm_appl term)
      : trname(term)
    {
      assert(fdr::detail::check_term_normalise(m_term));
    }
};

/// \brief A normal
class normalize: public trname
{
  public:
    /// \brief Default constructor.
    normalize()
      : trname(fdr::detail::constructnormalize())
    {}

    /// \brief Constructor.
    /// \param term A term
    normalize(atermpp::aterm_appl term)
      : trname(term)
    {
      assert(fdr::detail::check_term_normalize(m_term));
    }
};

/// \brief A sbsim
class sbsim: public trname
{
  public:
    /// \brief Default constructor.
    sbsim()
      : trname(fdr::detail::constructsbsim())
    {}

    /// \brief Constructor.
    /// \param term A term
    sbsim(atermpp::aterm_appl term)
      : trname(term)
    {
      assert(fdr::detail::check_term_sbsim(m_term));
    }
};

/// \brief A tau_loop_factor
class tau_loop_factor: public trname
{
  public:
    /// \brief Default constructor.
    tau_loop_factor()
      : trname(fdr::detail::constructtau_loop_factor())
    {}

    /// \brief Constructor.
    /// \param term A term
    tau_loop_factor(atermpp::aterm_appl term)
      : trname(term)
    {
      assert(fdr::detail::check_term_tau_loop_factor(m_term));
    }
};

/// \brief A diamond
class diamond: public trname
{
  public:
    /// \brief Default constructor.
    diamond()
      : trname(fdr::detail::constructdiamond())
    {}

    /// \brief Constructor.
    /// \param term A term
    diamond(atermpp::aterm_appl term)
      : trname(term)
    {
      assert(fdr::detail::check_term_diamond(m_term));
    }
};

/// \brief A model compress
class model_compress: public trname
{
  public:
    /// \brief Default constructor.
    model_compress()
      : trname(fdr::detail::constructmodel_compress())
    {}

    /// \brief Constructor.
    /// \param term A term
    model_compress(atermpp::aterm_appl term)
      : trname(term)
    {
      assert(fdr::detail::check_term_model_compress(m_term));
    }
};

/// \brief An explicate
class explicate: public trname
{
  public:
    /// \brief Default constructor.
    explicate()
      : trname(fdr::detail::constructexplicate())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicate(atermpp::aterm_appl term)
      : trname(term)
    {
      assert(fdr::detail::check_term_explicate(m_term));
    }
};
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
