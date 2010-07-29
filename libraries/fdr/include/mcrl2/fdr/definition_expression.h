// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/definition_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_DEFINITION_EXPRESSION_H
#define MCRL2_FDR_DEFINITION_EXPRESSION_H

#include "mcrl2/fdr/term_include_files.h"
#include "mcrl2/fdr/filename.h"
#include "mcrl2/fdr/name.h"
#include "mcrl2/fdr/any_expression_fwd.h"
#include "mcrl2/fdr/check_expression_fwd.h"
#include "mcrl2/fdr/expression_fwd.h"
#include "mcrl2/fdr/trname_expression_fwd.h"
#include "mcrl2/fdr/type_expression_fwd.h"
#include "mcrl2/fdr/vartype_expression_fwd.h"

namespace mcrl2 {

namespace fdr {

//--- start generated definition expression class declarations ---//
/// \brief class definition_expression
class definition_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    definition_expression()
      : atermpp::aterm_appl(fdr::detail::constructDefn())
    {}

    /// \brief Constructor.
    /// \param term A term
    definition_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_rule_Defn(m_term));
    }
};

/// \brief list of definition_expressions
typedef atermpp::term_list<definition_expression> definition_expression_list;

/// \brief vector of definition_expressions
typedef atermpp::vector<definition_expression>    definition_expression_vector;

/// \brief An assignment
class assign: public definition_expression
{
  public:
    /// \brief Default constructor.
    assign();

    /// \brief Constructor.
    /// \param term A term
    assign(atermpp::aterm_appl term);

    /// \brief Constructor.
    assign(const any_expression& left, const any_expression& right);

    any_expression left() const;

    any_expression right() const;
};

/// \brief A channel
class channel: public definition_expression
{
  public:
    /// \brief Default constructor.
    channel();

    /// \brief Constructor.
    /// \param term A term
    channel(atermpp::aterm_appl term);

    /// \brief Constructor.
    channel(const name_list& names, const type_expression& type_name);

    name_list names() const;

    type_expression type_name() const;
};

/// \brief A simple channel
class simple_channel: public definition_expression
{
  public:
    /// \brief Default constructor.
    simple_channel();

    /// \brief Constructor.
    /// \param term A term
    simple_channel(atermpp::aterm_appl term);

    /// \brief Constructor.
    simple_channel(const name_list& names);

    name_list names() const;
};

/// \brief A nametype
class nametype: public definition_expression
{
  public:
    /// \brief Default constructor.
    nametype();

    /// \brief Constructor.
    /// \param term A term
    nametype(atermpp::aterm_appl term);

    /// \brief Constructor.
    nametype(const name& id, const type_expression& type_name);

    name id() const;

    type_expression type_name() const;
};

/// \brief A datatype
class datatype: public definition_expression
{
  public:
    /// \brief Default constructor.
    datatype();

    /// \brief Constructor.
    /// \param term A term
    datatype(atermpp::aterm_appl term);

    /// \brief Constructor.
    datatype(const name& id, const vartype_expression_list& vartypes);

    name id() const;

    vartype_expression_list vartypes() const;
};

/// \brief A subtype
class subtype: public definition_expression
{
  public:
    /// \brief Default constructor.
    subtype();

    /// \brief Constructor.
    /// \param term A term
    subtype(atermpp::aterm_appl term);

    /// \brief Constructor.
    subtype(const name& id, const vartype_expression_list& vartypes);

    name id() const;

    vartype_expression_list vartypes() const;
};

/// \brief An external
class external: public definition_expression
{
  public:
    /// \brief Default constructor.
    external();

    /// \brief Constructor.
    /// \param term A term
    external(atermpp::aterm_appl term);

    /// \brief Constructor.
    external(const name_list& names);

    name_list names() const;
};

/// \brief A transparent
class transparent: public definition_expression
{
  public:
    /// \brief Default constructor.
    transparent();

    /// \brief Constructor.
    /// \param term A term
    transparent(atermpp::aterm_appl term);

    /// \brief Constructor.
    transparent(const trname_expression_list& trnames);

    trname_expression_list trnames() const;
};

/// \brief An assertion
class assert_: public definition_expression
{
  public:
    /// \brief Default constructor.
    assert_();

    /// \brief Constructor.
    /// \param term A term
    assert_(atermpp::aterm_appl term);

    /// \brief Constructor.
    assert_(const check_expression& chk);

    check_expression chk() const;
};

/// \brief A print
class print: public definition_expression
{
  public:
    /// \brief Default constructor.
    print();

    /// \brief Constructor.
    /// \param term A term
    print(atermpp::aterm_appl term);

    /// \brief Constructor.
    print(const expression& expr);

    expression expr() const;
};

/// \brief An include
class include: public definition_expression
{
  public:
    /// \brief Default constructor.
    include();

    /// \brief Constructor.
    /// \param term A term
    include(atermpp::aterm_appl term);

    /// \brief Constructor.
    include(const filename& file);

    filename file() const;
};
//--- end generated definition expression class declarations ---//

//--- start generated is-functions ---//

    /// \brief Test for a assign expression
    /// \param t A term
    /// \return True if it is a assign expression
    inline
    bool is_assign(const definition_expression& t)
    {
      return fdr::detail::gsIsAssign(t);
    }

    /// \brief Test for a channel expression
    /// \param t A term
    /// \return True if it is a channel expression
    inline
    bool is_channel(const definition_expression& t)
    {
      return fdr::detail::gsIsChannel(t);
    }

    /// \brief Test for a simple_channel expression
    /// \param t A term
    /// \return True if it is a simple_channel expression
    inline
    bool is_simple_channel(const definition_expression& t)
    {
      return fdr::detail::gsIsSimpleChannel(t);
    }

    /// \brief Test for a nametype expression
    /// \param t A term
    /// \return True if it is a nametype expression
    inline
    bool is_nametype(const definition_expression& t)
    {
      return fdr::detail::gsIsNameType(t);
    }

    /// \brief Test for a datatype expression
    /// \param t A term
    /// \return True if it is a datatype expression
    inline
    bool is_datatype(const definition_expression& t)
    {
      return fdr::detail::gsIsDataType(t);
    }

    /// \brief Test for a subtype expression
    /// \param t A term
    /// \return True if it is a subtype expression
    inline
    bool is_subtype(const definition_expression& t)
    {
      return fdr::detail::gsIsSubType(t);
    }

    /// \brief Test for a external expression
    /// \param t A term
    /// \return True if it is a external expression
    inline
    bool is_external(const definition_expression& t)
    {
      return fdr::detail::gsIsExternal(t);
    }

    /// \brief Test for a transparent expression
    /// \param t A term
    /// \return True if it is a transparent expression
    inline
    bool is_transparent(const definition_expression& t)
    {
      return fdr::detail::gsIsTransparent(t);
    }

    /// \brief Test for a assert expression
    /// \param t A term
    /// \return True if it is a assert expression
    inline
    bool is_assert(const definition_expression& t)
    {
      return fdr::detail::gsIsAssert(t);
    }

    /// \brief Test for a print expression
    /// \param t A term
    /// \return True if it is a print expression
    inline
    bool is_print(const definition_expression& t)
    {
      return fdr::detail::gsIsPrint(t);
    }

    /// \brief Test for a include expression
    /// \param t A term
    /// \return True if it is a include expression
    inline
    bool is_include(const definition_expression& t)
    {
      return fdr::detail::gsIsInclude(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_DEFINITION_EXPRESSION_H
