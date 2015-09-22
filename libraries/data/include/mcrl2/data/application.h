// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/application.h
/// \brief The class application.

// This is to avoid incomplete type errors when including this header standalone
#ifndef MCRL2_DATA_DATA_EXPRESSION_H
#include "mcrl2/data/data_expression.h"
#endif

#ifndef MCRL2_DATA_APPLICATION_H
#define MCRL2_DATA_APPLICATION_H

#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/utilities/workarounds.h" // for nullptr on older compilers

namespace mcrl2
{

namespace data
{

namespace detail
{

/// \brief Iterator for term_appl which prepends a data_expression to a list convertible to data_expressions.
template <class ForwardIterator >
class term_appl_prepend_iterator
{
  public:
    /// \brief The value_type.
    typedef data_expression value_type;
    /// \brief The reference type.
    typedef const data_expression& reference;
    /// \brief The pointer type.
    typedef const data_expression* pointer;
    /// \brief Difference type
    typedef ptrdiff_t difference_type;
    /// \brief The iterator category.
    typedef std::forward_iterator_tag iterator_category;

  protected:
    ForwardIterator m_it;
    pointer m_prepend;

  private:
    // Prevent the use of the following operators in this class, including the
    // postfix increment.
    reference operator[](difference_type n) const;
    term_appl_prepend_iterator operator++(int);
    term_appl_prepend_iterator& operator--();
    term_appl_prepend_iterator& operator--(int);
    term_appl_prepend_iterator& operator+=(difference_type n);
    term_appl_prepend_iterator& operator-=(difference_type n);
    term_appl_prepend_iterator operator+(difference_type n) const;
    term_appl_prepend_iterator operator-(difference_type n) const;
    difference_type operator-(const term_appl_prepend_iterator& other) const;
    difference_type distance_to(const term_appl_prepend_iterator& other) const;

  public:

    /// \brief Constructor.
    /// \param t A term
    term_appl_prepend_iterator(ForwardIterator it,
                               pointer prepend=nullptr)
      : m_it(it), m_prepend(prepend)
    {}


    /// \brief The copy constructor.
    /// \param other The iterator that is copy constructed.
    term_appl_prepend_iterator(const term_appl_prepend_iterator& other)
      : m_it(other.m_it),
        m_prepend(other.m_prepend)
    {
    }

    /// \brief The assignment operator.
    /// \param other The term to be assigned.
    /// \return A reference to the assigned iterator.
    term_appl_prepend_iterator& operator=(const term_appl_prepend_iterator& other)
    {
      m_it=other.m_it;
      m_prepend=other.m_prepend;
      return *this;
    }

    /// \brief The dereference operator.
    /// \return The dereferenced term.
    reference operator*() const
    {
      if (m_prepend)
      {
        return *m_prepend;
      }
      return *(this->m_it);
    }

    /// \brief Dereference the current iterator.
    /// \return The dereference term.
    pointer operator->() const
    {
      if (m_prepend)
      {
        return m_prepend;
      }
      return &*(this->m_it);
    }

    /// \brief Prefix increment.
    /// \return The iterator after it is incremented.
    term_appl_prepend_iterator& operator++()
    {
      if (m_prepend)
      {
        m_prepend = nullptr;
      }
      else
      {
        ++(this->m_it);
      }
      return *this;
    }

    /// \brief Equality of iterators.
    /// \param other The iterator with which this iterator is compared.
    /// \return true if the iterators point to the same term_list.
    bool operator ==(const term_appl_prepend_iterator& other) const
    {
      return m_prepend==other.m_prepend && this->m_it==other.m_it;
    }

    /// \brief Inequality of iterators.
    /// \param other The iterator with which this iterator is compared.
    /// \return true if the iterators do not point to the same term_appl.
    bool operator !=(const term_appl_prepend_iterator& other) const
    {
      return !(*this==other);
    }

    /// \brief Comparison of iterators.
    /// \param other The iterator with which this iterator is compared.
    /// \return true if the pointer to this termterm is smaller than the other pointer.
    bool operator <(const term_appl_prepend_iterator& other) const
    {
      return m_prepend < other.m_prepend || (m_prepend==other.m_prepend && this->m_it<other.m_it);
    }

    /// \brief Comparison of iterators.
    /// \param other The iterator with which this iterator is compared.
    /// \return true if the iterators point to the same term_appl.
    bool operator <=(const term_appl_prepend_iterator& other) const
    {
      return m_prepend < other.m_prepend || (m_prepend==other.m_prepend && this->m_it<=other.m_it);
    }

    /// \brief Comparison of iterators.
    /// \param other The iterator with which this iterator is compared.
    /// \return true if the iterators point to the same term_appl.
    bool operator >(const term_appl_prepend_iterator& other) const
    {
      return other<*this;
    }

    /// \brief Comparison of iterators.
    /// \param other The iterator with which this iterator is compared.
    /// \return true if the iterators point to the same term_appl.
    bool operator >=(const term_appl_prepend_iterator& other) const
    {
      return other<=*this;
    }
};

/// \brief Iterator for term_appl which prepends a single term to the list, applying ArgumentConvertor to all arguments.
template <typename ForwardIterator, class ArgumentConverter>
class transforming_term_appl_prepend_iterator: public term_appl_prepend_iterator<ForwardIterator>
{
  protected:
    mutable data_expression m_stable_store;
    // ForwardIterator m_it;
    // const data_expression *m_prepend;
    ArgumentConverter m_argument_converter;

  public:

    /// \brief Constructor.
    /// \param t A term
    transforming_term_appl_prepend_iterator(ForwardIterator it,
                                            const data_expression* prepend,
                                            const ArgumentConverter arg_convert)
      : term_appl_prepend_iterator<ForwardIterator>(it,prepend),
        m_argument_converter(arg_convert)
    {}

    /// \brief The copy constructor.
    /// \param other The iterator that is copy constructed.
    transforming_term_appl_prepend_iterator(const transforming_term_appl_prepend_iterator& other)
      : term_appl_prepend_iterator<ForwardIterator>(other),
        m_stable_store(other.m_stable_store),
        m_argument_converter(other.m_argument_converter)
    {
    }

    /// \brief The assignment operator.
    /// \param other The term to be assigned.
    /// \return A reference to the assigned iterator.
    transforming_term_appl_prepend_iterator& operator=(const transforming_term_appl_prepend_iterator& other)
    {
      term_appl_prepend_iterator<ForwardIterator>::operator=(other);
      m_stable_store=other.m_stable_store;
      m_argument_converter=other.m_argument_converter;
      return *this;
    }

    /// \brief The dereference operator.
    /// \return The dereferenced term.
    typename term_appl_prepend_iterator<ForwardIterator>::reference operator*()
    {
      if (term_appl_prepend_iterator<ForwardIterator>::m_prepend)
      {
        return *term_appl_prepend_iterator<ForwardIterator>::m_prepend;
      }
      m_stable_store=m_argument_converter(*term_appl_prepend_iterator<ForwardIterator>::m_it);
      return m_stable_store;
    }

    /// \brief Dereference the current iterator.
    /// \return The dereference term.
    typename term_appl_prepend_iterator<ForwardIterator>::pointer operator->()
    {
      if (term_appl_prepend_iterator<ForwardIterator>::m_prepend)
      {
        return term_appl_prepend_iterator<ForwardIterator>::m_prepend;
      }
      m_stable_store=m_argument_converter(*term_appl_prepend_iterator<ForwardIterator>::m_it);
      return &m_stable_store;
    }
};

} // namespace detail

/// \brief An application of a data expression to a number of arguments
class application: public data_expression
{
  public:
    /// \brief Default constructor.
    application()
      : data_expression(atermpp::term_appl<aterm>(core::detail::function_symbol_DataAppl(0)))
    {}

    /// \brief Constructor.
    application(const data_expression& head,
                const data_expression& arg1)
      : data_expression(atermpp::term_appl<aterm>(core::detail::function_symbol_DataAppl(2),head,arg1))
    {
    }

    /// \brief Constructor.
    application(const data_expression& head,
                const data_expression& arg1,
                const data_expression& arg2)
      : data_expression(atermpp::term_appl<aterm>(core::detail::function_symbol_DataAppl(3),head,arg1,arg2))
    {
    }

    /// \brief Constructor.
    application(const data_expression& head,
                const data_expression& arg1,
                const data_expression& arg2,
                const data_expression& arg3)
      : data_expression(atermpp::term_appl<aterm>(core::detail::function_symbol_DataAppl(4),head,arg1,arg2,arg3))
    {
    }

    /// \brief Constructor.
    application(const data_expression& head,
                const data_expression& arg1,
                const data_expression& arg2,
                const data_expression& arg3,
                const data_expression& arg4)
      : data_expression(atermpp::term_appl<aterm>(core::detail::function_symbol_DataAppl(5),head,arg1,arg2,arg3,arg4))
    {
    }

    /// \brief Constructor
    application(const data_expression& head,
                const data_expression& arg1,
                const data_expression& arg2,
                const data_expression& arg3,
                const data_expression& arg4,
                const data_expression& arg5)
      : data_expression(atermpp::term_appl<aterm>(core::detail::function_symbol_DataAppl(6),head,arg1,arg2,arg3,arg4,arg5))
    {
    }

    /// \brief Constructor
    application(const data_expression& head,
                const data_expression& arg1,
                const data_expression& arg2,
                const data_expression& arg3,
                const data_expression& arg4,
                const data_expression& arg5,
                const data_expression& arg6)
      : data_expression(atermpp::term_appl<aterm>(core::detail::function_symbol_DataAppl(7),head,arg1,arg2,arg3,arg4,arg5,arg6))
    {
    }

    /// \brief Constructor.
    /// \param term A term
    explicit application(const atermpp::aterm& term)
      : data_expression(term)
    {
      assert(core::detail::check_term_DataAppl(*this));
    }

    /// \brief Constructor.
    template <typename Container>
    application(const data_expression& head,
                const Container& arguments,
                typename atermpp::enable_if_container<Container, data_expression>::type* = 0)
      : data_expression(atermpp::term_appl<aterm>(core::detail::function_symbol_DataAppl(arguments.size() + 1),
                                         detail::term_appl_prepend_iterator<typename Container::const_iterator>(arguments.begin(), &head),
                                         detail::term_appl_prepend_iterator<typename Container::const_iterator>(arguments.end())))
    {
      assert(arguments.size()>0);
    }

  private:
    // forbid the use of iterator, which is silently inherited from
    // aterm_appl. Modifying the arguments of an application through the iterator
    // is not allowed!
    typedef data_expression::iterator iterator;

  public:

    /// \brief An iterator to traverse the arguments of an application.
    /// \details There is a subtle difference with the arguments of an iterator on
    ///          the arguments of an aterm_appl from which an application is derived.
    ///          As an application has a head as its first argument, the iterator
    ///          of the aterm_appl starts at this head, where the iterator of the
    ///          application starts at the first argument. This also means that
    ///          t[n] for t an application is equal to t[n+1] if t is interpreted as an
    ///          aterm_appl.
    class const_iterator : public atermpp::term_appl_iterator<const data_expression>
    {
      public:
        /// \brief Constructor from a data_expression::const_iterator
        explicit const_iterator(const data_expression::const_iterator& p)
          : atermpp::term_appl_iterator<const data_expression>(static_cast<const data_expression*>(&*p))
        {}

    };

    /// \brief Constructor.
    template <typename FwdIter>
    application(const data_expression& head,
                FwdIter first,
                FwdIter last,
                typename std::enable_if< !std::is_base_of<data_expression, FwdIter>::value>::type* = 0)
      : data_expression(atermpp::term_appl<aterm>(core::detail::function_symbol_DataAppl(std::distance(first, last) + 1),
                                         detail::term_appl_prepend_iterator<FwdIter>(first, &head),
                                         detail::term_appl_prepend_iterator<FwdIter>(last)))
    {
      assert(first!=last);
    }

    /// \brief Constructor.
    template <typename FwdIter>
    application(const size_t arity,
                const data_expression& head,
                FwdIter first,
                FwdIter last,
                typename std::enable_if< !std::is_base_of<data_expression, FwdIter>::value>::type* = 0)
      : data_expression(atermpp::term_appl<aterm>(core::detail::function_symbol_DataAppl(arity + 1),
                                         detail::term_appl_prepend_iterator<FwdIter>(first, &head),
                                         detail::term_appl_prepend_iterator<FwdIter>(last)))
    {
      assert(arity>0);
      assert(std::distance(first, last)==arity);
    }


    /// \brief Constructor.
    template <typename FwdIter, class ArgumentConverter>
    application(const data_expression& head,
                FwdIter first,
                FwdIter last,
                ArgumentConverter convert_arguments,
                typename std::enable_if< !std::is_base_of<data_expression, FwdIter>::value>::type* = 0,
                typename std::enable_if< !std::is_base_of<data_expression, ArgumentConverter>::value>::type* = 0)
      : data_expression(atermpp::term_appl<aterm>(core::detail::function_symbol_DataAppl(std::distance(first, last) + 1),
                                         detail::transforming_term_appl_prepend_iterator<FwdIter, ArgumentConverter>(first, &head, convert_arguments),
                                         detail::transforming_term_appl_prepend_iterator<FwdIter, ArgumentConverter>(last,nullptr,convert_arguments)))
    {
      assert(first!=last);
    }

    /// \brief Get the function at the head of this expression.
    const data_expression& head() const
    {
      return atermpp::down_cast<const data_expression>(atermpp::aterm_appl::operator[](0));
    }

    /// \brief Get the i-th argument of this expression.
    const data_expression& operator[](size_t index) const
    {
      assert(index<size());
      return atermpp::down_cast<const data_expression>(atermpp::aterm_appl::operator[](index+1));
    }

    /// \brief Returns an iterator pointing to the first argument of the
    ///        application.
    const_iterator begin() const
    {
      return const_iterator(data_expression::begin()+1);
      // return ++const_iterator(data_expression::begin());
    }

    /// \brief Returns an iterator pointing past the last argument of the
    ///        application.
    const_iterator end() const
    {
      return const_iterator(data_expression::end());
    }

    /// \brief Returns an iterator pointing past the last argument of the
    ///        application.
    size_t size() const
    {
      using namespace atermpp;
      return atermpp::aterm_appl::size() - 1;
    }
};

/// \brief swap overload
inline void swap(application& t1, application& t2)
{
  t1.swap(t2);
}

//--- start generated class application ---//
// prototype declaration
std::string pp(const application& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const application& x)
{
  return out << data::pp(x);
}
//--- end generated class application ---//

// The precedence function must be declared here. Unfortunately this cannot be done using the include below.
// #include "mcrl2/data/precedence.h"
// Instead we do a forward declare of the precedence function. The user must make sure the file precedence.h is actually included.
// TOOO: fix this by moving the is_??? functions to the file application.h
int left_precedence(const data_expression& x);
int right_precedence(const data_expression& x);
int left_precedence(const application& x);
int right_precedence(const application& x);

inline
const data_expression& unary_operand(const application& x)
{
  return x[0];
}

inline
const data_expression& binary_left(const application& x)
{
  return x[0];
}

inline
const data_expression& binary_right(const application& x)
{
  return x[1];
}

inline
const data_expression& unary_operand1(const data_expression& x)
{
  const application& y = atermpp::down_cast<application>(x);
  return y[0];
}

inline
const data_expression& binary_left1(const data_expression& x)
{
  const application& y = atermpp::down_cast<application>(x);
  return y[0];
}

inline
const data_expression& binary_right1(const data_expression & x)
{
  const application& y = atermpp::down_cast<application>(x);
  return y[1];
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_APPLICATION_H
