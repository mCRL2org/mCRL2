// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm.h
/// \brief The term_appl class represents function application.

#ifndef MCRL2_ATERMPP_ATERM_APPL_H
#define MCRL2_ATERMPP_ATERM_APPL_H

#include "mcrl2/atermpp/aterm_core.h"
#include "mcrl2/atermpp/detail/aterm_appl_iterator.h"
#include "mcrl2/atermpp/detail/aterm_list.h"
#include "mcrl2/atermpp/detail/global_aterm_pool.h"
#include "mcrl2/utilities/type_traits.h"

namespace atermpp
{

class aterm: public aterm_core
{
protected:
  /// \brief Constructor.
  /// \param t A pointer internal data structure from which the term is constructed.
  /// \details This function is explicitly protected such that is not used in common code. 
  explicit aterm(detail::_term_appl *t)
   : aterm_core(reinterpret_cast<detail::_aterm*>(t))
  {}

public:
  /// An unsigned integral type.
  typedef std::size_t size_type;

  /// A signed integral type.
  typedef ptrdiff_t difference_type;

  /// Iterator used to iterate through an term_appl.
  typedef term_appl_iterator<aterm> iterator;

  /// Const iterator used to iterate through an term_appl.
  typedef term_appl_iterator<aterm> const_iterator;

  /// \brief Default constructor.
  aterm():aterm_core()
  {}

  /* /// \brief Explicit constructor from an aterm_core.
  /// \param t The aterm_core from which the term is constructed.
  explicit aterm(const aterm_core& t) 
   : aterm_core(t)
  {} */

  /// This class has user-declared copy constructor so declare default copy and move operators.
  aterm(const aterm& other) noexcept = default;
  aterm& operator=(const aterm& other) noexcept = default;
  aterm(aterm&& other) noexcept = default;
  aterm& operator=(aterm&& other) noexcept = default;

  /// \brief Constructor that provides an aterm based on a function symbol and forward iterator providing the arguments. 
  /// \details The iterator range is traversed more than once. If only one traversal is required
  ///          use term_appl with a TermConverter argument. But this function
  ///          is substantially less efficient.
  ///          The length of the iterator range must match the arity of the function symbol.
  /// \param sym A function symbol.
  /// \param begin The start of a range of elements.
  /// \param end The end of a range of elements.
  template <class ForwardIterator,
            typename std::enable_if<mcrl2::utilities::is_iterator<ForwardIterator>::value>::type* = nullptr,
            typename std::enable_if<!std::is_same<typename ForwardIterator::iterator_category, std::input_iterator_tag>::value>::type* = nullptr,
            typename std::enable_if<!std::is_same<typename ForwardIterator::iterator_category, std::output_iterator_tag>::value>::type* = nullptr>
  aterm(const function_symbol& sym,
            ForwardIterator begin,
            ForwardIterator end)
  {
    detail::g_thread_term_pool().create_appl_dynamic(*this, sym, begin, end);
    static_assert(!std::is_same<typename ForwardIterator::iterator_category, std::input_iterator_tag>::value,
                  "A forward iterator has more requirements than an input iterator.");
    static_assert(!std::is_same<typename ForwardIterator::iterator_category, std::output_iterator_tag>::value,
                  "A forward iterator has more requirements than an output iterator.");
  }

  /// \brief Constructor that provides an aterm based on a function symbol and an input iterator providing the arguments. 
  /// \details The given iterator is traversed only once. So it can be used with an input iterator.
  ///          This means that the TermConverter is applied exactly once to each element.
  ///          The length of the iterator range must be equal to the arity of the function symbol.
  /// \param sym A function symbol.
  /// \param begin The start of a range of elements.
  /// \param end The end of a range of elements.
  template <class InputIterator,
            typename std::enable_if<mcrl2::utilities::is_iterator<InputIterator>::value>::type* = nullptr,
            typename std::enable_if<std::is_same<typename InputIterator::iterator_category, std::input_iterator_tag>::value>::type* = nullptr>
  aterm(const function_symbol& sym,
            InputIterator begin,
            InputIterator end)
    : aterm(sym, begin, end, [](const unprotected_aterm_core& term) -> const unprotected_aterm_core& { return term; } )
  {
    static_assert(std::is_same<typename InputIterator::iterator_category, std::input_iterator_tag>::value,
                  "The InputIterator is missing the input iterator tag.");
  }

  /// \details The given iterator is traversed only once. So it can be used with an input iterator.
  ///          This means that the TermConverter is applied exactly once to each element.
  ///          The length of the iterator range must be equal to the arity of the function symbol.
  /// \param sym A function symbol.
  /// \param begin The start of a range of elements.
  /// \param end The end of a range of elements.
  /// \param converter An class or lambda term containing an operator Term operator()(const Term& t) which is
  ///        applied to each each element in the iterator range before it becomes an argument of this term.
  template <class InputIterator,
            class TermConverter,
            typename std::enable_if<mcrl2::utilities::is_iterator<InputIterator>::value>::type* = nullptr>
  aterm(const function_symbol& sym,
            InputIterator begin,
            InputIterator end,
            TermConverter converter)
  {
    detail::g_thread_term_pool().create_appl_dynamic(*this, sym, converter, begin, end);
    static_assert(!std::is_same<typename InputIterator::iterator_category, std::output_iterator_tag>::value,
                  "The InputIterator has the output iterator tag.");
  }

  /// \brief Constructor.
  /// \param sym A function symbol.
  aterm(const function_symbol& sym)
  {
    detail::g_thread_term_pool().create_term(*this, sym);
  }

  /// \brief Constructor for n-arity function application.
  /// \param symbol A function symbol.
  /// \param arguments The arguments of the function application.
  template<typename ...Terms>
  aterm(const function_symbol& symbol, const Terms& ...arguments)
  {
    detail::g_thread_term_pool().create_appl(*this, symbol, arguments...);
  }

  /// \brief Returns the function symbol belonging to an aterm.
  /// \return The function symbol of this term.
  const function_symbol& function() const
  {
    return m_term->function();
  }

  /// \brief Returns the number of arguments of this term.
  /// \return The number of arguments of this term.
  size_type size() const
  {
    return m_term->function().arity();
  }

  /// \brief Returns true if the term has no arguments.
  /// \return True if this term has no arguments.
  bool empty() const
  {
    return size()==0;
  }

  /// \brief Returns an iterator pointing to the first argument.
  /// \return An iterator pointing to the first argument.
  const_iterator begin() const
  {
    return const_iterator(&static_cast<const aterm&>(reinterpret_cast<const detail::_term_appl*>(m_term)->arg(0)));
  }

  /// \brief Returns a const_iterator pointing past the last argument.
  /// \return A const_iterator pointing past the last argument.
  const_iterator end() const
  {
    return const_iterator(&static_cast<const aterm&>(reinterpret_cast<const detail::_term_appl*>(m_term)->arg(size())));
  }

  /// \brief Returns the largest possible number of arguments.
  /// \return The largest possible number of arguments.
  constexpr size_type max_size() const
  {
    return std::numeric_limits<size_type>::max();
  }

  /// \brief Returns the i-th argument.
  /// \param i A positive integer.
  /// \return The argument with the given index.
  const aterm& operator[](const size_type i) const
  {
    assert(i < size()); // Check the bounds.
    return static_cast<const aterm&>(reinterpret_cast<const detail::_term_appl*>(m_term)->arg(i));
  }
};

typedef void(*term_callback)(const aterm&);

extern void add_deletion_hook(const function_symbol&, term_callback);


/// \brief Constructor an aterm in a variable based on a function symbol and an forward iterator providing the arguments. 
/// \details The iterator range is traversed more than once. If only one traversal is required
///          use term_appl with a TermConverter argument. But this function
///          is substantially less efficient.
///          The length of the iterator range must match the arity of the function symbol.
/// \param target The variable in which the result will be put. This variable may be used for scratch purposes.
/// \param sym A function symbol.
/// \param begin The start of a range of elements.
/// \param end The end of a range of elements.
template <class Term,
          class ForwardIterator,
          typename std::enable_if<mcrl2::utilities::is_iterator<ForwardIterator>::value>::type* = nullptr,
          typename std::enable_if<!std::is_same<typename ForwardIterator::iterator_category, std::input_iterator_tag>::value>::type* = nullptr,
          typename std::enable_if<!std::is_same<typename ForwardIterator::iterator_category, std::output_iterator_tag>::value>::type* = nullptr>
void make_term_appl(Term& target,
                    const function_symbol& sym,
                    ForwardIterator begin,
                    ForwardIterator end)
{
  detail::g_thread_term_pool().create_appl_dynamic(target, sym, begin, end);
  
  static_assert((std::is_base_of<aterm, Term>::value),"Term must be derived from an aterm");
  static_assert(sizeof(Term)==sizeof(std::size_t),"Term derived from an aterm must not have extra fields");
  static_assert(!std::is_same<typename ForwardIterator::iterator_category, std::input_iterator_tag>::value,
                "A forward iterator has more requirements than an input iterator.");
  static_assert(!std::is_same<typename ForwardIterator::iterator_category, std::output_iterator_tag>::value,
                "A forward iterator has more requirements than an output iterator.");
}


/// \brief Constructor an aterm in a variable based on a function symbol and an input iterator providing the arguments. 
/// \details The given iterator is traversed only once. So it can be used with an input iterator.
///          This means that the TermConverter is applied exactly once to each element.
///          The length of the iterator range must be equal to the arity of the function symbol.
/// \param target The variable in which the result will be put. This variable may be used for scratch purposes.
/// \param sym A function symbol.
/// \param begin The start of a range of elements.
/// \param end The end of a range of elements.
template <class Term,
          class InputIterator,
          typename std::enable_if<mcrl2::utilities::is_iterator<InputIterator>::value>::type* = nullptr,
          typename std::enable_if<std::is_same<typename InputIterator::iterator_category, std::input_iterator_tag>::value>::type* = nullptr>
void make_term_appl(Term& target,
                    const function_symbol& sym,
                    InputIterator begin,
                    InputIterator end)
{
  make_term_appl(target, sym, begin, end, [](const Term& term) -> const Term& { return term; } );

  static_assert((std::is_base_of<aterm, Term>::value),"Term must be derived from an aterm");
  static_assert(sizeof(Term)==sizeof(std::size_t),"Term derived from an aterm must not have extra fields");
  static_assert(std::is_same<typename InputIterator::iterator_category, std::input_iterator_tag>::value,
                "The InputIterator is missing the input iterator tag.");
}

/// \brief Constructor an aterm in a variable based on a function symbol and an forward iterator providing the arguments. 
/// \details The given iterator is traversed only once. So it can be used with an input iterator.
///          This means that the TermConverter is applied exactly once to each element.
///          The length of the iterator range must be equal to the arity of the function symbol.
/// \param target The variable in which the result will be put. This variable may be used for scratch purposes.
/// \param sym A function symbol.
/// \param begin The start of a range of elements.
/// \param end The end of a range of elements.
/// \param converter An class or lambda term containing an operator Term operator()(const Term& t) which is
///        applied to each each element in the iterator range before it becomes an argument of this term.
template <class Term,
          class InputIterator,
          class TermConverter,
          typename std::enable_if<mcrl2::utilities::is_iterator<InputIterator>::value>::type* = nullptr>
void make_term_appl(Term& target,
                    const function_symbol& sym,
                    InputIterator begin,
                    InputIterator end,
                    TermConverter converter)
{
  detail::g_thread_term_pool().create_appl_dynamic(target, sym, converter, begin, end);

  static_assert(std::is_base_of<aterm, Term>::value,"Term must be derived from an aterm");
  static_assert(sizeof(Term)==sizeof(std::size_t),"Term derived from an aterm must not have extra fields");
  static_assert(!std::is_same<typename InputIterator::iterator_category, std::output_iterator_tag>::value,
                "The InputIterator has the output iterator tag.");
}

/// \brief Make an term_appl consisting of a single function symbol. 
/// \param target The variable in which the result will be put. This variable may be used for scratch purposes.
/// \param sym A function symbol.
template <class Term>
void make_term_appl(Term& target,
                    const function_symbol& sym)
{
  detail::g_thread_term_pool().create_term(target, sym);

  static_assert(std::is_base_of<aterm, Term>::value,"Term must be derived from an aterm");
  static_assert(sizeof(Term)==sizeof(std::size_t),"Term derived from an aterm must not have extra fields");
}

/// \brief Make an aterm application for n-arity function application.
/// \param target The variable in which the result will be put. This variable may be used for scratch purposes.
/// \param symbol A function symbol.
/// \param arguments The arguments of the function application.
template<class Term,
         typename ...Terms>
void make_term_appl(Term& target, const function_symbol& symbol, const Terms& ...arguments)
{
  detail::g_thread_term_pool().create_appl(target, symbol, arguments...);
}

/// \brief Constructor for n-arity function application with an index.
/// \param target The variable in which the result will be put. This variable may be used for scratch purposes.
/// \param symbol A function symbol.
/// \param arguments The arguments of the function application.
template<class Term,
         class INDEX_TYPE,
         typename ...Terms>
void make_term_appl_with_index(aterm& target, const function_symbol& symbol, const Terms& ...arguments)
{
  detail::g_thread_term_pool().create_appl_index<Term, INDEX_TYPE>(target, symbol, arguments...);
}

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
                "aterm cast can only be applied to types derived from aterms where no extra fields are added");
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
                                                !std::is_base_of<Derived, Base>::value >::type* = nullptr)
{
  static_assert(sizeof(Base) == sizeof(aterm), 
                "aterm cast can only be applied to terms directly derived from aterms");
  static_assert(sizeof(Derived) == sizeof(aterm),
                "aterm cast can only be applied to types derived from aterms where no extra fields are added");
  // We do not check types as the content of the term t is likely to be overwritten shortly. 
  return reinterpret_cast<Derived&>(t);
}

/// \brief A cast from one aterm based type to another, as a reference, allowing to assign to it.
//         This can be useful when assigning to a term type that contains the derived term type. 
//         In case Derived and Base are equal, nothing needs to be done. 
/// \param   t A term of a type inheriting from an aterm.
/// \return  A term of type Derived&.
template <class Derived>
Derived& reference_cast(Derived& t)
{
  static_assert(sizeof(Derived) == sizeof(aterm), 
                "aterm cast can only be applied to terms directly derived from aterms");
  // We do not check types as the content of the term t is likely to be overwritten shortly. 
  return t;
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

/* namespace detail
{
  /// \returns A pointer to the underlying aterm.
  inline _aterm* address(const unprotected_aterm_core& t)
  {
    return const_cast<_aterm*>(t.m_term);
  }
} */

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
inline void swap(atermpp::unprotected_aterm_core& t1, atermpp::unprotected_aterm_core& t2) noexcept
{
  t1.swap(t2);
}
} // namespace std
namespace std
{

/// \brief Swaps two term_applss.
/// \details This operation is more efficient than exchanging terms by an assignment,
///          as swapping does not require to change the protection of terms.
/// \param t1 The first term.
/// \param t2 The second term.
inline void swap(atermpp::aterm& t1, atermpp::aterm& t2) noexcept
{
  t1.swap(t2);
}

/// \brief Standard hash function.
template<> struct hash<atermpp::aterm>
{
  std::size_t operator()(const atermpp::aterm& t) const
  {
    return std::hash<atermpp::aterm_core>()(t);
  }
};

} // namespace std

#endif // MCRL2_ATERMPP_ATERM_APPL_H
