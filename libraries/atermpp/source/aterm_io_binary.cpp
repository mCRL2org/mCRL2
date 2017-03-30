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

#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#endif

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/atermpp/detail/utility.h"
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/detail/aterm_io_implementation.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/logger.h"


/* Integers in BAF are always exactly 32 or 64 bits.  The size must be fixed so that
 *  *  * BAF terms can be exchanged between platforms. */
static const size_t INT_SIZE_IN_BAF = 64;


namespace atermpp
{

using detail::readInt;
using detail::writeInt;

using namespace std;

static void aterm_io_init(std::basic_ios<char>&
#ifdef WIN32 // This suppresses a compiler warning.
s
#endif
)
{
  /* Check for reasonably sized aterm (32 bits, 4 bytes)     */
  /* This check might break on perfectly valid architectures */
  /* that have char == 2 bytes, and sizeof(header_type) == 2 */
  static_assert(sizeof(size_t) == sizeof(aterm*), "");
  static_assert(sizeof(size_t) >= 4, "");
#ifdef WIN32
  if (s.rdbuf() == std::cout.rdbuf())
  {
    fflush(stdout);
    if (_setmode(_fileno(stdout), _O_BINARY) == -1)
    {
      mCRL2log(mcrl2::log::warning) << "Cannot set stdout to binary mode.\n";
    }
    else
    {
      mCRL2log(mcrl2::log::debug) << "Converted stdout to binary mode.\n";
    }
  }
  if (s.rdbuf() == std::cerr.rdbuf())
  {
    fflush(stderr);
    if (_setmode(_fileno(stderr), _O_BINARY) == -1)
    {
      mCRL2log(mcrl2::log::warning) << "Cannot set stderr to binary mode.\n";
    }
    else
    {
      mCRL2log(mcrl2::log::debug) << "Converted stderr to binary mode.\n";
    }
  }
#endif
}

// Count the total number of occurrences of function symbols t where 
// t is viewed as a shared term. The result is stored in the map count. 
// Visited is used to avoid visiting terms too often. 
static void count_the_number_of_unique_function_symbols_in_a_term_rec(
                  const aterm& t,
                  std::set<aterm>& visited,
                  std::map<function_symbol, size_t>& count)
{
  if (visited.count(t)>0)
  {
    return;
  }

  if (t.type_is_int())
  {
    count[detail::function_adm.AS_INT()]++;
  }
  else if (t.type_is_list())
  {
    aterm_list list = down_cast<const aterm_list>(t);
    while (list!=aterm_list() && visited.count(list)==0)
    {
      visited.insert(list);
      count[detail::function_adm.AS_LIST()]++;
      count_the_number_of_unique_function_symbols_in_a_term_rec(list.front(),visited,count);
      list = list.tail();
    }
    if (list==aterm_list() && visited.count(list)==0)
    {
      visited.insert(list);
      count[detail::function_adm.AS_EMPTY_LIST()]++;
    }
  }
  else
  {
    aterm_appl ta=down_cast<aterm_appl>(t);
    function_symbol sym = ta.function();
    count[sym]++;
    for (const aterm& arg: ta) 
    {
      count_the_number_of_unique_function_symbols_in_a_term_rec(arg,visited,count);
    }
  }

  visited.insert(t);

  return;
}

static void count_the_number_of_unique_function_symbols_in_a_term(const aterm& t, std::map<function_symbol, size_t>& count)
{
  std::set<aterm> visited;
  count_the_number_of_unique_function_symbols_in_a_term_rec(t,visited,count);
}


static const size_t BAF_MAGIC = 0xbaf;

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

static const size_t BAF_VERSION = 0x0303;

typedef struct _trm_bucket
{
  struct _trm_bucket* next;
  aterm t;
} trm_bucket;

typedef struct _top_symbol
{
  struct _top_symbol* next;
  function_symbol s;

  size_t index;
  size_t count;

  size_t code_width;
  size_t code;
} top_symbol;

class top_symbols_t
{
  public:
    size_t      nr_symbols;
    std::vector<top_symbol> symbols;

    vector<top_symbol*> toptable;

    top_symbols_t():
      nr_symbols(0)
    {}

};

class sym_entry
{
  public:
    function_symbol id;
    size_t arity;

    size_t nr_terms;
    std::vector <trm_bucket> terms;

    std::vector<top_symbols_t> top_symbols; /* top symbols occuring in this symbol */

    std::vector<trm_bucket*> termtable;  

    size_t term_width;

    size_t cur_index;
    size_t nr_times_top; /* # occurences of this symbol as topsymbol */

    sym_entry* next_topsym;

    sym_entry():
      arity(0),
      nr_terms(0),
      top_symbols(0),
      termtable(),
      term_width(0),
      cur_index(0),
      nr_times_top(0)
    {}
};

class sym_read_entry
{
  public:
    function_symbol   sym;
    size_t arity;
    size_t nr_terms;
    size_t term_width;
    std::vector<aterm> terms;
    std::vector<size_t> nr_topsyms;
    std::vector<size_t> sym_width;
    std::vector<vector<size_t> > topsyms;

    sym_read_entry():
       arity(0),
       nr_terms(0),
       term_width(0)
    {
    }

};

static size_t nr_unique_symbols = 0;
static std::vector<sym_read_entry> read_symbols;
static std::vector<sym_entry> sym_entries;
static sym_entry* first_topsym = nullptr;

static char* text_buffer = nullptr;
static size_t text_buffer_size = 0;

static unsigned char bit_buffer = '\0';
static size_t  bits_in_buffer = 0; /* how many bits in bit_buffer are used */

static void writeBits(size_t val, const size_t nr_bits, ostream& os)
{
  for (size_t cur_bit=0; cur_bit<nr_bits; cur_bit++)
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


static int flushBitsToWriter(ostream& os)
{
  int result = 0;
  if (bits_in_buffer > 0)
  {
    size_t left = 8-bits_in_buffer;
    bit_buffer <<= left;
    os.put(bit_buffer);
    result = os.fail() ? -1 : 0;
    bits_in_buffer = 0;
    bit_buffer = '\0';
  }

  return result;
}

/**
 * @brief readBits Reads an n-bit integer from the input stream.
 * @param val      Variable to store integer in.
 * @param nr_bits  Number of bits to read from the input stream.
 * @param is       The input stream.
 * @return true on success, false on failure (EOF).
 */
static
bool readBits(size_t& val, const size_t nr_bits, istream& is)
{
  size_t cur_bit, mask = 1;

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

static void writeString(const char* str, const size_t len, ostream& os)
{
  /* Write length. */
  writeInt(len, os);

  /* Write actual string. */
  os.write(str, len);
}


static size_t readString(istream& is)
{
  size_t len;

  /* Get length of string */
  len = readInt(is);

  /* Assure buffer can hold the string */
  if (text_buffer_size < (len+1))
  {
    text_buffer_size = 1+(len*3)/2;
    text_buffer = (char*) realloc(text_buffer, text_buffer_size);
    if (!text_buffer)
    {
      throw aterm_io_error("out of memory in readString (" + to_string(text_buffer_size) + ")");
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
  const char* name = sym.name().c_str();
  writeString(name, strlen(name), os);
  writeInt(sym.arity(), os);
  writeInt(true, os);
}

/**
 * Retrieve the top symbol of a term. Could be a special symbol
 * (AS_INT, etc) when the term is not an application.
 */

static sym_entry* get_top_symbol(const aterm& t, const std::map<function_symbol, size_t>& index)
{
  function_symbol sym;

  if (t.type_is_int())
  {
      sym = detail::function_adm.AS_INT();
  }
  else if (t.type_is_list())
  {
    sym = (t==aterm_list() ? detail::function_adm.AS_EMPTY_LIST() : detail::function_adm.AS_LIST());
  }
  else if (t.type_is_appl())
  {
    sym = down_cast<aterm_appl>(t).function();
  }
  else
  {
    throw aterm_io_error("get_top_symbol: illegal term (" + to_string(t) + ")");
  }
  return &sym_entries[(index.count(sym)==0?size_t(-1):index.at(sym))];
}


/* How many bits are needed to represent <val> */
static size_t bit_width(size_t val)
{
  size_t nr_bits = 0;

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

static void gather_top_symbols(sym_entry* cur_entry,
                               const size_t cur_arg,
                               const size_t total_top_symbols)
{
  size_t index;
  size_t hnr;
  top_symbols_t* tss;
  sym_entry* top_entry;

  tss = &cur_entry->top_symbols[cur_arg];
  tss->nr_symbols = total_top_symbols;
  tss->symbols = std::vector<top_symbol>(total_top_symbols);
  tss->toptable = std::vector<top_symbol*>((total_top_symbols*5)/4); 

  index = 0;
  for (top_entry=first_topsym; top_entry; top_entry=top_entry->next_topsym)
  {
    top_symbol* ts;
    ts = &cur_entry->top_symbols[cur_arg].symbols[index];
    ts->index = top_entry-&sym_entries[0];
    ts->count = top_entry->nr_times_top;
    ts->code_width = bit_width(total_top_symbols);
    ts->code = index;
    ts->s = top_entry->id;

    hnr = detail::addressf(ts->s) % tss->toptable.size();
    ts->next = tss->toptable[hnr];
    tss->toptable[hnr] = ts;

    top_entry->nr_times_top = 0;
    index++;
  }
}

static void build_arg_tables(const std::map<function_symbol, size_t>& index)
{
  size_t cur_trm;
  size_t cur_arg;
  sym_entry* topsym;

  for (size_t cur_sym=0; cur_sym<nr_unique_symbols; cur_sym++)
  {
    sym_entry* cur_entry = &sym_entries[cur_sym];
    size_t arity = cur_entry->arity;

    assert(arity == cur_entry->id.arity());

    cur_entry->top_symbols = std::vector<top_symbols_t>(arity);

    if (cur_entry->id!=detail::function_adm.AS_INT())
    {
      for (cur_arg=0; cur_arg<arity; cur_arg++)
      {
        size_t total_top_symbols = 0;
        first_topsym = nullptr;
        for (cur_trm=0; cur_trm<cur_entry->nr_terms; cur_trm++)
        {
          aterm term = cur_entry->terms[cur_trm].t;
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
              arg = (aterm)(list.tail());
            }
          }
          else if (term.type_is_appl())
          {
            arg = down_cast<const aterm_appl>(term)[cur_arg];
          }
          else
          {
            throw aterm_io_error("build_arg_tables: illegal term");
          }
          topsym = get_top_symbol(arg,index);
          if (!topsym->nr_times_top++)
          {
            total_top_symbols++;
            topsym->next_topsym = first_topsym;
            first_topsym = topsym;
          }
        }

        gather_top_symbols(cur_entry, cur_arg, total_top_symbols);
      }
    }
  }
}


/**
  * Add a term to the termtable of a symbol.
  */
static void add_term(sym_entry* entry, const aterm& t)
{
  size_t hnr = hash_number(detail::address(t)) % entry->termtable.size();
  entry->terms[entry->cur_index].t = t;
  entry->terms[entry->cur_index].next = entry->termtable[hnr];
  entry->termtable[hnr] = &entry->terms[entry->cur_index];
  entry->cur_index++;
}

/**
 * Collect all terms in the appropriate symbol table.
 */

static const aterm& subterm(const aterm& t, size_t i)
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

typedef struct { aterm term; sym_entry* entry; size_t arg; } write_todo;

static void collect_terms(const aterm& t, const std::map<function_symbol, size_t>& index)
{
  std::stack<write_todo> stack;
  std::set<aterm> visited;
  write_todo item = { t, get_top_symbol(t, index), 0 };
  stack.push(item);

  do
  {
    write_todo& current = stack.top();
    if (current.term.type_is_int())
    {
      add_term(current.entry, current.term);
      visited.insert(current.term);
      stack.pop();
    }
    else if (current.arg < current.entry->arity)
    {
      item.term = subterm(current.term, current.arg++);
      if (visited.count(item.term) == 0)
      {
        item.entry = get_top_symbol(item.term, index);
        stack.push(item);
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

static void write_symbols(ostream& os)
{
  for (size_t sym_idx=0; sym_idx<nr_unique_symbols; sym_idx++)
  {
    sym_entry* cur_sym = &sym_entries[sym_idx];
    write_symbol(cur_sym->id, os);
    writeInt(cur_sym->nr_terms, os);

    for (size_t arg_idx=0; arg_idx<cur_sym->arity; arg_idx++)
    {
      size_t nr_symbols = cur_sym->top_symbols[arg_idx].nr_symbols;
      writeInt(nr_symbols, os);
      for (size_t top_idx=0; top_idx<nr_symbols; top_idx++)
      {
        top_symbol* ts = &cur_sym->top_symbols[arg_idx].symbols[top_idx];
        writeInt(ts->index, os);
      }
    }
  }
}


/**
  * Find a term in a sym_entry.
  */

static size_t find_term(sym_entry* entry, const aterm& t)
{
  size_t hnr = hash_number(detail::address(t)) % entry->termtable.size();
  trm_bucket* cur = entry->termtable[hnr];

  assert(cur);
  while (cur->t != t)
  {
    cur = cur->next;
    assert(cur);
  }

  return cur - &entry->terms[0];
}

/**
 * Find a top symbol in a topsymbol table.
 */

static top_symbol* find_top_symbol(top_symbols_t* syms, const function_symbol& sym)
{
  size_t hnr = detail::addressf(sym) % syms->toptable.size();
  top_symbol* cur = syms->toptable[hnr];

  assert(cur);
  while (cur->s != sym)
  {
    cur = cur->next;
    assert(cur);
  }

  return cur;
}

/**
 * Write a term using a writer.
 */

static bool write_term(const aterm& t, const std::map<function_symbol, size_t>& index, ostream& os)
{
  std::stack<write_todo> stack;

  write_todo item = { t, get_top_symbol(t, index), 0 };
  stack.push(item);

  do
  {
    write_todo& current = stack.top();

    if (current.term.type_is_int())
    {
      // If aterm integers are > 32 bits, then they cannot be read on a 32 bit machine.
      writeBits(aterm_int(current.term).value(), INT_SIZE_IN_BAF, os);
    }
    else
    if (current.arg < current.entry->arity)
    {
      item.term = subterm(current.term, current.arg);
      item.entry = get_top_symbol(item.term, index);

      top_symbol* ts = find_top_symbol(&current.entry->top_symbols[current.arg], item.entry->id);
      writeBits(ts->code, ts->code_width, os);
      sym_entry* arg_sym = &sym_entries[ts->index];
      size_t arg_trm_idx = find_term(arg_sym, item.term);
      writeBits(arg_trm_idx, arg_sym->term_width, os);

      ++current.arg;

      if (arg_trm_idx >= arg_sym->cur_index)
      {
        stack.push(item);
      }
      continue;
    }

    ++current.entry->cur_index;
    stack.pop();
  }
  while (!stack.empty());
  return true;
}


static bool write_baf(const aterm& t, ostream& os)
{
  size_t nr_unique_terms = 0;

  /* Initialize bit buffer */
  bit_buffer     = '\0';
  bits_in_buffer = 0; /* how many bits in bit_buffer are used */


  std::map<function_symbol, size_t> count; /* (nr_symbols,0); */
  std::map<function_symbol, size_t> index; /* (nr_symbols,size_t(-1)); */
  count_the_number_of_unique_function_symbols_in_a_term(t,count);
  nr_unique_symbols = count.size();

  sym_entries = std::vector<sym_entry>(nr_unique_symbols);

  /* Collect all unique symbols in the input term */

  size_t cur=0;
  // for (size_t lcv=cur=0; lcv<nr_symbols; lcv++)
  // for(detail::_function_symbol& entry: function_symbol::function_symbol_store())
  for(const pair<const function_symbol, size_t>& p: count)
  {
    const function_symbol sym=p.first;
    const size_t nr_of_occurrences=p.second;
    /* if (count[sym]>0)
    { */
      nr_unique_terms += nr_of_occurrences;

      sym_entries[cur].term_width = bit_width(nr_of_occurrences);
      sym_entries[cur].id = sym;
      sym_entries[cur].arity = sym.arity();
      sym_entries[cur].nr_terms = nr_of_occurrences;
      sym_entries[cur].terms.resize(nr_of_occurrences);
      sym_entries[cur].termtable =std::vector<trm_bucket*>((nr_of_occurrences*5)/4);
      index[sym] = cur;
      // count[sym] = 0; /* restore invariant that symbolcount is zero */

      cur++;
    // }
  }

  assert(cur == nr_unique_symbols);

  collect_terms(t, index);

  /* reset cur_index */
  for (size_t lcv=0; lcv < nr_unique_symbols; lcv++)
  {
    sym_entries[lcv].cur_index = 0;
  }

  build_arg_tables(index);

  /* write header */

  writeInt(0, os);
  writeInt(BAF_MAGIC, os);
  writeInt(BAF_VERSION, os);
  writeInt(nr_unique_symbols, os);
  writeInt(nr_unique_terms, os);
  write_symbols(os);

  /* Write the top symbol */
  writeInt(get_top_symbol(t,index)-&sym_entries[0], os);

  if (!write_term(t, index, os))
  {
    return false;
  }

  if (flushBitsToWriter(os)<0)
  {
    return false;
  }

  sym_entries.clear();

  return true;
}

void write_term_to_binary_stream(const aterm& t, std::ostream& os)
{
  aterm_io_init(os);
  if (!write_baf(t, os))
  {
    throw aterm_io_error("Fail to write term to the output file.");
  }
}

/**
  * Read a single symbol from file.
  */

static function_symbol read_symbol(istream& is)
{
  std::size_t len=readString(is);

  text_buffer[len] = '\0';

  std::size_t arity = readInt(is);
  /* std::size_t quoted = */ readInt(is);

  return function_symbol(text_buffer, arity);
}

/**
 * Read all symbols from file.
 */

static void read_all_symbols(istream& is)
{
  size_t val;

  for (size_t i=0; i<nr_unique_symbols; i++)
  {
    /* Read the actual symbol */

    function_symbol sym = read_symbol(is);
    read_symbols[i].sym = sym;
    read_symbols[i].arity = sym.arity();

    /* Read term count and allocate space */
    val = readInt(is);
    if (val == 0)
    {
      throw aterm_io_error("Read file: internal file error: failed to read all symbols.");
    }
    read_symbols[i].nr_terms = val;
    read_symbols[i].term_width = bit_width(val);
    if (val == 0)
    {
      assert(0);
    }
    else
    {
      read_symbols[i].terms = std::vector<aterm>(val);
    }

    /*  Allocate space for topsymbol information */
    read_symbols[i].nr_topsyms = std::vector<size_t>(sym.arity()); 
    read_symbols[i].sym_width = std::vector<size_t>(sym.arity()); 
    read_symbols[i].topsyms = std::vector< vector <size_t> > (sym.arity()); 

    for (size_t j=0; j<sym.arity(); j++)
    {
      val = readInt(is);
      read_symbols[i].nr_topsyms[j] = val;
      read_symbols[i].sym_width[j] = bit_width(val);
      read_symbols[i].topsyms[j] = std::vector<size_t>(val); 

      for (size_t k=0; k<read_symbols[i].nr_topsyms[j]; k++)
      {
        read_symbols[i].topsyms[j][k] = readInt(is);
      }
    }
  }

  return;
}

typedef struct { sym_read_entry* sym; size_t arg; std::vector<aterm> args; aterm* result; aterm* callresult; } read_todo;

static aterm read_term(sym_read_entry* sym, istream& is)
{
  aterm result;
  size_t value;
  std::stack<read_todo> stack;

  read_todo item = { sym, 0, std::vector<aterm>(sym->arity), &result, nullptr };
  stack.push(item);

  do
  {
    sym_read_entry* arg_sym;
    read_todo& current = stack.top();

    if (current.callresult != nullptr)
    {
      current.args[current.arg++] = *current.callresult;
      current.callresult = nullptr;
    }
    // AS_INT is registered as having 1 argument, but that needs to be retrieved in a special way.
    if (current.sym->sym != detail::function_adm.AS_INT() && current.arg < current.sym->arity)
    {
      if (readBits(value, current.sym->sym_width[current.arg], is) &&
          value < current.sym->nr_topsyms[current.arg])
      {
        arg_sym = &read_symbols[current.sym->topsyms[current.arg][value]];
        if (readBits(value, arg_sym->term_width, is) &&
            value < arg_sym->nr_terms)
        {
          current.callresult = &arg_sym->terms[value];
          if (!current.callresult->defined())
          {
            item.sym = arg_sym;
            item.args = std::vector<aterm>(arg_sym->arity);
            item.result = &(*current.callresult);
            stack.push(item);
          }
          continue;
        }
      }
      throw mcrl2::runtime_error("Could not read valid aterm from stream.");
    }

    if (current.sym->sym == detail::function_adm.AS_INT())
    {
      if (readBits(value, INT_SIZE_IN_BAF, is))
      {
        *current.result = aterm_int(value);
      }
    }
    else if (current.sym->sym==detail::function_adm.AS_EMPTY_LIST())
    {
      *current.result = aterm_list();
    }
    else if (current.sym->sym == detail::function_adm.AS_LIST())
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
  size_t val;
  aterm result;

  // Initialize bit buffer
  bit_buffer     = '\0';
  bits_in_buffer = 0; // how many bits in bit_buffer are used

  // Read header
  val = readInt(is);
  if (val == 0)
  {
    val = readInt(is);
  }
  if (val != BAF_MAGIC)
  {
    throw aterm_io_error("Error while reading file: The file is not correct as it does not have the BAF_MAGIC control sequence at the right place.");
  }

  std::size_t version = readInt(is);
  if (version != BAF_VERSION)
  {
    throw baf_version_error(version, BAF_VERSION);
  }

  nr_unique_symbols = readInt(is);
  /* std::size_t nr_unique_terms = */ readInt(is);

  // Allocate symbol space
  read_symbols = std::vector<sym_read_entry>(nr_unique_symbols);

  read_all_symbols(is);

  val = readInt(is);
  result = read_term(&read_symbols[val], is);
  read_symbols=std::vector<sym_read_entry>(); // Release memory, and prevent read symbols to be
                                              // destructed after the destruction of function_symbols, which leads
                                              // to decreasing reference counters, after function_lookup_table has
                                              // been destroyed (i.e. core dump).
  return result;
}


aterm read_term_from_binary_stream(istream& is)
{
  aterm_io_init(is);
  aterm result=read_baf(is);
  if (!result.defined())
  {
    throw aterm_io_error("Failed to read a term from the input. The file is not a proper binary file.");
  }
  return result;
}

} // namespace atermpp
