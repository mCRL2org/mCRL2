// Author(s): Jan Friso Groote, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/representative_generator.h
/// \brief Component for generating representatives of sorts

#ifndef MCRL2_DATA_REPRESENTATIVE_GENERATOR_H__
#define MCRL2_DATA_REPRESENTATIVE_GENERATOR_H__

#include <algorithm>
#include <functional>

#include "boost/bind.hpp"

#include "mcrl2/data/data_specification.h"

namespace mcrl2 {

  namespace data {
    /// \cond INTERNAL_DOCS
    namespace detail {

      struct has_result_sort : public std::unary_function< data_expression const&, bool >
      {
        sort_expression m_sort;

        has_result_sort(sort_expression const& sort) : m_sort(sort)
        {
        }

        bool operator()(data_expression const& e)
        {
          return e.sort().is_function_sort() && function_sort(e.sort()).target_sort() == m_sort;
        }
      };

      struct has_non_function_sort : public std::unary_function< data_expression const&, bool >
      {
        bool operator()(data_expression const& e)
        {
          return !e.sort().is_function_sort();
        }
      };
    }
    /// \endcond

    /// \brief Components for generating an arbitrary element of a sort
    ///
    /// A representative is an arbitrary element of a given sort. This
    /// component takes a specification and generates representatives for sorts
    /// defined by the specification. An important property is that for the
    /// same sort the same representative is returned. For this it assumes
    /// that the context -constructors and mappings for the sort- remain
    /// unchanged.
    ///
    /// The maximum recursion depth -of applying constructors and mappings- is
    /// available as a control mechanism to limit element construction. The
    /// general aim is to keep the representative expression as simple. Use of
    /// constructors is preferred above mappings and constructors or
    /// mappings representing constants are preferred over those that have
    /// non-empty domain.
    /// 
    /// Constructors and functions that have arrows in their target sorts
    /// (e.g. f:A->(B->C)) are not used to construct default terms. Once an
    /// element is generated it is kept for later requests, which is done for
    /// performance when used frequently on the same specification. At some
    /// point a sufficiently advanced enumerator may be used to replace the
    /// current implementation.
    ///
    /// This component will evolve through time, in the sense that more
    /// complex expressions will be generated over time to act as
    /// representative a certain sort, for instance containing fucntion symbols
    /// with complex target sorts, containing explicit function constructors
    /// (lambda's). So, no reliance is possible on the particular shape of the
    /// terms that are generated.
    class representative_generator
    {

      protected:

        /// \brief Data specification context
        data_specification const&                        m_specification;

        /// \brief Serves as a cache for later find operations
        atermpp::map< sort_expression, data_expression > m_representatives;

      protected:

        /// \brief Sets a data expression as representative of the sort
        /// \param[in] sort the sort of which to set the representative
        /// \param[in] representative the data expression that serves as representative
        data_expression set_representative(sort_expression const& sort, data_expression const& representative)
        {
          m_representatives[sort] = representative;

          return representative;
        }

        /// \brief Finds a representative for a function symbol
        /// \param[in] symbol the function symbol for which to find the representative
        /// \param[in] maximum_depth the maximum depth for recursive exploration of the sort
        /// \return an element of sort sort using a constructor or mapping; or the default constructed data_expression object
        /// \pre symbol.sort() is of type function_sort
        data_expression find_representative(function_symbol const& symbol, const unsigned int maximum_depth)
        {
          assert(symbol.sort().is_function_sort());

          data_expression_vector arguments;

          for (boost::iterator_range< function_sort::domain_const_range::iterator > r(function_sort(symbol.sort()).domain()); !r.empty(); r.advance_begin(1))
          {
            data_expression representative = find_representative(r.front(), maximum_depth - 1);

            if (representative == data_expression())
            {
              return data_expression();
            }

            arguments.push_back(representative);
          }

          // a suitable set of arguments is found
          return application(symbol, arguments);
        }

        /// \brief Finds a representative element for an arbitrary sort expression
        /// \param[in] sort the sort for which to find the representative
        /// \param[in] maximum_depth the maximum depth for recursive exploration of the sort
        /// \return an element of sort sort or the default constructed data_expression object
        data_expression find_representative(sort_expression const& sort, const unsigned int maximum_depth)
        {
          data_specification::constructors_const_range local_constructors(m_specification.constructors(sort));
          data_specification::mappings_const_range     local_mappings(m_specification.mappings());

          if (sort.is_function_sort())
          { // s is a function sort. We search for a constructor of mapping of this sort
            // Although in principle possible, we do not do a lot of effort to construct
            // a term of this sort. We just look whether a term of exactly this sort is
            // present.

            // check if there is a mapping with sort s (constructors with sort s cannot exist).
            for (data_specification::mappings_const_range::const_iterator i =
                std::find_if(local_mappings.begin(), local_mappings.end(), detail::has_result_sort(sort));
                                                                                         i != local_mappings.end(); )
            {
              return set_representative(sort, *i);
            }
          }
          else
          {
            // s is a constant (not a function sort).
            // check if there is a constant constructor for s
            for (data_specification::constructors_const_range::const_iterator i =
               std::find_if(local_constructors.begin(), local_constructors.end(), detail::has_non_function_sort());
                                                                                         i != local_constructors.end(); )
            {
               return set_representative(sort, *i);
            }

            // check if there is a constant mapping for s
            for (data_specification::mappings_const_range::const_iterator i =
                 std::find_if(local_mappings.begin(), local_mappings.end(),
                     boost::bind(std::logical_and< bool >(),
                       boost::bind(detail::has_result_sort(sort), _1), boost::bind(detail::has_non_function_sort(), _1)));
                                                                                         i != local_mappings.end(); )
            {
              return set_representative(sort, *i);
            }

            if (maximum_depth != 0)
            {
              // recursively traverse constructor functions of the form f:s1#...#sn -> sort.
              // operators with f:s1#...#sn->G where G is a complex sort expression are ignored
              for (data_specification::constructors_const_range::const_iterator i = local_constructors.begin();
                                                                                         i != local_constructors.end(); ++i)
              {
                // attempt to find representative based on constructor *i
                data_expression possible_representative = find_representative(*i, maximum_depth);

                if (possible_representative != data_expression())
                {
                  return set_representative(sort, possible_representative);
                }
              }

              for (data_specification::mappings_const_range::const_iterator i =
                   std::find_if(local_mappings.begin(), local_mappings.end(), detail::has_result_sort(sort));
                                                                                         i != local_mappings.end(); ++i)
              {
                // attempt to find representative based on constructor *i
                data_expression possible_representative = find_representative(*i, maximum_depth);

                if (possible_representative != data_expression())
                {
                  return set_representative(sort, possible_representative);
                }
              }
            }
          }

          return set_representative(sort, data_expression());
        }

      public:

        /// \brief Constructor with data specification as context
        representative_generator(data_specification const& specification) : m_specification(specification)
        {
        }

        /// \brief Returns a representative of a sort
        /// \param[in] sort sort of which to find a representatitive
        /// \param[in] maximum_depth unfold generate terms recursively up to this depth
        data_expression operator()(sort_expression const& sort, const unsigned int maximum_depth = 3)
        {
          for (atermpp::map< sort_expression, data_expression >::iterator i = m_representatives.find(sort); i != m_representatives.end(); )
          {
            return i->second;
          }

          return find_representative(sort, maximum_depth);
        }
    };

  } // namespace data
} // namespace mcrl2
#endif

