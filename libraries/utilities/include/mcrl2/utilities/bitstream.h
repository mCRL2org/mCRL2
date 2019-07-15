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
    if (stream.rdbuf() == std::cout.rdbuf())
    {
      set_stream_binary("cout", stdout);
    }
    else if (stream.rdbuf() == std::cerr.rdbuf())
    {
      set_stream_binary("cerr", stderr);
    }
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

  void writeInt(const std::size_t val)
  {
    std::size_t nr_items = encode_variablesize_int(val, integer_buffer);
    stream.write(reinterpret_cast<char*>(integer_buffer), nr_items);
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

  /// \brief Encodes an unsigned variable-length integer using the most significant bit (MSB) algorithm.
  ///        This function assumes that the value is stored as little endian.
  /// \param value The input value. Any standard integer type is allowed.
  /// \param output A pointer to a piece of reserved memory. Must have a minimum size dependent on the input size (32 bit = 5 bytes, 64 bit = 10 bytes).
  /// \return The number of bytes used in the output memory.
  /// \details Implementation taken from https://techoverflow.net/2013/01/25/efficiently-encoding-variable-length-integers-in-cc/
  template<typename int_t = std::size_t>
  size_t encode_variablesize_int(int_t value, uint8_t* output)
  {
    size_t outputSize = 0;

    // While more than 7 bits of data are left, occupy the last output byte
    // and set the next byte flag
    while (value > 127)
    {
      //|128: Set the next byte flag
      output[outputSize] = (static_cast<uint8_t>(value & 127)) | 128;
      //Remove the seven bits we just wrote
      value >>= 7;
      outputSize++;
    }

    output[outputSize++] = (static_cast<uint8_t>(value)) & 127;

    if (sizeof(std::size_t) > 4 && value > ((std::size_t)1 << 4 * sizeof(std::size_t)))
    {
      mCRL2log(mcrl2::log::warning) << "losing precision of integers when writing to .baf file" << std::endl;
    }

    return outputSize;
  }

  void set_stream_binary(const std::string& name, FILE* handle)
  {
    mcrl2::utilities::mcrl2_unused(name, handle);
  #ifdef MCRL2_PLATFORM_WINDOWS
    if (_setmode(_fileno(handle), _O_BINARY) == -1)
    {
      mCRL2log(mcrl2::log::warning) << "Cannot set " << name << " to binary mode.\n";
    }
    else
    {
      mCRL2log(mcrl2::log::debug) << "Converted " << name << " to binary mode.\n";
    }

    fflush(stderr);
  #endif // MCRL2_PLATFORM_WINDOWS
  }

  std::ostream& stream;

  /// \brief Buffer that is filled starting from bit 127 when reading or writing
  std::bitset<128> read_write_buffer = 0;

  std::size_t  bits_in_buffer = 0; ///< how many bits in bit_buffer are used.

  std::uint8_t integer_buffer[10]; ///< Reserved space to space an 8 byte integer (std::size_t).
};

class ibitstream
{
public:
  ibitstream(std::istream& stream)
    : stream(stream)
  {
    if (stream.rdbuf() == std::cin.rdbuf())
    {
      set_stream_binary("cin", stdin);
    }
  }

  /// \returns A pointer to a character string, remains valid until the next readString call.
  const char* readString()
  {
    std::size_t length;

    // Get length of string.
    length = readInt();

    // Assure buffer can hold the string.
    if (m_text_buffer.size() <  (length + 1))
    {
      m_text_buffer.resize(2 * m_text_buffer.size());
    }

    // Read the actual string.
    stream.read(m_text_buffer.data(), length);
    m_text_buffer[length] = '\0';

    return m_text_buffer.data();
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
    return decode_variablesize_int();
  }

private:
  void set_stream_binary(const std::string& name, FILE* handle)
  {
    mcrl2::utilities::mcrl2_unused(name, handle);
  #ifdef MCRL2_PLATFORM_WINDOWS
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

  /// \brief Decodes an unsigned variable-length integer using the MSB algorithm.
  /// \param value A variable-length encoded integer of arbitrary size.
  /// \param inputSize How many bytes it should read at most
  template<typename int_t = std::size_t>
  int_t decode_variablesize_int()
  {
    int_t value = 0;
    for (size_t i = 0; i < sizeof(int_t); i++)
    {
      // Read the next byte from the stream.
      int byte = stream.get();

      value |= (byte & 127) << (7 * i);

      // If the next-byte flag is not set then we are finished.
      if(!(byte & 128))
      {
        break;
      }
    }

    return value;
  }


  std::istream& stream;


  /// \brief Buffer that is filled starting from bit 127 when reading or writing
  std::bitset<128> read_write_buffer = 0;

  std::size_t bits_in_buffer = 0; ///< how many bits in bit_buffer are used.

  std::vector<char> m_text_buffer;

};

} // namespace utilities
} // namespace mcrl2

#endif // MCRL2_UTILITIES_BITSTREAM_H
