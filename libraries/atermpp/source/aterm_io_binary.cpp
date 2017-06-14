// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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

// Count the total number of occurrences of function symbols t where 
// t is viewed as a shared term. The result is stored in the unordered_map count. 
// Visited is used to avoid visiting terms too often. 
static std::unordered_map<function_symbol, std::size_t> count_the_number_of_unique_function_symbols_in_a_term(
                  const aterm& t)
{
  std::stack<aterm> todo_stack({t});
  std::unordered_set<aterm> visited({t});
  std::unordered_map<function_symbol, std::size_t> count;

  while (!todo_stack.empty())
  {
    const aterm t=todo_stack.top();
    todo_stack.pop();
    assert(visited.count(t)>0);
    
    if (t.type_is_int())
    {
      count[detail::function_adm.AS_INT]++;
    }
    else if (t.type_is_list())
    {
      const aterm_list& list = down_cast<const aterm_list>(t);
      if (list==aterm_list())
      {
        count[detail::function_adm.AS_EMPTY_LIST]++;
      }
      else 
      { 
        count[detail::function_adm.AS_LIST]++;
        if (visited.count(list.tail())==0) 
        {
          visited.insert(list.tail());
          todo_stack.push(list.tail());
        }
        if (visited.count(list.front())==0)
        {
          visited.insert(list.front());
          todo_stack.push(list.front());
        }
      }
    }
    else
    {
      const aterm_appl& ta=down_cast<aterm_appl>(t);
      function_symbol sym = ta.function();
      count[sym]++;
      for (std::size_t i=ta.size(); i>0; )
      {
        --i;
        if (visited.count(ta[i])==0)
        {
          visited.insert(ta[i]);
          todo_stack.push(ta[i]);
        }
      }
    }
  }
  return count;
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
  std::size_t index;
  std::size_t code;

  top_symbol(std::size_t index_, std::size_t code_)
   : index(index_), code(code_)
  {}
};

class top_symbols_t
{
  public:
    std::size_t code_width;               /* This is the log of the number of symbols both "symbols" and "index_into_symbols". */
    std::vector<top_symbol> symbols; /* The set of symbols that occur directly below the top symbol. 
                                        The order of the symbols in this vector is important. */
    unordered_map<function_symbol, std::size_t> index_into_symbols; 
                                     /* This mapping helps to find the entry in symbols with the given 
                                        function symbol */
};

class sym_write_entry
{
  public:
    function_symbol id;
    std::size_t term_width;
    unordered_map<aterm, std::size_t> write_terms; /* Collect the terms with this id as top symbol, 
                                                 and maintain a consecutive index for each term. 
                                                 This set can be restricted to those terms that occur
                                                 at least twice in the term. This can effectively be
                                                 seen by inspecting the reference count of terms. */

    std::vector<top_symbols_t> top_symbols; /* top symbols occuring in this symbol */

    std::size_t cur_index;

    sym_write_entry(const function_symbol& id_, 
                    std::size_t term_width_)
     : id(id_), 
       term_width(term_width_),
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
 * Retrieve the sym_write_entry belonging to the top symbol of a term. Could be a special symbol
 * (AS_INT, etc) when the term is not an application.
 */

static sym_write_entry* get_top_symbol(const aterm& t, const std::unordered_map<function_symbol, std::size_t>& index, std::vector<sym_write_entry>& sym_entries)
{
  function_symbol sym;

  if (t.type_is_int())
  {
      sym = detail::function_adm.AS_INT;
  }
  else if (t.type_is_list())
  {
    sym = (t==aterm_list() ? detail::function_adm.AS_EMPTY_LIST : detail::function_adm.AS_LIST);
  }
  else if (t.type_is_appl())
  {
    sym = down_cast<aterm_appl>(t).function();
  }
  else
  {
    throw mcrl2::runtime_error("Internal error. Illegal term encountered (" + pp(t) + ")");
  }
  assert(index.count(sym)>0);
  return &sym_entries[index.at(sym)];
}


/* How many bits are needed to represent <val> */
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
  * Build argument tables given the fact that the
  * terms have been sorted by symbol.
  */


static void build_arg_tables(const std::unordered_map<function_symbol, std::size_t>& index, 
                             std::vector<sym_write_entry>& sym_entries)
{
  for (sym_write_entry& cur_entry: sym_entries)
  {
    std::size_t arity = cur_entry.id.arity();
    cur_entry.top_symbols = std::vector<top_symbols_t>(arity);

    if (cur_entry.id!=detail::function_adm.AS_INT)
    {
      for (std::size_t cur_arg=0; cur_arg<arity; cur_arg++)
      {
        top_symbols_t& tss = cur_entry.top_symbols[cur_arg];
        std::size_t total_top_symbols = 0;
        for(const pair<aterm, std::size_t>& p: cur_entry.write_terms)
        {
          aterm term = p.first;
          aterm arg;
          if (term.type_is_list())
          {
            aterm_list list(term);
            assert(list!=aterm_list());
            assert(arity == 2);
            if (cur_arg == 0)
            {
              arg = list.front();
            }
            else
            {
              arg = list.tail();
            }
          }
          else if (term.type_is_appl())
          {
            arg = down_cast<const aterm_appl>(term)[cur_arg];
          }
          else
          {
            throw mcrl2::runtime_error("Internal inconsistency found in internal data structure. Illegal term.");
          }
          sym_write_entry* topsym = get_top_symbol(arg, index, sym_entries);
          
          if (tss.index_into_symbols.count(topsym->id)==0)
          {
            total_top_symbols++;
            std::size_t index=tss.symbols.size();
            tss.symbols.emplace_back(topsym-&sym_entries[0], index);
            tss.index_into_symbols[topsym->id]=index;
          }
        }
        tss.code_width=bit_width(total_top_symbols);
      }
    }
  }
}


/**
  * Add a term to the termtable of a symbol.
  */
static void add_term(sym_write_entry* entry, const aterm& t)
{
  const std::size_t oldsize=entry->write_terms.size(); /* Do not combine with the next line,
                                                     as write_terms.size() may be calculated after 
                                                     write_terms[t] has been executed */
  entry->write_terms[t]=oldsize;
}

/**
 * Collect all terms in the appropriate symbol table.
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

struct write_todo
{ 
  aterm term; 
  sym_write_entry* entry; 
  std::size_t arg; 

  write_todo(const aterm& t, const std::unordered_map<function_symbol, std::size_t>& index, std::vector<sym_write_entry>& sym_entries)
   :  term(t),
      entry(get_top_symbol(t, index, sym_entries)),
      arg(0)
  {}
};

static void collect_terms(const aterm& t, const std::unordered_map<function_symbol, std::size_t>& index, std::vector<sym_write_entry>& sym_entries)
{
  std::stack<write_todo> stack;
  std::unordered_set<aterm> visited;
  stack.emplace(t, index, sym_entries);

  do
  {
    write_todo& current=stack.top();
    if (current.term.type_is_int())
    {
      add_term(current.entry, current.term);
      visited.insert(current.term);
      stack.pop();
    }
    else if (current.arg < current.entry->id.arity())
    {
      const aterm t = subterm(current.term, current.arg++);
      if (visited.count(t) == 0)
      {
        stack.emplace(t, index, sym_entries);
      }
    }
    else
    {
      add_term(current.entry, current.term);
      visited.insert(current.term);
      stack.pop();
    }
  }
  while (!stack.empty());
}

/**
 * Write all symbols in a term to file.
 */

static void write_all_symbols(ostream& os, const std::vector<sym_write_entry>& sym_entries)
{
  for(const sym_write_entry& cur_sym: sym_entries)
  {
    write_symbol(cur_sym.id, os);
    writeInt(cur_sym.write_terms.size(), os);

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
 * Find a top symbol in a topsymbol table.
 */

static const top_symbol& find_top_symbol(top_symbols_t* syms, const function_symbol& sym)
{
  return syms->symbols[syms->index_into_symbols[sym]];
}

/**
 * Write a term using a writer.
 */

static void write_term(const aterm& t, const std::unordered_map<function_symbol, std::size_t>& index, ostream& os, std::vector<sym_write_entry>& sym_entries)
{
  std::stack<write_todo> stack;
  stack.emplace(t, index, sym_entries);

  do
  {
    write_todo& current = stack.top();

    if (current.term.type_is_int())
    {
      // If aterm integers are > 32 bits, then they cannot be read on a 32 bit machine.
      writeBits(aterm_int(current.term).value(), INT_SIZE_IN_BAF, os);
    }
    else
    if (current.arg < current.entry->id.arity())
    {
      write_todo item(subterm(current.term, current.arg), index, sym_entries);

      const top_symbol& ts = find_top_symbol(&current.entry->top_symbols[current.arg], item.entry->id);
      writeBits(ts.code, current.entry->top_symbols[current.arg].code_width, os);
      sym_write_entry& arg_sym = sym_entries[ts.index];
      std::size_t arg_trm_idx = arg_sym.write_terms.at(item.term); 
      writeBits(arg_trm_idx, arg_sym.term_width, os);

      ++current.arg;

      if (arg_trm_idx >= arg_sym.cur_index)
      {
        stack.push(item);
      }
      continue;
    }

    ++current.entry->cur_index;
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

  std::unordered_map<function_symbol, std::size_t> index; 
  std::unordered_map<function_symbol, std::size_t> count=count_the_number_of_unique_function_symbols_in_a_term(t);
  std::size_t nr_unique_symbols = count.size();

  std::vector<sym_write_entry> sym_entries;
  sym_entries.reserve(nr_unique_symbols);

  /* Collect all unique symbols in the input term */

  std::size_t cur=0;
  for(const pair<const function_symbol, std::size_t>& p: count)
  {
    const function_symbol sym=p.first;
    const std::size_t nr_of_occurrences=p.second;

    sym_entries.emplace_back(sym, bit_width(nr_of_occurrences));
    index[sym] = cur;

    cur++;
  }

  assert(cur == nr_unique_symbols);

  collect_terms(t, index, sym_entries);
  build_arg_tables(index, sym_entries);

  /* write header */

  writeInt(0, os);
  writeInt(BAF_MAGIC, os);
  writeInt(BAF_VERSION, os);
  writeInt(nr_unique_symbols, os);
  write_all_symbols(os, sym_entries);

  /* Write the top symbol */
  writeInt(get_top_symbol(t,index,sym_entries)-&sym_entries[0], os);

  write_term(t, index, os, sym_entries);
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
