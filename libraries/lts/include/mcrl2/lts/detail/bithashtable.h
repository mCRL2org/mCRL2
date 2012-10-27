// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lts/bithashtable.h

#include "boost.hpp" // for precompiled headers

#ifndef MCRL2_LTS_DETAIL_BITHASHTABLE_H
#define MCRL2_LTS_DETAIL_BITHASHTABLE_H

#include <vector>
#include "mcrl2/exception.h"

namespace mcrl2
{
namespace lts
{

using namespace atermpp;

class bit_hash_table
{
  private:
    typedef std::vector < bool>::size_type size_t;

    std::vector < bool > m_bit_hash_table;

  public:
    bit_hash_table()
    {};

    bit_hash_table(const size_t size) :
      m_bit_hash_table(size,false)
    {};

    ~bit_hash_table()
    {}

  private:


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

    void calc_hash_add(const size_t n,
                       size_t& sh_a,
                       size_t& sh_b,
                       size_t& sh_c,
                       size_t& sh_i)
    {
      switch (sh_i)
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

    void calc_hash_aterm(aterm t,
                         size_t& sh_a,
                         size_t& sh_b,
                         size_t& sh_c,
                         size_t& sh_i)
    {
      switch (t.type())
      {
        case AT_APPL:
          calc_hash_add(0x13ad3780,sh_a,sh_b,sh_c,sh_i);
          {
            size_t len = aterm_cast<aterm_appl>(t).function().arity();
            for (size_t i=0; i<len; i++)
            {
              calc_hash_aterm(((aterm_appl) t)(i),sh_a,sh_b,sh_c,sh_i);
            }
          }
          break;
        case AT_LIST:
          calc_hash_add(0x7eb9cdba,sh_a,sh_b,sh_c,sh_i);
          for (aterm_list l=(aterm_list) t; !l.empty(); l=l.tail())
          {
            calc_hash_aterm(l.front(),sh_a,sh_b,sh_c,sh_i);
          }
          break;
        case AT_INT:
          calc_hash_add(((aterm_int) t).value(),sh_a,sh_b,sh_c,sh_i);
          break;
        default:
          calc_hash_add(0xaa143f06,sh_a,sh_b,sh_c,sh_i);
          break;
      }
    }

    size_t calc_hash_finish(size_t& sh_a,
                            size_t& sh_b,
                            size_t& sh_c,
                            size_t& sh_i)
    {
      while (sh_i != 0)
      {
        calc_hash_add(0x76a34e87,sh_a,sh_b,sh_c,sh_i);
      }
      return (((size_t)(sh_a & 0xffff0000)) << 24) |
             (((size_t)(sh_b & 0xffff0000)) << 16) |
             (((size_t)(sh_c & 0xffff0000))) |
             ((sh_a & 0x0000ffff)^(sh_b & 0x0000ffff)^(sh_c & 0x0000ffff));
    }

    size_t calc_hash(aterm state)
    {
      assert(m_bit_hash_table.size()>0);
      size_t sh_a = 0x9e3779b9;
      size_t sh_b = 0x65e3083a;
      size_t sh_c = 0xa45f7582;
      size_t sh_i = 0;

      calc_hash_aterm(state,sh_a,sh_b,sh_c,sh_i);
      return calc_hash_finish(sh_a,sh_b,sh_c,sh_i) % m_bit_hash_table.size();
    }

  public:
    void remove_state_from_bithash(const aterm state)
    {
      size_t i = calc_hash(state);
      m_bit_hash_table[i] = false;
    }

    /* void remove_state_from_bithash(atermpp::aterm state)
    {
      remove_state_from_bithash((const aterm)state);
    } */

    size_t add_state(aterm state, bool& is_new)
    {
      size_t i = calc_hash(state);
      is_new = !m_bit_hash_table[i];
      m_bit_hash_table[i] = true;
      return i;
    }

    std::pair<size_t, bool> add_state(atermpp::aterm state)
    {
      std::pair<size_t, bool> output;
      output.first = add_state(state, output.second);
      return output;
    }

    size_t state_index(aterm state)
    {
      assert(m_bit_hash_table[calc_hash(state)]);
      return calc_hash(state);
    }
};


}
}

#endif // MCRL2_LTS_DETAIL_BITHASHTABLE_H

