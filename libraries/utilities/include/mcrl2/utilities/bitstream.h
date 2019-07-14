// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_UTILITIES_BITSTREAM_H
#define MCRL2_UTILITIES_BITSTREAM_H

#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/unused.h"
#include "mcrl2/utilities/logger.h"

#include <bitset>
#include <limits>

#ifdef MCRL2_PLATFORM_WINDOWS
#include <io.h>
#include <fcntl.h>
#endif

namespace mcrl2
{
namespace utilities
{

/**
 * \brief Reverse the order of bits in val.
 * \details In BAF version 0x0304 the bits are written in reverse order. When bumping
 * the version number, one should also consider to remove this reversal.
 */
static void reverse_bit_order(std::size_t& val)
{
  if(std::numeric_limits<std::size_t>::digits == 64)
  {
    val = ((val << 32) & 0xFFFFFFFF00000000) | ((val >> 32) & 0x00000000FFFFFFFF);
  }
  val = ((val << 16) & 0xFFFF0000FFFF0000) | ((val >> 16) & 0x0000FFFF0000FFFF);
  val = ((val << 8)  & 0xFF00FF00FF00FF00) | ((val >> 8)  & 0x00FF00FF00FF00FF);
  val = ((val << 4)  & 0xF0F0F0F0F0F0F0F0) | ((val >> 4)  & 0x0F0F0F0F0F0F0F0F);
  val = ((val << 2)  & 0xCCCCCCCCCCCCCCCC) | ((val >> 2)  & 0x3333333333333333);
  val = ((val << 1)  & 0xAAAAAAAAAAAAAAAA) | ((val >> 1)  & 0x5555555555555555);
}

/// \returns The number of bits needed to represent val.
/// \details This function is equal to log2(val), except that it maps 0 to 0
static std::size_t bit_width(std::size_t val)
{
  std::size_t nr_bits = 0;

  if (val <= 1)
  {
    return 0;
  }

  while (val)
  {
    val>>=1;
    nr_bits++;
  }

  return nr_bits;
}

/// \brief A bitstream provided per bit writing of data to any stream (including stdout). Internally uses
///        bitpacking and buffering for compact and efficient IO.
class obitstream
{
public:
  obitstream(std::ostream& stream)
    : stream(stream)
  {
    mcrl2::utilities::mcrl2_unused(stream);

  #ifdef MCRL2_PLATFORM_WINDOWS
    std::string name;
    FILE* handle;
    if (stream.rdbuf() == std::cin.rdbuf())
    {
      name = "cin";
      handle = stdin;
    }
    else
    if (stream.rdbuf() == std::cout.rdbuf())
    {
      name = "cout";
      handle = stdout;
      fflush(stdout);
    }
    else
    if (stream.rdbuf() == std::cerr.rdbuf())
    {
      name = "cerr";
      handle = stderr;
      fflush(stderr);
    }
    if (!name.empty())
    {
      if (_setmode(_fileno(handle), _O_BINARY) == -1)
      {
        mCRL2log(mcrl2::log::warning) << "Cannot set " << name << " to binary mode.\n";
      }
      else
      {
        mCRL2log(mcrl2::log::debug) << "Converted " << name << " to binary mode.\n";
      }
    }
  #endif // MCRL2_PLATFORM_WINDOWS
  }

  ~obitstream()
  {
    flush();
  }

  /// \brief Write the nr_bits least significant bits from val to os
  void writeBits(std::size_t val, const std::size_t nr_bits)
  {
    if(nr_bits == 0)
    {
      return;
    }
    reverse_bit_order(val);
    // Add val to the buffer
    read_write_buffer |= std::bitset<128>(val) << (128 - std::numeric_limits<std::size_t>::digits - bits_in_buffer);
    bits_in_buffer += nr_bits;
    // Write 8 bytes if available
    if(bits_in_buffer >= 64)
    {
      unsigned long long write_value = (read_write_buffer >> 64).to_ullong();
      read_write_buffer <<= 64;
      bits_in_buffer -= 64;
      for(uint32_t i = 8; i > 0; --i)
      {
        stream.put((write_value >> (8*(i-1))) & 0xFF);
      }
    }
  }

  void writeString(const std::string& string)
  {
    // Write length.
    writeInt(string.size());

    // Write actual string.
    stream.write(string.c_str(), string.size());
  }

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

  void writeInt(const std::size_t val)
  {
    unsigned char buf[8];
    std::size_t nr_items = writeIntToBuf(val, buf);
    stream.write((char*)buf, nr_items);
  }

private:

  /// \brief Flush the remaining bits in the buffer to the output stream.
  void flush()
  {
    if (bits_in_buffer > 0)
    {
      unsigned long long write_value = (read_write_buffer >> 64).to_ullong();
      for(uint32_t i = 8; i > 7 - bits_in_buffer / 8; --i)
      {
        stream.put((write_value >> (8*(i-1))) & 0xFF);
      }
      if (stream.fail())
      {
        throw mcrl2::runtime_error("Failed to write the last byte to the output file/stream.");
      }

      read_write_buffer = std::bitset<128>(0);
      bits_in_buffer = 0;
    }
  }

  std::ostream& stream;

  /// \brief Buffer that is filled starting from bit 127 when reading or writing
  std::bitset<128> read_write_buffer = 0;

  std::size_t  bits_in_buffer = 0; ///< how many bits in bit_buffer are used.
};

class ibitstream
{
public:
  ibitstream(std::istream& stream)
    : stream(stream)
  {}

  /// \returns A pointer to a character string, remains valid until the next readString call.
  const char* readString()
  {
    std::size_t len;

    // Get length of string.
    len = readInt();

    // Assure buffer can hold the string.
    if (text_buffer_size < (len+1))
    {
      text_buffer_size = 2*len;
      text_buffer = (char*)realloc(text_buffer, text_buffer_size);
      if (!text_buffer)
      {
        throw mcrl2::runtime_error("Out of memory while reading the input file. Fail to claim a block of memory of size "+ std::to_string(text_buffer_size) + ".");
      }
    }

    // Read the actual string.
    stream.read(text_buffer, len);
    text_buffer[len] = '\0';

    return text_buffer;
  }

  /// @brief readBits Reads an n-bit integer from the input stream.
  /// @param val      Variable to store integer in.
  /// @param nr_bits  Number of bits to read from the input stream.
  /// @param is       The input stream.
  /// @return true on success, false on failure (EOF).
  bool readBits(std::size_t& val, const unsigned int nr_bits)
  {
    val = 0;
    if(nr_bits == 0)
    {
      return true;
    }
    while(bits_in_buffer < nr_bits)
    {
      // Read bytes until the buffer is sufficiently full
      int byte = stream.get();
      if(stream.fail())
      {
        return false;
      }
      read_write_buffer |= std::bitset<128>(byte) << (56 + 64 - bits_in_buffer);
      bits_in_buffer += 8;
    }
    val = (read_write_buffer >> (128 - std::numeric_limits<std::size_t>::digits)).to_ullong() &
        (std::numeric_limits<std::size_t>::max() <<
           (std::numeric_limits<std::size_t>::digits - std::min(nr_bits, static_cast<unsigned int>(std::numeric_limits<std::size_t>::digits))));
    bits_in_buffer -= nr_bits;
    read_write_buffer <<= nr_bits;
    reverse_bit_order(val);
    return true;
  }

  // read a positive integer from a stream
  // returns the number of bytes read
  std::size_t readInt()
  {
    int buf[8];

    // Try to read 1st character
    if ((buf[0] = stream.get()) == EOF)
    {
      throw std::runtime_error("Fail to read an int from the input");
    }

    // Check if 1st character stream enough
    if ((buf[0] & 0x80) == 0)
    {
      return buf[0];
    }

    // Try to read 2nd character
    if ((buf[1] = stream.get()) == EOF)
    {
      throw std::runtime_error("Fail to read an int from the input");
    }

    // Check if 2nd character is enough
    if ((buf[0] & 0x40) == 0)
    {
      return buf[1] + ((buf[0] & ~0xc0) << 8);
    }

    // Try to read 3rd character
    if ((buf[2] = stream.get()) == EOF)
    {
      throw std::runtime_error("Fail to read an int from the input");
    }

    // Check if 3rd character is enough
    if ((buf[0] & 0x20) == 0)
    {
      return buf[2] + (buf[1] << 8) + ((buf[0] & ~0xe0) << 16);
    }

    // Try to read 4th character
    if ((buf[3] = stream.get()) == EOF)
    {
      throw std::runtime_error("Fail to read an int from the input");
    }

    // Check if 4th character is enough
    if ((buf[0] & 0x10) == 0)
    {
      return buf[3] + (buf[2] << 8) + (buf[1] << 16) + ((buf[0] & ~0xf0) << 24);
    }

    // Try to read 5th character
    if ((buf[4] = stream.get()) == EOF)
    {
      throw std::runtime_error("Fail to read an int from the input");
    }

    // Now 5th character should be enough
    return buf[4] + (buf[3] << 8) + (buf[2] << 16) + (buf[1] << 24);
  }


private:
  std::istream& stream;

  /// \brief Buffer that is filled starting from bit 127 when reading or writing
  std::bitset<128> read_write_buffer = 0;

  std::size_t  bits_in_buffer = 0; ///< how many bits in bit_buffer are used.

  char* text_buffer = nullptr;
  std::size_t text_buffer_size = 0;

};

} // namespace utilities
} // namespace mcrl2

#endif // MCRL2_UTILITIES_BITSTREAM_H
