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
class typeproduct
{
  public:
    /// \brief Default constructor.
    typeproduct()
      : atermpp::aterm_appl(fdr::detail::constructTypeProduct())
    {}

    /// \brief Constructor.
    /// \param term A term
    typeproduct(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_TypeProduct(m_term));
    }

    /// \brief Constructor.
    typeproduct(const type& left, const type& right)
      : atermpp::aterm_appl(fdr::detail::gsMakeTypeProduct(left, right))
    {}

    type left() const
    {
      return atermpp::arg1(*this);
    }

    type right() const
    {
      return atermpp::arg2(*this);
    }
};/// \brief list of typeproducts
    typedef atermpp::term_list<typeproduct> typeproduct_list;

    /// \brief vector of typeproducts
    typedef atermpp::vector<typeproduct>    typeproduct_vector;


/// \brief A type tuple
class typetuple
{
  public:
    /// \brief Default constructor.
    typetuple()
      : atermpp::aterm_appl(fdr::detail::constructTypeTuple())
    {}

    /// \brief Constructor.
    /// \param term A term
    typetuple(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_TypeTuple(m_term));
    }

    /// \brief Constructor.
    typetuple(const type_list& types)
      : atermpp::aterm_appl(fdr::detail::gsMakeTypeTuple(types))
    {}

    type_list types() const
    {
      return atermpp::list_arg1(*this);
    }
};/// \brief list of typetuples
    typedef atermpp::term_list<typetuple> typetuple_list;

    /// \brief vector of typetuples
    typedef atermpp::vector<typetuple>    typetuple_vector;


/// \brief A type expression
class typeexpr
{
  public:
    /// \brief Default constructor.
    typeexpr()
      : atermpp::aterm_appl(fdr::detail::constructTypeExpr())
    {}

    /// \brief Constructor.
    /// \param term A term
    typeexpr(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_TypeExpr(m_term));
    }

    /// \brief Constructor.
    typeexpr(const expression& expr)
      : atermpp::aterm_appl(fdr::detail::gsMakeTypeExpr(expr))
    {}

    expression expr() const
    {
      return atermpp::arg1(*this);
    }
};/// \brief list of typeexprs
    typedef atermpp::term_list<typeexpr> typeexpr_list;

    /// \brief vector of typeexprs
    typedef atermpp::vector<typeexpr>    typeexpr_vector;


/// \brief A simple type name
class simpletypename
{
  public:
    /// \brief Default constructor.
    simpletypename()
      : atermpp::aterm_appl(fdr::detail::constructSympleTypeName())
    {}

    /// \brief Constructor.
    /// \param term A term
    simpletypename(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_SympleTypeName(m_term));
    }

    /// \brief Constructor.
    simpletypename(const name& name)
      : atermpp::aterm_appl(fdr::detail::gsMakeSympleTypeName(name))
    {}

    name name() const
    {
      return atermpp::arg1(*this);
    }
};/// \brief list of simpletypenames
    typedef atermpp::term_list<simpletypename> simpletypename_list;

    /// \brief vector of simpletypenames
    typedef atermpp::vector<simpletypename>    simpletypename_vector;


/// \brief A type name
class typename
{
  public:
    /// \brief Default constructor.
    typename()
      : atermpp::aterm_appl(fdr::detail::constructTypeName())
    {}

    /// \brief Constructor.
    /// \param term A term
    typename(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_TypeName(m_term));
    }

    /// \brief Constructor.
    typename(const name& name, const type& type)
      : atermpp::aterm_appl(fdr::detail::gsMakeTypeName(name, type))
    {}

    name name() const
    {
      return atermpp::arg1(*this);
    }

    type type() const
    {
      return atermpp::arg2(*this);
    }
};/// \brief list of typenames
    typedef atermpp::term_list<typename> typename_list;

    /// \brief vector of typenames
    typedef atermpp::vector<typename>    typename_vector;

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
