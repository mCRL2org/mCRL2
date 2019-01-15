// Author(s): Wieger Wesselink, Jan Friso Groote. Based on the aterm library by Paul Klint and others.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm.h
/// \brief The aterm class.

#ifndef MCRL2_ATERMPP_ATERM_H
#define MCRL2_ATERMPP_ATERM_H

#include "mcrl2/atermpp/type_traits.h"
#include "mcrl2/atermpp/detail/aterm.h"

#include <string>
#include <sstream>
#include <iostream>

/// \brief The main namespace for the aterm++ library.
namespace atermpp
{

typedef void(*term_callback)(const aterm&);

extern void add_creation_hook(const function_symbol&, term_callback);
extern void add_deletion_hook(const function_symbol&, term_callback);

/// \brief An unprotected term does not change the reference count of the
///        shared term when it is copied or moved.
class unprotected_aterm
{
public:
  template < typename T >
  friend class term_appl;

  template < typename T >
  friend class term_list;

  friend detail::_aterm* detail::address(const unprotected_aterm& t);

public:

  unprotected_aterm() noexcept :
    m_term(nullptr)
  {}

  unprotected_aterm(detail::_aterm* term) noexcept :
    m_term(term)
  {}

  /// \return True iff this term is an term_appl.
  /// \details This function has constant complexity.
  ///          It is defined as !type_is_int() && !type_is_list().
  bool type_is_appl() const noexcept
  {
    return !type_is_int() && !type_is_list();
  }

  /// \return True iff this term has internal structure of an aterm_int.
  /// \details This function has constant complexity.
  bool type_is_int() const noexcept
  {
    const function_symbol& f=m_term->function();
    return f == detail::g_as_int;
  }

  /// \returns True iff this term has the structure of an term_list
  /// \details This function has constant complexity.
  bool type_is_list() const noexcept
  {
    const function_symbol& f=m_term->function();
    return f == detail::g_as_list || f == detail::g_as_empty_list;
  }

  /// \return true iff t is equal to the current term.
  /// \details Terms are stored in a maximally shared way. This
  ///         means that this equality operator can be calculated
  ///         in constant time.
  bool operator ==(const unprotected_aterm& t) const
  {
    return m_term == t.m_term;
  }

  /// \brief Inequality operator on two aterms.
  /// \details See note at the == operator. This operator requires constant time.
  /// \param t A term to which the current term is compared.
  /// \return false iff t is equal to the current term.
  bool operator !=(const unprotected_aterm& t) const
  {
    return m_term!=t.m_term;
  }

  /// \brief Comparison operator for two aterms.
  /// \details This operator requires constant time. It compares
  ///         the addresses where terms are stored. That means
  ///         that the outcome of this operator is only stable
  ///         as long as aterms are not garbage collected.
  /// \param t A term to which the current term is compared.
  /// \return True iff the current term is smaller than the argument.
  bool operator <(const unprotected_aterm& t) const
  {
    return m_term<t.m_term;
  }

  /// \brief Comparison operator for two aterms.
  /// \details This operator requires constant time. See note at the operator <.
  /// \param t A term to which the current term is compared.
  /// \return True iff the current term is larger than the argument.
  bool operator >(const unprotected_aterm& t) const
  {
    return m_term>t.m_term;
  }

  /// \brief Comparison operator for two aterms.
  /// \details This operator requires constant time. See note at the operator <.
  /// \param t A term to which the current term is compared.
  /// \return True iff the current term is smaller or equal than the argument.
  bool operator <=(const unprotected_aterm& t) const
  {
    return m_term<=t.m_term;
  }

  /// \brief Comparison operator for two aterms.
  /// \details This operator requires constant time. See note at the operator <.
  /// \param t A term to which the current term is compared.
  /// \return True iff the current term is larger or equalthan the argument.
  bool operator >=(const unprotected_aterm& t) const
  {
    return m_term>=t.m_term;
  }

  /// \brief Returns true if this term is not equal to the term assigned by
  ///        the default constructor of aterms, term_appl<T>'s and aterm_int.
  /// \details The default constructor of a term_list<T> is the empty list, on which
  ///          the operator defined yields true. This operation is more efficient
  ///          than comparing the current term with an aterm(), term_appl<T>() or an
  ///          aterm_int().
  /// \return A boolean indicating whether this term equals the default constructor.
  bool defined() const
  {
    return m_term != nullptr;
  }

  /// \brief Swaps this term with its argument.
  /// \details This operation is more efficient than exchanging terms by an assignment,
  ///          as swapping does not require to change the protection of terms.
  /// \param t The term with which this term is swapped.
  void swap(unprotected_aterm& t) noexcept
  {
    std::swap(m_term, t.m_term);
  }

  /// \returns The function symbol of the term, which can also be an AS_EMPTY_LIST,
  ///          AS_INT and AS_LIST.
  /// \details This is for internal use only.
  const function_symbol& function() const
  {
    return m_term->function();
  }

protected:
  detail::_aterm* m_term;
};

/// \brief The aterm base class that provides protection of the underlying shared terms.
class aterm : public unprotected_aterm
{
public:
  aterm() noexcept = default;
  ~aterm()
  {
    if (defined())
    {
      m_term->decrement_reference_count();
    }
  }

  /// \brief Takes ownership of the passed underlying term.
  /// \todo Should be protected, but this cannot yet be done due to a problem
  ///       in the compiling rewriter.
  explicit aterm(detail::_aterm *t) noexcept
  {
    t->increment_reference_count();
    m_term = t;
  }

  // This class has a non-trivial destructor so explicitly define the copy and move operators.
  aterm(const aterm& other) noexcept :
    unprotected_aterm()
  {
    m_term = other.m_term;
    if (defined())
    {
      m_term->increment_reference_count();
    }
  }

  aterm& operator=(const aterm& other) noexcept
  {
    // Increment first to prevent the same term from becoming reference zero temporarily.
    if (other.defined())
    {
      other.m_term->increment_reference_count();
    }

    // Decrement the reference from the term that is currently referred to.
    if (defined())
    {
      m_term->decrement_reference_count();
    }

    m_term = other.m_term;
    return *this;
  }

  aterm(aterm&& other) noexcept
  {
    std::swap(m_term, other.m_term);
  }

  aterm& operator=(aterm&& other) noexcept
  {
    if (defined())
    {
      m_term->decrement_reference_count();
    }

    m_term = other.m_term;
    other.m_term = nullptr;
    return *this;
  }

protected:
  /// \brief A copy constructor that does not protect the term.
  void copy_term(const aterm& t) noexcept
  {
    m_term = t.m_term;
  }
};

template <class Term1, class Term2>
struct is_convertible : public
    std::conditional<std::is_base_of<aterm, Term1>::value &&
                     std::is_base_of<aterm, Term2>::value && (
                     std::is_convertible<Term1, Term2>::value ||
                     std::is_convertible<Term2, Term1>::value),
                     std::true_type, std::false_type>::type
{ };

/// \brief A cheap cast from one aterm based type to another
///        When casting one aterm based type into another, generally  a new aterm is constructed,
///        and the old one is destroyed. This can cause undesired overhead, for instance due to
///        increasing and decreasing of reference counts. This cast changes the type, without
///        changing the aterm itself. It can only be used if Base and Derived inherit from aterm,
///        and contain no additional information than a
///          single aterm.
/// \param   t A term of a type inheriting from an aterm.
/// \return  A term of type Derived.
template <class Derived, class Base>
const Derived& down_cast(const Base& t,
                         typename std::enable_if<is_convertible<Base, Derived>::value &&
                                                 !std::is_base_of<Derived, Base>::value>::type* = nullptr)
{
  static_assert(sizeof(Derived) == sizeof(aterm),
                "aterm cast cannot be applied types derived from aterms where extra fields are added");
  return reinterpret_cast<const Derived&>(t);
}

template < typename DerivedCont, typename Base, template <typename Elem> class Cont >
const DerivedCont& container_cast(const Cont<Base>& t,
                              typename std::enable_if<
                                is_container<DerivedCont>::value &&
                                std::is_same<Cont<typename DerivedCont::value_type>, DerivedCont>::value &&
                                !std::is_base_of<DerivedCont, Cont<Base> >::value &&
                                is_convertible<Base, typename DerivedCont::value_type>::value
                              >::type* = nullptr)
{
  static_assert(sizeof(typename DerivedCont::value_type) == sizeof(aterm),
                "aterm cast cannot be applied types derived from aterms where extra fields are added");
  return reinterpret_cast<const DerivedCont&>(t);
}

/// \brief A cast form an aterm derived class to a class that inherits in possibly multiple steps from this class.
/// \details The derived class is not allowed to contain extra fields. This conversion does not require runtime computation
///          effort. Also see down_cast.
/// \param t The term that is converted.
/// \return A term of type Derived.
template <class Derived, class Base>
const Derived& vertical_cast(const Base& t,
                          typename std::enable_if<is_convertible<Base, Derived>::value>::type* = nullptr)
{
  static_assert(sizeof(Derived) == sizeof(aterm),
                "aterm cast cannot be applied types derived from aterms where extra fields are added");
  return reinterpret_cast<const Derived&>(t);
}

template < typename DerivedCont, typename Base, template <typename Elem> class Cont >
const DerivedCont& vertical_cast(const Cont<Base>& t,
                              typename std::enable_if<
                                is_container<DerivedCont>::value &&
                                std::is_same<Cont<typename DerivedCont::value_type>, DerivedCont>::value &&
                                is_convertible<Base, typename DerivedCont::value_type>::value
                              >::type* = NULL)
{
  static_assert(sizeof(typename DerivedCont::value_type) == sizeof(aterm),
                "aterm cast cannot be applied types derived from aterms where extra fields are added");
  return reinterpret_cast<const DerivedCont&>(t);
}

/// \brief Cast from an aterm derived term to another aterm.
/// \deprecated.
template <class Derived, class Base>
const Derived& deprecated_cast(const Base& t,
                               typename std::enable_if<
                                  std::is_base_of<aterm, Base>::value &&
                                  std::is_base_of<aterm, Derived>::value
                               >::type* = nullptr)
{
  static_assert(sizeof(Derived) == sizeof(aterm),
                "aterm cast cannot be applied types derived from aterms where extra fields are added");
  return reinterpret_cast<const Derived&>(t);
}

/// \brief Send the term in textual form to the ostream.
std::ostream& operator<<(std::ostream& out, const aterm& t);

/// \param t The input aterm.
/// \return A string representation of the given term derived from an aterm.
inline std::string pp(const aterm& t)
{
  std::ostringstream oss;
  oss << t;
  return oss.str();
}

} // namespace atermpp


namespace mcrl2
{
namespace workaround
{
  /// \brief This is a workaround for the return by value diagnostic (clang -Wreturn-std-move).
  /// Description: The user probably expected a move, but they're not getting a move, perhaps because
  /// the type of "x" is different from the return type of the function.
  /// \details This constraint might be lifted by newer C++ standards and then this workaround can easily be removed.
  template<typename T>
  T&& return_std_move(T& t)
  {
    return std::move(t);
  }
}
}

namespace std
{

/// \brief Swaps two aterms.
/// \details This operation is more efficient than exchanging terms by an assignment,
///          as swapping does not require to change the protection of terms.
///          In order to be used in the standard containers, the declaration must
///          be preceded by an empty template declaration. This swap function is
///          not used for classes that derive from the aterm class. A specific
///          swap function must be provided for derived classes.
/// \param t1 The first term
/// \param t2 The second term
template <>
inline void swap(atermpp::unprotected_aterm& t1, atermpp::unprotected_aterm& t2) noexcept
{
  t1.swap(t2);
}

} // namespace std


#include "mcrl2/atermpp/detail/aterm_implementation.h"

#endif // MCRL2_ATERMPP_ATERM_H
