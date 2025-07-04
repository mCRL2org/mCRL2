// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/representative_generator.h
/// \brief Component for generating representatives of sorts

#ifndef MCRL2_DATA_REPRESENTATIVE_GENERATOR_H
#define MCRL2_DATA_REPRESENTATIVE_GENERATOR_H

#include "mcrl2/data/data_specification.h"

namespace mcrl2::data
{
/// \brief Components for generating an arbitrary element of a sort
/// \details This component delivers a term based on a lexicographic ordering of
///          function symbols. It delivers the same element each time it is run.
///
/// A representative is an arbitrary element of a given sort. This
/// component takes a specification and generates representatives for sorts
/// defined by the specification. An important property is that for the
/// same sort the same representative is returned. For this it assumes
/// that the context -constructors and mappings for the sort- remain
/// unchanged.
///
/// The general aim is to keep the representative expression simple. Use of
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
/// representative a certain sort, for instance containing function symbols
/// with complex target sorts, containing explicit function constructors
/// (lambda's). So, no reliance is possible on the particular shape of the
/// terms that are generated.

class representative_generator
{

  protected:

    /// \brief Data specification context
    const data_specification& m_specification;

    /// \brief Serves as a cache for later find operations
    std::map< sort_expression, data_expression > m_representatives_cache;

  protected:

    /// \brief Sets a data expression as representative of the sort
    /// \param[in] sort the sort of which to set the representative
    /// \param[in] representative the data expression that serves as representative
    void set_representative(const sort_expression& sort, const data_expression& representative)
    {
      assert(sort==representative.sort());
      m_representatives_cache[sort] = representative;
    }

    /// \brief Finds a representative of the form f(t1,...,tn) where f is the function symbol.
    /// \param[in] symbol The function symbol f using which the representative is constructed.
    /// \param[in] visited_sorts A set of sorts for which no representative can be constructed. This is used to prevent 
    //                           an infinite circular search through the sorts.
    /// \param[out] result The representative of the shape f(t1,...,tn). This is only set if this function yields true.
    /// \return a boolean indicating whether a representative has successfully been found.
    /// \pre symbol.sort() is of type function_sort
    bool find_representative(
                       const function_symbol& symbol, 
                       std::set < sort_expression >& visited_sorts,
                       data_expression& result) 
    {
      assert(is_function_sort(symbol.sort()));

      data_expression_vector arguments;

      for (const sort_expression& s: static_cast<const function_sort&>(symbol.sort()).domain())
      {
        data_expression representative;
        if (find_representative(s, visited_sorts, representative))
        {
          arguments.push_back(representative);
        }
        else
        {
          // a representative for this argument could not be found.
          return false;
        }
      }

      // a suitable set of arguments is found
      result=application(symbol, arguments);
      return true;
    }

    bool search_for_lexicographically_minimal_symbol(function_symbol& f_result, 
                                                   const sort_expression& sort,
                                                   const std::vector<function_symbol>& function_symbols)
    {
      bool function_symbol_found=false;
      for (const function_symbol& f: function_symbols)
      {
        if (f.sort()==sort)
        {
          if (function_symbol_found) 
          {
            if (static_cast<const std::string&>(f.name())<static_cast<const std::string&>(f_result.name()))
            {
              f_result=f;
            }
          }
          else
          {
            f_result=f;
            function_symbol_found=true;
          }
        }
      }
      return function_symbol_found;
    }

    /// \brief Finds a representative element for an arbitrary sort expression
    /// \param[in] sort the sort for which to find the representative
    /// \param[in] visited_sorts A set of sorts for which no representative can be constructed. This is used to prevent 
    //                           an infinite circular search through the sorts.
    /// \param[out] result The representative of the shape f(t1,...,tn). This is only set if this function yields true.
    /// \return a boolean indicating whether a representative has successfully been found.
    bool find_representative(
                    const sort_expression& sort, 
                    std::set < sort_expression >& visited_sorts, 
                    data_expression& result)
    {
      if (visited_sorts.count(sort)>0)
      {
        // This sort is already visited. We are looking to find a representative term of this sort
        // within the scope of finding a term of this sort. If this is to be succesful, a more compact
        // term can be found. Hence, stop searching further.
        return false;
      }

      const std::map< sort_expression, data_expression >::iterator i=m_representatives_cache.find(sort);
      if (i!=m_representatives_cache.end())
      {
        assert(i->second.sort()==sort);
        result=i->second;
        return true;
      }
      
      if (is_function_sort(sort))
      {
        // s is a function sort. 
        // First search for a lexicographically minimal mapping with sort s (constructors with sort s cannot exist).
        function_symbol minimal_f;
        const function_sort& fs=atermpp::down_cast<function_sort>(sort);
        if (search_for_lexicographically_minimal_symbol(minimal_f, sort, m_specification.mappings(fs.codomain())))
        {
           result=minimal_f;
           set_representative(sort, result);
           return true;
        }

        // If no explicit constant is found of this sort, we know that its shape is "s0#..#sn -> s". 
        // We search a term t of sort s and if found, construct a lambda term of the shape 
        // lambda x0:s0,...,xn:sn.t. Note that this can be strengthened slightly by incorporating
        // the variables xi in generating t, e.g. generating for D->D  the reprentant lambd d:D.d. 
        // Incorporating the variables in the generation of terms requires adapting all algorithms,
        // and is not done. Until now the need for such representant generators has not been felt. 

        const sort_expression& s2=fs.codomain();
        data_expression t;
        if (find_representative(s2, visited_sorts, t))
        {
          variable_vector bound_variables;
          std::size_t variable_index=0;
          for(const sort_expression& s: fs.domain())
          {
            bound_variables.emplace_back("x" + std::to_string(variable_index), s);
            variable_index++;
          }
          result=abstraction(lambda_binder(),variable_list(bound_variables.begin(),bound_variables.end()),t);
          return true;
        }
      }
      else
      {
        // s is a constant (not a function sort).
        // check if there is a constant constructor for s

        function_symbol minimal_c;
        if (search_for_lexicographically_minimal_symbol(minimal_c, sort, m_specification.constructors(sort)))
        {
          result=minimal_c;
          set_representative(sort, result);
          return true;
        }

        visited_sorts.insert(sort);

        // Check whether there is a representative f(t1,...,tn) for s, where f is a constructor function. 
        // We prefer this over a constant mapping, as a constant mapping generally does not have appropriate
        // rewrite rules.
        
        // recursively traverse constructor functions of the form f:s1#...#sn -> sort.
        // operators with f:s1#...#sn->G where G is a complex sort expression are ignored

        for (const function_symbol& f: m_specification.constructors(sort))
        {
          if (find_representative(f, visited_sorts, result))
          {
            set_representative(sort, result);
            visited_sorts.erase(sort);
            return true;
          }
        }

        // check if there is a constant mapping for s
        for (const function_symbol& f: m_specification.mappings(sort))
        {
          if (f.sort()==sort)
          {
            result=f;
            set_representative(sort, result);
            visited_sorts.erase(sort);
            return true;
          }
        }

        // Try to check whether there is a representative f(t1,...,tn) where f is a mapping. 
        for (const function_symbol& f: m_specification.mappings(sort))
        {
          if (find_representative(f, visited_sorts, result))
          {
            set_representative(sort, result);
            visited_sorts.erase(sort);
            return true;
          }
        }
        
        visited_sorts.erase(sort);

      }

      // No representative has been found.
      return false;
    }

  public:

    /// \brief Constructor with data specification as context
    representative_generator(const data_specification& specification) : m_specification(specification)
    {
    }

    /// \brief Returns a representative of a sort
    /// \param[in] sort sort of which to find a representatitive
    data_expression operator()(const sort_expression& sort)
    {
      // First see whether a term of this sort has already been constructed and resides in the representatives_cache. If yes return it. 
      const std::map< sort_expression, data_expression >::iterator i=m_representatives_cache.find(sort);
      if (i!=m_representatives_cache.end())
      {
        return i->second;
      }
      
      data_expression result;
      std::set<sort_expression> visited_sorts;
      if (find_representative(sort, visited_sorts, result))
      {
        // A term of the requested sort is found. Return it. 
        return result;
      }
      else 
      {
        throw mcrl2::runtime_error("Cannot find a term of sort " + data::pp(sort));
      }
     
    }
};

} // namespace mcrl2::data

#endif

