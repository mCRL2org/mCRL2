// Author(s): Jeroen Keiren, Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/utilities/bitstream.h"

#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/power_of_two.h"
#include "mcrl2/utilities/platform.h"

#include <algorithm>
#include <istream>
#include <ostream>

#ifdef MCRL2_PLATFORM_WINDOWS
#include <io.h>
#include <fcntl.h>
#endif

using namespace mcrl2::utilities;

/// \brief Encodes an unsigned variable-length integer using the most significant bit (MSB) algorithm.
///        This function assumes that the value is stored as little endian.
/// \param value The input value. Any standard integer type is allowed.
/// \param output A pointer to a piece of reserved memory. Must have a minimum size dependent on the input size (32 bit = 5 bytes, 64 bit = 10 bytes).
/// \returns The number of bytes used in the output.
/// \details Implementation taken from https://techoverflow.net/2013/01/25/efficiently-encoding-variable-length-integers-in-cc/
template<typename int_t = std::size_t>
static size_t encode_variablesize_int(int_t value, uint8_t* output)
{
  size_t outputSize = 0;

  // While more than 7 bits of data are left, occupy the last output byte
  // and set the next byte flag.
  while (value > 127)
  {
    // | 128: Sets the next byte flag.
    output[outputSize] = (static_cast<uint8_t>(value & 127)) | 128;

    // Remove the seven bits we just wrote from value.
    value >>= 7;
    outputSize++;
  }

  output[outputSize] = (static_cast<uint8_t>(value));
  return outputSize + 1;
}

/// \brief Decodes an unsigned variable-length integer using the MSB algorithm.
/// \param stream The stream from which the bytes for this value are read.
/// \details Implementation taken from https://techoverflow.net/2013/01/25/efficiently-encoding-variable-length-integers-in-cc/, 
///          but converted to a streaming method.
template<typename int_t = std::size_t>
int_t decode_variablesize_int(ibitstream& stream)
{
  int_t value = 0;
  for (size_t i = 0; i < integer_encoding_size<int_t>(); i++)
  {
    // Read the next byte from the stream.
    std::size_t byte = stream.read_bits(8);

    // Take 7 bits (mask 0x01111111) from byte and shift it before the bits already written to value.
    value |= (static_cast<int_t>(byte) & 127) << (7 * i);

    if (!(byte & 128))
    {
      // If the next-byte flag is not set then we are finished.
      break;
    }
    else if (i >= integer_encoding_size<int_t>() - 1)
    {
      // The next-byte flag was set, but we cannot represent it using int_t.
      throw std::runtime_error("Fail to read an int from the input");
    }
  }

  return value;
}

/// \brief Change the current stream to binary mode (no handle of newline characters),
static void set_stream_binary([[maybe_unused]] const std::string& name, [[maybe_unused]] FILE* handle)
{
#ifdef MCRL2_PLATFORM_WINDOWS
  if (_setmode(_fileno(handle), _O_BINARY) == -1)
  {
    mCRL2log(mcrl2::log::log_level_t::warning) << "Cannot set " << name << " to binary mode.\n";
  }
  else
  {
    mCRL2log(mcrl2::log::log_level_t::debug) << "Converted " << name << " to binary mode.\n";
  }

  fflush(stderr);
#endif // MCRL2_PLATFORM_WINDOWS
}

obitstream::obitstream(std::ostream& stream)
  : stream(stream),
    m_output_buffer(output_buffer_size)
{
  // Ensures that the given stream is changed to binary mode.
  if (stream.rdbuf() == std::cout.rdbuf())
  {
    set_stream_binary("cout", stdout);
  }
  else if (stream.rdbuf() == std::cerr.rdbuf())
  {
    set_stream_binary("cerr", stderr);
  }
}

void obitstream::write_bits_slow(std::size_t value, unsigned int number_of_bits)
{
  // Write at most the number of bits of a std::size_t.
  static_assert(std::numeric_limits<std::size_t>::digits <= bits_per_word);
  assert(number_of_bits <= std::numeric_limits<std::size_t>::digits);
  assert(m_bits_in_buffer < bits_per_word);

  while (number_of_bits > 0)
  {
    // Move the first count of the remaining bits of value into the bit buffer. Shifting a word by bits_per_word is
    // undefined, so the case that the bit buffer is empty and all bits_per_word bits fit is handled separately.
    const unsigned int count = std::min(number_of_bits, bits_per_word - m_bits_in_buffer);
    const word_t bits = static_cast<word_t>(value) >> (number_of_bits - count);
    if (count == bits_per_word)
    {
      m_bit_buffer = bits;
    }
    else
    {
      const word_t mask = (static_cast<word_t>(1) << count) - 1;
      m_bit_buffer |= (bits & mask) << (bits_per_word - m_bits_in_buffer - count);
    }
    m_bits_in_buffer += count;
    number_of_bits -= count;

    if (m_bits_in_buffer == bits_per_word)
    {
      empty_bit_buffer();
    }
  }
}

void obitstream::empty_bit_buffer()
{
  assert(m_bits_in_buffer == bits_per_word);
  static_assert(output_buffer_size % bytes_per_word == 0, "A full output buffer must have room for a whole word.");
  assert(m_output_buffer.size() - m_output_size >= bytes_per_word);

  // The most significant byte is written first.
  for (std::size_t i = 0; i < bytes_per_word; ++i)
  {
    m_output_buffer[m_output_size + i]
      = static_cast<std::uint8_t>(m_bit_buffer >> (bits_per_word - bits_per_byte * (i + 1)));
  }
  m_output_size += bytes_per_word;
  m_bit_buffer = 0;
  m_bits_in_buffer = 0;

  // Empty the output buffer only after the bit buffer has been emptied, so that this bitstream remains consistent
  // when writing to the stream fails.
  if (m_output_size == m_output_buffer.size())
  {
    empty_output_buffer();
  }
}

void obitstream::empty_output_buffer()
{
  std::streambuf* buffer = stream.rdbuf();
  const std::streamsize size = static_cast<std::streamsize>(m_output_size);
  m_output_size = 0;
  if (buffer == nullptr || buffer->sputn(reinterpret_cast<const char*>(m_output_buffer.data()), size) != size)
  {
    stream.setstate(std::ios_base::badbit);
    throw mcrl2::runtime_error("Failed to write bytes to the output file/stream.");
  }
}

void obitstream::write_string(const std::string& string)
{
  // Write length.
  write_integer(string.size());

  // Write actual string.
  write(reinterpret_cast<const std::uint8_t*>(string.c_str()), string.size());
}

void obitstream::write_integer(std::size_t val)
{
  const std::size_t nr_bytes = encode_variablesize_int(val, static_cast<std::uint8_t*>(integer_buffer));

  write(static_cast<const std::uint8_t*>(integer_buffer), nr_bytes);
}

ibitstream::ibitstream(std::istream& stream)
  : stream(stream),
    m_input_buffer(input_buffer_size)
{
  // Ensures that the given stream is changed to binary mode.
  if (stream.rdbuf() == std::cin.rdbuf())
  {
    set_stream_binary("cin", stdin);
  }
}

const char* ibitstream::read_string()
{
  std::size_t length;

  // Get length of string.
  length = read_integer();

  // Assure buffer can hold the string.
  if (m_text_buffer.size() < (length + 1))
  {
    m_text_buffer.resize(round_up_to_power_of_two(length + 1));
  }

  // Read the actual string.
  read(length, reinterpret_cast<std::uint8_t*>(m_text_buffer.data()));
  m_text_buffer[length] = '\0';

  return m_text_buffer.data();
}

std::size_t ibitstream::read_bits_slow(unsigned int number_of_bits)
{
  // Read at most the number of bits of a std::size_t.
  static_assert(std::numeric_limits<std::size_t>::digits <= bits_per_word);
  assert(number_of_bits <= std::numeric_limits<std::size_t>::digits);

  word_t value = 0;
  while (number_of_bits > 0)
  {
    if (m_bits_in_buffer == 0 && !refill_bit_buffer())
    {
      throw mcrl2::runtime_error("Unexpected end-of-file reached in the input file/stream.");
    }

    // Append the first bits of the bit buffer to value. Shifting a word by bits_per_word is undefined, so that case is
    // handled separately; then value is still empty, and the bit buffer becomes empty.
    const unsigned int count = std::min(number_of_bits, m_bits_in_buffer);
    value = (count == bits_per_word ? 0 : value << count) | (m_bit_buffer >> (bits_per_word - count));
    m_bit_buffer = (count == bits_per_word ? 0 : m_bit_buffer << count);
    m_bits_in_buffer -= count;
    number_of_bits -= count;
  }

  return value;
}

bool ibitstream::refill_bit_buffer()
{
  assert(m_bits_in_buffer == 0);

  if (m_input_size - m_input_position >= bytes_per_word)
  {
    // The common case: take a whole word at once, the first byte becoming the most significant one.
    word_t word = 0;
    for (std::size_t i = 0; i < bytes_per_word; ++i)
    {
      word = (word << bits_per_byte) | m_input_buffer[m_input_position + i];
    }
    m_input_position += bytes_per_word;
    m_bit_buffer = word;
    m_bits_in_buffer = bits_per_word;
    return true;
  }

  // Take the bytes one by one, as the input buffer must be refilled, or fewer than bytes_per_word bytes are left.
  word_t word = 0;
  unsigned int count = 0;
  while (count < bytes_per_word && (m_input_position < m_input_size || refill_input_buffer()))
  {
    word = (word << bits_per_byte) | m_input_buffer[m_input_position];
    ++m_input_position;
    ++count;
  }

  if (count == 0)
  {
    return false;
  }

  m_bit_buffer = word << (bits_per_word - bits_per_byte * count);
  m_bits_in_buffer = bits_per_byte * count;
  return true;
}

bool ibitstream::refill_input_buffer()
{
  std::streambuf* buffer = stream.rdbuf();
  if (buffer == nullptr)
  {
    throw mcrl2::runtime_error("Failed to read bytes from the input file/stream.");
  }

  const std::streamsize count = buffer->sgetn(reinterpret_cast<char*>(m_input_buffer.data()),
    static_cast<std::streamsize>(m_input_buffer.size()));
  m_input_position = 0;
  m_input_size = count > 0 ? static_cast<std::size_t>(count) : 0;
  return m_input_size > 0;
}

std::size_t ibitstream::read_integer()
{
  return decode_variablesize_int(*this);
}

// Private functions

void obitstream::flush()
{
  // Writing the buffer full to 64 bits should flush it internally, this also guarantees that the unnecessary bits are zeroed out.
  write_bits(0, bits_per_word - m_bits_in_buffer);
  assert(m_bits_in_buffer == 0);

  empty_output_buffer();
  stream.flush();
  if (stream.fail())
  {
    throw mcrl2::runtime_error("Failed to write the last byte to the output file/stream.");
  }
}

void obitstream::write(const uint8_t* buffer, std::size_t size)
{
  for (std::size_t index = 0; index < size; ++index)
  {
    // Write a single byte for every entry in the buffer that was filled (size).
    write_bits(buffer[index], 8);
  }
}

void ibitstream::read(std::size_t size, std::uint8_t* buffer)
{
  for (std::size_t index = 0; index < size; ++index)
  {
    // Read a single byte for every entry into the buffer that was filled (size).
    std::size_t value = read_bits(8);
    buffer[index] = static_cast<std::uint8_t>(value);
  }
}
