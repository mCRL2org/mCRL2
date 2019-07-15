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

#include <vector>
#include <bitset>

namespace mcrl2
{
namespace utilities
{

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

/// \returns The number of bits needed to represent a value of type T in most significant bit encoding.
template<typename T>
constexpr std::size_t bits_needed()
{
  return ((sizeof(T) + 1) * 8) / 7;
}

/// \brief A bitstream provided per bit writing of data to any stream (including stdout). Internally uses
///        bitpacking and buffering for compact and efficient IO.
class obitstream
{
public:
  obitstream(std::ostream& stream);
  ~obitstream() { flush(); }

  /// \brief Write the nr_bits least significant bits from val to this stream.
  /// @param val      Variable that contains the bits.
  /// @param nr_bits  Number of bits to write to the output stream.
  void write_bits(std::size_t val, const std::size_t nr_bits);

  /// \brief Write the given string to the output stream.
  /// \details Encoded in bits using <length, string>
  void write_string(const std::string& string);

  /// \brief Write the given value to the output stream using most significant bit encoding.
  void write_integer(std::size_t val);

private:
  /// \brief Flush the remaining bits in the buffer to the output stream.
  void flush();

  std::ostream& stream;

  /// \brief Buffer that is filled starting from bit 127 when reading or writing
  std::bitset<128> read_write_buffer = 0;

  std::size_t  bits_in_buffer = 0; ///< how many bits in bit_buffer are used.

  std::uint8_t integer_buffer[bits_needed<std::size_t>()]; ///< Reserved space to space an n byte integer.
};

class ibitstream
{
public:
  ibitstream(std::istream& stream);

  /// @brief Reads an n-bit integer from the input stream.
  /// @param val      Variable to store integer in.
  /// @param nr_bits  Number of bits to read from the input stream.
  /// @return true on success, false on failure (EOF).
  bool read_bits(std::size_t& val, const unsigned int nr_bits);

  /// \returns A pointer to a character string, remains valid until the next readString call.
  const char* read_string();

  /// \returns A natural number that was read from the binary stream encoded using most significant bit encoding.
  std::size_t read_integer();

private:
  std::istream& stream;

  /// \brief Buffer that is filled starting from bit 127 when reading or writing
  std::bitset<128> read_write_buffer = 0;

  std::size_t bits_in_buffer = 0; ///< how many bits in bit_buffer are used.

  std::vector<char> m_text_buffer;

};

} // namespace utilities
} // namespace mcrl2

#endif // MCRL2_UTILITIES_BITSTREAM_H
