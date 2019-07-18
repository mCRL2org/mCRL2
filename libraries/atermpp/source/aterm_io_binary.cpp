// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file atermpp/source/aterm_io_binary.cpp
/// \brief This file allows to read and write terms in compact binary aterm`
///        format. This code stems largely from the ATerm library designed by
///        Paul Klint cs. 


/* includes */

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/atermpp/aterm_list.h"

#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/platform.h"
#include "mcrl2/utilities/unordered_map.h"

#include <cassert>
#include <cmath>

namespace atermpp
{
using namespace mcrl2::utilities;

static constexpr std::size_t BAF_MAGIC = 0xbaf;

/// \brief The BAF_VERSION constant is the version number of the ATerms written in BAF
///        format. As of 29 August 2013 this version number is used by the mCRL2
///        toolset. Whenever the file format of mCRL2 files is changed, the BAF_VERSION
///        has to be increased.
///
/// \details History:
///
/// before 2013       : version 0x0300
/// 29 August 2013    : version changed to 0x0301
/// 23 November 2013  : version changed to 0x0302 (introduction of index for variable types)
/// 24 September 2014 : version changed to 0x0303 (introduction of stochastic distribution)
///  2 April 2017     : version changed to 0x0304 (removed a few superfluous fields in the format)
static constexpr std::size_t BAF_VERSION = 0x0304;

/// \brief Each packet has a header consisting of a type and an invisible bit (indicating no output).
enum class packet_type
{
  function_symbol = 0,
  aterm,
  aterm_output,
  aterm_int,
};

/// \brief The number of bits needed to store an element of packet_type.
static constexpr unsigned int packet_bits = 2;

binary_aterm_output::binary_aterm_output(std::ostream& stream)
  : m_stream(stream)
{
  // The term with function symbol index 0 indicates the end of the stream, its actual value does not matter.
  m_function_symbols.insert(detail::g_as_int);
  m_function_symbol_index_width = 1;

  // Write the header of the binary aterm format.
  m_stream.write_integer(0);
  m_stream.write_integer(BAF_MAGIC);
  m_stream.write_integer(BAF_VERSION);
}

binary_aterm_output::~binary_aterm_output()
{
  // Write the end of the stream.
  m_stream.write_bits(static_cast<std::size_t>(packet_type::aterm), packet_bits);
  m_stream.write_integer(0);
}

/// \brief Keep track of the remaining arguments that still has to be processed for this term.
struct write_todo
{
  const aterm_appl& term;
  std::size_t arg = 0;

  write_todo(const aterm_appl& term)
   :  term(term)
  {}
};

void binary_aterm_output::write_term(const aterm& term)
{
  assert(!term.type_is_int());

  // Traverse the term bottom up and store the subterms (and function symbol) before the actual term.
  std::stack<write_todo> stack;
  stack.emplace(static_cast<const aterm_appl&>(term));

  do
  {
    auto& current = stack.top();
    if (current.arg >= current.term.function().arity())
    {
      // Indicates that this term is output and not a subterm.
      bool is_output = stack.size() == 1;

      if (current.term.type_is_int())
      {
        // Write the packet identifier of an aterm_int followed by its value.
        m_stream.write_bits(static_cast<std::size_t>(packet_type::aterm_int), packet_bits);
        m_stream.write_integer(reinterpret_cast<const aterm_int&>(current.term).value());
      }
      else
      {
        // We are finished processing the arguments of this term (arg >= arity)
        std::size_t symbol_index = write_function_symbol(current.term.function());

        // Write the packet identifier, followed by the indices of its function symbol and arguments.
        m_stream.write_bits(static_cast<std::size_t>(is_output ? packet_type::aterm_output : packet_type::aterm), packet_bits);
        m_stream.write_bits(symbol_index, function_symbol_index_width());
        for (const auto& argument : current.term)
        {
          auto result = m_terms.find(argument);
          assert(result != m_terms.end());
          m_stream.write_bits(result->second, term_index_width());
        }
      }

      if (!is_output)
      {
        // Output terms are not shared and thus can be forgotten.
        m_terms.insert(current.term);
        m_term_index_width = static_cast<std::uint8_t>(std::log2(m_terms.size()) + 1);
      }

      stack.pop();
    }
    else
    {
      // Take the argument according to current.arg and increase the argument index.
      const aterm_appl& term = static_cast<const aterm_appl&>(current.term[current.arg]);
      if (m_terms.count(term) == 0)
      {
        stack.emplace(term);
      }
      ++current.arg;
    }

  }
  while (!stack.empty());
}

unsigned int binary_aterm_output::term_index_width()
{
  assert(m_term_index_width == static_cast<unsigned int>(std::log2(m_terms.size()) + 1));
  return m_term_index_width;
}

unsigned int binary_aterm_output::function_symbol_index_width()
{
  assert(m_function_symbol_index_width == static_cast<unsigned int>(std::log2(m_function_symbols.size()) + 1));
  return m_function_symbol_index_width;
}

binary_aterm_input::binary_aterm_input(std::istream& is)
  : m_stream(is)
{
  // The term with function symbol index 0 indicates the end of the stream.
  m_function_symbols.emplace_back();
  m_function_symbol_index_width = 1;

  // Read the binary aterm format header.
  if (m_stream.read_integer() != 0 || m_stream.read_integer() != BAF_MAGIC)
  {
    throw mcrl2::runtime_error("Error while reading file: The file is not correct as it does not have the BAF_MAGIC control sequence at the right place.");
  }

  std::size_t version = m_stream.read_integer();
  if (version != BAF_VERSION)
  {
    throw mcrl2::runtime_error("The BAF version (" + std::to_string(version) + ") of the input file is incompatible with the version (" + std::to_string(BAF_VERSION) +
                               ") of this tool. The input file must be regenerated. ");
  }
}

std::size_t binary_aterm_output::write_function_symbol(const function_symbol& symbol)
{
  auto result = m_function_symbols.find(symbol);

  if (result != m_function_symbols.end())
  {
    return result->second;
  }
  else
  {
    // The function symbol has not been written yet, write it now and return its index.
    m_stream.write_bits(static_cast<std::size_t>(packet_type::function_symbol), packet_bits);
    m_stream.write_string(symbol.name());
    m_stream.write_integer(symbol.arity());
    auto result = m_function_symbols.insert(symbol);
    m_function_symbol_index_width = static_cast<unsigned int>(std::log2(m_function_symbols.size()) + 1);
    return result.first->second;
  }
}

aterm binary_aterm_input::read_term()
{
  while(true)
  {
    // Determine the type of the next packet.
    std::size_t header = m_stream.read_bits(packet_bits);
    packet_type packet = static_cast<packet_type>(header);

    if (packet == packet_type::function_symbol)
    {
      // Read a single function symbol and insert it into the already read function symbols.
      std::string name = m_stream.read_string();
      std::size_t arity = m_stream.read_integer();
      m_function_symbols.emplace_back(name, arity);
      m_function_symbol_index_width = static_cast<unsigned int>(std::log2(m_function_symbols.size()) + 1);
    }
    else if (packet == packet_type::aterm || packet == packet_type::aterm_output)
    {
      // First read the function symbol of the following term.
      function_symbol symbol = m_function_symbols[m_stream.read_bits(function_symbol_index_width())];

      if (!symbol.defined())
      {
        // The term with function symbol zero marks the end of the stream.
        return aterm();
      }

      // Read arity number of arguments from the stream and search them in the already defined set of terms.
      std::vector<aterm> arguments(symbol.arity());
      for (std::size_t argument = 0; argument < symbol.arity(); ++argument)
      {
        arguments[argument] = m_terms[m_stream.read_bits(term_index_width())];
      }

      if (packet == packet_type::aterm_output)
      {
        // This aterm was marked as output in the file so return it.
        return static_cast<aterm>(aterm_appl(symbol, arguments.begin(), arguments.end()));
      }
      else
      {
        // Construct the term appl from the function symbol and the already read arguments and insert it.
        m_terms.emplace_back(aterm_appl(symbol, arguments.begin(), arguments.end()));
        m_term_index_width = static_cast<unsigned int>(std::log2(m_terms.size()) + 1);
      }
    }
    else if (packet == packet_type::aterm_int)
    {
      // Read the integer from the stream and construct an aterm_int.
      std::size_t value = m_stream.read_integer();
      m_terms.emplace_back(aterm_int(value));
      m_term_index_width = static_cast<unsigned int>(std::log2(m_terms.size()) + 1);
    }
  }
}

unsigned int binary_aterm_input::term_index_width()
{
  assert(m_term_index_width == static_cast<unsigned int>(std::log2(m_terms.size()) + 1));
  return m_term_index_width;
}

unsigned int binary_aterm_input::function_symbol_index_width()
{
  assert(m_function_symbol_index_width == static_cast<unsigned int>(std::log2(m_function_symbols.size()) + 1));
  return m_function_symbol_index_width;
}

void write_term_to_binary_stream(const aterm& t, std::ostream& os)
{
  binary_aterm_output output(os);
  output.write_term(t);
}

aterm read_term_from_binary_stream(std::istream& is)
{
  binary_aterm_input input(is);
  return input.read_term();
}

} // namespace atermpp
