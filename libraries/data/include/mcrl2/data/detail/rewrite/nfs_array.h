// Author(s): Sjoerd Cranen 
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/rewrite/nfs_array.h
/// \brief ?

#ifndef MCRL2_DATA_NFS_ARRAY_H
#define MCRL2_DATA_NFS_ARRAY_H

#include <cstddef>
#include <cstring>
#include <cassert>

// Maximal arity for which we generate functions for every combination of
// arguments that are in normal form or not. The value of 4 is chosen because a function
// updat has four arguments. This may yield 2^4=16 variants of each compiled function.
#define NF_MAX_ARITY 4  

namespace mcrl2
{
namespace data
{
namespace detail
{

class nfs_array
{
public:
  nfs_array(size_t size) : m_array(new size_t[size])
  {
    /* Explicitly initialise this array, as this is not done 
       automatically when it is constructed */

    for(size_t i=0; i<size; ++i)
    {
      m_array[i]=0;
    }
  }
  ~nfs_array()
  {
    delete[] m_array;
  }
  void clear(size_t arity)
  {
    if (arity > 0)
    {
      memset(m_array, 0, ((arity-1)/(sizeof(size_t)*8)+1)*sizeof(size_t));
    }
  }
  void fill(size_t arity, bool val = true)
  {
    size_t newval = val ? (~((size_t)0)) : 0;
    for(size_t i = 0; i * sizeof(size_t) * 8 < arity; ++i)
    {
      m_array[i] = newval;
    }
  }

  size_t get_value(size_t arity)
  {
    assert(arity <= NF_MAX_ARITY);
    return m_array[0] & (((size_t)1 << arity) - 1);
  }

  void set_value(size_t arity, size_t val)
  {
    assert(arity <= NF_MAX_ARITY || val == 0);
    m_array[0] = val;
  } 

  bool equals(nfs_array& other, size_t arity)
  {
    size_t i = 0;
    while (arity >= sizeof(size_t)*8)
    {
      if (m_array[i] != other.m_array[i])
      {
        return false;
      }
      arity -= sizeof(size_t)*8;
      ++i;
    }
    return (m_array[i] & ((1 << arity)-1)) == (other.m_array[i] & ((1 << arity)-1));
  } 

  bool get(size_t i)
  {
    return (m_array[i/(sizeof(size_t)*8)] & (((size_t) 1) << (i%(sizeof(size_t)*8))))>0;
  }

  void set(size_t i, bool val = true)
  {
    if (val)
    {
      m_array[i/(sizeof(size_t)*8)] |= ((size_t) 1) << (i%(sizeof(size_t)*8));
    }
    else
    {
      m_array[i/(sizeof(size_t)*8)] &= ~(((size_t) 1) << (i%(sizeof(size_t)*8)));
    }
  }

  bool is_clear(size_t arity)
  {
    size_t i = 0;
    while (arity >= sizeof(size_t)*8)
    {
      if (m_array[i++] != ((size_t) 0))
      {
        return false;
      }
      arity -= sizeof(size_t)*8;
    }
    return (m_array[i] & ((1 << arity)-1)) == 0;
  }

  bool is_filled(size_t arity)
  {
    size_t i = 0;
    while (arity >= sizeof(size_t)*8)
    {
      if (m_array[i++] != ~((size_t) 0))
      {
        return false;
      }
      arity -= sizeof(size_t)*8;
    }
    return (m_array[i] & ((1 << arity)-1)) == (size_t)((1 << arity)-1);
  }

  size_t getraw(size_t i)
  {
    return m_array[i];
  } 

private:
  size_t *m_array;
};

}
}
}

#endif // MCRL2_DATA_NFS_ARRAY_H
