// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/type_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_TYPE_EXPRESSION_H
#define MCRL2_FDR_TYPE_EXPRESSION_H

#include "mcrl2/fdr/term_include_files.h"
#include "mcrl2/fdr/name.h"
#include "mcrl2/fdr/set_expression_fwd.h"

namespace mcrl2 {

namespace fdr {

//--- start generated type expression class declarations ---//
/// \brief class type_expression
class type_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    type_expression()
      : atermpp::aterm_appl(fdr::detail::constructType())
    {}

    /// \brief Constructor.
    /// \param term A term
    type_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_rule_Type(m_term));
    }
};

/// \brief list of type_expressions
typedef atermpp::term_list<type_expression> type_expression_list;

/// \brief vector of type_expressions
typedef atermpp::vector<type_expression>    type_expression_vector;

/// \brief A type product
class typeproduct: public type_expression
{
  public:
    /// \brief Default constructor.
    typeproduct();

    /// \brief Constructor.
    /// \param term A term
    typeproduct(atermpp::aterm_appl term);

    /// \brief Constructor.
    typeproduct(const type_expression& left, const type_expression& right);

    type_expression left() const;

    type_expression right() const;
};

/// \brief A type tuple
class typetuple: public type_expression
{
  public:
    /// \brief Default constructor.
    typetuple();

    /// \brief Constructor.
    /// \param term A term
    typetuple(atermpp::aterm_appl term);

    /// \brief Constructor.
    typetuple(const type_expression_list& types);

    type_expression_list types() const;
};

/// \brief A type set
class typeset: public type_expression
{
  public:
    /// \brief Default constructor.
    typeset();

    /// \brief Constructor.
    /// \param term A term
    typeset(atermpp::aterm_appl term);

    /// \brief Constructor.
    typeset(const set_expression& set);

    set_expression set() const;
};

/// \brief A simple type name
class simpletypename: public type_expression
{
  public:
    /// \brief Default constructor.
    simpletypename();

    /// \brief Constructor.
    /// \param term A term
    simpletypename(atermpp::aterm_appl term);

    /// \brief Constructor.
    simpletypename(const name& id);

    name id() const;
};

/// \brief A type name
class typename_: public type_expression
{
  public:
    /// \brief Default constructor.
    typename_();

    /// \brief Constructor.
    /// \param term A term
    typename_(atermpp::aterm_appl term);

    /// \brief Constructor.
    typename_(const name& id, const type_expression& type_name);

    name id() const;

    type_expression type_name() const;
};
//--- end generated type expression class declarations ---//

//--- start generated is-functions ---//

    /// \brief Test for a typeproduct expression
    /// \param t A term
    /// \return True if it is a typeproduct expression
    inline
    bool is_typeproduct(const type_expression& t)
    {
      return fdr::detail::gsIsTypeProduct(t);
    }

    /// \brief Test for a typetuple expression
    /// \param t A term
    /// \return True if it is a typetuple expression
    inline
    bool is_typetuple(const type_expression& t)
    {
      return fdr::detail::gsIsTypeTuple(t);
    }

    /// \brief Test for a typeset expression
    /// \param t A term
    /// \return True if it is a typeset expression
    inline
    bool is_typeset(const type_expression& t)
    {
      return fdr::detail::gsIsTypeSet(t);
    }

    /// \brief Test for a simpletypename expression
    /// \param t A term
    /// \return True if it is a simpletypename expression
    inline
    bool is_simpletypename(const type_expression& t)
    {
      return fdr::detail::gsIsSimpleTypeName(t);
    }

    /// \brief Test for a typename expression
    /// \param t A term
    /// \return True if it is a typename expression
    inline
    bool is_typename(const type_expression& t)
    {
      return fdr::detail::gsIsTypeName(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_TYPE_EXPRESSION_H
