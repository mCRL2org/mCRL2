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

namespace mcrl2
{

namespace data
{

//--- start generated classes ---//
/// \brief Binder
class binder_type: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    binder_type()
      : atermpp::aterm_appl(core::detail::constructBindingOperator())
    {}

    /// \brief Constructor.
    /// \param term A term
    binder_type(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_rule_BindingOperator(*this));
    }
};

/// \brief list of binder_types
typedef atermpp::term_list<binder_type> binder_type_list;

/// \brief vector of binder_types
typedef std::vector<binder_type>    binder_type_vector;


/// \brief Binder for untyped set or bag comprehension
class untyped_set_or_bag_comprehension_binder: public binder_type
{
  public:
    /// \brief Default constructor.
    untyped_set_or_bag_comprehension_binder()
      : binder_type(core::detail::constructUntypedSetBagComp())
    {}

    /// \brief Constructor.
    /// \param term A term
    untyped_set_or_bag_comprehension_binder(const atermpp::aterm& term)
      : binder_type(term)
    {
      assert(core::detail::check_term_UntypedSetBagComp(*this));
    }
};

/// \brief Test for a untyped_set_or_bag_comprehension_binder expression
/// \param x A term
/// \return True if \a x is a untyped_set_or_bag_comprehension_binder expression
inline
bool is_untyped_set_or_bag_comprehension_binder(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsUntypedSetBagComp(x);
}


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
    set_comprehension_binder(const atermpp::aterm& term)
      : binder_type(term)
    {
      assert(core::detail::check_term_SetComp(*this));
    }
};

/// \brief Test for a set_comprehension_binder expression
/// \param x A term
/// \return True if \a x is a set_comprehension_binder expression
inline
bool is_set_comprehension_binder(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsSetComp(x);
}


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
    bag_comprehension_binder(const atermpp::aterm& term)
      : binder_type(term)
    {
      assert(core::detail::check_term_BagComp(*this));
    }
};

/// \brief Test for a bag_comprehension_binder expression
/// \param x A term
/// \return True if \a x is a bag_comprehension_binder expression
inline
bool is_bag_comprehension_binder(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsBagComp(x);
}


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
    forall_binder(const atermpp::aterm& term)
      : binder_type(term)
    {
      assert(core::detail::check_term_Forall(*this));
    }
};

/// \brief Test for a forall_binder expression
/// \param x A term
/// \return True if \a x is a forall_binder expression
inline
bool is_forall_binder(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsForall(x);
}


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
    exists_binder(const atermpp::aterm& term)
      : binder_type(term)
    {
      assert(core::detail::check_term_Exists(*this));
    }
};

/// \brief Test for a exists_binder expression
/// \param x A term
/// \return True if \a x is a exists_binder expression
inline
bool is_exists_binder(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsExists(x);
}


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
    lambda_binder(const atermpp::aterm& term)
      : binder_type(term)
    {
      assert(core::detail::check_term_Lambda(*this));
    }
};

/// \brief Test for a lambda_binder expression
/// \param x A term
/// \return True if \a x is a lambda_binder expression
inline
bool is_lambda_binder(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsLambda(x);
}

//--- end generated classes ---//

} // namespace data

} // namespace mcrl2

namespace std {
//--- start generated swap functions ---//
template <>
inline void swap(mcrl2::data::binder_type& t1, mcrl2::data::binder_type& t2)
{
  t1.swap(t2);
}

template <>
inline void swap(mcrl2::data::untyped_set_or_bag_comprehension_binder& t1, mcrl2::data::untyped_set_or_bag_comprehension_binder& t2)
{
  t1.swap(t2);
}

template <>
inline void swap(mcrl2::data::set_comprehension_binder& t1, mcrl2::data::set_comprehension_binder& t2)
{
  t1.swap(t2);
}

template <>
inline void swap(mcrl2::data::bag_comprehension_binder& t1, mcrl2::data::bag_comprehension_binder& t2)
{
  t1.swap(t2);
}

template <>
inline void swap(mcrl2::data::forall_binder& t1, mcrl2::data::forall_binder& t2)
{
  t1.swap(t2);
}

template <>
inline void swap(mcrl2::data::exists_binder& t1, mcrl2::data::exists_binder& t2)
{
  t1.swap(t2);
}

template <>
inline void swap(mcrl2::data::lambda_binder& t1, mcrl2::data::lambda_binder& t2)
{
  t1.swap(t2);
}
//--- end generated swap functions ---//
} // namespace std

#endif // MCRL2_DATA_BINDER_TYPE_H
