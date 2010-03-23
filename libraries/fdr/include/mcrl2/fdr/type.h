// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/type.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_TYPE_H
#define MCRL2_FDR_TYPE_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"

namespace mcrl2 {

namespace fdr {

  /// \brief Type
  class type: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      type()
        : atermpp::aterm_appl(fdr::detail::constructType())
      {}

      /// \brief Constructor.
      /// \param term A term
      type(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(fdr::detail::check_rule_Type(m_term));
      }
  };

//--- start generated classes ---//
/// \brief A type product
class typeproduct: public type
{
  public:
    /// \brief Default constructor.
    typeproduct()
      : type(fdr::detail::constructTypeProduct())
    {}

    /// \brief Constructor.
    /// \param term A term
    typeproduct(atermpp::aterm_appl term)
      : type(term)
    {
      assert(fdr::detail::check_term_TypeProduct(m_term));
    }

    /// \brief Constructor.
    typeproduct(const type& left, const type& right)
      : type(fdr::detail::gsMakeTypeProduct(left, right))
    {}

    type left() const
    {
      return atermpp::arg1(*this);
    }

    type right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief A type tuple
class typetuple: public type
{
  public:
    /// \brief Default constructor.
    typetuple()
      : type(fdr::detail::constructTypeTuple())
    {}

    /// \brief Constructor.
    /// \param term A term
    typetuple(atermpp::aterm_appl term)
      : type(term)
    {
      assert(fdr::detail::check_term_TypeTuple(m_term));
    }

    /// \brief Constructor.
    typetuple(const type_list& types)
      : type(fdr::detail::gsMakeTypeTuple(types))
    {}

    type_list types() const
    {
      return atermpp::list_arg1(*this);
    }
};

/// \brief A type expression
class typeexpr: public type
{
  public:
    /// \brief Default constructor.
    typeexpr()
      : type(fdr::detail::constructTypeExpr())
    {}

    /// \brief Constructor.
    /// \param term A term
    typeexpr(atermpp::aterm_appl term)
      : type(term)
    {
      assert(fdr::detail::check_term_TypeExpr(m_term));
    }

    /// \brief Constructor.
    typeexpr(const expression& expr)
      : type(fdr::detail::gsMakeTypeExpr(expr))
    {}

    expression expr() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief A simple type name
class simpletypename: public type
{
  public:
    /// \brief Default constructor.
    simpletypename()
      : type(fdr::detail::constructSympleTypeName())
    {}

    /// \brief Constructor.
    /// \param term A term
    simpletypename(atermpp::aterm_appl term)
      : type(term)
    {
      assert(fdr::detail::check_term_SympleTypeName(m_term));
    }

    /// \brief Constructor.
    simpletypename(const name& name)
      : type(fdr::detail::gsMakeSympleTypeName(name))
    {}

    name name() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief A type name
class typename: public type
{
  public:
    /// \brief Default constructor.
    typename()
      : type(fdr::detail::constructTypeName())
    {}

    /// \brief Constructor.
    /// \param term A term
    typename(atermpp::aterm_appl term)
      : type(term)
    {
      assert(fdr::detail::check_term_TypeName(m_term));
    }

    /// \brief Constructor.
    typename(const name& name, const type& type)
      : type(fdr::detail::gsMakeTypeName(name, type))
    {}

    name name() const
    {
      return atermpp::arg1(*this);
    }

    type type() const
    {
      return atermpp::arg2(*this);
    }
};
//--- end generated classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a typeproduct expression
    /// \param t A term
    /// \return True if it is a typeproduct expression
    inline
    bool is_typeproduct(const type& t)
    {
      return fdr::detail::gsIsTypeProduct(t);
    }

    /// \brief Test for a typetuple expression
    /// \param t A term
    /// \return True if it is a typetuple expression
    inline
    bool is_typetuple(const type& t)
    {
      return fdr::detail::gsIsTypeTuple(t);
    }

    /// \brief Test for a typeexpr expression
    /// \param t A term
    /// \return True if it is a typeexpr expression
    inline
    bool is_typeexpr(const type& t)
    {
      return fdr::detail::gsIsTypeExpr(t);
    }

    /// \brief Test for a simpletypename expression
    /// \param t A term
    /// \return True if it is a simpletypename expression
    inline
    bool is_simpletypename(const type& t)
    {
      return fdr::detail::gsIsSympleTypeName(t);
    }

    /// \brief Test for a typename expression
    /// \param t A term
    /// \return True if it is a typename expression
    inline
    bool is_typename(const type& t)
    {
      return fdr::detail::gsIsTypeName(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_TYPE_H
