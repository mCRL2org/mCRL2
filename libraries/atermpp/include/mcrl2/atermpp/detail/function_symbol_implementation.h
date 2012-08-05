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

    extern std::vector < detail::_function_symbol > at_lookup_table;
    
    template <bool CHECK>
    void increase_reference_count(const size_t n)
    {

      if (n!=size_t(-1))
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
      if (n!=size_t(-1))
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

    constant_function_symbols():
      AS_DEFAULT("<undefined term>", 0),
      AS_INT("<int>", 0),
      AS_LIST("[_,_]", 2),
      AS_EMPTY_LIST("[]", 0) 
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
      new (&AS_INT) function_symbol("<int>", 0);
      new (&AS_LIST) function_symbol("[_,_]", 2);
      new (&AS_EMPTY_LIST) function_symbol("[]", 0);
    }
};


extern constant_function_symbols function_adm;

} // namespace detail

inline
const std::string &function_symbol::name() const
{
  assert(AT_isValidAFun(m_number));
  return detail::at_lookup_table[m_number].name;
}

inline
size_t function_symbol::arity() const
{
  assert(AT_isValidAFun(m_number));
  return detail::at_lookup_table[m_number].arity();
}

inline
bool function_symbol::is_quoted() const
{
  assert(AT_isValidAFun(m_number));
  return detail::at_lookup_table[m_number].is_quoted();
}

inline
bool AT_isValidAFun(const size_t sym)
{
  return (sym != size_t(-1) &&
          sym < detail::at_lookup_table.size() &&
          detail::at_lookup_table[sym].reference_count>0);
}

size_t AT_printAFun(const size_t sym, FILE* f);

std::string ATwriteAFunToString(const function_symbol &t);


} // namespace atermpp
#endif // ATERMPP_DETAIL_FUNCTION_SYMBOL_IMPLEMENTATION_H

