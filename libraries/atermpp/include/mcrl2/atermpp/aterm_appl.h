// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm_appl.h
/// \brief The term_appl class represents function application.

#ifndef MCRL2_ATERMPP_ATERM_APPL_H
#define MCRL2_ATERMPP_ATERM_APPL_H

#include <unistd.h>
#include <stack>
#include <limits>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include "mcrl2/atermpp/detail/aterm_appl_iterator.h"
#include "mcrl2/atermpp/aterm.h"


namespace atermpp
{

template <class Term>
class term_appl:public aterm
{

  protected:
    /// \brief Constructor.
    term_appl (const detail::_aterm_appl<Term> *t):aterm(reinterpret_cast<const detail::_aterm*>(t))
    {
      BOOST_STATIC_ASSERT((boost::is_base_of<aterm, Term>::value));
      BOOST_STATIC_ASSERT(sizeof(Term)==sizeof(size_t));
    }

  public:
    /// The type of object, T stored in the term_appl.
    typedef Term value_type;

    /// Pointer to T.
    typedef Term* pointer;

    /// Reference to T.
    typedef Term& reference;

    /// Const reference to T.
    typedef const Term const_reference;

    /// An unsigned integral type.
    typedef size_t size_type;

    /// A signed integral type.
    typedef ptrdiff_t difference_type;

    /// Iterator used to iterate through an term_appl.
    typedef term_appl_iterator<Term> iterator;

    /// Const iterator used to iterate through an term_appl.
    typedef term_appl_iterator<Term> const_iterator;

    /// \brief Default constructor.
    term_appl():aterm()
    {}

    /// \brief Copy constructor from an aterm_appl.
    /// \param t The aterm.
    term_appl (const term_appl &t):aterm(t)
    {
      BOOST_STATIC_ASSERT((boost::is_base_of<aterm, Term>::value));
      BOOST_STATIC_ASSERT(sizeof(Term)==sizeof(size_t));
    }

    /// \brief Explicit constructor from an aterm.
    /// \param t The aterm.
    explicit term_appl (const aterm &t):aterm(t)
    {
      BOOST_STATIC_ASSERT((boost::is_base_of<aterm, Term>::value));
      BOOST_STATIC_ASSERT(sizeof(Term)==sizeof(size_t));
    }

    /// \brief Constructor.
    /// \details The iterator range is traversed more than once. If only one traversal is required
    //           use term_appl with a ATermConverter argument. But this function
    //           is substantially less efficient.
    ///          The length of the iterator range should must match the arity of the function symbol.
    /// \param sym A function symbol.
    /// \param begin The start of a range of elements.
    /// \param end The end of a range of elements.

    template <class ForwardIterator>
    term_appl(const function_symbol &sym,
              const ForwardIterator begin,
              const ForwardIterator end,
              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, ForwardIterator>::type>::type* = 0)
        :aterm(detail::local_term_appl<Term,ForwardIterator>(sym,begin,end))
    {
      BOOST_STATIC_ASSERT((boost::is_base_of<aterm, Term>::value));
      BOOST_STATIC_ASSERT(sizeof(Term)==sizeof(size_t));
    }

    /// \brief Constructor.
    /// \details The iterator range is traversed only once, assuming Iter is a forward iterator.
    ///          This means that the ATermConverter is applied exactly once to each element.
    ///          The length of the iterator range must be equal to the arity of the function symbol.
    /// \param sym A function symbol.
    /// \param begin The start of a range of elements.
    /// \param end The end of a range of elements.
    /// \param convert_to_aterm An optional translator that is applied to each element in the iterator range,
    //                              and which must translate these elements to type Term.
    template <class InputIterator, class ATermConverter>
    term_appl(const function_symbol &sym,
              InputIterator begin,
              InputIterator end,
              const ATermConverter &convert_to_aterm,
              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, InputIterator>::type>::type* = 0,
              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, ATermConverter>::type>::type* = 0)
         :aterm(detail::local_term_appl_with_converter<Term,InputIterator,ATermConverter>(sym,begin,end,convert_to_aterm))
    {
      BOOST_STATIC_ASSERT((boost::is_base_of<aterm, Term>::value));
      BOOST_STATIC_ASSERT(sizeof(Term)==sizeof(size_t));
    }

    /// \brief Constructor.
    /// \param sym A function symbol.
    term_appl(const function_symbol &sym)
         :aterm(detail::term_appl0(sym))
    {
      BOOST_STATIC_ASSERT((boost::is_base_of<aterm, Term>::value));
      BOOST_STATIC_ASSERT(sizeof(Term)==sizeof(size_t));
    }

    /// \brief Constructor for a unary function application.
    /// \param sym A function symbol.
    /// \param t1 The first argument.
    term_appl(const function_symbol &sym, const Term &t1)
         :aterm(detail::term_appl1<Term>(sym,t1))
    {
      BOOST_STATIC_ASSERT((boost::is_base_of<aterm, Term>::value));
      BOOST_STATIC_ASSERT(sizeof(Term)==sizeof(size_t));
    }

    /// \brief Constructor for a binary function application.
    /// \param sym A function symbol.
    /// \param t1 The first argument.
    /// \param t2 The second argument.
    term_appl(const function_symbol &sym, const Term &t1, const Term &t2)
         :aterm(detail::term_appl2<Term>(sym,t1,t2))
    {
      BOOST_STATIC_ASSERT((boost::is_base_of<aterm, Term>::value));
      BOOST_STATIC_ASSERT(sizeof(Term)==sizeof(size_t));
    }

    /// \brief Constructor for a ternary function application.
    /// \param sym A function symbol.
    /// \param t1 The first argument.
    /// \param t2 The second argument.
    /// \param t3 The third argument.
    term_appl(const function_symbol &sym, const Term &t1, const Term &t2, const Term &t3)
         :aterm(detail::term_appl3<Term>(sym,t1,t2,t3))
    {
      BOOST_STATIC_ASSERT((boost::is_base_of<aterm, Term>::value));
      BOOST_STATIC_ASSERT(sizeof(Term)==sizeof(size_t));
    }

    /// \brief Constructor for a function application to four arguments.
    /// \param sym A function symbol.
    /// \param t1 The first argument.
    /// \param t2 The second argument.
    /// \param t3 The third argument.
    /// \param t4 The fourth argument.
    term_appl(const function_symbol &sym, const Term &t1, const Term &t2, const Term &t3, const Term &t4)
         :aterm(detail::term_appl4<Term>(sym,t1,t2,t3,t4))
    {
      BOOST_STATIC_ASSERT((boost::is_base_of<aterm, Term>::value));
      BOOST_STATIC_ASSERT(sizeof(Term)==sizeof(size_t));
    }

    /// \brief Constructor for a function application to five arguments.
    /// \param sym A function symbol.
    /// \param t1 The first argument.
    /// \param t2 The second argument.
    /// \param t3 The third argument.
    /// \param t4 The fourth argument.
    /// \param t5 The fifth argument.
    term_appl(const function_symbol &sym, const Term &t1, const Term &t2, const Term &t3, const Term &t4, const Term &t5)
         :aterm(detail::term_appl5<Term>(sym,t1,t2,t3,t4,t5))
    {
      BOOST_STATIC_ASSERT((boost::is_base_of<aterm, Term>::value));
      BOOST_STATIC_ASSERT(sizeof(Term)==sizeof(size_t));
    }

    /// \brief Constructor for a function application to six arguments.
    /// \param sym A function symbol.
    /// \param t1 The first argument.
    /// \param t2 The second argument.
    /// \param t3 The third argument.
    /// \param t4 The fourth argument.
    /// \param t5 The fifth argument.
    /// \param t6 The sixth argument.
    term_appl(const function_symbol &sym, const Term &t1, const Term &t2, const Term &t3, const Term &t4, const Term &t5, const Term &t6)
         :aterm(detail::term_appl6<Term>(sym,t1,t2,t3,t4,t5,t6))
    {
      BOOST_STATIC_ASSERT((boost::is_base_of<aterm, Term>::value));
      BOOST_STATIC_ASSERT(sizeof(Term)==sizeof(size_t));
    }

    /// \brief The assignment operator
    /// \param t The assigned term
    /// \return A reference to the term to which an assignment took place.
    term_appl &operator=(const term_appl &t)
    {
      copy_term(t);
      return *this;
    }

    /// \brief Returns the function symbol belonging to an aterm_appl.
    /// \return The function symbol of this term.
    const function_symbol &function() const
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
      return const_iterator(&(reinterpret_cast<const detail::_aterm_appl<Term>*>(m_term)->arg[0]));
    }

    /// \brief Returns a const_iterator pointing past the last argument.
    /// \return A const_iterator pointing past the last argument.
    const_iterator end() const
    {
      return const_iterator(&reinterpret_cast<const detail::_aterm_appl<Term>*>(m_term)->arg[size()]);
    }

    /// \brief Returns the largest possible number of arguments.
    /// \return The largest possible number of arguments.
    size_type max_size() const
    {
      return (std::numeric_limits<size_type>::max)();
    }

    /// \brief Returns the i-th argument.
    /// \param i A positive integer
    /// \return The argument with the given index.
    const Term &operator[](const size_type i) const
    {
      assert(i<m_term->function().arity());
      return reinterpret_cast<const detail::_aterm_appl<Term>*>(m_term)->arg[i];
    }
};

typedef term_appl<aterm> aterm_appl;

/// \brief Swaps two term_applss.
/// \details This operation is more efficient than exchanging terms by an assignment,
///          as swapping does not require to change the protection of terms.
/// \param t1 The first term
/// \param t2 The second term
template <class T>
inline void swap(atermpp::term_appl<T> &t1, atermpp::term_appl<T> &t2)
{
  t1.swap(t2);
}

} // namespace atermpp

#include "mcrl2/atermpp/detail/aterm_appl_implementation.h"

#endif // MCRL2_ATERMPP_ATERM_APPL_H
