// Author(s): Jan Friso Groote, based on the ATerm library.
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/function_symbol.h
/// \brief Function symbol class.

#ifndef DETAIL_ATERM_ADMINISTRATION_H
#define DETAIL_ATERM_ADMINISTRATION_H

#include <iostream>
#include <vector>
#include "mcrl2/atermpp/detail/atypes.h"
#include "mcrl2/atermpp/detail/function_symbol.h"

namespace atermpp
{
namespace detail
{

class _aterm;

class aterm_administration
{
  protected:
    static const size_t INITIAL_AFUN_TABLE_CLASS = 14;
    static const size_t INITIAL_TERM_TABLE_CLASS = 17;
    
  public:
    size_t first_free;
    std::vector < detail::_function_symbol* > at_lookup_table;
    
    size_t afun_table_class;
    // size_t afun_table_size;
    size_t afun_table_mask;
    std::vector < size_t > function_symbol_hashtable;
    
    size_t table_class;
    // size_t table_size;
    size_t table_mask;
 
    std::vector <detail::_aterm*> aterm_hashtable;
    
    void initialise_aterm_administration()
    {
      if (aterm_hashtable.size()==0)
      {
        first_free=size_t(-1);
        afun_table_class=INITIAL_AFUN_TABLE_CLASS;
        // afun_table_size=AT_TABLE_SIZE(INITIAL_AFUN_TABLE_CLASS);
        afun_table_mask=AT_TABLE_MASK(INITIAL_AFUN_TABLE_CLASS);
        function_symbol_hashtable=std::vector < size_t >(AT_TABLE_SIZE(INITIAL_AFUN_TABLE_CLASS),size_t(-1));
   
        table_class=INITIAL_TERM_TABLE_CLASS;
        // table_size=AT_TABLE_SIZE(INITIAL_TERM_TABLE_CLASS);
        table_mask=AT_TABLE_MASK(INITIAL_TERM_TABLE_CLASS);
   
        aterm_hashtable=std::vector <detail::_aterm*>(AT_TABLE_SIZE(INITIAL_TERM_TABLE_CLASS),NULL);
  
      }
    }

    aterm_administration():
        first_free(first_free),
        afun_table_class(afun_table_class),
        // afun_table_size(afun_table_size),
        afun_table_mask(afun_table_mask),
        function_symbol_hashtable(function_symbol_hashtable),
   
        table_class(table_class),
        // table_size(table_size),
        table_mask(table_mask),
   
        aterm_hashtable(aterm_hashtable)
    { 
      initialise_aterm_administration();
    }

    ~aterm_administration()
    {
      // Reset aterm administration, to make clear that it cannot be used anymore.
      first_free=size_t(-1);
      afun_table_class=0;
      afun_table_mask=0;
      function_symbol_hashtable=std::vector < size_t >();
        
      table_class=0;
      // table_size=0;
      table_mask=0;
    
      aterm_hashtable=std::vector <detail::_aterm*>();
    }

    static _aterm *undefined_aterm();
    static _aterm *empty_aterm_list();
};

extern aterm_administration adm;

// _aterm* undefined_aterm();

} // namespace detail
} // namespace atermpp

#include "mcrl2/atermpp/detail/function_symbol_implementation.h"

#endif // DETAIL_ATERM_ADMINISTRATION_H
