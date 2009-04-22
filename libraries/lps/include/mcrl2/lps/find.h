// Author(s): Wieger Wesselink, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/find.h
/// \brief Search functions of the data library.

#ifndef MCRL2_LPS_FIND_H
#define MCRL2_LPS_FIND_H

#include <algorithm>
#include <functional>

#include "mcrl2/lps/action.h"
#include "mcrl2/lps/summand.h"
#include "mcrl2/data/find.h"

namespace mcrl2 {

  namespace lps {

    /// \cond INTERNAL_DOCS
    namespace detail {
      template <typename MatchPredicate, typename OutputIterator>
      struct free_variable_find_helper : public data::detail::free_variable_find_helper< MatchPredicate, OutputIterator >
      {
        using data::detail::free_variable_find_helper< MatchPredicate, OutputIterator >::find_all_if;

        void find_all_if(action const& a)
        {
          data::data_expression_list arguments(a.arguments());

          for (data::data_expression_list::const_iterator i = arguments.begin(); i != arguments.end(); ++i)
          {
            find_all_if(*i);
          }
        }

        template < typename T >
        void find_all_if(atermpp::term_list< T > const& l)
        {
          for (typename atermpp::term_list< T >::const_iterator i(l.begin()); i != l.end(); ++i)
          {
            find_all_if(*i);
          }
        }

        void find_all_if(summand const& s)
        {
          find_all_if(s.condition());
          find_all_if(s.actions());
          find_all_if(s.time());
          find_all_if(s.assignments());
        }

        free_variable_find_helper(MatchPredicate& match,
                                  OutputIterator const& destBegin,
		                  std::set< data::variable > const& bound_by_context) :
           data::detail::free_variable_find_helper< MatchPredicate, OutputIterator >(match, destBegin, bound_by_context)
        {
        }

        free_variable_find_helper(MatchPredicate& match, OutputIterator const& destBegin) :
           data::detail::free_variable_find_helper< MatchPredicate, OutputIterator >(match, destBegin)
        {
        }
      };

      template <typename T, typename MatchPredicate, typename OutputIterator>
      void find_all_free_variables_if(atermpp::term_list< T > const& t, MatchPredicate match, OutputIterator const& destBegin)
      {
        free_variable_find_helper< MatchPredicate, OutputIterator > context(match, destBegin);
  
        for (typename atermpp::term_list< T >::const_iterator i = t.begin(); i != t.end(); ++i)
        {
          context.find_all_if(*i);
        }
      }
  
      template <typename T, typename MatchPredicate, typename OutputIterator>
      void find_all_free_variables_if(T const& t, MatchPredicate match, OutputIterator const& destBegin, typename boost::disable_if< typename data::detail::is_container< T >::type >::type* = 0)
      {
        free_variable_find_helper< MatchPredicate, OutputIterator > context(match, destBegin);
  
        context.find_all_if(t);
      }
  
      template <typename T, typename MatchPredicate, typename OutputIterator>
      void find_all_free_variables_if(T const& t, MatchPredicate match, OutputIterator const& destBegin, typename boost::enable_if< typename data::detail::is_container< T >::type >::type* = 0)
      {
        free_variable_find_helper< MatchPredicate, OutputIterator > context(match, destBegin);
  
        for (typename T::const_iterator i = t.begin(); i != t.end(); ++i)
        {
          context.find_all_if(*i);
        }
      }

      template <typename T, typename MatchPredicate, typename OutputIterator>
      void find_all_free_variables_if(T const& t, std::set< data::variable > const& bound, MatchPredicate match,
                                      OutputIterator const& destBegin, typename boost::disable_if< typename data::detail::is_container< T >::type >::type* = 0)
      {
        free_variable_find_helper< MatchPredicate, OutputIterator > context(match, destBegin, bound);
  
        context.find_all_if(t);
      }
  
      /// \brief Returns all data variables that occur in a range of expressions
      /// \param[in] container a container with expressions
      /// \return All data variables that occur in the term t
      template <typename Container, typename OutputIterator >
      void find_all_free_variables(Container const& container, std::set< data::variable > const& bound, OutputIterator const& o)
      {
        detail::find_all_free_variables_if(container, bound, boost::bind(&data::detail::is_variable, _1), o);
      }
    } // namespace detail

    /// \brief Returns all data variables that occur in a range of expressions
    /// \param[in] container a container with expressions
    /// \return All data variables that occur in the term t
    template <typename Container, typename OutputIterator >
    void find_all_free_variables(Container const& container, OutputIterator const& o)
    {
      detail::find_all_free_variables_if(container, boost::bind(&data::detail::is_variable, _1), o);
    }

    /// \brief Returns all data variables that occur in a range of expressions
    /// \param[in] container a container with expressions
    /// \return All data variables that occur in the term t
    template <typename Container >
    std::set< data::variable > find_all_free_variables(Container const& container)
    {
      std::set< data::variable > result;
    
      find_all_free_variables(container, data::detail::make_inserter(result));
    
      return result;
    }
    
  } // namespace  lps
} // namespace mcrl2

#endif

