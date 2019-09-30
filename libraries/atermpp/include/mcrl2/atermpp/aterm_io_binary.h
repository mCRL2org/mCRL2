// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_ATERMPP_ATERM_IO_BINARY_H
#define MCRL2_ATERMPP_ATERM_IO_BINARY_H

#include "mcrl2/atermpp/aterm_io.h"

#include "mcrl2/utilities/bitstream.h"
#include "mcrl2/utilities/indexed_set.h"

namespace atermpp
{

/// \brief Writes terms in a streamable binary aterm format to an output stream.
/// \details The streamable aterm format:
///
///          Aterms (and function symbols) are written as packets (with an identifier in the header) and their
///          indices are derived from the number of aterms, resp. symbols, that occur before them in this stream. For each term
///          we first ensure that its arguments and symbol are written to the stream (avoiding duplicates). Then its
///          symbol index followed by a number of indices (depending on the arity) for its argments are written as integers.
///          Packet headers also contain a special value to indicate that the read term should be visible as output as opposed to
///          being only a subterm.
///          The start of the stream is a zero followed by a header and a version and a term with function symbol index zero
///          indicates the end of the stream.
///
class binary_aterm_output final : public aterm_output
{
public:
  /// \brief Provide the output stream to which the terms are written.
  /// \param transformer A function transforming the function symbols before writing, see the type for details.
  binary_aterm_output(std::ostream& os, std::function<aterm_transformer> transformer = identity);
  ~binary_aterm_output() override;

  /// \brief Writes an aterm in a compact binary format that keeps subterms shared. The term that is
  ///        written itself is not shared whenever it occurs as the argument of another term.
  const aterm_output& operator<<(const aterm& term) override;

private:
  /// \brief Write a function symbol to the output stream.
  std::size_t write_function_symbol(const function_symbol& symbol);

  /// \returns The number of bits needed to index terms.
  unsigned int term_index_width();

  /// \returns The number of bits needed to index function symbols.
  unsigned int function_symbol_index_width();

  mcrl2::utilities::obitstream m_stream;
  std::function<aterm_transformer> m_transformer;

  unsigned int m_term_index_width; ///< caches the result of term_index_width().
  unsigned int m_function_symbol_index_width; ///< caches the result of function_symbol_index_width().

  mcrl2::utilities::indexed_set<aterm> m_terms; ///< An index of already written terms.
  mcrl2::utilities::indexed_set<function_symbol> m_function_symbols; ///< An index of already written function symbols.
};

/// \brief Reads terms from a stream in the steamable binary aterm format.
class binary_aterm_input final : public aterm_input
{
public:
  /// \brief Provide the input stream from which terms are read.
  /// \param transformer A function transforming the function symbols after reading, see the type for details.
  binary_aterm_input(std::istream& is, std::function<aterm_transformer> transformer = identity);

  aterm get() override;
private:
  /// \returns The number of bits needed to index terms.
  unsigned int term_index_width();

  /// \returns The number of bits needed to index function symbols.
  unsigned int function_symbol_index_width();

  mcrl2::utilities::ibitstream m_stream;
  std::function<aterm_transformer> m_transformer;

  unsigned int m_term_index_width; ///< caches the result of term_index_width().
  unsigned int m_function_symbol_index_width; ///< caches the result of function_symbol_index_width().

  std::deque<aterm> m_terms; ///< An index of read terms.
  std::deque<function_symbol> m_function_symbols; ///< An index of read function symbols.
};

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_IO_BINARY_H
