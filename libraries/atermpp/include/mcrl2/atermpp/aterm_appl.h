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
#include <boost/type_traits/is_convertible.hpp>
#include "mcrl2/atermpp/detail/aterm_appl_iterator.h"


namespace atermpp
{

template <class Term>
class term_appl:public aterm
{
  public: // Should become protected.
    detail::_aterm_appl<Term> & operator *() const
    {
      // Note that this operator can be applied on a NULL pointer, i.e., in the case &*m_term is checked,
      // which is done quite commonly.
      assert(m_term->reference_count()>0);
      return *reinterpret_cast<detail::_aterm_appl<Term>*>(m_term); 
    }

    detail::_aterm_appl<Term> *operator ->() const
    {
      assert(m_term!=NULL);
      assert(m_term->reference_count()>0);
      return reinterpret_cast<detail::_aterm_appl<Term>*>(m_term);
    }

    /// \brief Constructor.
    term_appl (detail::_aterm_appl<Term> *t):aterm(reinterpret_cast<detail::_aterm*>(t))
    {
      // assert((boost::is_base_of<Term, aterm>::value));
      assert(sizeof(Term)==sizeof(size_t));
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
      // assert((boost::is_base_of<Term, aterm>::value));
      assert(sizeof(Term)==sizeof(size_t));
    }

    /// \brief Explicit constructor from an aterm.
    /// \param t The aterm.
    explicit term_appl (const aterm &t):aterm(t)
    {
      // assert((boost::is_base_of<Term, aterm>::value));
      assert(sizeof(Term)==sizeof(size_t));
    }

    /// \brief Constructor.
    /// \details The iterator range is traversed only once, assuming Iter is a forward iterator.
    ///          The length of the iterator range should must match the arity of the function symbol.
    /// \param sym A function symbol.
    /// \param first The start of a range of elements.
    /// \param last The end of a range of elements.
    template <class Iter>
    term_appl(const function_symbol &sym, Iter first, Iter last);
    /*           typename boost::enable_if<
              typename boost::is_convertible< typename boost::iterator_traversal< Iter >::type,
              boost::forward_traversal_tag >::type >::type* = 0); */
    
    /// \brief Constructor.
    /// \details The iterator range is traversed only once, assuming Iter is a forward iterator.
    ///          This means that the ATermConverter is applied exactly once to each element.
    ///          The length of the iterator range must be equal to the arity of the function symbol.
    /// \param sym A function symbol.
    /// \param first The start of a range of elements.
    /// \param last The end of a range of elements.
    /// \param convert_to_aterm. An optional translator that is applied to each element in the iterator range,
    //                              and which must translate these elements to type Term.
    template <class Iter, class ATermConverter>
    term_appl(const function_symbol &sym, Iter first, Iter last, ATermConverter convert_to_aterm);
    
    /// \brief Constructor.
    /// \param sym A function symbol.
    term_appl(const function_symbol &sym):aterm(sym)
    {}

    /// \brief Constructor for a unary function application.
    /// \param sym A function symbol.
    /// \param t1 The first argument.
    term_appl(const function_symbol &sym, const Term &t1);

    /// \brief Constructor for a binary function application.
    /// \param sym A function symbol.
    /// \param t1 The first argument.
    /// \param t2 The second argument.
    term_appl(const function_symbol &sym, const Term &t1, const Term &t2);

    /// \brief Constructor for a ternary function application.
    /// \param sym A function symbol.
    /// \param t1 The first argument.
    /// \param t2 The second argument.
    /// \param t3 The third argument.
    term_appl(const function_symbol &sym, const Term &t1, const Term &t2, const Term &t3);

    /// \brief Constructor for a unary function application.
    /// \param sym A function symbol.
    /// \param t1 The first argument.
    /// \param t2 The second argument.
    /// \param t3 The third argument.
    /// \param t4 The fourth argument.
    term_appl(const function_symbol &sym, const Term &t1, const Term &t2, const Term &t3, const Term &t4);

    /// \brief Constructor for a unary function application.
    /// \param sym A function symbol.
    /// \param t1 The first argument.
    /// \param t2 The second argument.
    /// \param t3 The third argument.
    /// \param t4 The fourth argument.
    /// \param t5 The fifth argument.
    term_appl(const function_symbol &sym, const Term &t1, const Term &t2, const Term &t3, const Term &t4, const Term &t5);

    /// \brief Constructor for a unary function application.
    /// \param sym A function symbol.
    /// \param t1 The first argument.
    /// \param t2 The second argument.
    /// \param t3 The third argument.
    /// \param t4 The fourth argument.
    /// \param t5 The fifth argument.
    /// \param t6 The sixth argument.
    term_appl(const function_symbol &sym, const Term &t1, const Term &t2, const Term &t3, const Term &t4, const Term &t5, const Term &t6);

    /// \brief The assignment operator
    /// \param t The assigned term
    term_appl &operator=(const term_appl &t)
    {
      copy_term(t.m_term);
      return *this;
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
      return const_iterator(&(static_cast<detail::_aterm_appl<Term>*>(m_term)->arg[0]));
    }

    /// \brief Returns a const_iterator pointing past the last argument.
    /// \return A const_iterator pointing past the last argument.
    const_iterator end() const
    {
      return const_iterator(&static_cast<detail::_aterm_appl<Term>*>(m_term)->arg[size()]);
    }

    /// \brief Returns the largest possible number of arguments.
    /// \return The largest possible number of arguments.
    size_type max_size() const
    {
      return (std::numeric_limits<unsigned long>::max)();
    }

    /// \brief Returns a copy of the term with the i-th child replaced by t.
    /// \deprecated
    /// \param arg The new i-th argument
    /// \param i A positive integer
    /// \return The term with one of its arguments replaced.
    term_appl<Term> set_argument(const Term &arg, const size_type i);

    /// \brief Returns the i-th argument.
    /// \param i A positive integer
    /// \return The argument with the given index.
    const Term &operator()(size_type i) const
    {
      assert(i<m_term->function().arity());
      return reinterpret_cast<detail::_aterm_appl<Term>*>(m_term)->arg[i];
    }
};

typedef term_appl<aterm> aterm_appl;

} // namespace atermpp

#include "mcrl2/atermpp/detail/memory.h"

#endif // MCRL2_ATERMPP_ATERM_APPL_H
