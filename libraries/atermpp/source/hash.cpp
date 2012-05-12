/*{{{  credits */

/* Added by JFG */
/* This file is made by Jan Friso Groote, January 2000. It contains
   implementations of the ATtable and ATindexedSet, which are different
   implementations of hashtables. The implementations are
   (intended to be) particularily efficient, both in space
   (appr 12 bytes are necessary for a set entry, and 16 for
   a table entry) and time. It is an improvement of the previously
   implemented ATtables which were a straightforward but not
   so efficient (both in time and space).
   Note that hashtables can only grow, until
   they are destroyed. Also resetting the hashtables does
   not help reducing memory consumption */

/*}}}  */
/*{{{  includes */
#include <stdexcept>
#include <stack>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "mcrl2/utilities/logger.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/table.h"
#include "mcrl2/atermpp/detail/memory.h"


/*}}}  */

namespace atermpp
{

/*{{{  defines */

/* ======================================================= */

static const size_t STEP = 1; /* The position on which the next hash entry //searched */

static const size_t TABLE_SHIFT = 14;
static const size_t ELEMENTS_PER_TABLE = 1L<<TABLE_SHIFT;
inline
size_t modELEMENTS_PER_TABLE(const size_t n)
{
  return (n & (ELEMENTS_PER_TABLE-1));
}

inline
size_t divELEMENTS_PER_TABLE(const size_t n)
{
  return n >> TABLE_SHIFT;
}

/*-----------------------------------------------------------*/

/* in the hashtable we use the following constants to
   indicate designated positions */
static const size_t EMPTY = (size_t)-1;
static const size_t DELETED = (size_t)-2;

static const size_t a_prime_number = 134217689;

/* A very simple hashing function. */

inline
size_t hashcode(void* a, const size_t sizeMinus1)
{
  return ((((size_t)(a) >> 2) * a_prime_number) & sizeMinus1);
}

/*}}}  */
/*{{{  static size_t approximatepowerof2(size_t n) */

/**
 * return smallest 2^m-1 larger or equal than n, where
 * returned size must at least be 127
 */

static size_t approximatepowerof2(size_t n)
{
  size_t mask = n;

  while (mask >>= 1)
  {
    n |= mask;
  }

  if (n<127)
  {
    n=127;
  }
  return n;
}

/*}}}  */
/*{{{  static size_t calc_size_t_max() */
static size_t calc_size_t_max()
{
  size_t try_size_t_max;
  size_t size_t_max;

  try_size_t_max = 1;
  do
  {
    size_t_max = try_size_t_max;
    try_size_t_max = size_t_max * 2+1;
  }
  while (try_size_t_max > size_t_max);

  return size_t_max;
}
/*}}}  */
/*{{{  static size_t calculateNewSize(sizeMinus1, nrdel, nrentries) */

static size_t calculateNewSize
(size_t sizeMinus1, size_t nr_deletions, size_t nr_entries)
{

  /* Hack: LONG_MAX (limits.h) is often unreliable, we need to find
   * out the maximum possible value of a size_t dynamically.
   */
  static size_t st_size_t_max = 0;

  /* the resulting length has the form 2^k-1 */

  if (nr_deletions >= nr_entries/2)
  {
    return sizeMinus1;
  }

  if (st_size_t_max == 0)
  {
    st_size_t_max = calc_size_t_max();
  }

  if (sizeMinus1 > st_size_t_max / 2)
  {
    return st_size_t_max-1;
  }

  return (2*sizeMinus1)+1;
}

/*}}}  */
/*{{{  static aterm tableGet(aterm **tableindex, size_t n) */

static aterm tableGet(const std::vector< std::vector<aterm> > &tableindex, size_t n)
{
  return tableindex[divELEMENTS_PER_TABLE(n)][modELEMENTS_PER_TABLE(n)];
}

/*}}}  */
/*{{{  static void insertKeyvalue(set, size_t n, aterm t, aterm v) */

static void insertKeyOrValue(std::vector<std::vector<aterm> >  &vec, size_t n, const aterm &t)
{
  const size_t x = divELEMENTS_PER_TABLE(n);
  const size_t y = modELEMENTS_PER_TABLE(n);

  
  if (x>=vec.size())
  {
    vec.push_back(std::vector<aterm>(ELEMENTS_PER_TABLE));
  } 

  vec[x][y] = t;
}

/*}}}  */
/*{{{  static size_t hashPut(ATerm_table s, aterm key, size_t n) */

size_t indexed_set::hashPut(const aterm &key, size_t n)
{
  size_t c,v;

  /* Find a place to insert key,
     and find whether key already exists */

  c = hashcode(&*key, sizeMinus1);

  while (1)
  {
    v = hashtable[c];
    if (v == EMPTY || v == DELETED)
    {
      /* Found an empty spot, insert a new index belonging to key */
      hashtable[c] = n;
      return n;
    }

    if (tableGet(m_keys, v)==key)
    {
      /* key is already in the set, return position of key */
      return v;
    }
    c = (c + STEP) & sizeMinus1;
  }
  return c;
}

/*}}}  */
/*{{{  static void hashResizeSet(ATerm_indexed_set s) */

void indexed_set::hashResizeSet()
{
  size_t newsizeMinus1 = calculateNewSize(sizeMinus1,
                                   free_positions.size(), m_size+free_positions.size());

  hashtable.clear();
  hashtable.resize(newsizeMinus1+1,EMPTY); 

  sizeMinus1=newsizeMinus1;
  max_entries = ((sizeMinus1/100)*max_load);

  /* rebuild the hashtable again */
  for (size_t i=0; i<m_size+free_positions.size(); i++)
  {
    aterm t = tableGet(m_keys, i);
    if (t != aterm())
    {
      hashPut(t, i);
    }
  }
  free_positions=std::stack < size_t >();
}

/*}}}  */
/*{{{  static ATermList tableContent(aterm **tableidx, entries) */

static aterm_list tableContent(const std::vector< std::vector<aterm> > &tableindex,size_t nr_entries)
{
  size_t i;
  aterm t;
  aterm_list result;

  for (i=nr_entries; i>0; i--)
  {
    t = tableGet(tableindex, i-1);
    if (t != aterm())
    {
      result = aterm_list(result, t);
    }
  }
  return result;
}

indexed_set::indexed_set(size_t initial_size /* = 100 */, unsigned int max_load_pct /* = 75 */)
      : sizeMinus1(approximatepowerof2(initial_size)),
        max_load(max_load_pct),
        max_entries(((sizeMinus1/100)*max_load)),
        hashtable(std::vector<size_t>(1+sizeMinus1,EMPTY)),
        m_size(0)
{
  // std::cerr << "MAKE INDEXED SET \n";
}

ssize_t indexed_set::index(const aterm& elem) const
{
  // std::cerr << " INDEX " << elem << "\n";
  size_t c,start,v;

  start = hashcode(&*elem, sizeMinus1);
  c = start;
  do
  {
    v=hashtable[c];
    if (v == EMPTY)
    {
  // std::cerr << " NOT FOUND \n";
      return ATERM_NON_EXISTING_POSITION; /* Not found. */
    }

    if (v != DELETED && elem==tableGet(m_keys, v))
    {
  // std::cerr << "FOUND " << v << "\n";
      return v;
    }

    c = (c+STEP) & sizeMinus1;
  }
  while (c != start);

  // std::cerr << " NOT FOUND2 \n";
  return ATERM_NON_EXISTING_POSITION; /* Not found. */
}

bool indexed_set::remove(const aterm& key)
{
// std::cerr << "REMOVE " << key << "\n";
  size_t start,c,v;

  start = hashcode(&*key,sizeMinus1);
  c = start;
  while (1)
  {
    v = hashtable[c];
    if (v == EMPTY)
    {
      return false;
    }
    if (v != DELETED && key==tableGet(m_keys, v))
    {
      break;
    }

    c = (c + STEP) & sizeMinus1;
    if (c == start)
    {
      return false;
    }
  }

  hashtable[c] = DELETED;

  insertKeyOrValue(m_keys, v, aterm());

  free_positions.push(v);
  m_size--;
  return true;
}

/*}}}  */
/*{{{  aterm ATindexedSetGetElem(ATerm_indexed_set hashset, size_t index) */

// aterm ATindexedSetGetElem(ATerm_indexed_set hashset, size_t index)

aterm indexed_set::get(size_t index) const
{
  assert(m_size+free_positions.size()>index);
  return tableGet(m_keys, index);
}

void indexed_set::reset()
{
  // std::cerr << "RESET " << "\n";
  for (size_t i=0; i<=sizeMinus1 ; i++)
  {
    hashtable[i] = EMPTY;
  }

  m_keys.clear();
  free_positions=std::stack<size_t>();
  m_size=0;
}

void table::reset()
{
  indexed_set::reset();
  m_values.clear();
}

std::pair<size_t, bool> indexed_set::put(const aterm& key)
{
  // std::cerr << "PUT " << key ;
  const size_t m=(free_positions.empty()? m_size : free_positions.top());
  const size_t n = hashPut(key,m);
// std::cerr << " N " << n << " M " << m << "   ";
  if (n != m)
  {
  // std::cerr << " NOT ADDED " << n << "\n";
      return std::make_pair(n,false);
  }
  
  m_size++;
  if (!free_positions.empty())
  {
    free_positions.pop();
  }

  insertKeyOrValue(m_keys, n, key);
  if (m_size+free_positions.size() >= max_entries)
  {
    hashResizeSet(); 
  }

  // std::cerr << " ADDED " << n << " M_SIZE " << m_size << "\n";
  return std::make_pair(n, true);
}

/*}}}  */
/*}}}  */
/*{{{  void ATtablePut(ATerm_table table, aterm key, aterm value) */
// void ATtablePut(ATerm_table table, const aterm &key, const aterm &value)

void table::put(const aterm &key, const aterm &value)
{
  /* insert entry key into the hashtable, and deliver
     an index. If key is already in the set, deliver 0 */

  std::pair<size_t, bool> p = indexed_set::put(key);
  insertKeyOrValue(m_values,p.first,value);
}

/*}}}  */
/*{{{  aterm ATtableGet(ATerm_table table, aterm key) */

// aterm ATtableGet(ATerm_table table, const aterm &key)

aterm table::get(const aterm &key) const
{
  const size_t v = index(key);
  if (v==ATERM_NON_EXISTING_POSITION)
  {
    return aterm();
  }
  return tableGet(m_values, v);
}


bool table::remove(const aterm &key)
{
// std::cerr << "TABLE REMOVE " <<  key << "\n";
  const bool removed=indexed_set::remove(key);
  // At the top of the stack is the freed position with the key, 
  // of which the value must still be removed.
  if (removed) 
  { 
    insertKeyOrValue(m_values, free_positions.top(), aterm());
  }
  return removed;
}

aterm_list indexed_set::keys() const
{
  return tableContent(m_keys, m_size+free_positions.size());
}

aterm_list table::values() const
{
  return tableContent(m_values, m_size+free_positions.size());
}

/*}}}  */

} // namespace atermpp
