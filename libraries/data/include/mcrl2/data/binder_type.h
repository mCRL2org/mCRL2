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
#include "mcrl2/core/detail/default_values.h"
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
      : atermpp::aterm_appl(core::detail::default_values::BindingOperator)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit binder_type(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_rule_BindingOperator(*this));
    }
};

/// \brief list of binder_types
typedef atermpp::term_list<binder_type> binder_type_list;

/// \brief vector of binder_types
typedef std::vector<binder_type>    binder_type_vector;

// prototype declaration
std::string pp(const binder_type& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const binder_type& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(binder_type& t1, binder_type& t2)
{
  t1.swap(t2);
}


/// \brief Binder for untyped set or bag comprehension
class untyped_set_or_bag_comprehension_binder: public binder_type
{
  public:
    /// \brief Default constructor.
    untyped_set_or_bag_comprehension_binder()
      : binder_type(core::detail::default_values::UntypedSetBagComp)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit untyped_set_or_bag_comprehension_binder(const atermpp::aterm& term)
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
  return x.function() == core::detail::function_symbols::UntypedSetBagComp;
}

// prototype declaration
std::string pp(const untyped_set_or_bag_comprehension_binder& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const untyped_set_or_bag_comprehension_binder& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(untyped_set_or_bag_comprehension_binder& t1, untyped_set_or_bag_comprehension_binder& t2)
{
  t1.swap(t2);
}


/// \brief Binder for set comprehension
class set_comprehension_binder: public binder_type
{
  public:
    /// \brief Default constructor.
    set_comprehension_binder()
      : binder_type(core::detail::default_values::SetComp)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit set_comprehension_binder(const atermpp::aterm& term)
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
  return x.function() == core::detail::function_symbols::SetComp;
}

// prototype declaration
std::string pp(const set_comprehension_binder& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const set_comprehension_binder& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(set_comprehension_binder& t1, set_comprehension_binder& t2)
{
  t1.swap(t2);
}


/// \brief Binder for bag comprehension
class bag_comprehension_binder: public binder_type
{
  public:
    /// \brief Default constructor.
    bag_comprehension_binder()
      : binder_type(core::detail::default_values::BagComp)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit bag_comprehension_binder(const atermpp::aterm& term)
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
  return x.function() == core::detail::function_symbols::BagComp;
}

// prototype declaration
std::string pp(const bag_comprehension_binder& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const bag_comprehension_binder& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(bag_comprehension_binder& t1, bag_comprehension_binder& t2)
{
  t1.swap(t2);
}


/// \brief Binder for universal quantification
class forall_binder: public binder_type
{
  public:
    /// \brief Default constructor.
    forall_binder()
      : binder_type(core::detail::default_values::Forall)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit forall_binder(const atermpp::aterm& term)
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
  return x.function() == core::detail::function_symbols::Forall;
}

// prototype declaration
std::string pp(const forall_binder& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const forall_binder& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(forall_binder& t1, forall_binder& t2)
{
  t1.swap(t2);
}


/// \brief Binder for existential quantification
class exists_binder: public binder_type
{
  public:
    /// \brief Default constructor.
    exists_binder()
      : binder_type(core::detail::default_values::Exists)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit exists_binder(const atermpp::aterm& term)
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
  return x.function() == core::detail::function_symbols::Exists;
}

// prototype declaration
std::string pp(const exists_binder& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const exists_binder& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(exists_binder& t1, exists_binder& t2)
{
  t1.swap(t2);
}


/// \brief Binder for lambda abstraction
class lambda_binder: public binder_type
{
  public:
    /// \brief Default constructor.
    lambda_binder()
      : binder_type(core::detail::default_values::Lambda)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit lambda_binder(const atermpp::aterm& term)
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
  return x.function() == core::detail::function_symbols::Lambda;
}

// prototype declaration
std::string pp(const lambda_binder& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const lambda_binder& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(lambda_binder& t1, lambda_binder& t2)
{
  t1.swap(t2);
}
//--- end generated classes ---//

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_BINDER_TYPE_H
