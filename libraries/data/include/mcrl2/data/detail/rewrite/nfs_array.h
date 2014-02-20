// Author(s): Sjoerd Cranen, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/rewrite/nfs_array.h
/// \brief This is an array in which it is recorded which arguments 
///        are normal forms and which are not.

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

class nfs_array:public std::vector<bool>
{
public:
  nfs_array(size_t size): 
       std::vector<bool>(size,false)
  {
  }

  void fill(bool val = true)
  {
    assign(size(),val);
  }

  // Return the values of this vector as if it encodes a number in bits..
  size_t get_encoded_number()   
  {
    assert(size() <= NF_MAX_ARITY);
    size_t result=0;
    for(size_t i=0; i<size(); ++i)
    {
      if ((*this)[i])
      { 
        result=result+(1<<i);
      }
    }
    return result;
  }

  // Set the values of this vector by viewing val as a binary number.
  // Position 0 contains the least significant bit.
  void set_encoded_number(size_t val)
  {
    for(size_t i=0; i<size(); ++i)
    {
      (*this)[i]=((val & 1)==1);
      val=val>>1;
    }
  } 

  bool get(size_t i)
  {
    return (*this)[i];
  }

  void set(size_t i, bool val = true)
  {
    assert(i<size());
    (*this)[i]=val;
  }

  bool is_clear()
  {
    for(auto i=begin(); i!=end(); ++i)
    {
      if (*i)
      {
        return false;
      }
    }
    return true;
  }

  bool is_filled()
  {
    for(auto i=begin(); i!=end(); ++i)
    {
      if (!*i)
      {
        return false;
      }
    }
    return true;
  }

};

}
}
}

#endif // MCRL2_DATA_NFS_ARRAY_H
