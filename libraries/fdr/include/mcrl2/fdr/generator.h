// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/generator.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_GENERATOR_H
#define MCRL2_FDR_GENERATOR_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"

namespace mcrl2 {

namespace fdr {

  /// \brief Generator
  class generator: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      generator()
        : atermpp::aterm_appl(fdr::detail::constructGen())
      {}

      /// \brief Constructor.
      /// \param term A term
      generator(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(fdr::detail::check_rule_Gen(m_term));
      }
  };

//--- start generated classes ---//
/// \brief A boolean
class bool_
{
  public:
    /// \brief Default constructor.
    bool_()
      : atermpp::aterm_appl(fdr::detail::constructBool())
    {}

    /// \brief Constructor.
    /// \param term A term
    bool_(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Bool(m_term));
    }

    /// \brief Constructor.
    bool_(const boolean_expression& operand)
      : atermpp::aterm_appl(fdr::detail::gsMakeBool(operand))
    {}

    boolean_expression operand() const
    {
      return atermpp::arg1(*this);
    }
};/// \brief list of bool_s
    typedef atermpp::term_list<bool_> bool__list;

    /// \brief vector of bool_s
    typedef atermpp::vector<bool_>    bool__vector;


/// \brief A generator
class gen
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
};/// \brief list of gens
    typedef atermpp::term_list<gen> gen_list;

    /// \brief vector of gens
    typedef atermpp::vector<gen>    gen_vector;

//--- end generated classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a bool expression
    /// \param t A term
    /// \return True if it is a bool expression
    inline
    bool is_bool(const generator& t)
    {
      return fdr::detail::gsIsBool(t);
    }

    /// \brief Test for a gen expression
    /// \param t A term
    /// \return True if it is a gen expression
    inline
    bool is_gen(const generator& t)
    {
      return fdr::detail::gsIsGen(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_GENERATOR_H
