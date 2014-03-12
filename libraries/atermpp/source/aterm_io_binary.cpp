/* includes */

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

namespace atermpp
{

using detail::readInt;
using detail::writeInt;

/**
 * Calculate the number of unique symbols.
 */

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
  assert(sizeof(size_t) == sizeof(aterm*));
  assert(sizeof(size_t) >= 4);
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

static size_t calcUniqueAFuns(
                  const aterm &t,
                  std::set<aterm> &visited,
                  std::vector<size_t> &count)
{
  size_t nr_unique = 0;

  if (visited.count(t)>0)
  {
    return 0;
  }

  if (t.type_is_int())
  {
    if (!count[detail::function_adm.AS_INT.number()]++)
    {
      nr_unique = 1;
    }
  }
  else if (t.type_is_list())
  {
    aterm_list list = aterm_cast<const aterm_list>(t);
    while (list!=aterm_list() && visited.count(list)==0)
    {
      visited.insert(list);
      if (!count[detail::function_adm.AS_LIST.number()]++)
      {
        nr_unique++;
      }
      nr_unique += calcUniqueAFuns(list.front(),visited,count);
      list = list.tail();
    }
    if (list==aterm_list() && visited.count(list)==0)
    {
      visited.insert(list);
      if (!count[detail::function_adm.AS_EMPTY_LIST.number()]++)
      {
        nr_unique++;
      }
    }
  }
  else
  {
    assert(t.type_is_appl());
    function_symbol sym = aterm_cast<aterm_appl>(t).function();
    nr_unique = count[sym.number()]>0 ? 0 : 1;
    count[sym.number()]++;
    size_t arity = sym.arity();
    for (size_t i = 0; i < arity; i++)
    {
      nr_unique += calcUniqueAFuns(aterm_cast<const aterm_appl>(t)[i],visited,count);
    }
  }

  visited.insert(t);

  return nr_unique;
}

static size_t AT_calcUniqueAFuns(const aterm &t, std::vector<size_t> &count)
{
  std::set<aterm> visited;
  size_t result = calcUniqueAFuns(t,visited,count);

  return result;
}


static const size_t BAF_MAGIC = 0xbaf;

// The BAF_VERSION constant is the version number of the ATerms written in BAF
// format. As of 29 August 2013 this version number is used by the mCRL2
// toolset. Whenever the file format of mCRL2 files is changed, the BAF_VERSION
// has to be increased.
//
// History:
//
// before 2013      : version 0x0300
// 29 August 2013   : version changed to 0x0301
// 23 November 2013 : version changed to 0x0302 (introduction of index for variable types)

static const size_t BAF_VERSION = 0x0302;

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

    size_t toptable_size;
    top_symbol** toptable;

    top_symbols_t():
      nr_symbols(0),
      toptable_size(0),
      toptable(NULL)
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

    size_t termtable_size;
    trm_bucket** termtable;

    size_t term_width;

    size_t cur_index;
    size_t nr_times_top; /* # occurences of this symbol as topsymbol */

    sym_entry* next_topsym;

    sym_entry():
      arity(0),
      nr_terms(0),
      top_symbols(0),
      termtable_size(0),
      termtable(NULL),
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
    size_t    term_width;
    std::vector<aterm> terms;
    size_t*   nr_topsyms;
    size_t*   sym_width;
    size_t**  topsyms;

    sym_read_entry():
       arity(0),
       nr_terms(0),
       term_width(0),
       nr_topsyms(NULL),
       sym_width(NULL),
       topsyms(NULL)
    {
    }

};

static size_t nr_unique_symbols = 0;
static std::vector<sym_read_entry> read_symbols;
static std::vector<sym_entry> sym_entries;
static sym_entry* first_topsym = NULL;

static char* text_buffer = NULL;
static size_t text_buffer_size = 0;

static unsigned char bit_buffer = '\0';
static size_t  bits_in_buffer = 0; /* how many bits in bit_buffer are used */

static void writeBits(size_t val, const size_t nr_bits, ostream &os)
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


static int flushBitsToWriter(ostream &os)
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
bool readBits(size_t& val, const size_t nr_bits, istream &is)
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

static void writeString(const char* str, const size_t len, ostream &os)
{
  /* Write length. */
  writeInt(len, os);

  /* Write actual string. */
  os.write(str, len);
}


static size_t readString(istream &is)
{
  size_t len;

  /* Get length of string */
  len = readInt(is);

  /* Assure buffer can hold the string */
  if (text_buffer_size < (len+1))
  {
    text_buffer_size = (len*3)/2;
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

static void write_symbol(const function_symbol sym, ostream &os)
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

static sym_entry* get_top_symbol(const aterm &t, const std::vector<size_t> &index)
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
    sym = aterm_cast<aterm_appl>(t).function();
  }
  else
  {
    throw aterm_io_error("get_top_symbol: illegal term (" + to_string(t) + ")");
  }
  return &sym_entries[index[sym.number()]];
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
  tss->toptable_size = (total_top_symbols*5)/4;
  tss->toptable = (top_symbol**) calloc(tss->toptable_size,
                  sizeof(top_symbol*));
  if (!tss->toptable)
  {
    throw aterm_io_error("build_arg_tables: out of memory (table_size: " + to_string(tss->toptable_size) + ")");
  }

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

    hnr = ts->s.number() % tss->toptable_size;
    ts->next = tss->toptable[hnr];
    tss->toptable[hnr] = ts;

    top_entry->nr_times_top = 0;
    index++;
  }
}

static void build_arg_tables(const std::vector<size_t> &index)
{
  // function_symbol cur_sym;
  size_t cur_trm;
  size_t cur_arg;
  sym_entry* topsym;

  for (size_t cur_sym=0; cur_sym<nr_unique_symbols; cur_sym++)
  {
    sym_entry* cur_entry = &sym_entries[cur_sym];
    size_t arity = cur_entry->arity;

    assert(arity == cur_entry->id.arity());

    cur_entry->top_symbols = std::vector<top_symbols_t>(arity);

    for (cur_arg=0; cur_arg<arity; cur_arg++)
    {
      size_t total_top_symbols = 0;
      first_topsym = NULL;
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
          arg = aterm_cast<const aterm_appl>(term)[cur_arg];
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


/**
  * Add a term to the termtable of a symbol.
  */
static void add_term(sym_entry* entry, const aterm &t)
{
  size_t hnr = hash_number(detail::address(t)) % entry->termtable_size;
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
    assert(i < aterm_cast<const aterm_appl>(t).function().arity());
    return atermpp::aterm_cast<const aterm_appl>(t)[i];
  }
  else
  {
    assert(t.type_is_list() && t != aterm_list());
    assert(i < 2);
    return i == 0 ? atermpp::aterm_cast<const aterm_list>(t).front()
                  : atermpp::aterm_cast<const aterm_list>(t).tail();
  }
}

typedef struct { aterm term; sym_entry* entry; size_t arg; } write_todo;

static void collect_terms(const aterm& t, const std::vector<size_t> &index)
{
  std::stack<write_todo> stack;
  std::set<aterm> visited;
  write_todo item = { t, get_top_symbol(t, index), 0 };
  stack.push(item);

  do
  {
    write_todo& current = stack.top();
    if (current.arg < current.entry->arity)
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

static void write_symbols(ostream &os)
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

static size_t find_term(sym_entry* entry, const aterm t)
{
  size_t hnr = hash_number(detail::address(t)) % entry->termtable_size;
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

static top_symbol* find_top_symbol(top_symbols_t* syms, const function_symbol sym)
{
  size_t hnr = sym.number() % syms->toptable_size;
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

static bool write_term(const aterm t, const std::vector<size_t> &index, ostream &os)
{
  std::stack<write_todo> stack;

  write_todo item = { t, get_top_symbol(t, index), 0 };
  stack.push(item);

  do
  {
    write_todo& current = stack.top();

    if (current.term.type_is_int())
    {
      /* WARNING: the following comment was taken from the original source code.
       *          I have no idea what it means. [scranen] */
      // If aterm integers are > 32 bits, then this can fail.
      writeBits(aterm_int(t).value(), INT_SIZE_IN_BAF, os);
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

/**
 * Free all space allocated by the bafio write functions.
 */

static void free_write_space()
{
  for (size_t i=0; i<nr_unique_symbols; i++)
  {
    sym_entry* entry = &sym_entries[i];

    free(entry->termtable);
    entry->termtable = NULL;

    for (size_t j=0; j<entry->arity; j++)
    {
      top_symbols_t* topsyms = &entry->top_symbols[j];
      topsyms->symbols=std::vector<top_symbol>();
      if (topsyms->toptable)
      {
        free(topsyms->toptable);
        topsyms->toptable = NULL;
      }
    }

    entry->top_symbols=std::vector<top_symbols_t>();
  }
  sym_entries=std::vector<sym_entry>();
}


static bool
write_baf(const aterm &t, ostream &os)
{
  size_t nr_unique_terms = 0;
  const size_t nr_symbols = detail::function_symbol_index_table_number_of_elements*FUNCTION_SYMBOL_BLOCK_SIZE;

  /* Initialize bit buffer */
  bit_buffer     = '\0';
  bits_in_buffer = 0; /* how many bits in bit_buffer are used */


  std::vector<size_t> count(nr_symbols,0);
  std::vector<size_t> index(nr_symbols,size_t(-1));
  nr_unique_symbols = AT_calcUniqueAFuns(t,count);

  sym_entries = std::vector<sym_entry>(nr_unique_symbols);

  /* Collect all unique symbols in the input term */

  size_t cur;
  for (size_t lcv=cur=0; lcv<nr_symbols; lcv++)
  {
    const detail::_function_symbol &entry = detail::function_symbol_index_table[lcv >> FUNCTION_SYMBOL_BLOCK_CLASS][lcv & FUNCTION_SYMBOL_BLOCK_MASK];
    if (entry.reference_count>0 && count[lcv]>0)
    {
      nr_unique_terms += count[lcv];

      sym_entries[cur].term_width = bit_width(count[lcv]);
      sym_entries[cur].id = function_symbol(lcv);
      sym_entries[cur].arity = function_symbol(lcv).arity();
      sym_entries[cur].nr_terms = count[lcv];
      sym_entries[cur].terms.resize(count[lcv]);
      sym_entries[cur].termtable_size = (count[lcv]*5)/4;
      sym_entries[cur].termtable =
        (trm_bucket**) calloc(sym_entries[cur].termtable_size,
                                 sizeof(trm_bucket*));
      if (!sym_entries[cur].termtable)
      {
        throw aterm_io_error("write_baf: out of memory (termtable_size: " + to_string(sym_entries[cur].termtable_size) + ")");
      }

      index[lcv] = cur;
      count[lcv] = 0; /* restore invariant that symbolcount is zero */

      cur++;
    }
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

  free_write_space();

  return true;
}

void write_term_to_binary_stream(const aterm &t, std::ostream &os)
{
  aterm_io_init(os);
  if (!write_baf(t, os))
  {
    throw aterm_io_error("Fail to write term to string");
  }
}

/**
  * Read a single symbol from file.
  */

static function_symbol read_symbol(istream &is)
{
  std::size_t len;
  if ((len = readString(is)) == atermpp::npos)
  {
    return function_symbol(atermpp::npos);
  }

  text_buffer[len] = '\0';

  std::size_t arity = readInt(is);
  /* std::size_t quoted = */ readInt(is);

  return function_symbol(text_buffer, arity);
}

/**
 * Read all symbols from file.
 */

static bool read_all_symbols(istream &is)
{
  size_t k, val;
  size_t i, j, arity;

  for (i=0; i<nr_unique_symbols; i++)
  {
    /* Read the actual symbol */

    function_symbol sym = read_symbol(is);
    read_symbols[i].sym = sym;
    arity = sym.arity();
    read_symbols[i].arity = arity;

    /* Read term count and allocate space */
    val = readInt(is);
    if (val == 0)
    {
      return false;
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

    if (arity == 0)
    {
      read_symbols[i].nr_topsyms = NULL;
      read_symbols[i].sym_width = NULL;
      read_symbols[i].topsyms = NULL;
    }
    else
    {
      read_symbols[i].nr_topsyms = (size_t*)calloc(arity, sizeof(size_t));
      if (!read_symbols[i].nr_topsyms)
        throw aterm_io_error("read_all_symbols: out of memory trying to allocate "
                           "space for " + to_string(arity) + " arguments.");

      read_symbols[i].sym_width = (size_t*)calloc(arity, sizeof(size_t));
      if (!read_symbols[i].sym_width)
        throw aterm_io_error("read_all_symbols: out of memory trying to allocate "
                           "space for " + to_string(arity) + " arguments.");

      read_symbols[i].topsyms = (size_t**)calloc(arity, sizeof(size_t*));
      if (!read_symbols[i].topsyms)
        throw aterm_io_error("read_all_symbols: out of memory trying to allocate "
                           "space for " + to_string(arity) + " arguments.");
    }

    for (j=0; j<read_symbols[i].arity; j++)
    {
      val = readInt(is);
      read_symbols[i].nr_topsyms[j] = val;
      read_symbols[i].sym_width[j] = bit_width(val);
      read_symbols[i].topsyms[j] = (size_t*)calloc(val, sizeof(size_t));
      if (!read_symbols[i].topsyms[j])
      {
        throw aterm_io_error("read_symbols: could not allocate space for " + to_string(val) + " top symbols.");
      }

      for (k=0; k<read_symbols[i].nr_topsyms[j]; k++)
      {
        val = readInt(is);
        read_symbols[i].topsyms[j][k] = val;
      }
    }
  }

  return true;
}

typedef struct { sym_read_entry* sym; size_t arg; std::vector<aterm> args; aterm* result; aterm* callresult; } read_todo;

static aterm read_term(sym_read_entry* sym, istream &is)
{
  aterm result;
  size_t value;
  std::stack<read_todo> stack;
  
  read_todo item = { sym, 0, std::vector<aterm>(sym->arity), &result, NULL };
  stack.push(item);

  do
  {
    sym_read_entry* arg_sym;
    read_todo& current = stack.top();

    if (current.callresult != NULL)
    {
      current.args[current.arg++] = *current.callresult;
      current.callresult = NULL;
    }
    if (current.arg < current.sym->arity)
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
      aterm_list result = atermpp::aterm_cast<aterm_list>(current.args[1]);
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
 * Free all temporary space allocated by the baf read functions.
 */

static void free_read_space()
{
  size_t i, j;

  for (i=0; i<nr_unique_symbols; i++)
  {
    sym_read_entry* entry = &read_symbols[i];

    if (entry->nr_topsyms)
    {
      free(entry->nr_topsyms);
    }
    if (entry->sym_width)
    {
      free(entry->sym_width);
    }

    for (j=0; j<entry->arity; j++)
    {
      free(entry->topsyms[j]);
    }
    if (entry->topsyms)
    {
      free(entry->topsyms);
    }

  }
  read_symbols=std::vector<sym_read_entry>(); // Release memory, and prevent read symbols to be
                                              // destructed after the destruction of function_symbols, which leads
                                              // to decreasing reference counters, after function_lookup_table has
                                              // been destroyed (i.e. core dump).
}

/**
 * Read a term from a BAF reader.
 */

static
aterm read_baf(istream &is)
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
    throw aterm_io_error("read_baf: error reading BAF_MAGIC!");
  }

  std::size_t version = readInt(is);
  if (version != BAF_VERSION)
  {
    throw baf_version_error("wrong BAF version number ", version, BAF_VERSION);
  }

  nr_unique_symbols = readInt(is);
  /* std::size_t nr_unique_terms = */ readInt(is);

  // Allocate symbol space
  read_symbols = std::vector<sym_read_entry>(nr_unique_symbols);

  if (!read_all_symbols(is))
  {
    throw aterm_io_error("read_baf: failed to read all symbols!");
  }

  val = readInt(is);
  result = read_term(&read_symbols[val], is);
  free_read_space();
  return result;
}


aterm read_term_from_binary_stream(istream &is)
{
  aterm_io_init(is);
  aterm result=read_baf(is);
  if (!result.defined())
  {
    throw aterm_io_error("Failed to read term from binary file.");
  }
  return result;
}

} // namespace atermpp
