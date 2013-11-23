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
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/atermpp/convert.h"
#include "mcrl2/utilities/workarounds.h" // for nullptr on older compilers

namespace mcrl2
{

namespace data
{

class default_converter
{
  public:
    const data_expression& operator()(const data_expression& t) const
    {
      return t;
    }
};

/// \brief Iterator for term_appl which prepends a single term to the list.
template <typename ForwardIterator, class ArgumentConverter=default_converter >
class term_appl_prepend_iterator: public boost::iterator_facade<
  term_appl_prepend_iterator<ForwardIterator, ArgumentConverter>, // Derived
  data_expression,                                                // Value
  boost::forward_traversal_tag,                                   // CategoryOrTraversal
  const data_expression&                                          // Reference
  >
{
  public:

    /// \brief Constructor.
    /// \param t A term
    term_appl_prepend_iterator(ForwardIterator it, 
                               const data_expression* prepend=nullptr, 
                               const ArgumentConverter arg_convert=default_converter())
      : m_it(it), m_prepend(prepend), m_argument_converter(arg_convert)
    {}

  private:
    friend class boost::iterator_core_access;

    /// \brief Equality check
    /// \param other An iterator
    /// \return True if the iterators are equal
    bool equal(term_appl_prepend_iterator const& other) const
    {
      return this->m_prepend == other.m_prepend && this->m_it == other.m_it;
    }

    /// \brief Dereference operator
    /// \return The value that the iterator references
    const data_expression &dereference() const  
    {
      if (m_prepend)
      {
        return *m_prepend;
      }
      
      m_stable_store=m_argument_converter(*m_it);
      return m_stable_store;
    }

    /// \brief Increments the iterator
    void increment()
    {
      if (m_prepend)
      {
        m_prepend = nullptr;
      }
      else
      {
        ++m_it;
      }
    }

    mutable data_expression m_stable_store;
    ForwardIterator m_it;
    const data_expression *m_prepend;
    ArgumentConverter m_argument_converter;
};

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

    /// \brief Constructor.
    application(const data_expression& head,
                const data_expression& arg1,
                const data_expression& arg2,
                const data_expression& arg3,
                const data_expression& arg4,
                const data_expression& arg5)
      : data_expression(atermpp::term_appl<aterm>(core::detail::function_symbol_DataAppl(6),head,arg1,arg2,arg3,arg4,arg5))
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
                typename atermpp::detail::enable_if_container<Container, data_expression>::type* = 0)
      : data_expression(atermpp::term_appl<aterm>(core::detail::function_symbol_DataAppl(arguments.size() + 1),
                                         term_appl_prepend_iterator<typename Container::const_iterator>(arguments.begin(), &head),
                                         term_appl_prepend_iterator<typename Container::const_iterator>(arguments.end())))
    { 
      assert(arguments.size()>0);
    }

  private:
    // forbid the use of iterator, which is silently inherited from
    // aterm_appl. Modifying the arguments of an application through the iterator
    // is not allowed!
    typedef data_expression::iterator iterator;

  public:

    class const_iterator : public boost::iterator_adaptor<
            const_iterator                     // Derived
          , data_expression::const_iterator    // Base
          , const data_expression              // Value
          , boost::random_access_traversal_tag // CategoryOrTraversal
        >
    {
      public:
        explicit const_iterator(const data_expression::const_iterator& p)
          : const_iterator::iterator_adaptor_(p) {}
      private:
        friend class boost::iterator_core_access;
        reference dereference() const
        {
          return atermpp::aterm_cast<const data_expression>(*base_reference());
        }
    };

    /// \brief Constructor.
    template <typename FwdIter>
    application(const data_expression& head,
                FwdIter first,
                FwdIter last,
                typename boost::disable_if<typename boost::is_base_of<data_expression, FwdIter>::type>::type* = 0)
      : data_expression(atermpp::term_appl<aterm>(core::detail::function_symbol_DataAppl(std::distance(first, last) + 1),
                                         term_appl_prepend_iterator<FwdIter>(first, &head),
                                         term_appl_prepend_iterator<FwdIter>(last)))
    {
      assert(first!=last);
    }

    /// \brief Constructor.
    template <typename FwdIter>
    application(const size_t arity,
                const data_expression& head,
                FwdIter first,
                FwdIter last,
                typename boost::disable_if<typename boost::is_base_of<data_expression, FwdIter>::type>::type* = 0)
      : data_expression(atermpp::term_appl<aterm>(core::detail::function_symbol_DataAppl(arity + 1),
                                         term_appl_prepend_iterator<FwdIter>(first, &head),
                                         term_appl_prepend_iterator<FwdIter>(last)))
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
                typename boost::disable_if<typename boost::is_base_of<data_expression, FwdIter>::type>::type* = 0,
                typename boost::disable_if<typename boost::is_base_of<data_expression, ArgumentConverter>::type>::type* = 0)
      : data_expression(atermpp::term_appl<aterm>(core::detail::function_symbol_DataAppl(std::distance(first, last) + 1),
                                         term_appl_prepend_iterator<FwdIter, ArgumentConverter>(first, &head, convert_arguments),
                                         term_appl_prepend_iterator<FwdIter, ArgumentConverter>(last,NULL,convert_arguments)))
    {
      assert(first!=last);
    }

    /// \brief Get the function at the head of this expression.
    const data_expression& head() const
    {
      return atermpp::aterm_cast<const data_expression>(static_cast<atermpp::aterm_appl>(*this)[0]);
    }

    /// \brief Get the arguments of this expression.
    /// \deprecated
    /// \details This function is quite inefficient. It is
    ///          linear in the number of arguments, and requires
    ///          the relatively expensive construction of an data_expression_list.
    data_expression_list arguments() const
    {
      return data_expression_list(begin(), end());
    }

    /// \brief Get the i-th argument of this expression.
    const data_expression& operator[](size_t index) const
    {
      assert(index<size());
      return atermpp::aterm_cast<const data_expression>(static_cast<const atermpp::aterm_appl&>(*this)[index+1]);
    }

    /// \brief Returns an iterator pointing to the first argument of the
    ///        application.
    const_iterator begin() const
    {
      return ++const_iterator(data_expression::begin());
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
      return aterm_cast<aterm_appl>(*this).size()-1; 
    }
};

/// \brief swap overload
inline void swap(application& t1, application& t2)
{
  t1.swap(t2);
}

//--- end generated class application ---//

// The precedence function must be declared here. Unfortunately this cannot be done using the include below.
// #include "mcrl2/data/precedence.h"
// Instead we do a forward declare of the precedence function. The user must make sure the file precedence.h is actually included.
// TOOO: fix this by moving the is_??? functions to the file application.h
int precedence(const data_expression& x);
int precedence(const application& x);

inline
const data_expression& unary_operand(const application& x)
{
  return *x.begin();
}

inline
const data_expression& binary_left(const application& x)
{
  return *x.begin();
}

inline
const data_expression& binary_right(const application& x)
{
  return *(++x.begin());
}

inline
const data_expression& unary_operand1(const data_expression& x)
{
  const application& y = core::static_down_cast<const application&>(x);
  return *y.begin();
}

inline
const data_expression& binary_left1(const data_expression& x)
{
  const application& y = core::static_down_cast<const application&>(x);
  return *y.begin();
}

inline
const data_expression& binary_right1(const data_expression & x)
{
  const application& y = core::static_down_cast<const application&>(x);
  return *(++y.begin());
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_APPLICATION_H
