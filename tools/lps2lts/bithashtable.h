// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bithashtable.h

#include "boost.hpp" // precompiled headers

#ifndef MCRL2_LPS2LTS_BITHASHTABLE_H
#define MCRL2_LPS2LTS_BITHASHTABLE_H

// 96 bit mix function of Robert Jenkins
#define mix(a,b,c) \
  { a -= b; a -= c; a ^= (c>>13); \
    b -= c; b -= a; b ^= (a<<8);  \
    c -= a; c -= b; c ^= (b>>13); \
    a -= b; a -= c; a ^= (c>>12); \
    b -= c; b -= a; b ^= (a<<16); \
    c -= a; c -= b; c ^= (b>>5);  \
    a -= b; a -= c; a ^= (c>>3);  \
    b -= c; b -= a; b ^= (a<<10); \
    c -= a; c -= b; c ^= (b>>15); \
  }

#include <aterm2.h>
#include "boost/cstdint.hpp"
#include "mcrl2/exception.h"

class bit_hash_table
{
  private:
    unsigned long sh_a,sh_b,sh_c;
    unsigned int sh_i;
    boost::uint64_t bithashsize;
    boost::uint64_t table_size;
    unsigned long* bithashtable;

  public:
    bit_hash_table() :
      sh_a(0x9e3779b9),
      sh_b(0x65e3083a),
      sh_c(0xa45f7582),
      sh_i(0),
      bithashsize(0),
      bithashtable(NULL)
    {};

    bit_hash_table(boost::uint64_t const size) :
      sh_a(0x9e3779b9),
      sh_b(0x65e3083a),
      sh_c(0xa45f7582),
      sh_i(0),
      bithashsize(size)
    {
      if ( bithashsize > ULLONG_MAX-4*sizeof(unsigned long) )
      {
        table_size = (1ULL << (sizeof(boost::uint64_t)*8-3)) / sizeof(unsigned long);
      } else {
        table_size = (bithashsize+4*sizeof(unsigned long))/(8*sizeof(unsigned long));
      }
      bithashtable = (unsigned long *) calloc(table_size,sizeof(unsigned long)); // sizeof(unsigned int) * lgopts->bithashsize/(8*sizeof(unsigned int))
      if ( bithashtable == NULL )
      {
        throw mcrl2::runtime_error("cannot create bit has table");
      }
    };

    ~bit_hash_table()
    {
      if(bithashtable != NULL)
      {
        free(bithashtable);
      }
    }

  private:
    void calc_hash_init()
    {
      sh_a = 0x9e3779b9;
      sh_b = 0x65e3083a;
      sh_c = 0xa45f7582;
      sh_i = 0;
    }

    void calc_hash_add(unsigned long n)
    {
      switch ( sh_i )
      {
        case 0:
          sh_a += n;
          sh_i = 1;
          break;
        case 1:
          sh_b += n;
          sh_i = 2;
          break;
        case 2:
          sh_c += n;
          sh_i = 0;
          mix(sh_a,sh_b,sh_c);
          break;
      }
    }

    boost::uint64_t calc_hash_finish()
    {
      while ( sh_i != 0 )
      {
        calc_hash_add(0x76a34e87);
      }
      return (((boost::uint64_t) (sh_a & 0xffff0000)) << 24) |
             (((boost::uint64_t) (sh_b & 0xffff0000)) << 16) |
             (((boost::uint64_t) (sh_c & 0xffff0000))     ) |
             ((sh_a & 0x0000ffff)^(sh_b & 0x0000ffff)^(sh_c & 0x0000ffff));
    }

    void calc_hash_aterm(ATerm t)
    {
      switch ( ATgetType(t) )
      {
        case AT_APPL:
          calc_hash_add(0x13ad3780);
          {
            unsigned int len = ATgetArity(ATgetAFun((ATermAppl) t));
            for (unsigned int i=0; i<len; i++)
            {
              calc_hash_aterm(ATgetArgument((ATermAppl) t, i));
            }
          }
          break;
        case AT_LIST:
          calc_hash_add(0x7eb9cdba);
          for (ATermList l=(ATermList) t; !ATisEmpty(l); l=ATgetNext(l))
          {
            calc_hash_aterm(ATgetFirst(l));
          }
          break;
        case AT_INT:
          calc_hash_add(ATgetInt((ATermInt) t));
          break;
        default:
          calc_hash_add(0xaa143f06);
          break;
      }
    }

    boost::uint64_t calc_hash(ATerm state)
    {
      calc_hash_init();
    
      calc_hash_aterm(state);
    
      return calc_hash_finish() % bithashsize;
    }

    bool get_bithash(boost::uint64_t i)
    {
      return (( bithashtable[i/(8*sizeof(unsigned long))] >> (i%(8*sizeof(unsigned long))) ) & 1UL) == 1UL;
    }

    void set_bithash(boost::uint64_t i)
    {
      bithashtable[i/(8*sizeof(unsigned long))] |= 1UL << (i%(8*sizeof(unsigned long)));
    }

  public:
    void remove_state_from_bithash(ATerm state)
    {
      boost::uint64_t i = calc_hash(state);
      bithashtable[i/(8*sizeof(unsigned long))] &=  ~(1UL << (i%(8*sizeof(unsigned long))));
    }

    boost::uint64_t add_state(ATerm state, bool *is_new)
    {
      boost::uint64_t i = calc_hash(state);
      *is_new = !get_bithash(i);
      set_bithash(i);
      return i;
    }

    boost::uint64_t state_index(ATerm state)
    {
      assert(get_bithash(calc_hash(state)));
      return calc_hash(state);
    }
};

#endif // MCRL2_LPS2LTS_BITHASHTABLE_H

