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
#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/aterm/util.h"
#include "mcrl2/aterm/memory.h"


/*}}}  */

namespace aterm
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
/*{{{  types */

struct _ATermTable
{
  size_t sizeMinus1;
  size_t nr_entries; /* the number of occupied positions in the hashtable,
          including the elements that are explicitly marked
          as deleted */
  size_t nr_deletions;
  unsigned int max_load;
  size_t max_entries;
  size_t* hashtable;
  std::vector < std::vector <ATerm > > keys;
  std::stack < size_t > free_positions;
  bool contains_values;
  std::vector < std::vector < ATerm > > values;
}; 

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
/*{{{  static ATerm tableGet(ATerm **tableindex, size_t n) */

static ATerm tableGet(const std::vector< std::vector<ATerm> > &tableindex, size_t n)
{
  return tableindex[divELEMENTS_PER_TABLE(n)][modELEMENTS_PER_TABLE(n)];
}

/*}}}  */
/*{{{  static void insertKeyvalue(set, size_t n, ATerm t, ATerm v) */

static void insertKeyValue(ATermIndexedSet s,
                           size_t n, const ATerm &t, const ATerm &v)
{
  assert(s->keys.size()==s->values.size() || (!s->contains_values));

  const size_t x = divELEMENTS_PER_TABLE(n);
  const size_t y = modELEMENTS_PER_TABLE(n);

  
  if (x>=s->keys.size())
  {
    s->keys.push_back(std::vector<ATerm>(ELEMENTS_PER_TABLE));
    if (s->contains_values) 
    {
      s->values.push_back(std::vector<ATerm>(ELEMENTS_PER_TABLE));
    }
  } 

  s->keys[x][y] = t;
  if (s->contains_values)
  {
    s->values[x][y] = v;
  }
}

/*}}}  */
/*{{{  static size_t hashPut(ATermTable s, ATerm key, size_t n) */

static size_t hashPut(ATermTable s, const ATerm &key, size_t n)
{
  size_t c,v;

  /* Find a place to insert key,
     and find whether key already exists */

  c = hashcode(&*key, s->sizeMinus1);

  while (1)
  {
    v = s->hashtable[c];
    if (v == EMPTY || v == DELETED)
    {
      /* Found an empty spot, insert a new index belonging to key */
      s->hashtable[c] = n;
      return n;
    }

    if (ATisEqual(tableGet(s->keys, v), key))
    {
      /* key is already in the set, return position of key */
      return v;
    }
    c = (c + STEP) & s->sizeMinus1;
  }
  return c;
}

/*}}}  */
/*{{{  static void hashResizeSet(ATermIndexedSet s) */

static void hashResizeSet(ATermIndexedSet s)
{
  size_t i,newsizeMinus1;
  ATerm t;
  size_t* newhashtable;

  newsizeMinus1 = calculateNewSize(s->sizeMinus1,
                                   s->nr_deletions, s->nr_entries);

  newhashtable = (size_t*)malloc(sizeof(size_t) * (newsizeMinus1+1));

  if (newhashtable!=NULL)
  {
    /* the hashtable has properly been resized */
    free(s->hashtable);

    s->hashtable = newhashtable;
    s->sizeMinus1=newsizeMinus1;
    s->max_entries = ((s->sizeMinus1/100)*s->max_load);
  }
  else
  {
    /* resizing the hashtable failed and s->hashtable still
       points to the old hashtable. We keep this old hashtable */
#ifndef NDEBUG
    mCRL2log(mcrl2::log::warning) << "No memory to increase the size of the indexed set hash table" << std::endl;
#endif
    if (s->nr_entries-s->nr_deletions+2>=s->sizeMinus1)
    {
      std::runtime_error("hashResizeSet: Hashtable of indexed set is full");
    }

    /* I do not know whether the bound below is very optimal,
       but we are anyhow in a precarious situation now */
    s->max_entries = (s->nr_entries-s->nr_deletions+s->sizeMinus1)/2;
  }

  /* reset the hashtable. */
  for (i=0; i<=s->sizeMinus1 ; i++)
  {
    s->hashtable[i] = EMPTY;
  }

  /* rebuild the hashtable again */
  for (i=0; i<s->nr_entries; i++)
  {
    t = tableGet(s->keys, i);
    if (t != ATerm())
    {
      hashPut(s, t, i);
    }
  }
  s->nr_deletions = 0;
}

/*}}}  */
/*{{{  static ATermList tableContent(ATerm **tableidx, entries) */

static ATermList tableContent(const std::vector< std::vector<ATerm> > &tableindex,size_t nr_entries)
{
  size_t i;
  ATerm t;
  ATermList result = ATempty;

  for (i=nr_entries; i>0; i--)
  {
    t = tableGet(tableindex, i-1);
    if (t != ATerm())
    {
      result = ATinsert(result, t);
    }
  }
  return result;
}

/*}}}  */

/*{{{  ATermIndexedSet ATindexedSetCreate(init_size, max_load_pct) */

ATermIndexedSet ATindexedSetCreate(size_t initial_size, unsigned int max_load_pct)
{
  ATermIndexedSet hashset=new _ATermTable;

  hashset->sizeMinus1 = approximatepowerof2(initial_size);
  hashset->nr_entries = 0;
  hashset->nr_deletions = 0;
  hashset->max_load = max_load_pct;
  hashset->max_entries = ((hashset->sizeMinus1/100)*hashset->max_load);
  hashset->hashtable=
    (size_t*)malloc(sizeof(size_t)*(1+hashset->sizeMinus1));
  if (hashset->hashtable==NULL)
  {
    std::runtime_error("ATindexedSetCreate: cannot allocate ATermIndexedSet "
                       "of " + to_string(initial_size) + " entries");
  }
  for (size_t i=0 ; i<=hashset->sizeMinus1 ; i++)
  {
    hashset->hashtable[i] = EMPTY;
  }

  hashset->contains_values=false;

  return hashset;
}

/*}}}  */
/*{{{  void ATindexedSetDestroy(ATermIndexedSet hashset) */

void ATindexedSetDestroy(ATermIndexedSet hashset)
{
  ATtableDestroy(hashset);
}

/*}}}  */
/*{{{  void ATindexedSetReset(ATermIndexedSet hashset) */

void ATindexedSetReset(ATermIndexedSet hashset)
{
  ATtableReset(hashset);
}

/*}}}  */
/*{{{  static size_t keyPut(ATermIndexedSet hashset, key, value, ATbool *isnew)*/

static size_t keyPut(ATermIndexedSet hashset, const ATerm &key,
                     const ATerm &value, bool* isnew)
{
  size_t n;
  if (hashset->free_positions.size() == 0)
  {
    const size_t m = hashset->nr_entries;
    n = hashPut(hashset,key,m);
    if (n != m)
    {
      if (isnew != NULL)
      {
        *isnew = false;
      }
      if (value != ATerm())
      {
        assert(hashset->contains_values);
        hashset->values[ divELEMENTS_PER_TABLE(n)]
        [ modELEMENTS_PER_TABLE(n)] = &*value;
      }
      return n;
    }
    hashset->nr_entries++;
  }
  else
  {
    const size_t m = hashset->free_positions.top();
    hashset->free_positions.pop();
    n = hashPut(hashset, key, m);
    if (n != m)
    {
      if (isnew != NULL)
      {
        *isnew = false;
      }
      if (value != ATerm())
      {
        assert(hashset->contains_values);
        hashset->values[ divELEMENTS_PER_TABLE(n)]
        [ modELEMENTS_PER_TABLE(n)] = &*value;
      }
      return n;
    }
    // hashset->first_free_position--;
  }

  if (isnew != NULL)
  {
    *isnew = true;
  }
  insertKeyValue(hashset, n, key, value);
  if (hashset->nr_entries >= hashset->max_entries)
  {
    hashResizeSet(hashset); 
  }

  return n;
}

/*}}}  */
/*{{{  size_t ATindexedSetPut(ATermIndexedSet hashset, elem, *isnew) */

/**
 * insert entry elem into the hashtable, and deliver
 * an index. If elem is already in the set, deliver 0
 */

size_t ATindexedSetPut(ATermIndexedSet hashset, const ATerm &elem, bool* isnew)
{
  return keyPut(hashset, elem, ATerm(), isnew);
}

/*}}}  */
/*{{{  size_t ATindexedSetGetIndex(ATermIndexedSet hashset, ATerm key) */

ssize_t ATindexedSetGetIndex(ATermIndexedSet hashset, const ATerm &elem)
{
  size_t c,start,v;

  start = hashcode(&*elem, hashset->sizeMinus1);
  c = start;
  do
  {
    v=hashset->hashtable[c];
    if (v == EMPTY)
    {
      return -1; /* Not found. */
    }

    if (v != DELETED && ATisEqual(elem,tableGet(hashset->keys, v)))
    {
      return (ssize_t)v;
    }

    c = (c+STEP) & hashset->sizeMinus1;
  }
  while (c != start);

  return -1; /* Not found. */
}

/*}}}  */
/*{{{  void ATindexedSetRemove(ATermIndexedSet hashset, ATerm elem) */

bool ATindexedSetRemove(ATermIndexedSet hashset, const ATerm &elem)
{
  return ATtableRemove(hashset, elem);
}

/*}}}  */
/*{{{  ATermList ATindexedSetElements(ATermIndexedSet hashset) */

ATermList ATindexedSetElements(ATermIndexedSet hashset)
{
  return tableContent(hashset->keys, hashset->nr_entries);
}

/*}}}  */
/*{{{  ATerm ATindexedSetGetElem(ATermIndexedSet hashset, size_t index) */

ATerm ATindexedSetGetElem(ATermIndexedSet hashset, size_t index)
{
  assert(hashset->nr_entries>index);
  return tableGet(hashset->keys, index);
}

/*}}}  */

/*{{{  ATermTable ATtableCreate(size_t initial_size, int max_load_pct) */

ATermTable ATtableCreate(const size_t initial_size, const unsigned int max_load_pct)
{
  ATermTable hashtable = (ATermTable)ATindexedSetCreate(initial_size, max_load_pct);

  hashtable->contains_values=true;

  return hashtable;
}

/*}}}  */
/*{{{  void ATtableDestroy(ATermTable table) */

void ATtableDestroy(ATermTable table)
{
  free(table->hashtable);
  delete table;
}

/*}}}  */
/*{{{  void ATtableReset(ATermTable table) */

void ATtableReset(ATermTable table)
{
  table->nr_entries = 0;
  table->nr_deletions = 0;

  for (size_t i=0; i<=table->sizeMinus1 ; i++)
  {
    table->hashtable[i] = EMPTY;
  }

  table->keys.clear();
  if (table->contains_values)
  {
    table->values.clear();
  }
  table->free_positions=std::stack<size_t>();
}

/*}}}  */
/*{{{  void ATtablePut(ATermTable table, ATerm key, ATerm value) */

void ATtablePut(ATermTable table, const ATerm &key, const ATerm &value)
{
  /* insert entry key into the hashtable, and deliver
     an index. If key is already in the set, deliver 0 */

  bool isnew;

  keyPut(table, key, value, &isnew);
}

/*}}}  */
/*{{{  ATerm ATtableGet(ATermTable table, ATerm key) */

ATerm ATtableGet(ATermTable table, const ATerm &key)
{
  const size_t v = ATindexedSetGetIndex(table, key);
  if (v==ATERM_NON_EXISTING_POSITION)
  {
    return ATerm();
  }
  return tableGet(table->values, v);
}

/*}}}  */
/*{{{  void ATtableRemove(ATermTable table, ATerm key) */

bool ATtableRemove(ATermTable table, const ATerm &key)
{
  size_t start,c,v;
  // size_t* ltable;

  start = hashcode(&*key,table->sizeMinus1);
  c = start;
  while (1)
  {
    v = table->hashtable[c];
    if (v == EMPTY)
    {
      return false;
    }
    if (v != DELETED && ATisEqual(key, tableGet(table->keys, v)))
    {
      break;
    }

    c = (c + STEP) & table->sizeMinus1;
    if (c == start)
    {
      return false;
    }
  }

  table->hashtable[c] = DELETED;

  insertKeyValue(table, v, ATerm(), ATerm());

  table->free_positions.push(v);
  table->nr_deletions++;
  return true;
}

/*}}}  */
/*{{{  ATermList ATtableKeys(ATermTable table) */

ATermList ATtableKeys(ATermTable table)
{
  return tableContent(table->keys, table->nr_entries);
}

/*}}}  */
/*{{{  ATermList  ATtableValues(ATermTable table) */

ATermList  ATtableValues(ATermTable table)
{
  return tableContent(table->values, table->nr_entries);
}

/*}}}  */

} // namespace aterm
