// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file atermpp/source/function_symbol.cpp
/// \brief This file provides the data structures for the function symbols occurring in 
///        the atermpp library. 

#include <cctype>
#include <cstdlib>
#include <cstdarg>
#include <cassert>
#include <stdexcept>

#include <set>
#include <cstring>
#include <sstream>


#include "mcrl2/utilities/logger.h"
#include "mcrl2/atermpp/aterm.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

namespace atermpp
{

/* Global variable. */
bool function_symbol::m_function_symbol_store_is_defined=false;
bool function_symbol::aterm_administration_is_initialised=false;
function_symbol function_symbol::AS_DEFAULT;

namespace detail
{
  /* More global variables. */
   constant_function_symbols function_adm;


  // A map that records for each prefix a function that must be called to set the
  // postfix number to a sufficiently high number if a function symbol with the same
  // prefix string is registered.

  static std::map < std::string, detail::index_increaser> prefix_to_register_function_map;

  std::size_t get_sufficiently_large_postfix_index(const std::string& prefix_)
  {
    std::size_t index=0;
    for(const detail::_function_symbol& f: function_symbol::function_symbol_store())
    {
      const std::string& function_name=f.first.name();
  
      if (function_name.compare(0,prefix_.size(),prefix_)==0)   // The function name starts with the prefix
      {
        std::string potential_number=function_name.substr(prefix_.size()); // Get the trailing string after prefix_ of function_name.
        std::size_t end_of_number;
        try
        {
          std::size_t number=std::stol(potential_number,&end_of_number);
          if (end_of_number==potential_number.size()) // A proper number was read.
          {
            if (number>=index)
            {
              index=number+1;
            }
          }
        }
        catch (std::exception&)
        {
          // Can be std::invalid_argument or an out_of_range exception.
          // In both cases nothing needs to be done, and the exception can be ignored.
        }
      }
      
    }
    return index;
  }

  // register a prefix for a function symbol, such that the index of this prefix can be increased when
  // some other process makes a function symbol with the same prefix.
  void register_function_symbol_prefix_string(const std::string& prefix, index_increaser& increase_index)
  {
    prefix_to_register_function_map[prefix]=increase_index;
  }

  // deregister a prefix for a function symbol.
  void deregister_function_symbol_prefix_string(const std::string& prefix)
  {
    prefix_to_register_function_map.erase(prefix);
  }


  void initialise_function_map_administration()
  {
    // Take care that the prefix_to_register_function_map is initialized. Use a placement new, because
    // the memory address of prefix_to_register_function_map may be unitialized.
    new (&prefix_to_register_function_map) std::map < std::string, detail::index_increaser>();
  }
} // namespace detail

function_symbol::function_symbol(const std::string& name_, const std::size_t arity_, const bool check_for_registered_functions)
{
  initialise_aterm_administration_if_needed();
  function_symbol_iterator_bool_pair 
       i=function_symbol_store().emplace(detail::_function_symbol_primary_data(name_,arity_),
                                         detail::_function_symbol_auxiliary_data(0));
  m_function_symbol=&(*(i.first));
  increase_reference_count<false>();

  if (i.second && check_for_registered_functions) // The element is not already present in the unordered set.
  {
    // Check whether there is a registered prefix p such that name equal pn where n is a number.
    // In that case prevent that pn will be generated as a fresh function name.
    std::size_t start_of_index=name().find_last_not_of("0123456789")+1;
    if (start_of_index<name().size()) // Otherwise there is no trailing number.
    {
      std::string potential_number=name().substr(start_of_index); // Get the trailing string after prefix_ of function_name.
      std::string prefix=name().substr(0,start_of_index);
      std::map < std::string, detail::index_increaser>::iterator i=detail::prefix_to_register_function_map.find(prefix);
      if (i!=detail::prefix_to_register_function_map.end())  // i points to the prefix.
      {
        try
        {
          std::size_t number=std::stol(potential_number);
          i->second(number+1); // Set the index belonging to the found prefix to at least a safe number+1.
        }
        catch (std::exception)
        {
          // Can be std::invalid_argument or an out_of_range exception.
          // In both cases nothing needs to be done, and the exception can be ignored.
        }
      }
    }
  }

}

} // namespace atermpp

