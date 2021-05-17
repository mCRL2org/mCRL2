// Author(s): Wieger Wesselink, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_ATERMPP_ATERM_LIST_H
#define MCRL2_ATERMPP_ATERM_LIST_H

#include "mcrl2/atermpp/detail/aterm_list.h"
#include "mcrl2/atermpp/detail/aterm_list_iterator.h"
#include "mcrl2/atermpp/type_traits.h"

namespace atermpp
{

/// \brief A list of aterm objects.
template <typename Term>
class term_list: public aterm
{
protected:
  /// \brief Constructor for term lists from internally constructed terms delivered as reference.
  explicit term_list(detail::_aterm* t) noexcept :aterm(t)
  {
    assert(!defined() || type_is_list());
  }

public:
  /// The type of object, T stored in the term_list.
  typedef Term value_type;

  /// Pointer to T.
  typedef Term* pointer;

  /// Reference to T.
  typedef Term& reference;

  /// Const reference to T.
  typedef const Term& const_reference;

  /// An unsigned integral type.
  typedef std::size_t size_type;

  /// A signed integral type.
  typedef ptrdiff_t difference_type;

  /// Iterator used to iterate through an term_list.
  typedef term_list_iterator<Term> iterator;

  /// Const iterator used to iterate through an term_list.
  typedef term_list_iterator<Term> const_iterator;

  /// \brief Default constructor. Creates an empty list.
  term_list() noexcept
    : aterm(detail::g_term_pool().empty_list())
  {}

  /// \brief Copy constructor.
  /// \param t A list.
  term_list(const term_list<Term>& t) noexcept
    : aterm(t)
  {
    assert(!defined() || type_is_list());
  }

  /// \brief Move constructor.
  /// \param t A list.
  term_list(term_list<Term>&& t) noexcept
    : aterm(std::move(t))
  {
    assert(!defined() || type_is_list());
  }

  /// This class has user-declared copy constructor so declare copy and move assignment.
  term_list& operator=(const term_list& other) noexcept = default;
  term_list& operator=(term_list&& other) noexcept = default;

  /// \brief Creates a term_list with the elements from first to last.
  /// \details It is assumed that the range can be traversed from last to first.
  /// \param first The start of a range of elements.
  /// \param last The end of a range of elements.
  template <class Iter>
  explicit term_list(Iter first, Iter last, typename std::enable_if<std::is_base_of<
                std::bidirectional_iterator_tag,
                typename std::iterator_traits<Iter>::iterator_category
            >::value>::type* = nullptr) :
      aterm(detail::make_list_backward<Term,Iter,
                detail::do_not_convert_term<Term> >(first, last,detail::do_not_convert_term<Term>()))
  {
    assert(!defined() || type_is_list());
  }

  /// \brief Creates a term_list with the elements from first to last converting the elements before inserting.
  /// \details It is assumed that the range can be traversed from last to first. The operator () in the class
  ///          ATermConverter is applied to each element before inserting it in the list.
  /// \param first The start of a range of elements.
  /// \param last The end of a range of elements.
  /// \param convert_to_aterm A class with a () operation, which is applied to each element
  ///                   before it is put into the list.
  template <class Iter, class ATermConverter>
  explicit term_list(Iter first, Iter last, const ATermConverter& convert_to_aterm,
            typename std::enable_if<std::is_base_of<
              std::bidirectional_iterator_tag,
              typename std::iterator_traits<Iter>::iterator_category
            >::value>::type* = 0):
       aterm(detail::make_list_backward<Term,Iter,ATermConverter>(first, last, convert_to_aterm))
  {
    assert(!defined() || type_is_list());
  }

  /// \brief Creates a term_list with the elements from first to last, converting and filtering the list.
  /// \details It is assumed that the range can be traversed from last to first. The operator () in the class
  ///          ATermConverter is applied to each element before inserting it in the list. Elements are only
  ///          inserted if the operator () of the class ATermFilter yields true when applied to such an element.
  /// \param first The start of a range of elements.
  /// \param last The end of a range of elements.
  /// \param convert_to_aterm A class with a () operation, which is applied to each element
  ///                   before it is put into the list.
  /// \param aterm_filter A class with an operator () that is used to determine whether elements can be inserted in the list.
  template <class Iter, class ATermConverter, class ATermFilter>
  explicit term_list(Iter first, Iter last, const ATermConverter& convert_to_aterm, const ATermFilter& aterm_filter,
            typename std::enable_if<std::is_base_of<
              std::bidirectional_iterator_tag,
              typename std::iterator_traits<Iter>::iterator_category
            >::value>::type* = 0):
       aterm(detail::make_list_backward<Term,Iter,ATermConverter,ATermFilter>(first, last, convert_to_aterm, aterm_filter))
  {
    assert(!defined() || type_is_list());
  }

  /// \brief Creates a term_list from the elements from first to last.
  /// \details The range is traversed from first to last. This requires
  ///           to copy the elements internally, which is less efficient
  ///           than this function with random access iterators as arguments.
  /// \param first The start of a range of elements.
  /// \param last The end of a range of elements.
  template <class Iter>
  explicit term_list(Iter first, Iter last,
                     typename std::enable_if< !std::is_base_of<
                       std::bidirectional_iterator_tag,
                       typename std::iterator_traits<Iter>::iterator_category
                     >::value>::type* = nullptr):
       aterm(detail::make_list_forward<Term,Iter,detail::do_not_convert_term<Term> >
                               (first, last, detail::do_not_convert_term<Term>()))
  {
    assert(!defined() || type_is_list());
  }

  /// \brief Creates a term_list from the elements from first to last converting the elements before inserting.
  /// \details The range is traversed from first to last. This requires
  ///           to copy the elements internally, which is less efficient
  ///           than this function with random access iterators as arguments.
  ///           The operator () in the class
  ///           ATermConverter is applied to each element before inserting it in the list.
  /// \param first The start of a range of elements.
  /// \param last The end of a range of elements.
  /// \param convert_to_aterm A class with a () operation, whic is applied to each element
  ///                      before it is put into the list.
  template <class Iter, class  ATermConverter>
  explicit term_list(Iter first, Iter last, const ATermConverter& convert_to_aterm,
                     typename std::enable_if< !std::is_base_of<
                       std::bidirectional_iterator_tag,
                       // std::random_access_iterator_tag,
                       typename std::iterator_traits<Iter>::iterator_category
                     >::value>::type* = nullptr):
       aterm(detail::make_list_forward<Term,Iter,ATermConverter>
                               (first, last, convert_to_aterm))
  {
    assert(!defined() || type_is_list());
  }

  /// \brief Creates a term_list from the elements from first to last converting and filtering the elements before inserting.
  /// \details The range is traversed from first to last. This requires
  ///           to copy the elements internally, which is less efficient
  ///           than this function with random access iterators as arguments.
  ///           The operator () in the class ATermConverter is applied to
  ///           each element before inserting it in the list. Elements are only
  ///           inserted if the operator () of the class ATermFilter yields true when applied to such an element.
  /// \param first The start of a range of elements.
  /// \param last The end of a range of elements.
  /// \param convert_to_aterm A class with a () operation, whic is applied to each element
  ///                      before it is put into the list.
  /// \param aterm_filter A class with an operator () that is used to determine whether elements can be inserted in the list.
  template <class Iter, class  ATermConverter, class ATermFilter>
  explicit term_list(Iter first, Iter last, const ATermConverter& convert_to_aterm, const ATermFilter& aterm_filter,
                     typename std::enable_if< !std::is_base_of<
                       std::random_access_iterator_tag,
                       typename std::iterator_traits<Iter>::iterator_category
                     >::value>::type* = nullptr):
       aterm(detail::make_list_forward<Term,Iter,ATermConverter>
                               (first, last, convert_to_aterm, aterm_filter))
  {
    assert(!defined() || type_is_list());
  }

  /// \brief A constructor based on an initializer list.
  /// \details This constructor is not made explicit to conform to initializer lists in standard containers.
  /// \param init The initialiser list.
  term_list(std::initializer_list<Term> init)
    : aterm(detail::make_list_backward<Term,
                                       typename std::initializer_list<Term>::const_iterator,
                                       detail::do_not_convert_term<Term> >
                (init.begin(), init.end(), detail::do_not_convert_term<Term>()))
  {
    assert(!defined() || type_is_list());
  }

  /// \brief Returns the tail of the list.
  /// \return The tail of the list.
  const term_list<Term>& tail() const
  {
    assert(!empty());
    return (static_cast<const detail::_aterm_list<Term>&>(*m_term)).tail();
  }

  /// \brief Removes the first element of the list.
  void pop_front()
  {
    *this = tail();
  }

  /// \brief Returns the first element of the list.
  /// \return The term at the head of the list.
  const Term& front() const
  {
    return static_cast<const detail::_aterm_list<Term>&>(*m_term).head();
  }

  /// \brief Inserts a new element at the beginning of the current list.
  /// \param el The term that is added.
  void push_front(const Term& el);

  /// \brief Construct and insert a new element at the beginning of the current list.
  /// \param el The term that is added.
  template<typename ...Args>
  void emplace_front(Args&&... arguments);

  /// \brief Returns the size of the term_list.
  /// \details The complexity of this function is linear in the size of the list.
  /// \return The size of the list.
  size_type size() const
  {
    std::size_t size=0;
    for(const_iterator i=begin(); i!=end(); ++i)
    {
      ++size;
    }
    return size;
  }

  /// \brief Returns true if the list's size is 0.
  /// \return True iff the list is empty.
  bool empty() const
  {
    return m_term->function() == detail::g_term_pool().as_empty_list();
  }

  /// \brief Returns a const_iterator pointing to the beginning of the term_list.
  /// \return The beginning of the list.
  const_iterator begin() const
  {
    return const_iterator(m_term);
  }

  /// \brief Returns a const_iterator pointing to the end of the term_list.
  /// \return The end of the list.
  const_iterator end() const
  {
    return const_iterator(detail::address(detail::g_term_pool().empty_list()));
  }

  /// \brief Returns the largest possible size of the term_list.
  /// \return The largest possible size of the list.
  size_type max_size() const
  {
    return std::numeric_limits<std::size_t>::max();
  }
};

/// \cond INTERNAL_DOCS
namespace detail
{

/// \brief Template specialization to make a term_list recognizable as a container type (see
///        type_traits.h and detail/type_traits_impl.h).
template < typename T >
struct is_container_impl< atermpp::term_list< T > > : public std::true_type
{ };


template <class Term>
class _aterm_list : public _aterm_appl<2>
{
public:
  /// \returns A reference to the head of the list.
  const Term& head() const { return static_cast<const Term&>(arg(0)); }

  /// \returns A reference to the tail of the list.
  const term_list<Term>& tail() const { return static_cast<const term_list<Term>&>(arg(1)); }
};

} // namespace detail
/// \endcond


/// \brief A term_list with elements of type aterm.
typedef term_list<aterm> aterm_list;


/// \brief Returns the list with the elements in reversed order.
/// \param l A list.
/// \details This operator is linear in the size of the list.
/// \return The reversed list.
template <typename Term>
inline
term_list<Term> reverse(const term_list<Term>& l);

/// \brief Returns the list with the elements sorted according to the <-operator on the addresses of terms. 
/// \param l A list.
/// \param ordering An total orderings relation on Term, by default the ordering relation on Terms. 
/// \details This operator has complexity nlog n where n is the size of the list.
/// \return The sorted list.
template <typename Term>
inline
term_list<Term> sort_list(const term_list<Term>& l, 
                          const std::function<bool(const Term&, const Term&)>& ordering 
                                      = [](const Term& t1, const Term& t2){ return t1<t2;});


/// \brief Returns the concatenation of two lists with convertible element types.
///  \details The type of the result is either the type of l, if the elements of m
///           can be converted implicitly to the type of the elements of l. Otherwise if the
///           elements of l can be converted implicitly to the type of the elements
///           of m, the result type is that or m.
/// \param l A list.
/// \param m A list.
/// \details The complexity of this operator is linear in the length of l.
/// \return The concatenation of the lists l followed by m.

template <typename Term1, typename Term2>
inline
typename std::conditional<std::is_convertible<Term2,Term1>::value,term_list<Term1>,term_list<Term2>>::type
operator+(const term_list<Term1>& l, const term_list<Term2>& m);


/// \brief Appends a new element at the end of the list. Note
///        that the complexity of this function is O(n), with n the number of
///        elements in the list!!!
/// \param l The list to which the term is appended.
/// \param el A term.
/// \return The list l with elem appended at the end.
template <typename Term>
inline
term_list<Term> push_back(const term_list<Term>& l, const Term& el);

/// \brief Converts the given term list to a vector.
template <typename T>
std::vector<T> as_vector(const atermpp::term_list<T>& x)
{
  return std::vector<T>(x.begin(), x.end());
}

/// \brief Converts the given term list to a set.
template <typename T>
std::set<T> as_set(const atermpp::term_list<T>& x)
{
  return std::set<T>(x.begin(), x.end());
}

} // namespace atermpp


namespace std
{
//
/// \brief Swaps two term_lists.
/// \details This operation is more efficient than exchanging terms by an assignment,
///          as swapping does not require to change the protection of terms.
/// \param t1 The first term
/// \param t2 The second term
template <class T>
inline void swap(atermpp::term_list<T>& t1, atermpp::term_list<T>& t2) noexcept
{
  t1.swap(t2);
}

/// \brief The standard hash class.
template <class Term>
struct hash<atermpp::term_list<Term> >
{
  /// \brief A specialization of the standard std::hash function.
  /// \param l The list for which a hash value is calculated.
  /// \return A hash value for l.
  std::size_t operator()(const atermpp::term_list<Term>& l) const
  {
    std::hash<atermpp::aterm> hasher;
    return hasher(l);
  }
};

} // namespace std

#include "mcrl2/atermpp/detail/aterm_list_implementation.h"

#endif // MCRL2_ATERMPP_ATERM_LIST_H
