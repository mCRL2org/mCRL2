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

namespace mcrl2 {

  namespace data {

    class container_type: public atermpp::aterm_appl
    {
      public:
        container_type()
         : atermpp::aterm_appl(core::detail::constructSortConsType())
        {}

        container_type(atermpp::aterm_appl term)
         : atermpp::aterm_appl(term)
        {
          assert(core::detail::check_rule_SortConsType(term));
        }
    };

//--- start generated classes ---//
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
    list_container(atermpp::aterm_appl term)
      : container_type(term)
    {
      assert(core::detail::check_term_SortList(m_term));
    }
};

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
    set_container(atermpp::aterm_appl term)
      : container_type(term)
    {
      assert(core::detail::check_term_SortSet(m_term));
    }
};

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
    bag_container(atermpp::aterm_appl term)
      : container_type(term)
    {
      assert(core::detail::check_term_SortBag(m_term));
    }
};

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
    fset_container(atermpp::aterm_appl term)
      : container_type(term)
    {
      assert(core::detail::check_term_SortFSet(m_term));
    }
};

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
    fbag_container(atermpp::aterm_appl term)
      : container_type(term)
    {
      assert(core::detail::check_term_SortFBag(m_term));
    }
};
//--- end generated classes ---//

  } // namespace data

} // namespace mcrl2
#endif // MCRL2_DATA_CONTAINER_TYPE_H
