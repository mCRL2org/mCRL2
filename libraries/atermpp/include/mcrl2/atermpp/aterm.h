// Author(s): Jan Friso Groote, Maurice Laveaux, Wieger Wesselink.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_ATERMPP_ATERM_H
#define MCRL2_ATERMPP_ATERM_H

#include <algorithm>
#include <assert.h>
#include <sstream>
#include "mcrl2/atermpp/detail/aterm.h"
#include "mcrl2/atermpp/type_traits.h"

/// \brief The main namespace for the aterm++ library.
namespace atermpp
{

typedef void(*term_callback)(const aterm&);

extern void add_deletion_hook(const function_symbol&, term_callback);

// Forward declaration
namespace detail
{
  class thread_aterm_pool;
}

/// \brief An unprotected term does not change the reference count of the
///        shared term when it is copied or moved.
class unprotected_aterm
{
  friend detail::_aterm* detail::address(const unprotected_aterm& t);

protected:
  const detail::_aterm* m_term;

public:

  /// \brief Default constuctor.
  unprotected_aterm() noexcept
   : m_term(nullptr)
  {}

  /// \brief Constructor.
  /// \param term The term from which the new term is constructed.
  unprotected_aterm(const detail::_aterm* term) noexcept
   : m_term(term)
  {}

  /// \brief Dynamic check whether the term is an aterm_appl.
  /// \return True iff this term is an term_appl.
  /// \details This function has constant complexity.
  ///          It is defined as !type_is_int() && !type_is_list().
  bool type_is_appl() const noexcept
  {
    return !type_is_int() && !type_is_list();
  }

  /// \brief Dynamic check whether the term is an aterm_int.
  /// \return True iff this term has internal structure of an aterm_int.
  /// \details This function has constant complexity.
  bool type_is_int() const noexcept
  {
    const function_symbol& f=m_term->function();
    return f == detail::g_as_int;
  }

  /// \brief Dynamic check whether the term is an aterm_list.
  /// \returns True iff this term has the structure of an term_list
  /// \details This function has constant complexity.
  bool type_is_list() const noexcept
  {
    const function_symbol& f=m_term->function();
    return f == detail::g_as_list || f == detail::g_as_empty_list;
  }

  /// \brief Comparison operator.
  /// \details Terms are stored in a maximally shared way. This
  ///         means that this equality operator can be calculated
  ///         in constant time.
  /// \return true iff t is equal to the current term.
  bool operator ==(const unprotected_aterm& t) const
  {
    return m_term == t.m_term;
  }

  /// \brief Inequality operator on two unprotected aterms.
  /// \details See note at the == operator. This operator requires constant time.
  /// \param t A term to which the current term is compared.
  /// \return false iff t is equal to the current term.
  bool operator !=(const unprotected_aterm& t) const
  {
    return m_term!=t.m_term;
  }

  /// \brief Comparison operator for two unprotected aterms.
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

  /// \brief Comparison operator for two unprotected aterms.
  /// \details This operator requires constant time. See note at the operator <.
  /// \param t A term to which the current term is compared.
  /// \return True iff the current term is larger than the argument.
  bool operator >(const unprotected_aterm& t) const
  {
    return m_term>t.m_term;
  }

  /// \brief Comparison operator for two unprotected aterms.
  /// \details This operator requires constant time. See note at the operator <.
  /// \param t A term to which the current term is compared.
  /// \return True iff the current term is smaller or equal than the argument.
  bool operator <=(const unprotected_aterm& t) const
  {
    return m_term<=t.m_term;
  }

  /// \brief Comparison operator for two unprotected aterms.
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

  /// \brief Yields the function symbol in an aterm.
  /// \returns The function symbol of the term, which can also be an AS_EMPTY_LIST,
  ///          AS_INT and AS_LIST.
  /// \details This is for internal use only.
  const function_symbol& function() const
  {
    return m_term->function();
  }
};

/// \brief The aterm base class that provides protection of the underlying shared terms.
/// \details Terms are protected using one of the following two invariants:
///          (1) A term that can be accessed is a subterm of a term with a reference count
///              larger than 0 (when reference counting is used). Or, 
///          (2) A term that can be accessed if it is a subterm of a term that occurs at
///              an address which exist in the protection set of a process, or which sits
///              in an atermpp container, which automatically is a container protection set.
///              Furthermore, every address in a protection set contains a valid term.
///          During garbage collection or rehashing, this situation is stable in the sense
///          that all terms that are protected remain protected until the end of the 
///          garbage collection or rehashing phase by the same address or term. This means
///          that during garbage collection no terms can be deleted, for instance in an
///          assignment, or in a destruct. 
//               
class aterm : public unprotected_aterm
{
public:

  /// \brief Default constructor.
  aterm() noexcept;

  /// \brief Standard destructor.
  ~aterm() noexcept;

  /// \brief Constructor based on an internal term data structure. This is not for public use.
  /// \details Takes ownership of the passed underlying term.
  /// \param t A pointer to an internal aterm data structure.
  /// \todo Should be protected, but this cannot yet be done due to a problem
  ///       in the compiling rewriter.
  explicit aterm(const detail::_aterm *t) noexcept;

  /// \brief Copy constructor.
  /// \param other The aterm that is copied.
  /// \details  This class has a non-trivial destructor so explicitly define the copy and move operators.
  aterm(const aterm& other) noexcept;

  /// \brief Move constructor.
  /// \param other The aterm that is moved into the new term. This term may have changed after this operation.
  /// \details This operation does not employ increments and decrements of reference counts and is therefore more
  ///          efficient than the standard copy construct.
  aterm(aterm&& other) noexcept;

  /// \brief Assignment operator.
  /// \param other The aterm that will be assigned.
  /// \return A reference to the assigned term.
  aterm& operator=(const aterm& other) noexcept;

  /// \brief Assignment operator, to be used if busy and forbidden flags are explicitly available.
  //  \detail This can be used as an optimisation, because it avoids getting access to thread local variables,
  //          which is as it stands relatively expensive. The effect is equal to the assignment operator =. 
  /// \param other The aterm that will be assigned.
  aterm& assign(const aterm& other,
                detail::thread_aterm_pool& pool) noexcept;

  /// \brief Assignment operator, to be used when the busy flags do not need to be set.
  /// \details This is only safe in the parallel context when the busy flag is already
  ///          known to be set. This is also checked by an assert. This can be used for
  ///          instance in a lambda function that is passed in a make_.... function, as
  ///          this unprotected assign will only be called when a term is constructed. 
  /// \param other The aterm that will be assigned.
  template <bool CHECK_BUSY_FLAG=true>
  aterm& unprotected_assign(const aterm& other) noexcept;

  /// \brief Move assignment operator.
  /// \param other The aterm that will be assigned.
  /// \return A reference to the assigned term.
  aterm& operator=(aterm&& other) noexcept;

#ifdef MCRL2_ATERMPP_REFERENCE_COUNTED
  /// \brief Obtain the reference count of this term.
  /// \detail This function is only intended for low level inspection of terms. 
  /// \return The current reference count of this term.
  std::size_t reference_count() const
  {
    return m_term->reference_count();
  }
#endif

protected:
#ifdef MCRL2_ATERMPP_REFERENCE_COUNTED
  /// \brief Increment the reference count.
  /// \details This increments the reference count unless the term contains null.
  ///          Use with care as this destroys the reference count mechanism.
  void increment_reference_count() const
  {
    if (defined())
    {
      m_term->increment_reference_count();
    }
  }

  /// \brief Decrement the reference count.
  /// \details This decrements the reference count unless the term contains null.
  ///          Use with care as this destroys the reference count mechanism.
  void decrement_reference_count() const
  {
    if (defined())
    {
      m_term->decrement_reference_count();
    }
  }
#endif
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
/// \return  A term of type const Derived&.
template <class Derived, class Base>
const Derived& down_cast(const Base& t,
                         typename std::enable_if<is_convertible<Base, Derived>::value &&
                                                 !std::is_base_of<Derived, Base>::value>::type* = nullptr)
{
  static_assert(sizeof(Derived) == sizeof(aterm),
                "aterm cast can only be applied ot types derived from aterms where no extra fields are added");
  assert(Derived(static_cast<const aterm&>(t)) != aterm());
  return reinterpret_cast<const Derived&>(t);
}

/// \brief A cast from one aterm based type to another, as a reference, allowing to assign to it.
//         This can be useful when assigning to a term type that contains the derived term type. 
/// \param   t A term of a type inheriting from an aterm.
/// \return  A term of type Derived&.
template <class Derived, class Base>
Derived& reference_cast(Base& t,
                        typename std::enable_if<is_convertible<Base, Derived>::value &&
                                                !std::is_base_of<Derived, Base>::value>::type* = nullptr)
{
  static_assert(sizeof(Base) == sizeof(aterm), 
                "aterm cast can only be applied to terms directly derived from aterms");
  static_assert(sizeof(Derived) == sizeof(aterm),
                "aterm cast can only be applied to types derived from aterms where no extra fields are added");
  // We do not check types as the content of the term t is likely to be overwritten shortly. 
  return reinterpret_cast<Derived&>(t);
}

template < typename DerivedCont, typename Base, template <typename Elem> class Cont >
const DerivedCont& container_cast(const Cont<Base>& t,
                              typename std::enable_if_t<
                                is_container<DerivedCont, aterm>::value &&
                                std::is_same_v<Cont<typename DerivedCont::value_type>, DerivedCont> &&
                                !std::is_base_of_v<DerivedCont, Cont<Base> > &&
                                is_convertible<Base, typename DerivedCont::value_type>::value
                              >* = nullptr)
{
  static_assert(sizeof(typename DerivedCont::value_type) == sizeof(aterm),
                "aterm cast cannot be applied types derived from aterms where extra fields are added");
  assert(std::all_of(t.begin(),t.end(),[](const Base& u){ return typename DerivedCont::value_type(static_cast<const aterm&>(u)) != aterm();} ));
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
  assert(Derived(static_cast<const aterm&>(t)) != aterm());
  return reinterpret_cast<const Derived&>(t);
}

template < typename DerivedCont, typename Base, template <typename Elem> class Cont >
const DerivedCont& vertical_cast(const Cont<Base>& t,
                              typename std::enable_if_t<
                                is_container<DerivedCont, aterm>::value &&
                                std::is_same_v<Cont<typename DerivedCont::value_type>, DerivedCont> &&
                                is_convertible<Base, typename DerivedCont::value_type>::value
                              >* = nullptr)
{
  static_assert(sizeof(typename DerivedCont::value_type) == sizeof(aterm),
                "aterm cast cannot be applied types derived from aterms where extra fields are added");
  assert(std::all_of(t.begin(),t.end(),[](const Base& u){ return typename DerivedCont::value_type(static_cast<const aterm&>(u)) != aterm();} ));
  return reinterpret_cast<const DerivedCont&>(t);
}

namespace detail
{
  /// \returns A pointer to the underlying aterm.
  inline _aterm* address(const unprotected_aterm& t)
  {
    return const_cast<_aterm*>(t.m_term);
  }
}

/// \brief Send the term in textual form to the ostream.
/// \param out The stream to which the term is sent. 
/// \param t   The term that is printed to the stream.
/// \return The stream to which the term is written.
std::ostream& operator<<(std::ostream& out, const atermpp::aterm& t);

/// \brief Transform an aterm to an ascii string.
/// \param t The input aterm.
/// \return A string representation of the given term derived from an aterm.
inline std::string pp(const atermpp::aterm& t)
{
  std::ostringstream oss;
  oss << t;
  return oss.str();
}

} // namespace atermpp

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

#endif // MCRL2_ATERMPP_ATERM_H
