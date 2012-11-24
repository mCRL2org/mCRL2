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
#include "mcrl2/atermpp/detail/indexed_set.h"


namespace atermpp
{

/* ======================================================= */

static const size_t STEP = 1; /* The position on which the next hash entry //searched */


/* in the hashtable we use the following constants to
   indicate designated positions */
static const size_t EMPTY = (size_t)-1;
static const size_t DELETED = (size_t)-2;

static const size_t a_prime_number = 134217689;

/* A very simple hashing function. */

static size_t hashcode(const void* a, const size_t sizeMinus1)
{
  return ((((size_t)(a) >> 2) * a_prime_number) & sizeMinus1);
}


/*static size_t approximatepowerof2(size_t n) 
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


static size_t calc_size_t_max()
{
  size_t size_t_max;

  size_t try_size_t_max = 1;
  do
  {
    size_t_max = try_size_t_max;
    try_size_t_max = size_t_max * 2+1;
  }
  while (try_size_t_max > size_t_max);

  return size_t_max;
}


static size_t calculateNewSize(size_t sizeMinus1, size_t nr_deletions, size_t nr_entries)
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


size_t indexed_set::hashPut(const aterm &key, size_t n)
{
  /* Find a place to insert key,
     and find whether key already exists */

  size_t c = hashcode(key.address(), sizeMinus1);

  while (1)
  {
    size_t v = hashtable[c];
    if (v == EMPTY || v == DELETED)
    {
      /* Found an empty spot, insert a new index belonging to key */
      hashtable[c] = n;
      return n;
    }

    if (detail::tableGet(m_keys, v)==key)
    {
      /* key is already in the set, return position of key */
      return v;
    }
    c = (c + STEP) & sizeMinus1;
  }
  return c;
}


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
    aterm t = detail::tableGet(m_keys, i);
    if (t.defined())
    {
      hashPut(t, i);
    }
  }
  free_positions=std::stack < size_t >();
}


indexed_set::indexed_set(size_t initial_size /* = 100 */, unsigned int max_load_pct /* = 75 */)
      : sizeMinus1(approximatepowerof2(initial_size)),
        max_load(max_load_pct),
        max_entries(((sizeMinus1/100)*max_load)),
        hashtable(std::vector<size_t>(1+sizeMinus1,EMPTY)),
        m_size(0)
{
}


ssize_t indexed_set::index(const aterm& elem) const
{
  size_t start = hashcode(elem.address(), sizeMinus1);
  size_t c = start;
  do
  {
    size_t v=hashtable[c];
    if (v == EMPTY)
    {
      return atermpp::npos; /* Not found. */
    }

    if (v != DELETED && elem==detail::tableGet(m_keys, v))
    {
      return v;
    }

    c = (c+STEP) & sizeMinus1;
  }
  while (c != start);

  return atermpp::npos; /* Not found. */
}


bool indexed_set::erase(const aterm& key)
{
  size_t start = hashcode(key.address(),sizeMinus1);
  size_t c = start;
  size_t v;
  while (1)
  {
    v = hashtable[c];
    if (v == EMPTY)
    {
      return false;
    }
    if (v != DELETED && key==detail::tableGet(m_keys, v))
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

  detail::insertKeyOrValue(m_keys, v, detail::static_undefined_aterm);

  free_positions.push(v);
  m_size--;
  return true;
}


const aterm &indexed_set::get(size_t index) const
{
  assert(m_size+free_positions.size()>index);
  return detail::tableGet(m_keys, index);
}


void indexed_set::clear()
{
  for (size_t i=0; i<=sizeMinus1 ; i++)
  {
    hashtable[i] = EMPTY;
  }

  m_keys.clear();
  free_positions=std::stack<size_t>();
  m_size=0;
}


std::pair<size_t, bool> indexed_set::put(const aterm& key)
{
  const size_t m=(free_positions.empty()? m_size : free_positions.top());
  const size_t n = hashPut(key,m);
  if (n != m)
  {
      return std::make_pair(n,false);
  }
  
  m_size++;
  if (!free_positions.empty())
  {
    free_positions.pop();
  }

  detail::insertKeyOrValue(m_keys, n, key);
  if (m_size+free_positions.size() >= max_entries)
  {
    hashResizeSet(); 
  }

  return std::make_pair(n, true);
}


aterm_list indexed_set::keys() const
{
  return detail::tableContent(m_keys, m_size+free_positions.size());
}

} // namespace atermpp
