// Author(s): Sjoerd Cranen, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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

namespace mcrl2::data::detail
{

class nfs_array : public std::vector<bool>
{
public:
  nfs_array(std::size_t size)
    : std::vector<bool>(size, false)
  { }

  void fill(bool val)
  {
    assign(size(), val);
  }

  operator bool() const
  {
    for (const bool b: *this)
    {
      if (b)
      {
        return true;
      }
    }
    return false;
  } 
};

}

#endif // MCRL2_DATA_NFS_ARRAY_H
