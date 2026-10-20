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

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include <cstdint>
#include <cstring>
#include <iomanip>
#include <limits>
#include <random>
#include <sstream>
#include <streambuf>
#include <string>
#include <variant>
#include <vector>

using namespace mcrl2::utilities;

namespace
{

/// \brief A stream buffer that serves the bytes of a string in small chunks, cycling through the chunk sizes
///        1, 2, ..., max_chunk. This mimics reading from a pipe, where every underlying read can return fewer
///        bytes than requested.
class chunked_streambuf : public std::streambuf
{
public:
  chunked_streambuf(std::string data, std::size_t max_chunk)
      : m_data(std::move(data)),
        m_max_chunk(max_chunk)
  {
    setg(m_data.data(), m_data.data(), m_data.data());
  }

protected:
  int_type underflow() override
  {
    if (gptr() < egptr())
    {
      return traits_type::to_int_type(*gptr());
    }

    char* end = m_data.data() + m_data.size();
    if (egptr() == end)
    {
      return traits_type::eof();
    }

    m_chunk = m_chunk % m_max_chunk + 1;
    char* next = egptr();
    char* last = next + std::min<std::size_t>(m_chunk, static_cast<std::size_t>(end - next));
    setg(m_data.data(), next, last);
    return traits_type::to_int_type(*gptr());
  }

private:
  std::string m_data;
  std::size_t m_max_chunk;
  std::size_t m_chunk = 0;
};

/// \brief One write operation on an obitstream: either bits with a width, an integer, or a string.
struct bits_operation
{
  std::size_t value;
  unsigned int width;
};

using operation = std::variant<bits_operation, std::size_t, std::string>;

void write_operations(obitstream& stream, const std::vector<operation>& operations)
{
  for (const operation& op : operations)
  {
    if (const bits_operation* bits = std::get_if<bits_operation>(&op))
    {
      stream.write_bits(bits->value, bits->width);
    }
    else if (const std::size_t* integer = std::get_if<std::size_t>(&op))
    {
      stream.write_integer(*integer);
    }
    else
    {
      stream.write_string(std::get<std::string>(op));
    }
  }
}

/// \brief Reads back the given operations and checks that the read values match; returns the number of mismatches.
std::size_t check_operations(ibitstream& stream, const std::vector<operation>& operations)
{
  std::size_t mismatches = 0;
  for (std::size_t i = 0; i < operations.size(); ++i)
  {
    const operation& op = operations[i];
    if (const bits_operation* bits = std::get_if<bits_operation>(&op))
    {
      const std::size_t mask = bits->width == 64 ? std::numeric_limits<std::size_t>::max()
                                                 : (std::size_t(1) << bits->width) - 1;
      const std::size_t read = stream.read_bits(bits->width);
      if (read != (bits->value & mask))
      {
        BOOST_ERROR("operation " << i << ": read_bits(" << bits->width << ") returned " << read << " instead of "
                                 << (bits->value & mask));
        ++mismatches;
      }
    }
    else if (const std::size_t* integer = std::get_if<std::size_t>(&op))
    {
      const std::size_t read = stream.read_integer();
      if (read != *integer)
      {
        BOOST_ERROR("operation " << i << ": read_integer() returned " << read << " instead of " << *integer);
        ++mismatches;
      }
    }
    else
    {
      const std::string read(stream.read_string());
      if (read != std::get<std::string>(op))
      {
        BOOST_ERROR("operation " << i << ": read_string() returned a string of length " << read.size()
                                 << " instead of " << std::get<std::string>(op).size());
        ++mismatches;
      }
    }

    if (mismatches > 10)
    {
      break; // The remainder is most likely misaligned, so further errors are not informative.
    }
  }
  return mismatches;
}

std::string write_to_string(const std::vector<operation>& operations)
{
  std::stringstream stream;
  {
    obitstream output(stream);
    write_operations(output, operations);
    // The buffer is flushed here.
  }
  return stream.str();
}

/// \brief Integers that sit on the boundaries of the variable-width (7 bits per byte) integer encoding.
std::vector<std::size_t> boundary_integers()
{
  std::vector<std::size_t> result = {0, 1, 2, 126, 127, 128, 129, 255, 256};
  for (unsigned int bits = 7; bits < 64; bits += 7)
  {
    const std::size_t power = std::size_t(1) << bits;
    result.push_back(power - 1);
    result.push_back(power);
    result.push_back(power + 1);
  }
  result.push_back(std::size_t(1) << 63);
  result.push_back(std::numeric_limits<std::size_t>::max() - 1);
  result.push_back(std::numeric_limits<std::size_t>::max());
  return result;
}

/// \brief Generates a random but reproducible sequence of write operations.
std::vector<operation> random_operations(std::size_t count, std::uint64_t seed, std::size_t max_string_length)
{
  std::mt19937_64 generator(seed);
  std::uniform_int_distribution<int> kind(0, 9);
  std::uniform_int_distribution<unsigned int> width(0, 64);
  std::uniform_int_distribution<std::size_t> any_value;
  std::uniform_int_distribution<unsigned int> magnitude(0, 64);
  std::uniform_int_distribution<std::size_t> string_length(0, max_string_length);
  std::uniform_int_distribution<int> character(1, 255); // read_string returns a C string, so no embedded zeros.

  const std::vector<std::size_t> boundaries = boundary_integers();
  std::uniform_int_distribution<std::size_t> boundary(0, boundaries.size() - 1);

  std::vector<operation> result;
  result.reserve(count);
  for (std::size_t i = 0; i < count; ++i)
  {
    const int k = kind(generator);
    if (k < 5)
    {
      // Bits; the value may contain bits beyond the width, which must be masked out by write_bits.
      result.emplace_back(bits_operation{any_value(generator), width(generator)});
    }
    else if (k < 7)
    {
      // An integer of random magnitude, so that every encoded length occurs.
      const unsigned int m = magnitude(generator);
      const std::size_t value = m == 0 ? 0 : any_value(generator) >> (64 - m);
      result.emplace_back(value);
    }
    else if (k < 8)
    {
      result.emplace_back(boundaries[boundary(generator)]);
    }
    else
    {
      std::string s(string_length(generator), ' ');
      for (char& c : s)
      {
        c = static_cast<char>(character(generator));
      }
      result.emplace_back(std::move(s));
    }
  }
  return result;
}

std::string hex(const std::string& bytes)
{
  std::ostringstream out;
  for (std::size_t i = 0; i < bytes.size(); ++i)
  {
    out << (i == 0 ? "" : ", ") << "0x" << std::hex << std::setw(2) << std::setfill('0')
        << static_cast<unsigned int>(static_cast<unsigned char>(bytes[i]));
  }
  return out.str();
}

} // namespace

BOOST_AUTO_TEST_CASE(fixed_sequence_test)
{
  std::stringstream stream;

  {
    obitstream input(stream);
    input.write_integer(50);
    input.write_bits(7, 3);

    input.write_string("test");
    input.write_integer(1337);

    input.write_string("function_symbol");
    input.write_integer(5);
    
    input.write_integer(std::size_t(1) << 63);

    // The buffer is flushed here.
  }

  // Check the resulting stream.
  ibitstream output(stream);
  BOOST_CHECK_EQUAL(output.read_integer(), 50);
  BOOST_CHECK_EQUAL(output.read_bits(3), 7);

  BOOST_CHECK_EQUAL(strcmp(output.read_string(), "test"), 0);
  BOOST_CHECK_EQUAL(output.read_integer(), 1337);

  BOOST_CHECK_EQUAL(strcmp(output.read_string(), "function_symbol"), 0);
  BOOST_CHECK_EQUAL(output.read_integer(), 5);
  
  BOOST_CHECK_EQUAL(output.read_integer(), std::size_t(1) << 63);
}

// The exact bytes written for a fixed sequence of operations. This pins down the on-disk format: files written by
// earlier versions of the toolset must remain readable, so neither the writer nor the reader may change it.
BOOST_AUTO_TEST_CASE(exact_encoding_test)
{
  const std::vector<operation> operations = {
      std::size_t(50),
      bits_operation{7, 3},
      std::string("test"),
      std::size_t(1337),
      bits_operation{0, 0},
      bits_operation{1, 1},
      std::string(""),
      std::size_t(std::numeric_limits<std::size_t>::max()),
      bits_operation{0x0123456789abcdefULL, 64},
      bits_operation{0x1ff, 9},
      std::size_t(128),
  };

  const std::string actual = write_to_string(operations);
  const std::vector<unsigned char> expected_bytes = {
      0x32, 0xe0, 0x8e, 0x8c, 0xae, 0x6e, 0x97, 0x21, 0x50, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xf0, 0x10, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xff, 0xfc, 0x00, 0x08, 0x00,
  };
  const std::string expected(expected_bytes.begin(), expected_bytes.end());

  BOOST_CHECK_MESSAGE(actual == expected, "Encoding changed; the bytes written are now:\n" << hex(actual));

  // The reader must decode the exact bytes, independently of what the current writer produces.
  std::stringstream stream(expected);
  ibitstream input(stream);
  BOOST_CHECK_EQUAL(check_operations(input, operations), 0U);
}

// The final flush pads the output with 1 to 64 zero bits up to a whole number of 64-bit words. Note that this always
// adds at least one bit, so even an empty stream or a stream of exactly 64 bits gets a padding word.
BOOST_AUTO_TEST_CASE(padding_test)
{
  BOOST_CHECK_EQUAL(write_to_string({}).size(), 8U);
  BOOST_CHECK_EQUAL(write_to_string({bits_operation{1, 1}}).size(), 8U);
  BOOST_CHECK_EQUAL(write_to_string({bits_operation{0, 63}}).size(), 8U);
  BOOST_CHECK_EQUAL(write_to_string({bits_operation{0, 64}}).size(), 16U);
  BOOST_CHECK_EQUAL(write_to_string({bits_operation{0, 64}, bits_operation{0, 1}}).size(), 16U);
  BOOST_CHECK_EQUAL(write_to_string({std::string(100, 'x')}).size(), 104U); // 1 length byte + 100 bytes, padded.

  const std::string padding = write_to_string({bits_operation{1, 1}});
  BOOST_CHECK_EQUAL(hex(padding), "0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00");
}

// write_bits ignores the bits of the value that do not fit in the width.
BOOST_AUTO_TEST_CASE(masking_test)
{
  std::stringstream stream;
  {
    obitstream output(stream);
    output.write_bits(0xff, 4);
    output.write_bits(0, 4);
    output.write_bits(std::numeric_limits<std::size_t>::max(), 3);
    output.write_bits(0, 5);
  }

  ibitstream input(stream);
  BOOST_CHECK_EQUAL(input.read_bits(4), 0xfU);
  BOOST_CHECK_EQUAL(input.read_bits(4), 0U);
  BOOST_CHECK_EQUAL(input.read_bits(3), 7U);
  BOOST_CHECK_EQUAL(input.read_bits(5), 0U);
}

BOOST_AUTO_TEST_CASE(boundary_integers_test)
{
  std::vector<operation> operations;
  for (std::size_t value : boundary_integers())
  {
    operations.emplace_back(value);
    operations.emplace_back(bits_operation{1, 1}); // Shift the alignment of the next integer.
  }
  for (std::size_t value : boundary_integers())
  {
    operations.emplace_back(value); // And again, now at a different alignment.
  }

  std::stringstream stream(write_to_string(operations));
  ibitstream input(stream);
  BOOST_CHECK_EQUAL(check_operations(input, operations), 0U);
}

// Every width from 0 to 64 at every bit offset within a 64-bit word.
BOOST_AUTO_TEST_CASE(all_widths_and_offsets_test)
{
  std::mt19937_64 generator(42);
  std::vector<operation> operations;
  for (unsigned int offset = 0; offset < 64; ++offset)
  {
    for (unsigned int width = 0; width <= 64; ++width)
    {
      operations.emplace_back(bits_operation{generator(), offset});
      operations.emplace_back(bits_operation{generator(), width});
    }
  }

  std::stringstream stream(write_to_string(operations));
  ibitstream input(stream);
  BOOST_CHECK_EQUAL(check_operations(input, operations), 0U);
}

BOOST_AUTO_TEST_CASE(strings_test)
{
  std::vector<operation> operations;
  for (std::size_t length : {0, 1, 7, 8, 9, 63, 64, 65, 127, 128, 255, 4095, 4096, 4097, 65537, 1 << 20})
  {
    std::string s(length, ' ');
    for (std::size_t i = 0; i < length; ++i)
    {
      s[i] = static_cast<char>(1 + (i * 37) % 255);
    }
    operations.emplace_back(s);
    operations.emplace_back(bits_operation{5, 3});
  }

  std::stringstream stream(write_to_string(operations));
  ibitstream input(stream);
  BOOST_CHECK_EQUAL(check_operations(input, operations), 0U);
}

// A long random sequence of mixed operations, which crosses many internal buffer boundaries.
BOOST_AUTO_TEST_CASE(random_round_trip_test)
{
  for (std::uint64_t seed : {1, 2, 3})
  {
    const std::vector<operation> operations = random_operations(20000, seed, 300);
    std::stringstream stream(write_to_string(operations));
    ibitstream input(stream);
    BOOST_CHECK_EQUAL(check_operations(input, operations), 0U);
  }
}

// The same sequence, read from a stream buffer that delivers only a few bytes per underlying read.
BOOST_AUTO_TEST_CASE(chunked_input_test)
{
  const std::vector<operation> operations = random_operations(5000, 7, 300);
  const std::string bytes = write_to_string(operations);

  for (std::size_t max_chunk : {1, 3, 7, 13, 4096})
  {
    chunked_streambuf buffer(bytes, max_chunk);
    std::istream stream(&buffer);
    ibitstream input(stream);
    BOOST_CHECK_EQUAL(check_operations(input, operations), 0U);
  }
}

// Two bitstreams that share the same underlying std::istream (as binary_aterm_istream and binary_ldd_istream share
// one ibitstream) see one consecutive sequence of bits.
BOOST_AUTO_TEST_CASE(shared_ibitstream_test)
{
  const std::vector<operation> first = random_operations(1000, 11, 50);
  const std::vector<operation> second = random_operations(1000, 12, 50);

  std::stringstream stream;
  {
    std::shared_ptr<obitstream> output = std::make_shared<obitstream>(stream);
    write_operations(*output, first);
    write_operations(*output, second);
  }

  std::shared_ptr<ibitstream> input = std::make_shared<ibitstream>(stream);
  std::shared_ptr<ibitstream> alias = input;
  BOOST_CHECK_EQUAL(check_operations(*input, first), 0U);
  BOOST_CHECK_EQUAL(check_operations(*alias, second), 0U);
}

BOOST_AUTO_TEST_CASE(end_of_file_test)
{
  {
    std::stringstream stream;
    ibitstream input(stream);
    BOOST_CHECK_THROW(input.read_bits(1), mcrl2::runtime_error);
  }

  {
    // Eight bytes are available, so 64 bits can be read, but not one more.
    std::stringstream stream(write_to_string({bits_operation{0x5a, 8}}));
    ibitstream input(stream);
    BOOST_CHECK_EQUAL(input.read_bits(8), 0x5aU);
    BOOST_CHECK_EQUAL(input.read_bits(56), 0U);
    BOOST_CHECK_THROW(input.read_bits(1), mcrl2::runtime_error);
  }

  {
    // A string whose length exceeds the remaining input.
    std::string bytes = write_to_string({std::string(20, 'a')});
    bytes.resize(10);
    std::stringstream stream(bytes);
    ibitstream input(stream);
    BOOST_CHECK_THROW(input.read_string(), mcrl2::runtime_error);
  }

  {
    // An integer whose encoding is cut off after the first byte, which has the continuation flag set.
    std::stringstream stream(std::string(1, static_cast<char>(0x80)));
    ibitstream input(stream);
    BOOST_CHECK_THROW(input.read_integer(), mcrl2::runtime_error);
  }
}

// An integer encoding with more continuation bytes than fit in a std::size_t is rejected.
BOOST_AUTO_TEST_CASE(integer_overflow_test)
{
  std::stringstream stream(std::string(16, static_cast<char>(0xff)));
  ibitstream input(stream);
  BOOST_CHECK_THROW(input.read_integer(), std::runtime_error);
}
