// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/application.h
/// \brief The class application.

#ifndef MCRL2_NEW_DATA_APPLICATION_H
#define MCRL2_NEW_DATA_APPLICATION_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/new_data/detail/container_utility.h"
#include "mcrl2/new_data/data_expression.h"

namespace mcrl2 {

  namespace new_data {

    /// \brief function symbol.
    ///
    class application: public data_expression
    {

      public:

        /// \brief Iterator over arguments
        typedef detail::term_list_random_iterator< data_expression > argument_iterator;

        /// \brief Iterator range over constant arguments
        typedef boost::iterator_range< argument_iterator >           arguments_const_range;

        /// \brief Iterator range over arguments
        typedef boost::iterator_range< argument_iterator >           arguments_range;

      public:

        /// \brief Constructor.
        ///
        application()
          : data_expression(core::detail::constructDataAppl())
        {}

        /// \brief Constructor.
        ///
        /// \param[in] d A new_data expression.
        /// \pre d has the internal structure of an application.
        application(const data_expression& d)
          : data_expression(d)
        {
          assert(d.is_application());
        }

        /// \brief Constructor.
        ///
        /// \param[in] head The new_data expression that is applied.
        /// \param[in] arguments The new_data expressions that head is applied to.
        /// \pre head.sort() is a function sort.
        /// \pre arguments is not empty.
        application(const data_expression& head,
                    const data_expression_list& arguments)
          : data_expression(core::detail::gsMakeDataAppl(head, arguments))
        {
          assert(head.sort().is_function_sort());
          assert(!arguments.empty());
        }

        /// \brief Constructor.
        ///
        /// \param[in] head The new_data expression that is applied.
        /// \param[in] arguments The new_data expressions that head is applied to.
        /// \pre head.sort() is a function sort.
        /// \pre arguments is not empty.
        application(const data_expression& head,
                    const data_expression_vector& arguments)
          : data_expression(core::detail::gsMakeDataAppl(head, convert< atermpp::aterm_list >(arguments)))
        {
          assert(head.sort().is_function_sort());
          assert(!arguments.empty());
        }

        /// \brief Constructor.
        ///
        /// \param[in] head The new_data expression that is applied.
        /// \param[in] arguments The new_data expressions that head is applied to.
        /// \pre head.sort() is a function sort.
        /// \pre arguments is not empty.
        template <typename ForwardIterator >
        application(const data_expression& head,
                    const typename boost::iterator_range< ForwardIterator >& arguments)
          : data_expression(core::detail::gsMakeDataAppl(head, convert< atermpp::aterm_list >(arguments)))
        {
          assert(head.sort().is_function_sort());
          assert(!arguments.empty());
        }

        /// \brief Convenience constructor for application with one argument
        ///
        /// \param[in] head The new_data expression that is applied
        /// \param[in] arg1 The argument head is applied to
        /// \post *this represents head(arg1)
        application(const data_expression& head,
                    const data_expression& arg1)
          : data_expression(core::detail::gsMakeDataAppl(head, atermpp::make_list(arg1)))
        { }

        /// \brief Convenience constructor for application with two arguments
        ///
        /// \param[in] head The new_data expression that is applied
        /// \param[in] arg1 The first argument head is applied to
        /// \param[in] arg2 The second argument head is applied to
        /// \post *this represents head(arg1, arg2)
        application(const data_expression& head,
                    const data_expression& arg1,
                    const data_expression& arg2)
          : data_expression(core::detail::gsMakeDataAppl(head, atermpp::make_list(arg1, arg2)))
        { }

        /// \brief Convenience constructor for application with three arguments
        ///
        /// \param[in] head The new_data expression that is applied
        /// \param[in] arg1 The first argument head is applied to
        /// \param[in] arg2 The second argument head is applied to
        /// \param[in] arg3 The third argument head is applied to
        /// \post *this represents head(arg1, arg2, arg3)
        application(const data_expression& head,
                    const data_expression& arg1,
                    const data_expression& arg2,
                    const data_expression& arg3)
          : data_expression(core::detail::gsMakeDataAppl(head, atermpp::make_list(arg1, arg2, arg3)))
        { }

        /// \brief Convenience constructor for application with three arguments
        ///
        /// \param[in] head The new_data expression that is applied
        /// \param[in] arg1 The first argument head is applied to
        /// \param[in] arg2 The second argument head is applied to
        /// \param[in] arg3 The third argument head is applied to
        /// \param[in] arg4 The fourth argument head is applied to
        /// \post *this represents head(arg1, arg2, arg3, arg4)
        application(const data_expression& head,
                    const data_expression& arg1,
                    const data_expression& arg2,
                    const data_expression& arg3,
                    const data_expression& arg4)
          : data_expression(core::detail::gsMakeDataAppl(head, atermpp::make_list(arg1, arg2, arg3, arg4)))
        { }

        /// \brief Returns the application of this application to an argument.
        /// \pre this->sort() is a function sort.
        /// \param[in] e The new_data expression to which the application is applied
        application operator()(const data_expression& e) const
        {
          assert(this->sort().is_function_sort());
          return application(*this, e);
        }

        /* Should be enabled when the implementation in data_expression is
         * removed
        /// \overload
        inline
        sort_expression sort() const
        {
          return static_cast<const function_sort>(head().sort()).codomain();
        }
        */

        /// \brief Returns the head of the application
        inline
        data_expression head() const
        {
          return atermpp::arg1(*this);
        }

        /// \brief Returns the arguments of the application
        inline
        boost::iterator_range< argument_iterator > arguments() const
        {
          return boost::make_iterator_range(add_random_access< data_expression >(atermpp::list_arg2(appl())));
        }

        /// \brief Returns the first argument of the application
        /// \pre head() is a binary operator
        /// \return arguments()
        inline
        data_expression left() const
        {
          assert(arguments().size() == 2);
          return arguments()[0];
        }

        /// \brief Returns the second argument of the application
        /// \pre head() is a binary operator
        inline
        data_expression right() const
        {
          assert(arguments().size() == 2);
          return arguments()[1];
        }

    }; // class application

    /// \brief list of applications
    typedef atermpp::term_list<application> application_list;

    /// \brief vector of applications
    typedef atermpp::vector<application> application_vector;

  } // namespace new_data

} // namespace mcrl2

#endif // MCRL2_NEW_DATA_APPLICATION_H

