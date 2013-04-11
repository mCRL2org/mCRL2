// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/container.h
/// \brief The class container_type.

#ifndef MCRL2_DATA_CONTAINER_TYPE_H
#define MCRL2_DATA_CONTAINER_TYPE_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/soundness_checks.h"

namespace mcrl2
{

namespace data
{

//--- start generated classes ---//
/// \brief Container type
class container_type: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    container_type()
      : atermpp::aterm_appl(core::detail::constructSortConsType())
    {}

    /// \brief Constructor.
    /// \param term A term
    container_type(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_rule_SortConsType(*this));
    }
};

/// \brief list of container_types
typedef atermpp::term_list<container_type> container_type_list;

/// \brief vector of container_types
typedef std::vector<container_type>    container_type_vector;


/// \brief Container type for lists
class list_container: public container_type
{
  public:
    /// \brief Default constructor.
    list_container()
      : container_type(core::detail::constructSortList())
    {}

    /// \brief Constructor.
    /// \param term A term
    list_container(const atermpp::aterm& term)
      : container_type(term)
    {
      assert(core::detail::check_term_SortList(*this));
    }
};

/// \brief Test for a list_container expression
/// \param t A term
/// \return True if it is a list_container expression
inline
bool is_list_container(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsSortList(x);
}


/// \brief Container type for sets
class set_container: public container_type
{
  public:
    /// \brief Default constructor.
    set_container()
      : container_type(core::detail::constructSortSet())
    {}

    /// \brief Constructor.
    /// \param term A term
    set_container(const atermpp::aterm& term)
      : container_type(term)
    {
      assert(core::detail::check_term_SortSet(*this));
    }
};

/// \brief Test for a set_container expression
/// \param t A term
/// \return True if it is a set_container expression
inline
bool is_set_container(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsSortSet(x);
}


/// \brief Container type for bags
class bag_container: public container_type
{
  public:
    /// \brief Default constructor.
    bag_container()
      : container_type(core::detail::constructSortBag())
    {}

    /// \brief Constructor.
    /// \param term A term
    bag_container(const atermpp::aterm& term)
      : container_type(term)
    {
      assert(core::detail::check_term_SortBag(*this));
    }
};

/// \brief Test for a bag_container expression
/// \param t A term
/// \return True if it is a bag_container expression
inline
bool is_bag_container(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsSortBag(x);
}


/// \brief Container type for finite sets
class fset_container: public container_type
{
  public:
    /// \brief Default constructor.
    fset_container()
      : container_type(core::detail::constructSortFSet())
    {}

    /// \brief Constructor.
    /// \param term A term
    fset_container(const atermpp::aterm& term)
      : container_type(term)
    {
      assert(core::detail::check_term_SortFSet(*this));
    }
};

/// \brief Test for a fset_container expression
/// \param t A term
/// \return True if it is a fset_container expression
inline
bool is_fset_container(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsSortFSet(x);
}


/// \brief Container type for finite bags
class fbag_container: public container_type
{
  public:
    /// \brief Default constructor.
    fbag_container()
      : container_type(core::detail::constructSortFBag())
    {}

    /// \brief Constructor.
    /// \param term A term
    fbag_container(const atermpp::aterm& term)
      : container_type(term)
    {
      assert(core::detail::check_term_SortFBag(*this));
    }
};

/// \brief Test for a fbag_container expression
/// \param t A term
/// \return True if it is a fbag_container expression
inline
bool is_fbag_container(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsSortFBag(x);
}

//--- end generated classes ---//

} // namespace data

} // namespace mcrl2

namespace std {
//--- start generated swap functions ---//
template <>
inline void swap(mcrl2::data::container_type& t1, mcrl2::data::container_type& t2)
{
  t1.swap(t2);
}

template <>
inline void swap(mcrl2::data::list_container& t1, mcrl2::data::list_container& t2)
{
  t1.swap(t2);
}

template <>
inline void swap(mcrl2::data::set_container& t1, mcrl2::data::set_container& t2)
{
  t1.swap(t2);
}

template <>
inline void swap(mcrl2::data::bag_container& t1, mcrl2::data::bag_container& t2)
{
  t1.swap(t2);
}

template <>
inline void swap(mcrl2::data::fset_container& t1, mcrl2::data::fset_container& t2)
{
  t1.swap(t2);
}

template <>
inline void swap(mcrl2::data::fbag_container& t1, mcrl2::data::fbag_container& t2)
{
  t1.swap(t2);
}
//--- end generated swap functions ---//
} // namespace std

#endif // MCRL2_DATA_CONTAINER_TYPE_H
