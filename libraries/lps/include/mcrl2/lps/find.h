// Author(s): Wieger Wesselink, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/find.h
/// \brief Search functions of the data library.

#ifndef MCRL2_LPS_FIND_H
#define MCRL2_LPS_FIND_H

#include <algorithm>
#include <functional>

#include "mcrl2/lps/action.h"
#include "mcrl2/new_data/find.h"

namespace mcrl2 {

  namespace lps {

    /// \cond INTERNAL_DOCS
    namespace detail {
      template <typename MatchPredicate, typename OutputIterator>
      struct free_variable_find_helper : public new_data::detail::free_variable_find_helper< MatchPredicate, OutputIterator >
      {
        using new_data::detail::free_variable_find_helper< MatchPredicate, OutputIterator >::find_all_if;

        void find_all_if(action const& a)
        {
          new_data::data_expression_list arguments(a.arguments());

          for (new_data::data_expression_list::const_iterator i = arguments.begin(); i != arguments.end(); ++i)
          {
            find_all_if(*i);
          }
        }

        free_variable_find_helper(MatchPredicate& match, OutputIterator const& destBegin) :
           new_data::detail::free_variable_find_helper< MatchPredicate, OutputIterator >(match, destBegin)
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
      void find_all_free_variables_if(T const& t, MatchPredicate match, OutputIterator const& destBegin, typename boost::disable_if< typename new_data::detail::is_container< T >::type >::type* = 0)
      {
        free_variable_find_helper< MatchPredicate, OutputIterator > context(match, destBegin);
  
        context.find_all_if(t);
      }
  
      template <typename T, typename MatchPredicate, typename OutputIterator>
      void find_all_free_variables_if(T const& t, MatchPredicate match, OutputIterator const& destBegin, typename boost::enable_if< typename new_data::detail::is_container< T >::type >::type* = 0)
      {
        free_variable_find_helper< MatchPredicate, OutputIterator > context(match, destBegin);
  
        for (typename T::const_iterator i = t.begin(); i != t.end(); ++i)
        {
          context.find_all_if(*i);
        }
      }
    } // namespace detail

    /// \brief Returns all data variables that occur in a range of expressions
    /// \param[in] container a container with expressions
    /// \return All data variables that occur in the term t
    template <typename Container, typename OutputIterator >
    void find_all_free_variables(Container const& container, OutputIterator const& o)
    {
      detail::find_all_free_variables_if(container, boost::bind(&new_data::detail::is_variable, _1), o);
    }

    /// \brief Returns all data variables that occur in a range of expressions
    /// \param[in] container a container with expressions
    /// \return All data variables that occur in the term t
    template <typename Container >
    std::set< new_data::variable > find_all_free_variables(Container const& container)
    {
      std::set< new_data::variable > result;
    
      find_all_free_variables(container, new_data::detail::make_inserter(result));
    
      return result;
    }
    
  } // namespace  lps
} // namespace mcrl2

#endif

