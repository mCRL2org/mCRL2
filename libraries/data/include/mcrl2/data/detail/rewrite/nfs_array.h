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
#include <vector>


namespace mcrl2
{
namespace data
{
namespace detail
{

class nfs_array : public std::vector<bool>
{
public:
  nfs_array(size_t size)
    : std::vector<bool>(size, false)
  { }

  void fill(bool val = true)
  {
    assign(size(), val);
  }

  ///
  /// \brief next iterates to the next combination of booleans (in terms of bit vectors:
  ///        if the nfs_array represents integer i, it computes i + 1).
  /// \return true if the operation was successful, false if an overflow occurred (i.e.,
  ///        if the bit vector was is_filled(), in which case after calling next(), it is
  ///        is_clear().
  ///
  bool next()
  {
    size_t index = 0;
    while (index < size())
    {
      if (at(index))
      {
        at(index) = false;
        ++index;
      }
      else
      {
        at(index) = true;
        return true;
      }
    }
    return false;
  }

  operator size_t() const
  {
    assert(8 * sizeof(size_t) > size());
    size_t result = 0;
    size_t mask = 1;
    for (size_t i = 0; i < size(); ++i, mask <<= 1)
    {
      if (at(i))
      {
        result |= mask;
      }
    }
    return result;
  }

  bool is_clear() const
  {
    for(std::vector<bool>::const_iterator i=begin(); i!=end(); ++i)
    {
      if (*i)
      {
        return false;
      }
    }
    return true;
  }

  bool is_filled() const
  {
    for(std::vector<bool>::const_iterator i=begin(); i!=end(); ++i)
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
