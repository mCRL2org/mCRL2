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

#ifndef ATERMPP_DETAIL_FUNCTION_SYMBOL_IMPLEMENTATION_H
#define ATERMPP_DETAIL_FUNCTION_SYMBOL_IMPLEMENTATION_H

#include "mcrl2/atermpp/function_symbol.h"
#include "mcrl2/atermpp/detail/function_symbol.h"


namespace atermpp
{
namespace detail
{

    template <bool CHECK>
    void increase_reference_count(const size_t n)
    {


      assert(n!=size_t(-1));
      {
#ifdef PRINT_GC_FUN_INFO
fprintf(stderr,"increase afun reference count %ld (%ld, %s)\n",n,at_lookup_table[n].reference_count,at_lookup_table[n].name.c_str());
#endif
        assert(n<at_lookup_table.size());
        if (CHECK) assert(at_lookup_table[n].reference_count>0);
        at_lookup_table[n].reference_count++;
      }
    }

    inline
    void decrease_reference_count(const size_t n)
    {
      assert(n!=size_t(-1));
      {
#ifdef PRINT_GC_FUN_INFO
fprintf(stderr,"decrease afun reference count %ld (%ld, %s)\n",n,at_lookup_table[n].reference_count,at_lookup_table[n].name.c_str());
#endif
        assert(n<at_lookup_table.size());
        assert(at_lookup_table[n].reference_count>0);

        if (--at_lookup_table[n].reference_count==0)
        {
          at_free_afun(n);
        }
      }
    }

struct constant_function_symbols
{
  public:
    atermpp::function_symbol AS_DEFAULT;
    atermpp::function_symbol AS_INT;
    atermpp::function_symbol AS_LIST;
    atermpp::function_symbol AS_EMPTY_LIST;

    // The function symbols below intentionally contain spaces, such that they are not
    // confused with function symbols that are used by applications of the aterms.
    constant_function_symbols():
      AS_DEFAULT("<undefined term>", 0),
      AS_INT("<aterm int>", 0),
      AS_LIST("<list constructor>", 2),
      AS_EMPTY_LIST("<empty list>", 0)   
    {} 

    // This function is used to explicitly initialise
    // the default constant function symbols, in case 
    // they are being used before being properly initialised.
    // This happens with initialising global variables and 
    // global and sometimes even with static variables in
    // functions, but this is compiler dependent.
    void initialise_function_symbols()
    {
      new (&AS_DEFAULT) function_symbol("<undefined term>", 0);
      new (&AS_INT) function_symbol("<aterm int>", 0);
      new (&AS_LIST) function_symbol("<list constructor>", 2);
      new (&AS_EMPTY_LIST) function_symbol("<empty list>", 0);
      // The following numbers are expected to be used. If not
      // something is most likely wrong. Moreover, some code
      // depends on low numbers to be assigned to the basic 
      // function symbols (e.g. type_is_appl).
      assert(AS_DEFAULT.number()==0);
      assert(AS_INT.number()==1);
      assert(AS_LIST.number()==2);
      assert(AS_EMPTY_LIST.number()==3);
    }
};


extern constant_function_symbols function_adm;

inline
bool AT_isValidAFun(const size_t sym)
{
  return (sym != size_t(-1) &&
          sym < detail::at_lookup_table.size() &&
          detail::at_lookup_table[sym].reference_count>0);
}

} // namespace detail

std::string ATwriteAFunToString(const function_symbol &t);

} // namespace atermpp
#endif // ATERMPP_DETAIL_FUNCTION_SYMBOL_IMPLEMENTATION_H

