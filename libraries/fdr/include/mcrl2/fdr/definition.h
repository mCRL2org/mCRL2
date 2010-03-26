// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/definition.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_DEFINITION_H
#define MCRL2_FDR_DEFINITION_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"

namespace mcrl2 {

namespace fdr {

  /// \brief Definition
  class definition: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      definition()
        : atermpp::aterm_appl(fdr::detail::constructDefn())
      {}

      /// \brief Constructor.
      /// \param term A term
      definition(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(fdr::detail::check_rule_Defn(m_term));
      }
  };

//--- start generated classes ---//
/// \brief An assignment
class assign
{
  public:
    /// \brief Default constructor.
    assign()
      : atermpp::aterm_appl(fdr::detail::constructAssign())
    {}

    /// \brief Constructor.
    /// \param term A term
    assign(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Assign(m_term));
    }

    /// \brief Constructor.
    assign(const any& left, const any& right)
      : atermpp::aterm_appl(fdr::detail::gsMakeAssign(left, right))
    {}

    any left() const
    {
      return atermpp::arg1(*this);
    }

    any right() const
    {
      return atermpp::arg2(*this);
    }
};/// \brief list of assigns
    typedef atermpp::term_list<assign> assign_list;

    /// \brief vector of assigns
    typedef atermpp::vector<assign>    assign_vector;


/// \brief A channel
class channel
{
  public:
    /// \brief Default constructor.
    channel()
      : atermpp::aterm_appl(fdr::detail::constructChannel())
    {}

    /// \brief Constructor.
    /// \param term A term
    channel(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Channel(m_term));
    }

    /// \brief Constructor.
    channel(const name_list& names, const type& type)
      : atermpp::aterm_appl(fdr::detail::gsMakeChannel(names, type))
    {}

    name_list names() const
    {
      return atermpp::list_arg1(*this);
    }

    type type() const
    {
      return atermpp::arg2(*this);
    }
};/// \brief list of channels
    typedef atermpp::term_list<channel> channel_list;

    /// \brief vector of channels
    typedef atermpp::vector<channel>    channel_vector;


/// \brief A simple channel
class channel
{
  public:
    /// \brief Default constructor.
    channel()
      : atermpp::aterm_appl(fdr::detail::constructSimpleChannel())
    {}

    /// \brief Constructor.
    /// \param term A term
    channel(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_SimpleChannel(m_term));
    }

    /// \brief Constructor.
    channel(const name_list& names)
      : atermpp::aterm_appl(fdr::detail::gsMakeSimpleChannel(names))
    {}

    name_list names() const
    {
      return atermpp::list_arg1(*this);
    }
};/// \brief list of channels
    typedef atermpp::term_list<channel> channel_list;

    /// \brief vector of channels
    typedef atermpp::vector<channel>    channel_vector;


/// \brief A nametype
class nametype
{
  public:
    /// \brief Default constructor.
    nametype()
      : atermpp::aterm_appl(fdr::detail::constructNameType())
    {}

    /// \brief Constructor.
    /// \param term A term
    nametype(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_NameType(m_term));
    }

    /// \brief Constructor.
    nametype(const name& name, const type& type)
      : atermpp::aterm_appl(fdr::detail::gsMakeNameType(name, type))
    {}

    name name() const
    {
      return atermpp::arg1(*this);
    }

    type type() const
    {
      return atermpp::arg2(*this);
    }
};/// \brief list of nametypes
    typedef atermpp::term_list<nametype> nametype_list;

    /// \brief vector of nametypes
    typedef atermpp::vector<nametype>    nametype_vector;


/// \brief A datatype
class datatype
{
  public:
    /// \brief Default constructor.
    datatype()
      : atermpp::aterm_appl(fdr::detail::constructDataType())
    {}

    /// \brief Constructor.
    /// \param term A term
    datatype(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_DataType(m_term));
    }

    /// \brief Constructor.
    datatype(const name& name, const vartype_list& vartypes)
      : atermpp::aterm_appl(fdr::detail::gsMakeDataType(name, vartypes))
    {}

    name name() const
    {
      return atermpp::arg1(*this);
    }

    vartype_list vartypes() const
    {
      return atermpp::list_arg2(*this);
    }
};/// \brief list of datatypes
    typedef atermpp::term_list<datatype> datatype_list;

    /// \brief vector of datatypes
    typedef atermpp::vector<datatype>    datatype_vector;


/// \brief A subtype
class subtype
{
  public:
    /// \brief Default constructor.
    subtype()
      : atermpp::aterm_appl(fdr::detail::constructSubType())
    {}

    /// \brief Constructor.
    /// \param term A term
    subtype(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_SubType(m_term));
    }

    /// \brief Constructor.
    subtype(const name& name, const vartype_list& vartypes)
      : atermpp::aterm_appl(fdr::detail::gsMakeSubType(name, vartypes))
    {}

    name name() const
    {
      return atermpp::arg1(*this);
    }

    vartype_list vartypes() const
    {
      return atermpp::list_arg2(*this);
    }
};/// \brief list of subtypes
    typedef atermpp::term_list<subtype> subtype_list;

    /// \brief vector of subtypes
    typedef atermpp::vector<subtype>    subtype_vector;


/// \brief An external
class external
{
  public:
    /// \brief Default constructor.
    external()
      : atermpp::aterm_appl(fdr::detail::constructExternal())
    {}

    /// \brief Constructor.
    /// \param term A term
    external(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_External(m_term));
    }

    /// \brief Constructor.
    external(const name_list& names)
      : atermpp::aterm_appl(fdr::detail::gsMakeExternal(names))
    {}

    name_list names() const
    {
      return atermpp::list_arg1(*this);
    }
};/// \brief list of externals
    typedef atermpp::term_list<external> external_list;

    /// \brief vector of externals
    typedef atermpp::vector<external>    external_vector;


/// \brief A transparent
class transparent
{
  public:
    /// \brief Default constructor.
    transparent()
      : atermpp::aterm_appl(fdr::detail::constructTransparent())
    {}

    /// \brief Constructor.
    /// \param term A term
    transparent(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Transparent(m_term));
    }

    /// \brief Constructor.
    transparent(const trname_list& trnames)
      : atermpp::aterm_appl(fdr::detail::gsMakeTransparent(trnames))
    {}

    trname_list trnames() const
    {
      return atermpp::list_arg1(*this);
    }
};/// \brief list of transparents
    typedef atermpp::term_list<transparent> transparent_list;

    /// \brief vector of transparents
    typedef atermpp::vector<transparent>    transparent_vector;


/// \brief An assertion
class assert_
{
  public:
    /// \brief Default constructor.
    assert_()
      : atermpp::aterm_appl(fdr::detail::constructAssert())
    {}

    /// \brief Constructor.
    /// \param term A term
    assert_(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Assert(m_term));
    }

    /// \brief Constructor.
    assert_(const check& check)
      : atermpp::aterm_appl(fdr::detail::gsMakeAssert(check))
    {}

    check check() const
    {
      return atermpp::arg1(*this);
    }
};/// \brief list of assert_s
    typedef atermpp::term_list<assert_> assert__list;

    /// \brief vector of assert_s
    typedef atermpp::vector<assert_>    assert__vector;


/// \brief A print
class print
{
  public:
    /// \brief Default constructor.
    print()
      : atermpp::aterm_appl(fdr::detail::constructPrint())
    {}

    /// \brief Constructor.
    /// \param term A term
    print(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Print(m_term));
    }

    /// \brief Constructor.
    print(const expression& expr)
      : atermpp::aterm_appl(fdr::detail::gsMakePrint(expr))
    {}

    expression expr() const
    {
      return atermpp::arg1(*this);
    }
};/// \brief list of prints
    typedef atermpp::term_list<print> print_list;

    /// \brief vector of prints
    typedef atermpp::vector<print>    print_vector;


/// \brief An include
class include
{
  public:
    /// \brief Default constructor.
    include()
      : atermpp::aterm_appl(fdr::detail::constructInclude())
    {}

    /// \brief Constructor.
    /// \param term A term
    include(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Include(m_term));
    }

    /// \brief Constructor.
    include(const filename& filename)
      : atermpp::aterm_appl(fdr::detail::gsMakeInclude(filename))
    {}

    filename filename() const
    {
      return atermpp::arg1(*this);
    }
};/// \brief list of includes
    typedef atermpp::term_list<include> include_list;

    /// \brief vector of includes
    typedef atermpp::vector<include>    include_vector;

//--- end generated classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a assign expression
    /// \param t A term
    /// \return True if it is a assign expression
    inline
    bool is_assign(const definition& t)
    {
      return fdr::detail::gsIsAssign(t);
    }

    /// \brief Test for a channel expression
    /// \param t A term
    /// \return True if it is a channel expression
    inline
    bool is_channel(const definition& t)
    {
      return fdr::detail::gsIsChannel(t);
    }

    /// \brief Test for a channel expression
    /// \param t A term
    /// \return True if it is a channel expression
    inline
    bool is_channel(const definition& t)
    {
      return fdr::detail::gsIsSimpleChannel(t);
    }

    /// \brief Test for a nametype expression
    /// \param t A term
    /// \return True if it is a nametype expression
    inline
    bool is_nametype(const definition& t)
    {
      return fdr::detail::gsIsNameType(t);
    }

    /// \brief Test for a datatype expression
    /// \param t A term
    /// \return True if it is a datatype expression
    inline
    bool is_datatype(const definition& t)
    {
      return fdr::detail::gsIsDataType(t);
    }

    /// \brief Test for a subtype expression
    /// \param t A term
    /// \return True if it is a subtype expression
    inline
    bool is_subtype(const definition& t)
    {
      return fdr::detail::gsIsSubType(t);
    }

    /// \brief Test for a external expression
    /// \param t A term
    /// \return True if it is a external expression
    inline
    bool is_external(const definition& t)
    {
      return fdr::detail::gsIsExternal(t);
    }

    /// \brief Test for a transparent expression
    /// \param t A term
    /// \return True if it is a transparent expression
    inline
    bool is_transparent(const definition& t)
    {
      return fdr::detail::gsIsTransparent(t);
    }

    /// \brief Test for a assert expression
    /// \param t A term
    /// \return True if it is a assert expression
    inline
    bool is_assert(const definition& t)
    {
      return fdr::detail::gsIsAssert(t);
    }

    /// \brief Test for a print expression
    /// \param t A term
    /// \return True if it is a print expression
    inline
    bool is_print(const definition& t)
    {
      return fdr::detail::gsIsPrint(t);
    }

    /// \brief Test for a include expression
    /// \param t A term
    /// \return True if it is a include expression
    inline
    bool is_include(const definition& t)
    {
      return fdr::detail::gsIsInclude(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_DEFINITION_H
