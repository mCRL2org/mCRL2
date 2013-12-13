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
#include "mcrl2/core/detail/default_values.h"
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
      : atermpp::aterm_appl(core::detail::default_value_SortConsType())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit container_type(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_rule_SortConsType(*this));
    }
};

/// \brief list of container_types
typedef atermpp::term_list<container_type> container_type_list;

/// \brief vector of container_types
typedef std::vector<container_type>    container_type_vector;

// prototype declaration
std::string pp(const container_type& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const container_type& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(container_type& t1, container_type& t2)
{
  t1.swap(t2);
}


/// \brief Container type for lists
class list_container: public container_type
{
  public:
    /// \brief Default constructor.
    list_container()
      : container_type(core::detail::default_value_SortList())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit list_container(const atermpp::aterm& term)
      : container_type(term)
    {
      assert(core::detail::check_term_SortList(*this));
    }
};

/// \brief Test for a list_container expression
/// \param x A term
/// \return True if \a x is a list_container expression
inline
bool is_list_container(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::SortList;
}

// prototype declaration
std::string pp(const list_container& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const list_container& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(list_container& t1, list_container& t2)
{
  t1.swap(t2);
}


/// \brief Container type for sets
class set_container: public container_type
{
  public:
    /// \brief Default constructor.
    set_container()
      : container_type(core::detail::default_value_SortSet())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit set_container(const atermpp::aterm& term)
      : container_type(term)
    {
      assert(core::detail::check_term_SortSet(*this));
    }
};

/// \brief Test for a set_container expression
/// \param x A term
/// \return True if \a x is a set_container expression
inline
bool is_set_container(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::SortSet;
}

// prototype declaration
std::string pp(const set_container& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const set_container& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(set_container& t1, set_container& t2)
{
  t1.swap(t2);
}


/// \brief Container type for bags
class bag_container: public container_type
{
  public:
    /// \brief Default constructor.
    bag_container()
      : container_type(core::detail::default_value_SortBag())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit bag_container(const atermpp::aterm& term)
      : container_type(term)
    {
      assert(core::detail::check_term_SortBag(*this));
    }
};

/// \brief Test for a bag_container expression
/// \param x A term
/// \return True if \a x is a bag_container expression
inline
bool is_bag_container(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::SortBag;
}

// prototype declaration
std::string pp(const bag_container& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const bag_container& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(bag_container& t1, bag_container& t2)
{
  t1.swap(t2);
}


/// \brief Container type for finite sets
class fset_container: public container_type
{
  public:
    /// \brief Default constructor.
    fset_container()
      : container_type(core::detail::default_value_SortFSet())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit fset_container(const atermpp::aterm& term)
      : container_type(term)
    {
      assert(core::detail::check_term_SortFSet(*this));
    }
};

/// \brief Test for a fset_container expression
/// \param x A term
/// \return True if \a x is a fset_container expression
inline
bool is_fset_container(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::SortFSet;
}

// prototype declaration
std::string pp(const fset_container& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const fset_container& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(fset_container& t1, fset_container& t2)
{
  t1.swap(t2);
}


/// \brief Container type for finite bags
class fbag_container: public container_type
{
  public:
    /// \brief Default constructor.
    fbag_container()
      : container_type(core::detail::default_value_SortFBag())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit fbag_container(const atermpp::aterm& term)
      : container_type(term)
    {
      assert(core::detail::check_term_SortFBag(*this));
    }
};

/// \brief Test for a fbag_container expression
/// \param x A term
/// \return True if \a x is a fbag_container expression
inline
bool is_fbag_container(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::SortFBag;
}

// prototype declaration
std::string pp(const fbag_container& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const fbag_container& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(fbag_container& t1, fbag_container& t2)
{
  t1.swap(t2);
}
//--- end generated classes ---//

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_CONTAINER_TYPE_H
