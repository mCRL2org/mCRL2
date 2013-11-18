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

/// \brief Iterator for term_appl which prepends a single term to the list.
template <typename Term, typename ForwardIterator>
class term_appl_prepend_iterator: public boost::iterator_facade<
  term_appl_prepend_iterator<Term, ForwardIterator>, // Derived
  Term,                                              // Value
  boost::forward_traversal_tag,                      // CategoryOrTraversal
  Term&                                              // Reference
  >
{
  public:

    /// \brief Constructor.
    /// \param t A term
    term_appl_prepend_iterator(ForwardIterator it, const Term* prepend = NULL)
      : m_it(it), m_prepend(prepend)
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
    const Term &dereference() const
    {
      if (m_prepend)
      {
        return *m_prepend;
      }
      return *m_it;
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

    ForwardIterator m_it;
    const Term *m_prepend;
};

/// \brief An application of a data expression to a number of arguments
class application: public data_expression
{
  public:
    /// \brief Default constructor.
    application()
      : data_expression(atermpp::term_appl<aterm>(core::detail::function_symbol_DataAppl(1),
                                         term_appl_prepend_iterator<const atermpp::term_appl<aterm>, data_expression*>(nullptr, &core::detail::constructDataExpr()),
                                         term_appl_prepend_iterator<const atermpp::term_appl<aterm>, data_expression*>(nullptr)))
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit application(const atermpp::aterm& term)
      : data_expression(term)
    {
      assert(core::detail::check_term_DataAppl(*this));
    }

    /// \brief Constructor.
    application(const data_expression& head, const data_expression_list& arguments)
      : data_expression(atermpp::term_appl<aterm>(core::detail::function_symbol_DataAppl(arguments.size() + 1),
                                         term_appl_prepend_iterator<const data_expression, data_expression_list::const_iterator>(arguments.begin(), &head),
                                         term_appl_prepend_iterator<const data_expression, data_expression_list::const_iterator>(arguments.end())))
    {}

    /// \brief Constructor.
    template <typename Container>
    application(const data_expression& head, const Container& arguments, typename atermpp::detail::enable_if_container<Container, data_expression>::type* = 0)
      : data_expression(atermpp::term_appl<aterm>(core::detail::function_symbol_DataAppl(arguments.size() + 1),
                                         term_appl_prepend_iterator<const data_expression, typename Container::const_iterator>(arguments.begin(), &head),
                                         term_appl_prepend_iterator<const data_expression, typename Container::const_iterator>(arguments.end())))
    {}

    const data_expression& head() const
    {
      return atermpp::aterm_cast<const data_expression>((*this)[0]);
    }

    data_expression_list arguments() const
    {
      return data_expression_list(begin(), end());
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
                FwdIter last)
      : data_expression(atermpp::term_appl<aterm>(core::detail::function_symbol_DataAppl(std::distance(first, last) + 1),
                                         term_appl_prepend_iterator<const data_expression, FwdIter>(first, &head),
                                         term_appl_prepend_iterator<const data_expression, FwdIter>(last)))
    {}

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
      return arguments().size();
    }

/*
    /// \brief Returns the first argument of the application
    /// \pre head() is a binary operator
    /// \return arguments()[0]
    inline
    const data_expression& left() const
    {
      assert(size() == 2);
      return *(begin());
    }

    /// \brief Returns the second argument of the application
    /// \pre head() is a binary operator
    /// \return arguments()[1]
    inline
    const data_expression& right() const
    {
      assert(size() == 2);
      return *(++(begin()));
    }
*/
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

/// \brief Apply data expression to a data expression
inline application make_application(data_expression const& head,
                                    data_expression const& e0)
{
  // Due to sort aliasing, the asserts below are not necessarily
  // valid anymore.
  // assert(is_function_sort(head.sort()));
  // assert(function_sort(head.sort()).domain().size() == 1);
  return application(head, atermpp::make_list(e0));
}

/// \brief Apply data expression to two data expression
inline application make_application(data_expression const& head,
                                    data_expression const& e0,
                                    data_expression const& e1)
{
  // See above for the reason to outcomment the asserts below
  // assert(is_function_sort(head.sort()));
  // assert(function_sort(head.sort()).domain().size() == 2);
  return application(head, atermpp::make_list(e0, e1));
}

/// \brief Apply data expression to three data expression
inline application make_application(data_expression const& head,
                                    data_expression const& e0,
                                    data_expression const& e1,
                                    data_expression const& e2)
{
  // See above for the reason to outcomment the asserts below
  // assert(is_function_sort(head.sort()));
  // assert(function_sort(head.sort()).domain().size() == 3);
  return application(head, atermpp::make_list(e0, e1, e2));
}

/// \brief Apply data expression to four data expression
inline application make_application(data_expression const& head,
                                    data_expression const& e0,
                                    data_expression const& e1,
                                    data_expression const& e2,
                                    data_expression const& e3)
{
  // See above for the reason to outcomment the asserts below
  // assert(is_function_sort(head.sort()));
  // assert(function_sort(head.sort()).domain().size() == 4);
  return application(head, atermpp::make_list(e0, e1, e2, e3));
}

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
