// Author(s): Jeroen Keiren, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/sort_utility.h
/// \brief Utility functions for sorts.

#ifndef MCRL2_DATA_SORT_UTILITY_H
#define MCRL2_DATA_SORT_UTILITY_H

#include <vector>
#include <cassert>
#include "mcrl2/data/data.h"
#include "mcrl2/data/data_specification.h"

namespace mcrl2 {

  namespace data {
    namespace detail {

      /// Test is a term is a sort, and if it is equal to s
      struct compare_sort : public std::unary_function< bool, atermpp::aterm_appl >
      {
        sort_expression s;

        compare_sort(sort_expression s_)
          : s(s_)
        {}

        bool operator()(atermpp::aterm_appl t) const
        {
          return s == t;
        }
      };

      ///\pre l is a list type of some sort (e.g. sort_expression_list)
      ///\return sort s occurs in l.
      template <typename list_type>
      bool occurs_in(list_type l, sort_expression s)
      {
        return atermpp::find_if(l, compare_sort(s)) != atermpp::aterm();
      }

      template < typename ForwardTraversalIterator >
      bool occurs_in(ForwardTraversalIterator const& begin, ForwardTraversalIterator const& end, sort_expression s)
      {
        for (ForwardTraversalIterator i = begin; i != end; ++i) {
          if (atermpp::find_if(*i, compare_sort(s)) != atermpp::aterm()) {
            return true;
          }
        }

        return false;
      }

      template < typename ForwardTraversalIterator >
      bool occurs_in(boost::iterator_range< ForwardTraversalIterator > const& range, sort_expression s)
      {
        return occurs_in(range.begin(), range.end(), s);
      }

      ///\return the list of all functions f of sort s in fl
      inline function_symbol_vector get_constructors(const function_symbol_list& fl, const sort_expression& s)
      {
        function_symbol_vector result;
        for(function_symbol_list::const_iterator i = fl.begin(); i != fl.end(); ++i)
        {
          if ((!i->sort().is_function_sort() && i->sort() == s) || function_sort(i->sort()).codomain() == s)
          {
            result.push_back(*i);
          }
        }
        return result;
      }

      ///\pre s is a sort that occurs in data_specfication data
      ///\return true iff there exists a constructor function with s as target sort
      inline bool is_constructorsort(const sort_expression &s,const data_specification &data)
      { // This function is added by Jan Friso Groote on 8/7/2007.
        // cl contains all constructors with target sort s.

        if (s.is_function_sort())
        {
          return false;
        }

        assert(occurs_in(data.sorts(),s));
        return (!data.constructors(s).empty());
      }

      /// Undocumented function.
      inline
      data::sort_expression_vector get_sorts(data::variable_list v)
      {
        data::sort_expression_vector result;
        for (data::variable_list::iterator i = v.begin(); i != v.end(); i++)
        {
          result.push_back(i->sort());
        }
        return result;
      }

      /// Undocumented function.
      inline
      data::sort_expression_vector get_sorts(data::data_expression_list v)
      {
        data::sort_expression_vector result;
        for (data::data_expression_list::iterator i = v.begin(); i != v.end(); i++)
        {
          result.push_back(i->sort());
        }
        return result;
      }

      /// Undocumented function.
      inline
      data::data_expression_vector enumerate_constructors(data::data_specification const& d, data::sort_expression s)
      {
        // All datasorts which are taken into account must be finite. Normally this is the case, because a check on finiteness is done in create_bes
        assert(d.is_certainly_finite(s));
        // The resulting data::data_expression_list.
        data_expression_vector ces;
        // For each constructor of sort s...
        for (data_specification::constructors_const_range r(d.constructors(s)); !r.empty(); r.advance_begin(1))
        {
          if (r.front().sort().is_function_sort())
          {
            // Result list
            data::data_expression_vector result;
            // At first put function f in result
            result.push_back(data::data_expression(r.front()));

            // For each sort of the constructor...
            for (function_sort::domain_const_range j(function_sort(r.front().sort()).domain()); !j.empty(); j.advance_begin(1))
            {
              // Put all values which the sort can have in a list
              data::data_expression_vector constructor_terms(enumerate_constructors(d, j.front()));

              //*i is a list of constructor expressions that should be applied to the elements of result
              data::data_expression_vector temporary;

              for (data::data_expression_vector::const_iterator k = constructor_terms.begin(); k != constructor_terms.end(); ++k)
              //*k is a constructor expression that should be applied to the elements of result
              {
                for (data::data_expression_vector::iterator l = result.begin(); l != result.end(); ++l)
                {
                  //  apply *l to *k
                  //  One of the constructor values is applied to f
                  temporary.push_back(data::application(*l, *k));
                }
              }
              // Next iteration replace all values which are created in tmp
              result.swap(temporary);
            }
            //concatenate ces and result
            ces.insert(ces.end(), result.begin(), result.end());
          }
          else {
            ces.push_back(r.front());
          }
        }
        // Put ces in the correct order
        return ces;
      }
   }

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SORT_UTILITY_H
