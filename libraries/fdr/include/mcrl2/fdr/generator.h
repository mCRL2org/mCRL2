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

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"
#include "mcrl2/fdr/detail/syntax_checks.h"

namespace mcrl2 {

namespace fdr {

//--- start generated classes ---//
/// \brief A boolean
class bgen: atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    bgen()
      : atermpp::aterm_appl(fdr::detail::constructBGen())
    {}

    /// \brief Constructor.
    /// \param term A term
    bgen(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_BGen(m_term));
    }

    /// \brief Constructor.
    bgen(const boolean_expression& operand)
      : atermpp::aterm_appl(fdr::detail::gsMakeBGen(operand))
    {}

    boolean_expression operand() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief list of bgens
typedef atermpp::term_list<bgen> bgen_list;

/// \brief vector of bgens
typedef atermpp::vector<bgen>    bgen_vector;


/// \brief A generator
class gen: atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    gen()
      : atermpp::aterm_appl(fdr::detail::constructGen())
    {}

    /// \brief Constructor.
    /// \param term A term
    gen(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Gen(m_term));
    }

    /// \brief Constructor.
    gen(const expression& left, const expression& right)
      : atermpp::aterm_appl(fdr::detail::gsMakeGen(left, right))
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

/// \brief list of gens
typedef atermpp::term_list<gen> gen_list;

/// \brief vector of gens
typedef atermpp::vector<gen>    gen_vector;

//--- end generated classes ---//

//--- start generated is-functions ---//
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_GENERATOR_H
