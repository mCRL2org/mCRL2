// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/type.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_TYPE_H
#define MCRL2_FDR_TYPE_H

#include "mcrl2/fdr/term_include_files.h"

namespace mcrl2 {

namespace fdr {

//--- start generated classes ---//
/// \brief A type product
class typeproduct: public atermpp::aterm_appl
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
};

/// \brief list of typeproducts
typedef atermpp::term_list<typeproduct> typeproduct_list;

/// \brief vector of typeproducts
typedef atermpp::vector<typeproduct>    typeproduct_vector;


/// \brief A type tuple
class typetuple: public atermpp::aterm_appl
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
};

/// \brief list of typetuples
typedef atermpp::term_list<typetuple> typetuple_list;

/// \brief vector of typetuples
typedef atermpp::vector<typetuple>    typetuple_vector;


/// \brief A type set
class typeset: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    typeset()
      : atermpp::aterm_appl(fdr::detail::constructTypeSet())
    {}

    /// \brief Constructor.
    /// \param term A term
    typeset(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_TypeSet(m_term));
    }

    /// \brief Constructor.
    typeset(const set_expression& set)
      : atermpp::aterm_appl(fdr::detail::gsMakeTypeSet(set))
    {}

    set_expression set() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief list of typesets
typedef atermpp::term_list<typeset> typeset_list;

/// \brief vector of typesets
typedef atermpp::vector<typeset>    typeset_vector;


/// \brief A simple type name
class simpletypename: public atermpp::aterm_appl
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
    simpletypename(const name& id)
      : atermpp::aterm_appl(fdr::detail::gsMakeSympleTypeName(id))
    {}

    name id() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief list of simpletypenames
typedef atermpp::term_list<simpletypename> simpletypename_list;

/// \brief vector of simpletypenames
typedef atermpp::vector<simpletypename>    simpletypename_vector;


/// \brief A type name
class typename: public atermpp::aterm_appl
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
    typename(const name& id, const type& type_name)
      : atermpp::aterm_appl(fdr::detail::gsMakeTypeName(id, type_name))
    {}

    name id() const
    {
      return atermpp::arg1(*this);
    }

    type type_name() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief list of typenames
typedef atermpp::term_list<typename> typename_list;

/// \brief vector of typenames
typedef atermpp::vector<typename>    typename_vector;

//--- end generated classes ---//

//--- start generated is-functions ---//
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_TYPE_H
