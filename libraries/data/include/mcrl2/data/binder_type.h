// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/binder_type.h
/// \brief The class binder_type.

#ifndef MCRL2_DATA_BINDER_TYPE_H
#define MCRL2_DATA_BINDER_TYPE_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/soundness_checks.h"

namespace mcrl2 {

  namespace data {

    class binder_type: public atermpp::aterm_appl
    {
      public:
        binder_type()
         : atermpp::aterm_appl(core::detail::constructBindingOperator())
        {}

        binder_type(atermpp::aterm_appl term)
         : atermpp::aterm_appl(term)
        {
          assert(core::detail::check_rule_BindingOperator(term));
        }
    };

//--- start generated classes ---//
/// \brief Binder for set or bag comprehension
class set_or_bag_comprehension_binder: public binder_type
{
  public:
    /// \brief Default constructor.
    set_or_bag_comprehension_binder()
      : binder_type(core::detail::constructSetBagComp())
    {}

    /// \brief Constructor.
    /// \param term A term
    set_or_bag_comprehension_binder(atermpp::aterm_appl term)
      : binder_type(term)
    {
      assert(core::detail::check_term_SetBagComp(m_term));
    }
};

/// \brief Binder for set comprehension
class set_comprehension_binder: public binder_type
{
  public:
    /// \brief Default constructor.
    set_comprehension_binder()
      : binder_type(core::detail::constructSetComp())
    {}

    /// \brief Constructor.
    /// \param term A term
    set_comprehension_binder(atermpp::aterm_appl term)
      : binder_type(term)
    {
      assert(core::detail::check_term_SetComp(m_term));
    }
};

/// \brief Binder for bag comprehension
class bag_comprehension_binder: public binder_type
{
  public:
    /// \brief Default constructor.
    bag_comprehension_binder()
      : binder_type(core::detail::constructBagComp())
    {}

    /// \brief Constructor.
    /// \param term A term
    bag_comprehension_binder(atermpp::aterm_appl term)
      : binder_type(term)
    {
      assert(core::detail::check_term_BagComp(m_term));
    }
};

/// \brief Binder for universal quantification
class forall_binder: public binder_type
{
  public:
    /// \brief Default constructor.
    forall_binder()
      : binder_type(core::detail::constructForall())
    {}

    /// \brief Constructor.
    /// \param term A term
    forall_binder(atermpp::aterm_appl term)
      : binder_type(term)
    {
      assert(core::detail::check_term_Forall(m_term));
    }
};

/// \brief Binder for existential quantification
class exists_binder: public binder_type
{
  public:
    /// \brief Default constructor.
    exists_binder()
      : binder_type(core::detail::constructExists())
    {}

    /// \brief Constructor.
    /// \param term A term
    exists_binder(atermpp::aterm_appl term)
      : binder_type(term)
    {
      assert(core::detail::check_term_Exists(m_term));
    }
};

/// \brief Binder for lambda abstraction
class lambda_binder: public binder_type
{
  public:
    /// \brief Default constructor.
    lambda_binder()
      : binder_type(core::detail::constructLambda())
    {}

    /// \brief Constructor.
    /// \param term A term
    lambda_binder(atermpp::aterm_appl term)
      : binder_type(term)
    {
      assert(core::detail::check_term_Lambda(m_term));
    }
};
//--- end generated classes ---//

  } // namespace data

} // namespace mcrl2
#endif // MCRL2_DATA_BINDER_TYPE_H
