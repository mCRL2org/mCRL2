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
#include "mcrl2/atermpp/detail/aterm_io_implementation.h"

#include "mcrl2/utilities/bitstream.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/indexed_set.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/platform.h"
#include "mcrl2/utilities/unordered_map.h"

#include <cassert>


namespace atermpp
{
using namespace mcrl2::utilities;

/* Integers in BAF are always exactly 32 or 64 bits.  The size must be fixed so that
 *  *  * BAF terms can be exchanged between platforms. */
// If aterm integers are > 32 bits, then they cannot be read on a 32 bit machine.
static const std::size_t INT_SIZE_IN_BAF = 64;

static const std::size_t BAF_MAGIC = 0xbaf;

// The BAF_VERSION constant is the version number of the ATerms written in BAF
// format. As of 29 August 2013 this version number is used by the mCRL2
// toolset. Whenever the file format of mCRL2 files is changed, the BAF_VERSION
// has to be increased.
//
// History:
//
// before 2013       : version 0x0300
// 29 August 2013    : version changed to 0x0301
// 23 November 2013  : version changed to 0x0302 (introduction of index for variable types)
// 24 September 2014 : version changed to 0x0303 (introduction of stochastic distribution)
//  2 April 2017     : version changed to 0x0304 (removed a few superfluous fields in the format)

static const std::size_t BAF_VERSION = 0x0304;

struct top_symbol
{
  /// \brief index into the vector with all sym_write_entries
  std::size_t index;
  /// \brief Identification number of this function symbol in the output
  std::size_t code;

  top_symbol(std::size_t index_, std::size_t code_)
   : index(index_), code(code_)
  {}
};

class top_symbols_t
{
  public:
    std::size_t code_width;          /**< This is the log of the number of symbols both for "symbols" and "index_into_symbols". */
    std::vector<top_symbol> symbols; /**< The set of symbols that occur directly below the top symbol.
                                        The order of the symbols in this vector is important. */
    mcrl2::utilities::indexed_set<function_symbol> index_into_symbols;
                                     /**< This mapping helps to find the entry in symbols with the given
                                          function symbol */
};

class sym_write_entry
{
  public:
    /// \brief The function symbol that this write entry is about
    const function_symbol id;
    /**
     * \brief After executing compute_num_bits, this stores the number of bits required
     * to uniquely identify each occurrence of id.
     */
    std::size_t term_width;
    /**
     * \brief The number of unique occurrences of id, i.e. the number of term that
     * have id as their function symbol.
     */
    std::size_t num_terms;
    /**
     * \brief Maps each argument index to a table with function symbols that may
     * occur at that index
     * top_symbols.size() == id.arity()
     */
    std::vector<top_symbols_t> top_symbols;
    /**
     * \brief Counter to indicate which argument is being worked on
     */
    std::size_t cur_index;

    sym_write_entry(const function_symbol& id_)
     : id(id_),
       term_width(0),
       num_terms(0),
       cur_index(0)
    {}
};

class sym_read_entry
{
  public:
    function_symbol sym;
    std::size_t term_width;
    std::vector<aterm> terms;
    std::vector<std::vector<std::size_t>> topsyms;
    std::vector<std::size_t> sym_width;

    sym_read_entry():
       term_width(0)
    {
    }
};

/**
 * Write a symbol to file.
 */
static void write_symbol(obitstream& stream, const function_symbol& sym)
{
  stream.writeString(sym.name());
  stream.writeInt(sym.arity());
}

/**
 * \brief Get the function symbol from an aterm
 * \details This function is necessary only becuase aterm::function() is protected
 */
static const function_symbol& get_function_symbol(const aterm& t)
{
  assert(t.type_is_int() || t.type_is_list() || t.type_is_appl());
  return detail::address(t)->function();
}

/**
 * \brief Retrieve the index into the sym_write_entry table belonging to the top symbol
 * of a term. Could be a special symbol (AS_INT, etc) when the term is not an application.
 * \details sym_entries[result].id == t.function()
 */
static std::size_t get_fn_symbol_index(const aterm& t, const mcrl2::utilities::indexed_set_large<function_symbol>& index)
{
  const function_symbol& sym = get_function_symbol(t);
  std::size_t result = index.at(sym);
  return result;
}

/**
 * \brief Get argument number i (zero indexed) from term t.
 */
static const aterm& subterm(const aterm& t, std::size_t i)
{
  if (t.type_is_appl())
  {
    assert(i < down_cast<const aterm_appl>(t).function().arity());
    return atermpp::down_cast<const aterm_appl>(t)[i];
  }
  else
  {
    assert(t.type_is_list() && t != aterm_list());
    assert(i < 2);
    return i == 0 ? atermpp::down_cast<const aterm_list>(t).front()
                  : atermpp::down_cast<const aterm_list>(t).tail();
  }
}

/**
 * \brief Add a term to the global term table. Update the symbol tables.
 */
static void add_term(sym_write_entry& entry, const aterm& term,
  const mcrl2::utilities::indexed_set_large<function_symbol>& symbol_index_map,
  mcrl2::utilities::unordered_map_large<aterm, std::size_t>& term_index_map,
  std::vector<sym_write_entry>& sym_entries)
{
  term_index_map[term] = entry.num_terms++;
  std::size_t arity = entry.id.arity();
  // Initialize the vector if necessary
  if(entry.top_symbols.size() != arity)
  {
    entry.top_symbols = std::vector<top_symbols_t>(arity);
  }

  if (entry.id != detail::g_term_pool().as_int())
  {
    // For every argument, check whether the term should be added to the table
    for (std::size_t cur_arg=0; cur_arg<arity; cur_arg++)
    {
      const aterm& arg = subterm(term, cur_arg);
      top_symbols_t& tss = entry.top_symbols[cur_arg];
      std::size_t top_symbol_index = get_fn_symbol_index(arg, symbol_index_map);
      const function_symbol& top_symbol = sym_entries[top_symbol_index].id;

      auto put_result = tss.index_into_symbols.insert(top_symbol);
      if (put_result.second)
      {
        tss.symbols.emplace_back(top_symbol_index, (*put_result.first).second);
      }
    }
  }
}

struct write_todo
{
  const aterm& term;
  std::size_t arg;

  write_todo(const aterm& t)
   :  term(t),
      arg(0)
  {}
};

/**
 * \brief If we see this function symbol for the first time, we initialize data
 * for it in symbol_index_map and sym_entries.
 * \return The sym_write_entry belonging to func.
 */
static sym_write_entry& initialize_function_symbol(const function_symbol& func,
  mcrl2::utilities::indexed_set_large<function_symbol>& symbol_index_map,
  std::vector<sym_write_entry>& sym_entries)
{
  auto insert_result = symbol_index_map.insert(func);
  if(insert_result.second)
  {
    // We just found a new function symbol, it has 1 occurrence so far
    sym_entries.emplace_back(func);
  }
  return sym_entries[(*insert_result.first).second];
}

/**
 * \brief Collect all terms in the term tables of each symbol
 */
static void collect_terms(const aterm& t,
  mcrl2::utilities::indexed_set_large<function_symbol>& symbol_index_map,
  mcrl2::utilities::unordered_map_large<aterm, std::size_t>& term_index_map,
  std::vector<sym_write_entry>& sym_entries)
{
  std::stack<write_todo> stack;
  stack.emplace(t);

  // Traverse the term in a postfix order: for every term, we first process each
  // of its arguments, before processing the term itself
  do
  {
    write_todo& current = stack.top();
    if (current.term.type_is_int() || current.arg >= get_function_symbol(current.term).arity())
    {
      // This term is an int or we are finished processing its arguments (arg >= arity)
      sym_write_entry& we = initialize_function_symbol(get_function_symbol(current.term), symbol_index_map, sym_entries);
      add_term(we, current.term, symbol_index_map, term_index_map, sym_entries);
      stack.pop();
    }
    else
    {
      // Take the argument according to current.arg and increase the counter
      const aterm& t = subterm(current.term, current.arg++);
      if (term_index_map.count(t) == 0)
      {
        stack.emplace(t);
      }
    }
  }
  while (!stack.empty());
}

/**
 * \brief Calculate the amount of bits required to store the top symbol for every
 * argument of every function symbol and the term for every function symbol.
 */
static void compute_num_bits(std::vector<sym_write_entry>& sym_entries)
{
  for(sym_write_entry& cur_entry: sym_entries)
  {
    for(std::size_t cur_arg = 0; cur_arg < cur_entry.id.arity(); cur_arg++)
    {
      top_symbols_t& tss = cur_entry.top_symbols[cur_arg];
      tss.code_width = bit_width(tss.symbols.size());
    }
    cur_entry.term_width = bit_width(cur_entry.num_terms);
  }
}

/**
 * Write all symbols in a term to file.
 */
static void write_all_symbols(mcrl2::utilities::obitstream& stream, const std::vector<sym_write_entry>& sym_entries)
{
  for(const sym_write_entry& cur_sym: sym_entries)
  {
    write_symbol(stream, cur_sym.id);
    stream.writeInt(cur_sym.num_terms);

    for (std::size_t arg_idx=0; arg_idx<cur_sym.id.arity(); arg_idx++)
    {
      std::size_t nr_symbols = cur_sym.top_symbols[arg_idx].symbols.size();
      stream.writeInt(nr_symbols);
      for (std::size_t top_idx=0; top_idx<nr_symbols; top_idx++)
      {
        const top_symbol& ts = cur_sym.top_symbols[arg_idx].symbols[top_idx];
        stream.writeInt(ts.index);
      }
    }
  }
}

/**
 * \brief Write the term t to os in BAF
 */
static void write_term(mcrl2::utilities::obitstream& stream, const aterm& t,
  const mcrl2::utilities::indexed_set_large<function_symbol>& symbol_index_map,
  const mcrl2::utilities::unordered_map_large<aterm, std::size_t>& term_index_map,
  std::vector<sym_write_entry>& sym_entries)
{
  std::stack<write_todo> stack;
  stack.emplace(t);

  do
  {
    write_todo& current = stack.top();
    sym_write_entry& cur_entry = sym_entries[symbol_index_map.at(get_function_symbol(current.term))];

    if (current.term.type_is_int())
    {
      stream.writeBits(atermpp::down_cast<aterm_int>(current.term).value(), INT_SIZE_IN_BAF);
    }
    else if (current.arg < get_function_symbol(current.term).arity())
    {
      write_todo item(subterm(current.term, current.arg));
      sym_write_entry& item_entry = sym_entries[symbol_index_map.at(get_function_symbol(item.term))];

      const top_symbols_t& symbol_table = cur_entry.top_symbols.at(current.arg);
      const top_symbol& ts = symbol_table.symbols.at(symbol_table.index_into_symbols.at(item_entry.id));
      stream.writeBits(ts.code, symbol_table.code_width);
      const sym_write_entry& arg_sym = sym_entries.at(ts.index);
      std::size_t arg_trm_idx = term_index_map.at(item.term);
      stream.writeBits(arg_trm_idx, arg_sym.term_width);

      ++current.arg;

      if (arg_trm_idx >= arg_sym.cur_index)
      {
        stack.push(item);
      }
      continue;
    }

    ++cur_entry.cur_index;
    stack.pop();
  }
  while (!stack.empty());

  stream.flushBitsToWriter();
}


static void write_baf(mcrl2::utilities::obitstream& stream, const aterm& t)
{
  mcrl2::utilities::indexed_set_large<function_symbol> symbol_index_map;
  mcrl2::utilities::unordered_map_large<aterm, std::size_t> term_index_map;
  std::vector<sym_write_entry> sym_entries;

  collect_terms(t, symbol_index_map, term_index_map, sym_entries);
  compute_num_bits(sym_entries);

  /* write header */
  stream.writeInt(0);
  stream.writeInt(BAF_MAGIC);
  stream.writeInt(BAF_VERSION);
  stream.writeInt(sym_entries.size());
  write_all_symbols(stream, sym_entries);

  /* Write the top symbol */
  stream.writeInt(get_fn_symbol_index(t,symbol_index_map));

  write_term(stream, t, symbol_index_map, term_index_map, sym_entries);
}

void write_term_to_binary_stream(const aterm& t, std::ostream& os)
{
  mcrl2::utilities::obitstream stream(os);
  write_baf(stream, t);
}

/// \brief Read a single function symbol from a stream <string, arity>.
static function_symbol read_symbol(ibitstream& stream)
{
  std::string name = stream.readString();
  std::size_t arity = stream.readInt();

  return function_symbol(name, arity);
}

/**
 * Read all symbols from file.
 */

static void read_all_symbols(mcrl2::utilities::ibitstream& stream, std::size_t nr_unique_symbols, std::vector<sym_read_entry>& read_symbols)
{
  std::size_t val;

  for (std::size_t i=0; i<nr_unique_symbols; i++)
  {
    // Read the actual symbol.
    function_symbol sym = read_symbol(stream);
    read_symbols[i].sym = sym;

    // Read term count and allocate space.
    val = stream.readInt();
    if (val == 0)
    {
      throw mcrl2::runtime_error("Read file: internal file error: failed to read all function symbols.");
    }
    read_symbols[i].term_width = bit_width(val);
    read_symbols[i].terms = std::vector<aterm>(val);

    /*  Allocate space for topsymbol information */
    read_symbols[i].sym_width = std::vector<std::size_t>(sym.arity());
    read_symbols[i].topsyms = std::vector<std::vector<std::size_t>> (sym.arity());

    for (std::size_t j=0; j<sym.arity(); j++)
    {
      val = stream.readInt();
      read_symbols[i].sym_width[j] = bit_width(val);
      read_symbols[i].topsyms[j] = std::vector<std::size_t>(val);

      for (std::size_t k=0; k<read_symbols[i].topsyms[j].size(); k++)
      {
        read_symbols[i].topsyms[j][k] = stream.readInt();
      }
    }
  }

  return;
}

struct read_todo
{
  sym_read_entry* sym;
  std::vector<aterm> args;
  aterm* result;
  aterm* callresult;

  read_todo(sym_read_entry* s, aterm* r)
   : sym(s), result(r), callresult(nullptr)
  {
    args.reserve(sym->sym.arity());
  }
};

static aterm read_term(mcrl2::utilities::ibitstream& stream, sym_read_entry* sym,  std::vector<sym_read_entry>& read_symbols)
{
  aterm result;
  std::size_t value;
  std::stack<read_todo> stack;
  stack.emplace(sym, &result);

  do
  {
    read_todo& current = stack.top();

    if (current.callresult != nullptr)
    {
      current.args.push_back(*current.callresult);
      current.callresult = nullptr;
    }

    // AS_INT is registered as having 1 argument, but that needs to be retrieved in a special way.
    if (current.sym->sym != detail::g_term_pool().as_int() && current.args.size() < current.sym->sym.arity())
    {
      if (stream.readBits(value, current.sym->sym_width[current.args.size()]) &&
          value < current.sym->topsyms[current.args.size()].size())
      {
        sym_read_entry* arg_sym = &read_symbols[current.sym->topsyms[current.args.size()][value]];
        if (stream.readBits(value, arg_sym->term_width) &&
            value < arg_sym->terms.size())
        {
          current.callresult = &arg_sym->terms[value];
          if (!current.callresult->defined())
          {
            stack.emplace(arg_sym, current.callresult);
          }
          continue;
        }
      }
      throw mcrl2::runtime_error("Could not read valid aterm from stream.");
    }

    if (current.sym->sym == detail::g_term_pool().as_int())
    {
      if (stream.readBits(value, INT_SIZE_IN_BAF))
      {
        *current.result = aterm_int(value);
      }
    }
    else if (current.sym->sym== detail::g_term_pool().as_empty_list())
    {
      *current.result = aterm_list();
    }
    else if (current.sym->sym == detail::g_term_pool().as_list())
    {
      aterm_list result = atermpp::down_cast<aterm_list>(current.args[1]);
      result.push_front(current.args[0]);
      *current.result = result;
    }
    else // sym is a function application
    {
      *current.result = aterm_appl(current.sym->sym, current.args.begin(), current.args.end());
    }
    stack.pop();
  }
  while (!stack.empty());

  return result;
}

/**
 * Read a term from a BAF reader.
 */

static
aterm read_baf(mcrl2::utilities::ibitstream& stream)
{
  // Read header
  std::size_t val = stream.readInt();
  if (val == 0)
  {
    val = stream.readInt();
  }
  if (val != BAF_MAGIC)
  {
    throw mcrl2::runtime_error("Error while reading file: The file is not correct as it does not have the BAF_MAGIC control sequence at the right place.");
  }

  std::size_t version = stream.readInt();
  if (version != BAF_VERSION)
  {
    throw mcrl2::runtime_error("The BAF version (" + std::to_string(version) + ") of the input file is incompatible with the version (" + std::to_string(BAF_VERSION) +
                               ") of this tool. The input file must be regenerated. ");
  }

  std::size_t nr_unique_symbols = stream.readInt();

  // Allocate symbol space
  std::vector<sym_read_entry> read_symbols(nr_unique_symbols);

  read_all_symbols(stream, nr_unique_symbols, read_symbols);

  val = stream.readInt();
  aterm result = read_term(stream, &read_symbols[val], read_symbols);
  return result;
}


aterm read_term_from_binary_stream(std::istream& is)
{
  mcrl2::utilities::ibitstream stream(is);

  aterm result = read_baf(stream);
  if (!result.defined())
  {
    throw mcrl2::runtime_error("Failed to read a term from the input. The file is not a proper binary file.");
  }
  return result;
}

} // namespace atermpp
