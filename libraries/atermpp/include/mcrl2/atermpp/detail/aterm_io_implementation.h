// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/detail/aterm_io_implementation.h
/// \brief add your file description here.

#ifndef MCRL2_ATERMPP_DETAIL_ATERM_IO_IMPLEMENTATION_H
#define MCRL2_ATERMPP_DETAIL_ATERM_IO_IMPLEMENTATION_H

#include <iostream>
#include <stdexcept>
#include "mcrl2/utilities/logger.h"

namespace atermpp {

namespace detail {

inline
std::size_t writeIntToBuf(const std::size_t val, unsigned char* buf)
{
  if (val < (1 << 7))
  {
    buf[0] = (unsigned char) val;
    return 1;
  }

  if (val < (1 << 14))
  {
    buf[0] = (unsigned char)((val >>  8) | 0x80);
    buf[1] = (unsigned char)((val >>  0) & 0xff);
    return 2;
  }

  if (val < (1 << 21))
  {
    buf[0] = (unsigned char)((val >> 16) | 0xc0);
    buf[1] = (unsigned char)((val >>  8) & 0xff);
    buf[2] = (unsigned char)((val >>  0) & 0xff);
    return 3;
  }

  if (val < (1 << 28))
  {
    buf[0] = (unsigned char)((val >> 24) | 0xe0);
    buf[1] = (unsigned char)((val >> 16) & 0xff);
    buf[2] = (unsigned char)((val >>  8) & 0xff);
    buf[3] = (unsigned char)((val >>  0) & 0xff);
    return 4;
  }

  if (sizeof(std::size_t)>4 && val>((std::size_t)1<<4*sizeof(std::size_t)))
  {
    mCRL2log(mcrl2::log::warning) << "losing precision of integers when writing to .baf file" << std::endl;
  }

  buf[0] = 0xf0;
  buf[1] = (unsigned char)((val >> 24) & 0xff);
  buf[2] = (unsigned char)((val >> 16) & 0xff);
  buf[3] = (unsigned char)((val >>  8) & 0xff);
  buf[4] = (unsigned char)((val >>  0) & 0xff);
  return 5;
}

inline
void writeInt(const std::size_t val, std::ostream &os)
{
  unsigned char buf[8];
  std::size_t nr_items = writeIntToBuf(val, buf);
  os.write((char*)buf, nr_items);
}

// read a positive integer from a stream
// returns the number of bytes read
inline
std::size_t readInt(std::istream &is)
{
  int buf[8];

  // Try to read 1st character
  if ((buf[0] = is.get()) == EOF)
  {
    throw std::runtime_error("Fail to read an int from the input");
  }

  // Check if 1st character is enough
  if ((buf[0] & 0x80) == 0)
  {
    return buf[0];
  }

  // Try to read 2nd character
  if ((buf[1] = is.get()) == EOF)
  {
    throw std::runtime_error("Fail to read an int from the input");
  }

  // Check if 2nd character is enough
  if ((buf[0] & 0x40) == 0)
  {
    return buf[1] + ((buf[0] & ~0xc0) << 8);
  }

  // Try to read 3rd character
  if ((buf[2] = is.get()) == EOF)
  {
    throw std::runtime_error("Fail to read an int from the input");
  }

  // Check if 3rd character is enough
  if ((buf[0] & 0x20) == 0)
  {
    return buf[2] + (buf[1] << 8) + ((buf[0] & ~0xe0) << 16);
  }

  // Try to read 4th character
  if ((buf[3] = is.get()) == EOF)
  {
    throw std::runtime_error("Fail to read an int from the input");
  }

  // Check if 4th character is enough
  if ((buf[0] & 0x10) == 0)
  {
    return buf[3] + (buf[2] << 8) + (buf[1] << 16) + ((buf[0] & ~0xf0) << 24);
  }

  // Try to read 5th character
  if ((buf[4] = is.get()) == EOF)
  {
    throw std::runtime_error("Fail to read an int from the input");
  }

  // Now 5th character should be enough
  return buf[4] + (buf[3] << 8) + (buf[2] << 16) + (buf[1] << 24);
}

} // namespace detail

} // namespace atermpp

#endif // MCRL2_ATERMPP_DETAIL_ATERM_IO_IMPLEMENTATION_H
