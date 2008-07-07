// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/structured_sort.h
/// \brief The class structured_sort.

#ifndef MCRL2_DATA_STRUCTURED_SORT_H
#define MCRL2_DATA_STRUCTURED_SORT_H

#include <string>
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/data/sort_expression.h"

namespace mcrl2 {
  
  namespace data {

    /// \brief Argument of a structured sort constructor.
    ///
    /// This comprises an optional name and a mandatory sort.
    class structured_sort_constructor_argument: public atermpp::aterm_appl
    {

      public:

        /// \brief Constructor
        ///
        structured_sort_constructor_argument()
          : atermpp::aterm_appl(core::detail::constructStructProj())
        {}

        /// \internal
        /// \brief Constructor
        ///
        /// \param[in] a A term.
        /// \pre a has the internal format of a constructor argument of a
        ///        structured sort.
        structured_sort_constructor_argument(const atermpp::aterm_appl& a)
          : atermpp::aterm_appl(a)
        {
          assert(core::detail::gsIsStructProj(a));
        }

        /// \brief Constructor
        ///
        /// \param[in] name The name of the argument.
        /// \param[in] sort The sort of the argument.
        structured_sort_constructor_argument(const std::string& name,
                                             const sort_expression& sort)
          : atermpp::aterm_appl(core::detail::gsMakeStructProj(atermpp::aterm_string(name), sort))
        {}

        /// \brief Constructor
        ///
        /// \param[in] sort The sort of the argument.
        structured_sort_constructor_argument(const sort_expression& sort)
          : atermpp::aterm_appl(core::detail::gsMakeStructProj(core::detail::gsMakeNil(), sort))
        {}

        /// \brief Returns the name of the constructor argument.
        ///
        inline
        std::string name() const
        {
          atermpp::aterm_appl n = atermpp::arg1(*this);
          if (n == core::detail::gsMakeNil())
          {
            return std::string();
          }
          else
          {
            return atermpp::aterm_string(n);
          }
        }

        /// \brief Returns the sort of the constructor argument.
        ///
        inline
        sort_expression sort() const
        {
          return atermpp::arg2(*this);
        }

    }; // class structured_sort_constructor_argument

    /// \brief List of structured_sort_constructor_argument
    ///
    typedef atermpp::vector<structured_sort_constructor_argument> structured_sort_constructor_argument_list;

    /// \brief A structured sort constructor.
    ///
    /// A structured sort constructor has a mandatory name, a mandatory,
    /// non-empty list of arguments and and optional recogniser name.
    class structured_sort_constructor: public atermpp::aterm_appl
    {
      protected:
        structured_sort_constructor_argument_list m_arguments; ///< The arguments of the constructor

      public:

        /// \brief Constructor
        structured_sort_constructor()
          : atermpp::aterm_appl(core::detail::constructStructCons())
        {}

        /// \internal
        /// \brief Constructor
        ///
        /// \param[in] c A term
        /// \pre c has the internal format of a constructor of a structured
        ///      sort.
        structured_sort_constructor(const atermpp::aterm_appl c)
          : atermpp::aterm_appl(c)
        {
          assert(core::detail::gsIsStructCons(c));
        }

        /// \brief Constructor
        ///
        /// \param[in] c is a structured sort constructor.
        structured_sort_constructor(const structured_sort_constructor& c)
          : atermpp::aterm_appl(c),
            m_arguments(atermpp::term_list<structured_sort_constructor_argument>(atermpp::list_arg2(c)).begin(),
                        atermpp::term_list<structured_sort_constructor_argument>(atermpp::list_arg2(c)).end())
        {}

        /// \brief Constructor
        ///
        /// \param[in] name The name of the constructor.
        /// \param[in] arguments The arguments of the constructor.
        /// \param[in] recogniser The name of the recogniser.
        /// \pre name is not empty.
        /// \pre recogniser is not empty.
        structured_sort_constructor(const std::string& name,
                                    const boost::iterator_range<structured_sort_constructor_argument_list::const_iterator>& arguments,
                                    const std::string& recogniser)
          : atermpp::aterm_appl(core::detail::gsMakeStructCons(atermpp::aterm_string(name),
                                                               atermpp::term_list<structured_sort_constructor_argument>(arguments.begin(), arguments.end()),
                                                               atermpp::aterm_string(recogniser))),
            m_arguments        (arguments.begin(), arguments.end())
        {
          assert(!name.empty());
          assert(!recogniser.empty());
        }

        /// \brief Constructor
        ///
        /// \param[in] name The name of the constructor.
        /// \param[in] arguments The arguments of the constructor.
        /// \pre name is not empty.
        structured_sort_constructor(const std::string& name,
                                    const boost::iterator_range<structured_sort_constructor_argument_list::const_iterator>& arguments)
          : atermpp::aterm_appl(core::detail::gsMakeStructCons(atermpp::aterm_string(name),
                                                               atermpp::term_list<structured_sort_constructor_argument>(arguments.begin(), arguments.end()),
                                                               core::detail::gsMakeNil())),
            m_arguments        (arguments.begin(), arguments.end())
        {
          assert(!name.empty());
        }

        /// \brief Returns the name of the constructor.
        ///
        inline
        std::string name() const
        {
          return atermpp::aterm_string(atermpp::arg1(*this));
        }

        /// \brief Returns the arguments of the constructor.
        ///
        inline
        boost::iterator_range<structured_sort_constructor_argument_list::const_iterator> arguments() const
        {
          return boost::make_iterator_range(m_arguments);
        }

        /// \brief Returns the name of the recogniser of the constructor.
        inline
        std::string recogniser() const
        {
          atermpp::aterm_appl r = arg3(*this);
          if (r == core::detail::gsMakeNil())
          {
            return std::string();
          }
          else
          {
            return atermpp::aterm_string(r);
          }
        }

    }; // class structured_sort_constructor

    /// \brief List of structured_sort_constructor.
    ///
    typedef atermpp::vector<structured_sort_constructor> structured_sort_constructor_list;


    /// \brief structured sort.
    ///
    /// A structured sort is a sort with the following structure:
    ///  struct c1(pr1,1:S1,1, ..., pr1,k1:S1,k1)?is_c1
    ///       | ...
    ///       | cn(prn,1:Sn,1, ..., prn,kn:Sn,kn)?is_cn
    /// in this:
    /// * c1, ..., cn are called constructors.
    /// * pri,j are the projection functions (or constructor arguments).
    /// * is_ci are the recognisers.
    class structured_sort: public sort_expression
    {
      protected:
        structured_sort_constructor_list m_struct_constructors; ///< The list of constructors of the structured sort.

      public:    

        /// \brief Constructor
        structured_sort()
          : sort_expression(core::detail::constructSortStruct())
        {}

        /// \brief Constructor
        ///
        /// \param[in] s A sort expression.
        /// \pre s is a structured sort.
        structured_sort(const sort_expression& s)
          : sort_expression(s),
            m_struct_constructors(atermpp::term_list<structured_sort_constructor>(atermpp::list_arg1(s)).begin(),
                                  atermpp::term_list<structured_sort_constructor>(atermpp::list_arg1(s)).end())
        {
          assert(s.is_structured_sort());
        }

        /// \brief Constructor
        ///
        /// \param[in] struct_constructors The list of constructors.
        /// \post struct_constructors is empty.
        structured_sort(const boost::iterator_range<structured_sort_constructor_list::const_iterator>& struct_constructors)
          : sort_expression(mcrl2::core::detail::gsMakeSortStruct(atermpp::term_list<structured_sort_constructor>(struct_constructors.begin(), struct_constructors.end()))),
            m_struct_constructors(struct_constructors.begin(), struct_constructors.end())
        {
          assert(!struct_constructors.empty());
        }

        /// \overload
        ///
        inline
        bool is_structured_sort() const
        {
          return true;
        }

        /// \brief Returns the struct constructors of this sort
        ///
        inline
        boost::iterator_range<structured_sort_constructor_list::const_iterator> struct_constructors() const
        {
          return boost::make_iterator_range(m_struct_constructors);
        }

    }; // class structured_sort

    /// \brief list of structured sorts
    ///
    typedef atermpp::vector<structured_sort> structured_sort_list;

  } // namespace data

} // namespace mcrl2

/// \cond INTERNAL_DOCS
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::data::structured_sort_constructor_argument);
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::data::structured_sort_constructor);
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::data::structured_sort);
/// \endcond


#endif // MCRL2_DATA_SORT_EXPRESSION_H

