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

namespace mcrl2
{

namespace data
{

namespace detail
{
//--- start generated class application ---//
//--- end generated class application ---//

/// \brief An application of a data expression to a number of arguments
class application_base: public data_expression
{
  public:
    /// \brief Default constructor.
    application_base()
      : data_expression(core::detail::constructDataAppl())
    {}

    /// \brief Constructor.
    /// \param term A term
    application_base(const atermpp::aterm_appl& term)
      : data_expression(term)
    {
      assert(core::detail::check_term_DataAppl(m_term));
    }

    /// \brief Constructor.
    application_base(const data_expression& head, const data_expression_list& arguments)
      : data_expression(core::detail::gsMakeDataAppl(head, arguments))
    {}

    /// \brief Constructor.
    template <typename Container>
    application_base(const data_expression& head, const Container& arguments, typename atermpp::detail::enable_if_container<Container, data_expression>::type* = 0)
      : data_expression(core::detail::gsMakeDataAppl(head, atermpp::convert<data_expression_list>(arguments)))
    {}

    data_expression head() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Give the arguments of this term as a list. 
    /// \details time for this operation is constant.
    data_expression_list arguments() const
    {
      return atermpp::list_arg2(*this);
    }
};

} // namespace detail

/// \brief application a data expression of a function sort to a number
///        of arguments.
///
/// An example of an application is f(x,y), where f is the head of the
/// application, and x,y are the arguments.
class application: public detail::application_base
{
  public:

    /// \brief Default constructor for an application, note that this is not
    ///        a valid data expression.
    ///
    application()
      : detail::application_base(core::detail::constructDataAppl())
    {}

    ///\overload
    application(atermpp::aterm_appl term)
      : detail::application_base(term)
    {}

    ///\overload
    application(const data_expression& head, data_expression_list const& arguments)
      : detail::application_base(head, arguments)
    {}

    ///\overload
    template < typename Container >
    application(const data_expression& head,
                const Container& arguments,
                typename atermpp::detail::enable_if_container< Container, data_expression >::type* = 0)
      : detail::application_base(head, arguments)
    {
      // Due to sort aliasing, it is possible that the sort of a function symbol is
      // not a function sort, but an alias of function sort. Therefore, the asserts below are
      // not always valid.
      // assert(is_function_sort(head.sort()));
      // assert(function_sort(head.sort()).domain().size() == static_cast< size_t >(boost::distance(arguments)));
      assert(!arguments.empty());
    }

    /// \brief Returns the first argument of the application
    /// \pre head() is a binary operator
    /// \return arguments()[0]
    inline
    data_expression left() const
    {
      assert(arguments().size() == 2);
      return *(arguments().begin());
    }

    /// \brief Returns the second argument of the application
    /// \pre head() is a binary operator
    /// \return arguments()[1]
    inline
    data_expression right() const
    {
      assert(arguments().size() == 2);
      return *(++(arguments().begin()));
    }

    /// \brief Returns the n-th argument of the application, where the first argument has index 0.
    /// \pre The number of arguments must be at least n.
    data_expression argument(const size_t n)
    {
      assert(arguments().size() > n);
      return data_expression((ATermAppl)ATelementAt(arguments(),n));
    }
}; // class application

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

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_APPLICATION_H
