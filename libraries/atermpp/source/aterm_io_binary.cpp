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

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <stdexcept>
#include <unordered_set>

#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#endif

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/detail/aterm_io_implementation.h"

#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/logger.h"


/* Integers in BAF are always exactly 32 or 64 bits.  The size must be fixed so that
 *  *  * BAF terms can be exchanged between platforms. */
static const std::size_t INT_SIZE_IN_BAF = 64;


namespace atermpp
{

using detail::readInt;
using detail::writeInt;

using namespace std;

static void aterm_io_init(std::ios&
#ifdef WIN32 // This suppresses a compiler warning.
stream
#endif
)
{
#ifdef WIN32
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
#endif
}

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
    std::size_t code_width;          /* This is the log of the number of symbols both for "symbols" and "index_into_symbols". */
    std::vector<top_symbol> symbols; /* The set of symbols that occur directly below the top symbol.
                                        The order of the symbols in this vector is important. */
    unordered_map<function_symbol, std::size_t> index_into_symbols;
                                     /* This mapping helps to find the entry in symbols with the given
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
    std::vector<vector<std::size_t> > topsyms;
    std::vector<std::size_t> sym_width;

    sym_read_entry():
       term_width(0)
    {
    }
};

static char* text_buffer = nullptr;
static std::size_t text_buffer_size = 0;

static unsigned char bit_buffer = '\0';
static std::size_t  bits_in_buffer = 0; /* how many bits in bit_buffer are used */

static void writeBits(std::size_t val, const std::size_t nr_bits, ostream& os)
{
  for (std::size_t cur_bit=0; cur_bit<nr_bits; cur_bit++)
  {
    bit_buffer <<= 1;
    bit_buffer |= (val & 0x01);
    val >>= 1;
    if (++bits_in_buffer == 8)
    {
      os.put(bit_buffer);
      bits_in_buffer = 0;
      bit_buffer = '\0';
    }
  }
  assert(val==0);
}


static void flushBitsToWriter(ostream& os)
{
  if (bits_in_buffer > 0)
  {
    std::size_t left = 8-bits_in_buffer;
    bit_buffer <<= left;
    os.put(bit_buffer);
    if (os.fail())
    {
      throw mcrl2::runtime_error("Failed to write the last byte to the output file/stream.");
    }
    bits_in_buffer = 0;
    bit_buffer = '\0';
  }
}

/**
 * @brief readBits Reads an n-bit integer from the input stream.
 * @param val      Variable to store integer in.
 * @param nr_bits  Number of bits to read from the input stream.
 * @param is       The input stream.
 * @return true on success, false on failure (EOF).
 */
static
bool readBits(std::size_t& val, const std::size_t nr_bits, istream& is)
{
  std::size_t cur_bit, mask = 1;

  val = 0;
  for (cur_bit=0; cur_bit<nr_bits; cur_bit++)
  {
    if (bits_in_buffer == 0)
    {
      int byte = is.get();
      if (is.fail())
      {
        return false;
      }
      bit_buffer = (unsigned char)byte;
      bits_in_buffer = 8;
    }
    val |= (bit_buffer & 0x80 ? mask : 0);
    mask <<= 1;
    bit_buffer <<= 1;
    bits_in_buffer--;
  }
  return true;
}

static void writeString(const std::string& str, ostream& os)
{
  /* Write length. */
  writeInt(str.size(), os);

  /* Write actual string. */
  os.write(str.c_str(), str.size());
}


static std::size_t readString(istream& is)
{
  std::size_t len;

  /* Get length of string */
  len = readInt(is);

  /* Assure buffer can hold the string */
  if (text_buffer_size < (len+1))
  {
    text_buffer_size = 2*len;
    text_buffer = (char*) realloc(text_buffer, text_buffer_size);
    if (!text_buffer)
    {
      throw mcrl2::runtime_error("Out of memory while reading the input file. Fail to claim a block of memory of size "+ std::to_string(text_buffer_size) + ".");
    }
  }

  /* Read the actual string */
  is.read(text_buffer, len);

  /* Ok, return length of string */
  return len;
}

/**
 * Write a symbol to file.
 */
static void write_symbol(const function_symbol& sym, ostream& os)
{
  writeString(sym.name(), os);
  writeInt(sym.arity(), os);
}

/**
 * \brief Get the function symbol from an aterm
 * \detail This function is necessary only becuase aterm::function() is protected
 */
static const function_symbol& get_function_symbol(const aterm& t)
{
  assert(t.type_is_int() || t.type_is_list() || t.type_is_appl());
  return detail::address(t)->function();
}

/**
 * \brief Retrieve the index into the sym_write_entry table belonging to the top symbol
 * of a term. Could be a special symbol (AS_INT, etc) when the term is not an application.
 * \detail sym_entries[result].id == t.function()
 */
static std::size_t get_fn_symbol_index(const aterm& t, const std::unordered_map<function_symbol, std::size_t>& index)
{
  const function_symbol& sym = get_function_symbol(t);
  assert(index.count(sym)>0);
  return index.at(sym);
}


/**
 * How many bits are needed to represent <val>
 * Basically, this function is equal to log2(val), except that it maps 0 to 0
 */
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
  const std::unordered_map<function_symbol, std::size_t>& symbol_index_map,
  std::unordered_map<aterm, std::size_t>& term_index_map,
  std::vector<sym_write_entry>& sym_entries)
{
  term_index_map[term] = entry.num_terms++;
  std::size_t arity = entry.id.arity();
  // Initialize the vector if necessary
  if(entry.top_symbols.size() != arity)
  {
    entry.top_symbols = std::vector<top_symbols_t>(arity);
  }

  if (entry.id != detail::function_adm.AS_INT)
  {
    // For every argument, check whether the term should be added to the table
    for (std::size_t cur_arg=0; cur_arg<arity; cur_arg++)
    {
      const aterm& arg = subterm(term, cur_arg);
      top_symbols_t& tss = entry.top_symbols[cur_arg];
      std::size_t top_symbol_index = get_fn_symbol_index(arg, symbol_index_map);
      const function_symbol& top_symbol = sym_entries[top_symbol_index].id;

      if (tss.index_into_symbols.count(top_symbol)==0)
      {
        std::size_t num_symbols = tss.symbols.size();
        tss.symbols.emplace_back(top_symbol_index, num_symbols);
        tss.index_into_symbols[top_symbol] = num_symbols;
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
  std::unordered_map<function_symbol, std::size_t>& symbol_index_map,
  std::vector<sym_write_entry>& sym_entries)
{
  auto insert_result = symbol_index_map.insert(std::make_pair(func, sym_entries.size()));
  if(insert_result.second)
  {
    // We just found a new function symbol, it has 1 occurrence so far
    sym_entries.emplace_back(func);
  }
  return sym_entries[insert_result.first->second];
}

/**
 * \brief Collect all terms in the term tables of each symbol
 */
static void collect_terms(const aterm& t,
  std::unordered_map<function_symbol, std::size_t>& symbol_index_map,
  std::unordered_map<aterm, std::size_t>& term_index_map,
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
static void write_all_symbols(ostream& os, const std::vector<sym_write_entry>& sym_entries)
{
  for(const sym_write_entry& cur_sym: sym_entries)
  {
    write_symbol(cur_sym.id, os);
    writeInt(cur_sym.num_terms, os);

    for (std::size_t arg_idx=0; arg_idx<cur_sym.id.arity(); arg_idx++)
    {
      std::size_t nr_symbols = cur_sym.top_symbols[arg_idx].symbols.size();
      writeInt(nr_symbols, os);
      for (std::size_t top_idx=0; top_idx<nr_symbols; top_idx++)
      {
        const top_symbol& ts = cur_sym.top_symbols[arg_idx].symbols[top_idx];
        writeInt(ts.index, os);
      }
    }
  }
}

/**
 * \brief Write the term t to os in BAF
 */
static void write_term(const aterm& t, ostream& os,
  const std::unordered_map<function_symbol, std::size_t>& symbol_index_map,
  const std::unordered_map<aterm, std::size_t>& term_index_map,
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
      // If aterm integers are > 32 bits, then they cannot be read on a 32 bit machine.
      writeBits(aterm_int(current.term).value(), INT_SIZE_IN_BAF, os);
    }
    else if (current.arg < get_function_symbol(current.term).arity())
    {
      write_todo item(subterm(current.term, current.arg));
      sym_write_entry& item_entry = sym_entries[symbol_index_map.at(get_function_symbol(item.term))];

      const top_symbols_t& symbol_table = cur_entry.top_symbols.at(current.arg);
      const top_symbol& ts = symbol_table.symbols.at(symbol_table.index_into_symbols.at(item_entry.id));
      writeBits(ts.code, symbol_table.code_width, os);
      const sym_write_entry& arg_sym = sym_entries.at(ts.index);
      std::size_t arg_trm_idx = term_index_map.at(item.term);
      writeBits(arg_trm_idx, arg_sym.term_width, os);

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
  flushBitsToWriter(os);
}


static void write_baf(const aterm& t, ostream& os)
{
  /* Initialize bit buffer */
  bit_buffer     = '\0';
  bits_in_buffer = 0; /* how many bits in bit_buffer are used */

  std::unordered_map<function_symbol, std::size_t> symbol_index_map;
  std::unordered_map<aterm, std::size_t> term_index_map;
  std::vector<sym_write_entry> sym_entries;

  collect_terms(t, symbol_index_map, term_index_map, sym_entries);
  compute_num_bits(sym_entries);

  /* write header */
  writeInt(0, os);
  writeInt(BAF_MAGIC, os);
  writeInt(BAF_VERSION, os);
  writeInt(sym_entries.size(), os);
  write_all_symbols(os, sym_entries);

  /* Write the top symbol */
  writeInt(get_fn_symbol_index(t,symbol_index_map), os);

  write_term(t, os, symbol_index_map, term_index_map, sym_entries);
}

void write_term_to_binary_stream(const aterm& t, std::ostream& os)
{
  aterm_io_init(os);
  write_baf(t, os);
}

/**
  * Read a single symbol from file.
  */

static function_symbol read_symbol(istream& is)
{
  std::size_t len=readString(is);

  text_buffer[len] = '\0';

  std::size_t arity = readInt(is);

  return function_symbol(text_buffer, arity);
}

/**
 * Read all symbols from file.
 */

static void read_all_symbols(istream& is, std::size_t nr_unique_symbols, std::vector<sym_read_entry>& read_symbols)
{
  std::size_t val;

  for (std::size_t i=0; i<nr_unique_symbols; i++)
  {
    /* Read the actual symbol */

    function_symbol sym = read_symbol(is);
    read_symbols[i].sym = sym;

    /* Read term count and allocate space */
    val = readInt(is);
    if (val == 0)
    {
      throw mcrl2::runtime_error("Read file: internal file error: failed to read all function symbols.");
    }
    read_symbols[i].term_width = bit_width(val);
    read_symbols[i].terms = std::vector<aterm>(val);

    /*  Allocate space for topsymbol information */
    read_symbols[i].sym_width = std::vector<std::size_t>(sym.arity());
    read_symbols[i].topsyms = std::vector< vector <std::size_t> > (sym.arity());

    for (std::size_t j=0; j<sym.arity(); j++)
    {
      val = readInt(is);
      read_symbols[i].sym_width[j] = bit_width(val);
      read_symbols[i].topsyms[j] = std::vector<std::size_t>(val);

      for (std::size_t k=0; k<read_symbols[i].topsyms[j].size(); k++)
      {
        read_symbols[i].topsyms[j][k] = readInt(is);
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

static aterm read_term(sym_read_entry* sym, istream& is, std::vector<sym_read_entry>& read_symbols)
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
    if (current.sym->sym != detail::function_adm.AS_INT && current.args.size() < current.sym->sym.arity())
    {
      if (readBits(value, current.sym->sym_width[current.args.size()], is) &&
          value < current.sym->topsyms[current.args.size()].size())
      {
        sym_read_entry* arg_sym = &read_symbols[current.sym->topsyms[current.args.size()][value]];
        if (readBits(value, arg_sym->term_width, is) &&
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

    if (current.sym->sym == detail::function_adm.AS_INT)
    {
      if (readBits(value, INT_SIZE_IN_BAF, is))
      {
        *current.result = aterm_int(value);
      }
    }
    else if (current.sym->sym==detail::function_adm.AS_EMPTY_LIST)
    {
      *current.result = aterm_list();
    }
    else if (current.sym->sym == detail::function_adm.AS_LIST)
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
aterm read_baf(istream& is)
{
  // Initialize bit buffer
  bit_buffer     = '\0';
  bits_in_buffer = 0; // how many bits in bit_buffer are used

  // Read header
  std::size_t val = readInt(is);
  if (val == 0)
  {
    val = readInt(is);
  }
  if (val != BAF_MAGIC)
  {
    throw mcrl2::runtime_error("Error while reading file: The file is not correct as it does not have the BAF_MAGIC control sequence at the right place.");
  }

  std::size_t version = readInt(is);
  if (version != BAF_VERSION)
  {
    throw mcrl2::runtime_error("The BAF version (" + std::to_string(version) + ") of the input file is incompatible with the version (" + std::to_string(BAF_VERSION) +
                               ") of this tool. The input file must be regenerated. ");
  }

  std::size_t nr_unique_symbols = readInt(is);

  // Allocate symbol space
  std::vector<sym_read_entry> read_symbols(nr_unique_symbols);

  read_all_symbols(is, nr_unique_symbols, read_symbols);

  val = readInt(is);
  aterm result=read_term(&read_symbols[val], is, read_symbols);
  return result;
}


aterm read_term_from_binary_stream(istream& is)
{
  aterm_io_init(is);
  aterm result=read_baf(is);
  if (!result.defined())
  {
    throw mcrl2::runtime_error("Failed to read a term from the input. The file is not a proper binary file.");
  }
  return result;
}

} // namespace atermpp
