// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/generator.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_GENERATOR_H
#define MCRL2_FDR_GENERATOR_H

#include "mcrl2/fdr/term_include_files.h"
#include "mcrl2/fdr/expression_fwd.h"
#include "mcrl2/fdr/seq_expression_fwd.h"
#include "mcrl2/fdr/set_expression_fwd.h"

namespace mcrl2 {

namespace fdr {

//--- start generated classes ---//
/// \brief A set generator
class setgen: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    setgen()
      : atermpp::aterm_appl(fdr::detail::constructSetGen())
    {}

    /// \brief Constructor.
    /// \param term A term
    setgen(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_SetGen(m_term));
    }

    /// \brief Constructor.
    setgen(const expression& expr, const set_expression& set)
      : atermpp::aterm_appl(fdr::detail::gsMakeSetGen(expr, set))
    {}

    expression expr() const
    {
      return atermpp::arg1(*this);
    }

    set_expression set() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief list of setgens
typedef atermpp::term_list<setgen> setgen_list;

/// \brief vector of setgens
typedef atermpp::vector<setgen>    setgen_vector;


/// \brief A seq generator
class seqgen: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    seqgen()
      : atermpp::aterm_appl(fdr::detail::constructSeqGen())
    {}

    /// \brief Constructor.
    /// \param term A term
    seqgen(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_SeqGen(m_term));
    }

    /// \brief Constructor.
    seqgen(const expression& expr, const seq_expression& seq)
      : atermpp::aterm_appl(fdr::detail::gsMakeSeqGen(expr, seq))
    {}

    expression expr() const
    {
      return atermpp::arg1(*this);
    }

    seq_expression seq() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief list of seqgens
typedef atermpp::term_list<seqgen> seqgen_list;

/// \brief vector of seqgens
typedef atermpp::vector<seqgen>    seqgen_vector;

//--- end generated classes ---//

//--- start generated is-functions ---//
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_GENERATOR_H
