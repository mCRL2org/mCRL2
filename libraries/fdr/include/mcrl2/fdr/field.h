// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/field.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_FIELD_H
#define MCRL2_FDR_FIELD_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"
#include "mcrl2/fdr/detail/syntax_checks.h"

namespace mcrl2 {

namespace fdr {

//--- start generated classes ---//
/// \brief A simple input
class simpleinput: atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    simpleinput()
      : atermpp::aterm_appl(fdr::detail::constructSimpleInput())
    {}

    /// \brief Constructor.
    /// \param term A term
    simpleinput(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_SimpleInput(m_term));
    }

    /// \brief Constructor.
    simpleinput(const expression& expr)
      : atermpp::aterm_appl(fdr::detail::gsMakeSimpleInput(expr))
    {}

    expression expr() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief list of simpleinputs
typedef atermpp::term_list<simpleinput> simpleinput_list;

/// \brief vector of simpleinputs
typedef atermpp::vector<simpleinput>    simpleinput_vector;


/// \brief An input
class input: atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    input()
      : atermpp::aterm_appl(fdr::detail::constructInput())
    {}

    /// \brief Constructor.
    /// \param term A term
    input(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Input(m_term));
    }

    /// \brief Constructor.
    input(const expression& expr, const set_expression& restriction)
      : atermpp::aterm_appl(fdr::detail::gsMakeInput(expr, restriction))
    {}

    expression expr() const
    {
      return atermpp::arg1(*this);
    }

    set_expression restriction() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief list of inputs
typedef atermpp::term_list<input> input_list;

/// \brief vector of inputs
typedef atermpp::vector<input>    input_vector;


/// \brief An output
class output: atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    output()
      : atermpp::aterm_appl(fdr::detail::constructOutput())
    {}

    /// \brief Constructor.
    /// \param term A term
    output(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Output(m_term));
    }

    /// \brief Constructor.
    output(const expression& expr)
      : atermpp::aterm_appl(fdr::detail::gsMakeOutput(expr))
    {}

    expression expr() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief list of outputs
typedef atermpp::term_list<output> output_list;

/// \brief vector of outputs
typedef atermpp::vector<output>    output_vector;

//--- end generated classes ---//

//--- start generated is-functions ---//
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_FIELD_H
