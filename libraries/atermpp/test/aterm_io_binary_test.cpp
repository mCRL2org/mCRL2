// Author(s): Jeroen Keiren, Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/aterm_io_binary.h"
#include "mcrl2/atermpp/aterm_io_text.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/utilities/exception.h"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include <cstdint>
#include <iomanip>
#include <limits>
#include <random>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>

using namespace atermpp;

namespace
{

/// \brief A stream buffer that serves the bytes of a string in small chunks, cycling through the chunk sizes
///        1, 2, ..., max_chunk. This mimics reading from a pipe.
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

std::string write_terms(const std::vector<aterm>& sequence)
{
  std::stringstream stream;
  {
    binary_aterm_ostream output(stream);
    for (const aterm& term : sequence)
    {
      output << term;
    }
    // The end of the stream is written and the buffer is flushed here.
  }
  return stream.str();
}

/// \brief Reads terms from the stream and checks them against the sequence, followed by the end of the stream.
void check_terms(binary_aterm_istream& input, const std::vector<aterm>& sequence)
{
  std::size_t mismatches = 0;
  for (std::size_t i = 0; i < sequence.size() && mismatches < 10; ++i)
  {
    aterm t;
    input.get(t);
    if (t != sequence[i])
    {
      BOOST_ERROR("term " << i << " differs: read " << t << " instead of " << sequence[i]);
      ++mismatches;
    }
  }

  if (mismatches == 0)
  {
    // The default constructed term indicates the end of the stream.
    aterm end;
    input.get(end);
    BOOST_CHECK(!end.defined());
  }
}

void check_round_trip(const std::vector<aterm>& sequence)
{
  std::stringstream stream(write_terms(sequence));
  binary_aterm_istream input(stream);
  check_terms(input, sequence);
}

std::string hex(const std::string& bytes)
{
  std::ostringstream out;
  for (std::size_t i = 0; i < bytes.size(); ++i)
  {
    out << (i == 0 ? "" : ", ") << (i % 16 == 0 && i > 0 ? "\n" : "") << "0x" << std::hex << std::setw(2)
        << std::setfill('0') << static_cast<unsigned int>(static_cast<unsigned char>(bytes[i]));
  }
  return out.str();
}

/// \brief Generates a reproducible random sequence of terms. Terms are built from a pool of earlier terms, so that
///        subterms are shared in many different ways, and some outputs are reused as subterms of later outputs.
std::vector<aterm> random_terms(std::size_t count, std::uint64_t seed)
{
  std::mt19937_64 generator(seed);

  std::vector<function_symbol> symbols;
  for (std::size_t arity = 0; arity <= 4; ++arity)
  {
    for (std::size_t i = 0; i < 5; ++i)
    {
      symbols.emplace_back("f" + std::to_string(i), arity);
    }
  }
  std::uniform_int_distribution<std::size_t> symbol_index(0, symbols.size() - 1);
  std::uniform_int_distribution<int> kind(0, 9);
  std::uniform_int_distribution<std::size_t> any_value;

  std::vector<aterm> pool = {aterm(function_symbol("leaf", 0))};
  std::vector<aterm> result;
  for (std::size_t i = 0; i < count; ++i)
  {
    std::uniform_int_distribution<std::size_t> pool_index(0, pool.size() - 1);
    aterm term;
    const int k = kind(generator);
    if (k == 0)
    {
      term = aterm_int(any_value(generator) >> (generator() % 64));
    }
    else if (k == 1)
    {
      aterm_list list;
      for (std::size_t j = generator() % 5; j > 0; --j)
      {
        list.push_front(pool[pool_index(generator)]);
      }
      term = list;
    }
    else
    {
      const function_symbol& f = symbols[symbol_index(generator)];
      std::vector<aterm> arguments;
      for (std::size_t j = 0; j < f.arity(); ++j)
      {
        arguments.push_back(pool[pool_index(generator)]);
      }
      term = aterm(f, arguments.begin(), arguments.end());
    }

    pool.push_back(term);
    if (generator() % 3 == 0)
    {
      result.push_back(term);
    }
  }
  return result;
}

// Transformers for transformer_test; a transformer is a plain function.
aterm rename_a_to_b(const aterm& x)
{
  if (x.function() == function_symbol("a", 1))
  {
    return aterm(function_symbol("b", 1), x[0]);
  }
  return x;
}

aterm rename_b_to_c(const aterm& x)
{
  if (x.function() == function_symbol("b", 1))
  {
    return aterm(function_symbol("c", 1), x[0]);
  }
  return x;
}

} // namespace

BOOST_AUTO_TEST_CASE(simple_int_test)
{
  std::stringstream stream;
  {
    binary_aterm_ostream output(stream);
    output << aterm_int(50);
  }

  binary_aterm_istream input(stream);
  aterm t;
  input.get(t);
  BOOST_CHECK_EQUAL(t, aterm_int(50));
}

BOOST_AUTO_TEST_CASE(simple_term_test)
{

  // The sequence of terms to send.
  std::vector<aterm> sequence;

  aterm f = aterm(function_symbol("f", 0));
  aterm g = aterm(function_symbol("g", 0));

  sequence.emplace_back(function_symbol("test", 2), f, g);
  sequence.push_back(static_cast<const aterm&>(f));
  sequence.emplace_back(function_symbol("nested", 2), sequence.front(), g);
  sequence.push_back(static_cast<const aterm&>(g));
  sequence.emplace_back(function_symbol("deeply_nested", 3), sequence[3], sequence.front(), sequence[3]);

  std::stringstream stream;
  {
    binary_aterm_ostream output(stream);

    for (const atermpp::aterm& term : sequence)
    {
      output << term;
    }

    // The buffer is flushed here.
  }

  binary_aterm_istream input(stream);

  for (const atermpp::aterm& index : sequence)
  {
    aterm t;
    input.get(t);
    BOOST_CHECK_EQUAL(t, index);
  }
}

BOOST_AUTO_TEST_CASE(transitions_test)
{
  std::vector<aterm> sequence;

  function_symbol transition("transition", 2);
  aterm label = aterm(function_symbol("state", 1), aterm_int(0));
  aterm time = aterm(function_symbol("time", 1), aterm_int(50));

  aterm_list states;

  for (std::size_t index = 0; index < 2; ++index)
  {
    sequence.emplace_back(transition, states, time);

    // Increase the state labels size.
    states.push_front(label);
  }

  std::stringstream stream;
  {
    binary_aterm_ostream output(stream);

    for (const atermpp::aterm& term : sequence)
    {
      output << term;
    }

    // The buffer is flushed here.
  }

  binary_aterm_istream input(stream);

  for (const atermpp::aterm& term : sequence)
  {
    aterm t;
    input.get(t);
    BOOST_CHECK_EQUAL(t, term);
  }
}

// The exact bytes written for a fixed sequence of terms. This pins down the binary aterm format (BAF): files written
// by earlier versions of the toolset must remain readable, so neither the writer nor the reader may change it
// without increasing BAF_VERSION.
BOOST_AUTO_TEST_CASE(exact_encoding_test)
{
  const aterm f(function_symbol("f", 0));
  const aterm g(function_symbol("g", 1), aterm_int(5));
  const aterm h(function_symbol("h", 3), f, g, f);
  aterm_list list;
  list.push_front(g);
  list.push_front(aterm_int(1000));

  const std::vector<aterm> sequence = {h, aterm_int(7), list, g, h, aterm_list()};

  const std::string actual = write_terms(sequence);
  const std::vector<unsigned char> expected_bytes = {
      0x00, 0x8b, 0xaf, 0x83, 0x08, 0x00, 0x59, 0x80, 0x14, 0x0b, 0x3c, 0x61, 0x74, 0x65, 0x72, 0x6d, //
      0x5f, 0x69, 0x6e, 0x74, 0x3e, 0x00, 0x60, 0x50, 0x05, 0x9c, 0x05, 0x68, 0x02, 0xd0, 0x07, 0x42, //
      0x30, 0x70, 0x30, 0xf1, 0x95, 0xb5, 0xc1, 0xd1, 0xe5, 0x7d, 0xb1, 0xa5, 0xcd, 0xd0, 0xf8, 0x01, //
      0xa0, 0x91, 0xe3, 0x63, 0x4b, 0x9b, 0xa2, 0xfb, 0x1b, 0x7b, 0x73, 0x9b, 0xa3, 0x93, 0xab, 0x1b, //
      0xa3, 0x7b, 0x91, 0xf0, 0x13, 0x93, 0x57, 0x40, 0x3d, 0xac, 0x99, 0xa0, 0x42, 0xa8, 0x00, 0x00, //
  };
  const std::string expected(expected_bytes.begin(), expected_bytes.end());

  BOOST_CHECK_MESSAGE(actual == expected, "Encoding changed; the bytes written are now:\n" << hex(actual));

  // The reader must decode the exact bytes, independently of what the current writer produces.
  std::stringstream stream(expected);
  binary_aterm_istream input(stream);
  check_terms(input, sequence);
}

BOOST_AUTO_TEST_CASE(empty_stream_test)
{
  std::stringstream stream(write_terms({}));
  binary_aterm_istream input(stream);
  aterm t;
  input.get(t);
  BOOST_CHECK(!t.defined());
}

BOOST_AUTO_TEST_CASE(integers_test)
{
  std::vector<aterm> sequence;
  const std::vector<std::size_t> values = {0,
      1,
      127,
      128,
      16383,
      16384,
      std::size_t(1) << 32,
      std::size_t(1) << 63,
      std::numeric_limits<std::size_t>::max()};

  const function_symbol pair("pair", 2);
  for (std::size_t value : values)
  {
    // As output (aterm_int_output packet), as an argument (shared aterm_int), and the same integer twice.
    sequence.push_back(aterm_int(value));
    sequence.emplace_back(pair, aterm_int(value), aterm_int(value));
    sequence.push_back(aterm_int(value));
  }
  check_round_trip(sequence);
}

// Integers written as aterm_int can be read with get_integer, interleaved with terms that introduce new function
// symbols and shared subterms, also when the same integer occurs as a subterm.
BOOST_AUTO_TEST_CASE(get_integer_test)
{
  const std::vector<std::size_t> values
    = {0, 1, 127, 128, std::size_t(1) << 32, std::size_t(1) << 63, std::numeric_limits<std::size_t>::max()};

  std::vector<aterm> sequence;
  for (std::size_t i = 0; i < values.size(); ++i)
  {
    // Every term uses a new function symbol and the integer as a subterm.
    sequence.push_back(aterm_int(values[i]));
    sequence.emplace_back(function_symbol("f" + std::to_string(i), 2), aterm_int(values[i]), aterm_int(values[i]));
    sequence.push_back(aterm_int(values[i]));
  }

  std::stringstream stream(write_terms(sequence));
  binary_aterm_istream input(stream);
  for (std::size_t i = 0; i < values.size(); ++i)
  {
    BOOST_CHECK_EQUAL(input.get_integer(), values[i]);
    aterm t;
    input.get(t);
    BOOST_CHECK_EQUAL(t, sequence[3 * i + 1]);
    BOOST_CHECK_EQUAL(input.get_integer(), values[i]);
  }

  aterm end;
  input.get(end);
  BOOST_CHECK(!end.defined());
}

// get_integer reports an error when the next term is not an integer, or at the end of the stream.
BOOST_AUTO_TEST_CASE(get_integer_error_test)
{
  const aterm a(function_symbol("a", 1), aterm_int(3));
  std::stringstream stream(write_terms({a, aterm_int(3)}));
  binary_aterm_istream input(stream);

  BOOST_CHECK_THROW(input.get_integer(), mcrl2::runtime_error);
  BOOST_CHECK_EQUAL(input.get_integer(), 3u);
  BOOST_CHECK_THROW(input.get_integer(), mcrl2::runtime_error);
}

// The default get_integer of aterm_istream, used by the text format, reads an aterm_int and takes its value. The text
// format reads integers with atol, so values above the largest long cannot be used here.
BOOST_AUTO_TEST_CASE(text_get_integer_test)
{
  const std::size_t large = std::size_t(1) << 32;
  const aterm a(function_symbol("a", 1), aterm_int(3));

  std::stringstream stream;
  {
    text_aterm_ostream output(stream, true);
    output << aterm_int(0);
    output << a;
    output << aterm_int(large);
  }

  text_aterm_istream text_input(stream);
  aterm_istream& input = text_input;
  BOOST_CHECK_EQUAL(input.get_integer(), 0u);
  BOOST_CHECK_THROW(input.get_integer(), mcrl2::runtime_error);
  BOOST_CHECK_EQUAL(input.get_integer(), large);
}

// More than 2^k function symbols for several k, so that the width of function symbol indices grows repeatedly. Also
// symbols with the same name and different arities, and names with unusual characters.
BOOST_AUTO_TEST_CASE(many_function_symbols_test)
{
  std::vector<aterm> sequence;
  const aterm leaf(function_symbol("leaf", 0));
  for (std::size_t i = 0; i < 1100; ++i)
  {
    const std::size_t arity = i % 3;
    const std::string name = (i % 7 == 0) ? "symbol with spaces, \"quotes\" and \xc3\xa9 " + std::to_string(i)
                                           : "s" + std::to_string(i / 3);
    std::vector<aterm> arguments(arity, leaf);
    sequence.emplace_back(function_symbol(name, arity), arguments.begin(), arguments.end());
  }
  sequence.emplace_back(function_symbol(std::string(5000, 'x'), 0)); // A long name.
  check_round_trip(sequence);
}

// More than 2^16 distinct shared subterms, so that the width of term indices grows past 16 bits.
BOOST_AUTO_TEST_CASE(many_terms_test)
{
  const function_symbol node("node", 2);
  std::vector<aterm> sequence;
  aterm chain(function_symbol("nil", 0));
  for (std::size_t i = 0; i < 70000; ++i)
  {
    chain = aterm(node, aterm_int(i), chain);
    if (i % 4096 == 0 || i == 65535 || i == 65536)
    {
      sequence.push_back(chain);
    }
  }
  sequence.push_back(chain);

  // Terms that refer to subterms with both small and large indices.
  sequence.emplace_back(node, aterm_int(0), sequence[1]);
  sequence.emplace_back(node, chain, aterm_int(1));
  check_round_trip(sequence);
}

BOOST_AUTO_TEST_CASE(lists_test)
{
  aterm_list empty;
  aterm_list ints;
  for (std::size_t i = 0; i < 10; ++i)
  {
    ints.push_front(aterm_int(i));
  }
  aterm_list nested;
  nested.push_front(empty);
  nested.push_front(ints);
  nested.push_front(ints);

  // Not much longer: in Debug builds, every garbage collection verifies the whole tail of every list cell, which is
  // quadratic in the length of the list.
  aterm_list long_list;
  for (std::size_t i = 0; i < 5000; ++i)
  {
    long_list.push_front(aterm_int(i % 1000));
  }

  check_round_trip({empty, ints, nested, empty, long_list, ints});
}

// The container operators write the number of elements followed by the elements.
BOOST_AUTO_TEST_CASE(container_test)
{
  std::vector<aterm> terms = {aterm_int(1), aterm(function_symbol("f", 0)), aterm_list()};
  std::vector<aterm> empty;

  std::stringstream stream;
  {
    binary_aterm_ostream output(stream);
    output << terms;
    output << empty;
    output << terms;
  }

  binary_aterm_istream input(stream);
  std::vector<aterm> read1;
  std::vector<aterm> read2;
  std::vector<aterm> read3;
  input >> read1;
  input >> read2;
  input >> read3;
  BOOST_CHECK(read1 == terms);
  BOOST_CHECK(read2.empty());
  BOOST_CHECK(read3 == terms);
}

// A term that has been written as output is not shared, so it must be written again when it later occurs as a
// subterm; and a term that has been written as a subterm can later be written as output.
BOOST_AUTO_TEST_CASE(output_and_subterm_test)
{
  const aterm a(function_symbol("a", 1), aterm_int(1));
  const aterm b(function_symbol("b", 2), a, a);
  const aterm c(function_symbol("c", 2), b, a);
  check_round_trip({a, b, a, c, b, c, c});
}

BOOST_AUTO_TEST_CASE(transformer_test)
{
  const aterm a(function_symbol("a", 1), aterm_int(3));
  const aterm nested(function_symbol("pair", 2), a, aterm(function_symbol("b", 1), a));

  std::stringstream stream;
  {
    binary_aterm_ostream output(stream);
    output << rename_a_to_b;
    output << a;
    output << nested;
  }

  // Without a transformer, the read terms are the transformed written terms.
  {
    std::stringstream copy(stream.str());
    binary_aterm_istream input(copy);
    const aterm b(function_symbol("b", 1), aterm_int(3));
    check_terms(input, {b, aterm(function_symbol("pair", 2), b, aterm(function_symbol("b", 1), b))});
  }

  // With a transformer, it is applied to every read subterm as well.
  {
    std::stringstream copy(stream.str());
    binary_aterm_istream input(copy);
    input >> rename_b_to_c;
    const aterm c(function_symbol("c", 1), aterm_int(3));
    check_terms(input, {c, aterm(function_symbol("pair", 2), c, aterm(function_symbol("c", 1), c))});
  }
}

BOOST_AUTO_TEST_CASE(random_round_trip_test)
{
  for (std::uint64_t seed : {1, 2, 3})
  {
    check_round_trip(random_terms(30000, seed));
  }
}

// The same terms read from a stream buffer that delivers only a few bytes per underlying read.
BOOST_AUTO_TEST_CASE(chunked_input_test)
{
  const std::vector<aterm> sequence = random_terms(5000, 4);
  const std::string bytes = write_terms(sequence);
  for (std::size_t max_chunk : {1, 5, 64})
  {
    chunked_streambuf buffer(bytes, max_chunk);
    std::istream stream(&buffer);
    binary_aterm_istream input(stream);
    check_terms(input, sequence);
  }
}

BOOST_AUTO_TEST_CASE(invalid_header_test)
{
  const std::string bytes = write_terms({aterm_int(1)});

  // Not a binary aterm stream at all.
  {
    std::stringstream stream("This is not a binary aterm stream.");
    BOOST_CHECK_THROW(binary_aterm_istream input(stream), mcrl2::runtime_error);
  }

  // An empty input.
  {
    std::stringstream stream;
    BOOST_CHECK_THROW(binary_aterm_istream input(stream), mcrl2::runtime_error);
  }

  // A corrupted magic number; the first byte is zero, the next two bytes are the magic number 0x8baf.
  {
    std::string corrupted = bytes;
    corrupted[1] = static_cast<char>(0x8c);
    std::stringstream stream(corrupted);
    BOOST_CHECK_THROW(binary_aterm_istream input(stream), mcrl2::runtime_error);
  }

  // A different version; bytes three and four contain the version.
  {
    std::string corrupted = bytes;
    corrupted[4] = static_cast<char>(corrupted[4] - 1);
    std::stringstream stream(corrupted);
    BOOST_CHECK_THROW(binary_aterm_istream input(stream), mcrl2::runtime_error);
  }
}

// A stream that ends before the end-of-stream marker is reported as an error, not silently accepted.
BOOST_AUTO_TEST_CASE(truncated_stream_test)
{
  const std::vector<aterm> sequence = random_terms(2000, 5);
  const std::string bytes = write_terms(sequence);

  for (std::size_t length : {bytes.size() / 4, bytes.size() / 2, bytes.size() - 9})
  {
    std::stringstream stream(bytes.substr(0, length));
    binary_aterm_istream input(stream);
    bool thrown = false;
    try
    {
      for (std::size_t i = 0; i <= sequence.size(); ++i)
      {
        aterm t;
        input.get(t);
      }
    }
    catch (const mcrl2::runtime_error&)
    {
      thrown = true;
    }
    BOOST_CHECK_MESSAGE(thrown, "reading a stream truncated to " << length << " of " << bytes.size()
                                                                 << " bytes did not throw");
  }
}
