/*{{{  includes */

#include <cstdio>
#include <stdlib.h>
#include <assert.h>
#include <stdexcept>

#ifdef WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include "mcrl2/utilities/logger.h"
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/atermpp/detail/utility.h"
#include "mcrl2/atermpp/aterm_int.h"

namespace atermpp
{

static int write_byte(const int byte, std::ostream &os)
{
  os.put(byte);
  return byte;
}

static size_t write_bytes(const char* buf, const size_t count, std::ostream &os)
{
  os.write(buf,count);
  return count;
}

static int read_byte(std::istream &is)
{
  int c = is.get();
  return c;
}

static size_t read_bytes(char* buf, size_t count, std::istream &is)
{
  is.read(buf, count);
  return count;
}


/**
 * Calculate the number of unique symbols.
 */

using namespace std;

static size_t calcUniqueAFuns(
                  const aterm &t, 
                  std::set<aterm> &visited, 
                  std::vector<size_t> &count)
{
  size_t nr_unique = 0;
  size_t  i, arity;
  aterm_list list;

  if (visited.count(t)>0)
  {
    return 0;
  }

  switch (t.type())
  {
    case AT_INT:
      if (!count[detail::function_adm.AS_INT.number()]++)
      {
        nr_unique = 1;
      }
      break;

    case AT_APPL:
    {
      function_symbol sym = aterm_cast<aterm_appl>(t).function(); 
      assert(detail::function_lookup_table_size>sym.number());
      nr_unique = count[sym.number()]>0 ? 0 : 1;
      count[sym.number()]++;
      arity = sym.arity();
      for (i = 0; i < arity; i++)
      {
        nr_unique += calcUniqueAFuns(static_cast<aterm_appl>(t)(i),visited,count);
      }
      break;
    }
    case AT_LIST:
      list = static_cast<aterm_list>(t);
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
      break;
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
static const size_t BAF_VERSION = 0x0300;      /* version 3.0 */

static const size_t BAF_DEFAULT_TABLE_SIZE = 1024;

static const size_t BAF_LIST_BATCH_SIZE = 64;

static const size_t SYMBOL_OFFSET = 10;


/* Maximum # of arguments to reserve space for on the stack in read_term */
static const size_t MAX_STACK_ARGS = 4;

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

char bafio_id[] = "$Id$";

static size_t nr_unique_symbols = 0;
static std::vector<sym_read_entry> read_symbols;
static std::vector<sym_entry> sym_entries;
static sym_entry* first_topsym = NULL;

static char* text_buffer = NULL;
static size_t text_buffer_size = 0;

static unsigned char bit_buffer = '\0';
static size_t  bits_in_buffer = 0; /* how many bits in bit_buffer are used */



static
size_t
writeIntToBuf(const size_t val, unsigned char* buf)
{
  if (val < (1 << 7))
  {
    buf[0] = (unsigned char) val;
    return 1;
  }

  if (val < (1 << 14))
  {
    buf[0] = (unsigned char)((val >>  8) | 0x80);
    buf[1] = (unsigned char)((val >>  0) & 0xff);
    return 2;
  }

  if (val < (1 << 21))
  {
    buf[0] = (unsigned char)((val >> 16) | 0xc0);
    buf[1] = (unsigned char)((val >>  8) & 0xff);
    buf[2] = (unsigned char)((val >>  0) & 0xff);
    return 3;
  }

  if (val < (1 << 28))
  {
    buf[0] = (unsigned char)((val >> 24) | 0xe0);
    buf[1] = (unsigned char)((val >> 16) & 0xff);
    buf[2] = (unsigned char)((val >>  8) & 0xff);
    buf[3] = (unsigned char)((val >>  0) & 0xff);
    return 4;
  }

  if (sizeof(size_t)>4 && val>((size_t)1<<4*sizeof(size_t)))
  {
    mCRL2log(mcrl2::log::warning) << "losing precision of integers when writing to .baf file" << std::endl;
  }

  buf[0] = 0xf0;
  buf[1] = (unsigned char)((val >> 24) & 0xff);
  buf[2] = (unsigned char)((val >> 16) & 0xff);
  buf[3] = (unsigned char)((val >>  8) & 0xff);
  buf[4] = (unsigned char)((val >>  0) & 0xff);
  return 5;
}


static int writeBits(size_t val, const size_t nr_bits, ostream &os)
{
  size_t cur_bit;

  for (cur_bit=0; cur_bit<nr_bits; cur_bit++)
  {
    bit_buffer <<= 1;
    bit_buffer |= (val & 0x01);
    val >>= 1;
    if (++bits_in_buffer == 8)
    {
      if (write_byte((int)bit_buffer, os) == -1)
      {
        return -1;
      }
      bits_in_buffer = 0;
      bit_buffer = '\0';
    }
  }

  if (val)
  {
    return -1;
  }

  /* Ok */
  return 0;
}


static int flushBitsToWriter(ostream &os)
{
  int result = 0;
  if (bits_in_buffer > 0)
  {
    size_t left = 8-bits_in_buffer;
    bit_buffer <<= left;
    result = (write_byte((int)bit_buffer, os) == EOF) ? -1 : 0;
    bits_in_buffer = 0;
    bit_buffer = '\0';
  }

  return result;
}


static
int
readBits(size_t* val, const size_t nr_bits, istream &is)
{
  size_t cur_bit, mask = 1;

  *val = 0;
  for (cur_bit=0; cur_bit<nr_bits; cur_bit++)
  {
    if (bits_in_buffer == 0)
    {
      int val = read_byte(is);
      if (val == EOF)
      {
        return -1;
      }
      bit_buffer = (char) val;
      bits_in_buffer = 8;
    }
    *val |= (bit_buffer & 0x80 ? mask : 0);
    mask <<= 1;
    bit_buffer <<= 1;
    bits_in_buffer--;
  }

  /* Ok */
  return 0;
}


static bool writeInt(const size_t val, ostream &os)
{
  size_t nr_items;
  unsigned char buf[8];

  nr_items = writeIntToBuf(val, buf);
  if (write_bytes((char*)buf, nr_items, os) != nr_items)
  {
    return false;
  }

  /* Ok */
  return true;
}


static int readInt(size_t* val, istream &is)
{
  int buf[8];

  /* Try to read 1st character */
  if ((buf[0] = read_byte(is)) == EOF)
  {
    return EOF;
  }

  /* Check if 1st character is enough */
  if ((buf[0] & 0x80) == 0)
  {
    *val = buf[0];
    return 1;
  }

  /* Try to read 2nd character */
  if ((buf[1] = read_byte(is)) == EOF)
  {
    return EOF;
  }

  /* Check if 2nd character is enough */
  if ((buf[0] & 0x40) == 0)
  {
    *val = buf[1] + ((buf[0] & ~0xc0) << 8);
    return 2;
  }

  /* Try to read 3rd character */
  if ((buf[2] = read_byte(is)) == EOF)
  {
    return EOF;
  }

  /* Check if 3rd character is enough */
  if ((buf[0] & 0x20) == 0)
  {
    *val = buf[2] + (buf[1] << 8) + ((buf[0] & ~0xe0) << 16);
    return 3;
  }

  /* Try to read 4th character */
  if ((buf[3] = read_byte(is)) == EOF)
  {
    return EOF;
  }

  /* Check if 4th character is enough */
  if ((buf[0] & 0x10) == 0)
  {
    *val = buf[3] + (buf[2] << 8) + (buf[1] << 16) +
           ((buf[0] & ~0xf0) << 24);
    return 4;
  }

  /* Try to read 5th character */
  if ((buf[4] = read_byte(is)) == EOF)
  {
    return EOF;
  }

  /* Now 5th character should be enough */
  *val = buf[4] + (buf[3] << 8) + (buf[2] << 16) + (buf[1] << 24);
  return 5;
}


static bool writeString(const char* str, const size_t len, ostream &os)
{
  /* Write length. */
  if (!writeInt(len, os))
  {
    return false;
  }

  /* Write actual string. */
  if (write_bytes(str, len, os) != len)
  {
    return false;
  }

  /* Ok */
  return true;
}


static size_t readString(istream &is)
{
  size_t len;

  /* Get length of string */
  if (readInt(&len, is) < 0)
  {
    return atermpp::npos;
  }

  /* Assure buffer can hold the string */
  if (text_buffer_size < (len+1))
  {
    text_buffer_size = (len*3)/2;
    text_buffer = (char*) realloc(text_buffer, text_buffer_size);
    if (!text_buffer)
    {
      throw std::runtime_error("out of memory in readString (" + to_string(text_buffer_size) + ")");
    }
  }

  /* Read the actual string */
  if (read_bytes(text_buffer, len, is) != len)
  {
    return atermpp::npos;
  }

  /* Ok, return length of string */
  return len;
}

/**
 * Write a symbol to file.
 */

static bool write_symbol(const function_symbol sym, ostream &os)
{
  const char* name = sym.name().c_str();
  if (!writeString(name, strlen(name), os))
  {
    return false;
  }

  if (!writeInt(sym.arity(), os))
  {
    return false;
  }

  if (!writeInt(true, os))
  {
    return false;
  }

  return true;
}

/*{{{  static sym_entry *get_top_symbol(aterm t,index) */

/**
 * Retrieve the top symbol of a term. Could be a special symbol
 * (AS_INT, etc) when the term is not an AT_APPL.
 */

static sym_entry* get_top_symbol(const aterm &t, const std::vector<size_t> &index)
{
  function_symbol sym;

  switch (t.type())
  {
    case AT_INT:
      sym = detail::function_adm.AS_INT;
      break;
    case AT_LIST:
      sym = (t==aterm_list() ? detail::function_adm.AS_EMPTY_LIST : detail::function_adm.AS_LIST);
      break;
    case AT_APPL:
      sym = aterm_cast<aterm_appl>(t).function();
      break;
    default:
      throw std::runtime_error("get_top_symbol: illegal term (" + t.to_string() + ")");
      // sym = (function_symbol)-1; // error path...
      // break;
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
    throw std::runtime_error("build_arg_tables: out of memory (table_size: " + to_string(tss->toptable_size) + ")");
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

    /* if (arity == 0)
    {
      cur_entry->top_symbols = NULL;
    }
    else */
    {
      cur_entry->top_symbols = std::vector<top_symbols_t>(arity);
    }

    for (cur_arg=0; cur_arg<arity; cur_arg++)
    {
      size_t total_top_symbols = 0;
      first_topsym = NULL;
      for (cur_trm=0; cur_trm<cur_entry->nr_terms; cur_trm++)
      {
        aterm term = cur_entry->terms[cur_trm].t;
        aterm arg;
        switch (term.type())
        {
          case AT_LIST:
          {
            aterm_list list = static_cast<aterm_list>(term);
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
          break;
          case AT_APPL:
            arg = static_cast<aterm_appl>(term)(cur_arg);
            break;
          default:
            throw std::runtime_error("build_arg_tables: illegal term");
            break;
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
  size_t hnr = hash_number(t.address()) % entry->termtable_size;
  entry->terms[entry->cur_index].t = t;
  entry->terms[entry->cur_index].next = entry->termtable[hnr];
  entry->termtable[hnr] = &entry->terms[entry->cur_index];
  entry->cur_index++;
}

/**
 * Collect all terms in the appropriate symbol table.
 */

static void collect_terms(const aterm &t, std::set<aterm> &visited, const std::vector<size_t> &index)
{
  function_symbol sym;
  sym_entry* entry;

  if (visited.count(t)==0)
  {
    switch (t.type())
    {
      case AT_INT:
        sym = detail::function_adm.AS_INT;
        break;
      case AT_LIST:
      {
        aterm_list list(t);
        if (list==aterm_list())
        {
          sym = detail::function_adm.AS_EMPTY_LIST;
        }
        else
        {
          sym = detail::function_adm.AS_LIST;
          collect_terms(list.front(),visited,index);
          collect_terms((aterm)(list.tail()),visited,index);
        }
      }
      break;
      case AT_APPL:
      {
        aterm_appl appl(t);

        sym = appl.function();
        const size_t cur_arity = sym.arity();
        for (size_t cur_arg=0; cur_arg<cur_arity; cur_arg++)
        {
          collect_terms(appl(cur_arg),visited,index);
        }
      }
      break;
      default:
        throw std::runtime_error("collect_terms: illegal term");
        // sym = (function_symbol)(-1); // Kill compiler warnings
        // break;
    }
    entry = &sym_entries[index[sym.number()]];

    assert(entry->id == sym);
    add_term(entry, t);

    visited.insert(t);
  }
}


/**
 * Write all symbols in a term to file.
 */

static bool write_symbols(ostream &os)
{
  size_t sym_idx, arg_idx, top_idx;

  for (sym_idx=0; sym_idx<nr_unique_symbols; sym_idx++)
  {
    sym_entry* cur_sym = &sym_entries[sym_idx];
    if (!write_symbol(cur_sym->id, os))
    {
      return false;
    }
    if (!writeInt(cur_sym->nr_terms, os))
    {
      return false;
    }

    for (arg_idx=0; arg_idx<cur_sym->arity; arg_idx++)
    {
      size_t nr_symbols = cur_sym->top_symbols[arg_idx].nr_symbols;
      if (!writeInt(nr_symbols, os))
      {
        return false;
      }
      for (top_idx=0; top_idx<nr_symbols; top_idx++)
      {
        top_symbol* ts = &cur_sym->top_symbols[arg_idx].symbols[top_idx];
        if (!writeInt(ts->index, os))
        {
          return false;
        }
      }
    }
  }

  return true;
}


/**
  * Find a term in a sym_entry.
  */

static size_t find_term(sym_entry* entry, const aterm t)
{
  size_t hnr = hash_number(t.address()) % entry->termtable_size;
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
 * Write an argument.
 */

/* forward declaration */
static bool write_term(const aterm, const std::vector<size_t> &index, ostream &os);

static bool write_arg(sym_entry* trm_sym, const aterm arg, const size_t arg_idx,
                      const std::vector<size_t> &index, ostream &os)
{
  top_symbol* ts;
  sym_entry* arg_sym;
  size_t arg_trm_idx;
  function_symbol sym;

  sym = get_top_symbol(arg,index)->id;
  ts = find_top_symbol(&trm_sym->top_symbols[arg_idx], sym);

  if (writeBits(ts->code, ts->code_width, os)<0)
  {
    return false;
  }

  arg_sym = &sym_entries[ts->index];

  arg_trm_idx = find_term(arg_sym, arg);
  if (writeBits(arg_trm_idx, arg_sym->term_width, os)<0)
  {
    return false;
  }

  if (arg_trm_idx >= arg_sym->cur_index &&
      !write_term(arg, index, os))
  {
    return false;
  }

  return true;
}


/**
 * Write a term using a writer.
 */

static bool write_term(const aterm t, const std::vector<size_t> &index, ostream &os)
{
  size_t arg_idx;
  sym_entry* trm_sym = NULL;
  {
    switch (t.type())
    {
      case AT_INT:
        /* If aterm integers are > 32 bits, then this can fail. */
        if (writeBits(aterm_int(t).value(), INT_SIZE_IN_BAF, os) < 0)
        {
          return false;
        }
        trm_sym = &sym_entries[index[detail::function_adm.AS_INT.number()]];
        break;
      case AT_LIST:
      {
        aterm_list list (t);
        if (list==aterm_list())
        {
          trm_sym = &sym_entries[index[detail::function_adm.AS_EMPTY_LIST.number()]];
        }
        else
        {
          trm_sym = &sym_entries[index[detail::function_adm.AS_LIST.number()]];
          if (!write_arg(trm_sym, list.front(), 0, index, os))
          {
            return false;
          }
          if (!write_arg(trm_sym, (aterm)(list.tail()), 1, index, os))
          {
            return false;
          }
        }
      }
      break;
      case AT_APPL:
      {
        size_t arity;
        function_symbol sym = aterm_cast<aterm_appl>(t).function();
        trm_sym = &sym_entries[index[sym.number()]];
        assert(sym == trm_sym->id);
        arity = sym.arity();
        for (arg_idx=0; arg_idx<arity; arg_idx++)
        {
          aterm cur_arg = static_cast<aterm_appl>(t)(arg_idx);
          if (!write_arg(trm_sym, cur_arg, arg_idx, index, os))
          {
            return false;
          }
        }
      }
      break;
      default:
        throw std::runtime_error("write_term: illegal term");
        break;
    }
  }
  if (trm_sym->terms[trm_sym->cur_index].t != t)
  {
    throw std::runtime_error("terms out of sync at pos " + to_string(trm_sym->cur_index) + " of sym " + trm_sym->id.name() +
                             ", term in table was " + (trm_sym->terms[trm_sym->cur_index].t).to_string() + ", expected " + t.to_string());
  }
  trm_sym->cur_index++;

  return true;
}

/**
 * Free all space allocated by the bafio write functions.
 */

static void free_write_space()
{
  size_t i, j;

  for (i=0; i<nr_unique_symbols; i++)
  {
    sym_entry* entry = &sym_entries[i];

    // free(entry->terms);
    // entry->terms = NULL;
    free(entry->termtable);
    entry->termtable = NULL;

    for (j=0; j<entry->arity; j++)
    {
      top_symbols_t* topsyms = &entry->top_symbols[j];
      /* if (topsyms->symbols)
      {
        free(topsyms->symbols);
        topsyms->symbols = NULL;
      } */
      topsyms->symbols=std::vector<top_symbol>();
      if (topsyms->toptable)
      {
        free(topsyms->toptable);
        topsyms->toptable = NULL;
      }
      /*free(topsyms);*/
    }

    /* if (entry->top_symbols)
    {
      free(entry->top_symbols);
      entry->top_symbols = NULL;
    } */
    entry->top_symbols=std::vector<top_symbols_t>();
  }
  sym_entries=std::vector<sym_entry>();

  // sym_entries = NULL;
}


static bool
write_baf(const aterm &t, ostream &os)
{
  size_t nr_unique_terms = 0;
  size_t nr_symbols = detail::function_lookup_table_size;
  size_t cur;

  /* Initialize bit buffer */
  bit_buffer     = '\0';
  bits_in_buffer = 0; /* how many bits in bit_buffer are used */


  std::vector<size_t> count(detail::function_lookup_table_size,0);
  std::vector<size_t> index(detail::function_lookup_table_size,size_t(-1));
  nr_unique_symbols = AT_calcUniqueAFuns(t,count);

  sym_entries = std::vector<sym_entry>(nr_unique_symbols);

  /*{{{  Collect all unique symbols in the input term */

  for (size_t lcv=cur=0; lcv<nr_symbols; lcv++)
  {
    const detail::_function_symbol &entry = detail::function_lookup_table[lcv];
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
        throw std::runtime_error("write_baf: out of memory (termtable_size: " + to_string(sym_entries[cur].termtable_size) + ")");
      }

      // entry->index = cur;
      index[lcv] = cur;
      count[lcv] = 0; /* restore invariant that symbolcount is zero */

      cur++;
    }
  }

  assert(cur == nr_unique_symbols);


  std::set<aterm> visited;
  collect_terms(t,visited,index);
  // AT_unmarkIfAllMarked(t);

  /* reset cur_index */
  for (size_t lcv=0; lcv < nr_unique_symbols; lcv++)
  {
    sym_entries[lcv].cur_index = 0;
  }

  build_arg_tables(index);
  /*print_sym_entries();*/

  /*{{{  write header */

  if (!writeInt(0, os))
  {
    return false;
  }

  if (!writeInt(BAF_MAGIC, os))
  {
    return false;
  }

  if (!writeInt(BAF_VERSION, os))
  {
    return false;
  }

  if (!writeInt(nr_unique_symbols, os))
  {
    return false;
  }

  if (!writeInt(nr_unique_terms, os))
  {
    return false;
  }

  /*}}}  */

  if (!write_symbols(os))
  {
    return false;
  }

  /* Write the top symbol */
  if (!writeInt(get_top_symbol(t,index)-&sym_entries[0], os))
  {
    return false;
  }

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

  if (!write_baf(t, os))
  {
    throw std::runtime_error("Fail to write term to string");
  }
}

/**
  * Read a single symbol from file.
  */

static function_symbol read_symbol(istream &is)
{
  size_t arity, quoted;
  size_t len;

  if ((len = readString(is)) == atermpp::npos)
  {
    return function_symbol(atermpp::npos);
  }

  text_buffer[len] = '\0';

  if (readInt(&arity, is) < 0)
  {
    return function_symbol(atermpp::npos);
  }

  if (readInt(&quoted, is) < 0)
  {
    return function_symbol(atermpp::npos);
  }

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
    /*{{{  Read the actual symbol */

    function_symbol sym = read_symbol(is);
    read_symbols[i].sym = sym;
    arity = sym.arity();
    read_symbols[i].arity = arity;

    /*}}}  */
    /*{{{  Read term count and allocate space */

    if (readInt(&val, is) < 0 || val == 0)
    {
      return false;
    }
    read_symbols[i].nr_terms = val;
    read_symbols[i].term_width = bit_width(val);
    if (val == 0)
    {
      assert(0);
      // read_symbols[i].terms = NULL;
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
        throw std::runtime_error("read_all_symbols: out of memory trying to allocate "
                           "space for " + to_string(arity) + " arguments.");

      read_symbols[i].sym_width = (size_t*)calloc(arity, sizeof(size_t));
      if (!read_symbols[i].sym_width)
        throw std::runtime_error("read_all_symbols: out of memory trying to allocate "
                           "space for " + to_string(arity) + " arguments.");

      read_symbols[i].topsyms = (size_t**)calloc(arity, sizeof(size_t*));
      if (!read_symbols[i].topsyms)
        throw std::runtime_error("read_all_symbols: out of memory trying to allocate "
                           "space for " + to_string(arity) + " arguments.");
    }

    /*}}}  */

    for (j=0; j<read_symbols[i].arity; j++)
    {
      if (readInt(&val, is) < 0)
      {
        return false;
      }

      read_symbols[i].nr_topsyms[j] = val;
      read_symbols[i].sym_width[j] = bit_width(val);
      read_symbols[i].topsyms[j] = (size_t*)calloc(val, sizeof(size_t));
      if (!read_symbols[i].topsyms[j])
      {
        throw std::runtime_error("read_symbols: could not allocate space for " + to_string(val) + " top symbols.");
      }

      for (k=0; k<read_symbols[i].nr_topsyms[j]; k++)
      {
        if (readInt(&val, is) < 0)
        {
          return false;
        }
        read_symbols[i].topsyms[j][k] = val;
      }
    }

  }

  return true;
}

static aterm read_term(sym_read_entry* sym, istream &is)
{
  size_t val;
  size_t i, arity = sym->arity;
  sym_read_entry* arg_sym;
  // aterm stack_args[MAX_STACK_ARGS];
  std::vector<aterm> args(arity);
  aterm result;

  for (i=0; i<arity; i++)
  {
    if (readBits(&val, sym->sym_width[i], is) < 0)
    {
      return aterm();
    }
    if (val >= sym->nr_topsyms[i])
    {
      return aterm();
    }
    arg_sym = &read_symbols[sym->topsyms[i][val]];

    if (readBits(&val, arg_sym->term_width, is) < 0)
    {
      return aterm();
    }

    if (val >= arg_sym->nr_terms)
    {
      return aterm();
    }
    if (!arg_sym->terms[val].defined())
    {
      arg_sym->terms[val] = read_term(arg_sym, is);
      if (!arg_sym->terms[val].defined())
      {
        return aterm();
      }
    }

    args[i] = arg_sym->terms[val];
  }

  if (sym->sym==detail::function_adm.AS_INT)
  {
    /*{{{  Read an integer */

    if (readBits(&val, INT_SIZE_IN_BAF, is) < 0)
    {
      return aterm();
    }

    result = aterm_int(val);

      /*}}}  */
  }
  else if (sym->sym==detail::function_adm.AS_LIST)
  {
      result = push_front((aterm_list)args[1], args[0]);
  }
  else if (sym->sym==detail::function_adm.AS_EMPTY_LIST)
  {
    result = aterm_list();
  }
  else
  {
    /* Must be a function application */
    result = aterm_appl(sym->sym, args.begin(),args.end());
  }

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

    // if (entry->terms)
    // {
    //   delete &entry->terms;
    // }
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
  size_t val, nr_unique_terms;
  aterm result;

  /* Initialize bit buffer */
  bit_buffer     = '\0';
  bits_in_buffer = 0; /* how many bits in bit_buffer are used */

  /*{{{  Read header */

  if (readInt(&val, is) < 0)
  {
    return aterm();
  }

  if (val == 0)
  {
    if (readInt(&val, is) < 0)
    {
      return aterm();
    }
  }

  if (val != BAF_MAGIC)
  {
    mCRL2log(mcrl2::log::error) << "read_baf: input is not in BAF!" << std::endl;
    return aterm();
  }

  if (readInt(&val, is) < 0)
  {
    return aterm();
  }

  if (val != BAF_VERSION)
  {
    mCRL2log(mcrl2::log::error) << "read_baf: wrong BAF version, giving up!" << std::endl;
    return aterm();
  }

  if (readInt(&val, is) < 0)
  {
    return aterm();
  }
  nr_unique_symbols = val;

  if (readInt(&nr_unique_terms, is) < 0)
  {
    return aterm();
  }

  /* Allocate symbol space */
  read_symbols = std::vector<sym_read_entry>(nr_unique_symbols);


  if (!read_all_symbols(is))
  {
    return aterm();
  }

  if (readInt(&val, is) < 0)
  {
    return aterm();
  }

  result = read_term(&read_symbols[val], is);
  free_read_space();
  return result;
}


aterm read_term_from_binary_stream(istream &is)
{
  aterm result=read_baf(is);
  if (!result.defined())
  {
    throw std::runtime_error("Failed to read term from binary file.");
  }
  return result;
}

} // namespace atermpp
