// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
// 
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// 
/// \file atermpp/detail/indexed_set.cpp
/// \brief This file contains some constants and functions shared
///        between indexed_sets and tables.

#ifndef MCRL2_ATERMPP_DETAIL_INDEXED_SET_H
#define MCRL2_ATERMPP_DETAIL_INDEXED_SET_H

namespace atermpp
{
namespace detail
{

static const size_t STEP = 1; /* The position on which the next hash entry //searched */


/* in the hashtable we use the following constants to
   indicate designated positions */
static const size_t EMPTY(-1);

static const size_t a_prime_number = 134217689;

/* A very simple hashing function. */

inline size_t hashcode(const void* a, const size_t sizeMinus1)
{
  return ((((size_t)(a) >> 2) * a_prime_number) & sizeMinus1);
}


/*static size_t approximatepowerof2(size_t n) 
 * return smallest 2^m-1 larger or equal than n, where
 * returned size must at least be 127
 */

inline size_t approximatepowerof2(size_t n)
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

} // namespace detail

template <class ELEMENT>
inline
size_t indexed_set<ELEMENT>::hashPut(const ELEMENT& key, size_t n)
{
  /* Find a place to insert key,
     and find whether key already exists */

  size_t c = detail::hashcode(detail::address(key), sizeMinus1);

  while (1)
  {
    size_t v = hashtable[c];
    if (v == detail::EMPTY)
    {
      /* Found an empty spot, insert a new index belonging to key */
      hashtable[c] = n;
      return n;
    }

    if (m_keys[v]==key)
    {
      /* key is already in the set, return position of key */
      return v;
    }
    c = (c + detail::STEP) & sizeMinus1;
  }
  return c;
}


template <class ELEMENT>
inline void indexed_set<ELEMENT>::hashResizeSet()
{
  size_t newsizeMinus1 = 2*sizeMinus1+1; 

  hashtable.clear();
  hashtable.resize(newsizeMinus1+1,detail::EMPTY); 

  sizeMinus1=newsizeMinus1;
  max_entries = ((sizeMinus1/100)*max_load);

  /* rebuild the hashtable again */
  for (size_t i=0; i<m_keys.size(); i++)
  {
    const ELEMENT& t = m_keys[i];
    hashPut(t, i);
  }
}


template <class ELEMENT>
inline indexed_set<ELEMENT>::indexed_set(size_t initial_size /* = 100 */, unsigned int max_load_pct /* = 75 */)
      : sizeMinus1(detail::approximatepowerof2(initial_size)),
        max_load(max_load_pct),
        max_entries(((sizeMinus1/100)*max_load)),
        hashtable(std::vector<size_t>(1+sizeMinus1,detail::EMPTY))
{
}


template <class ELEMENT>
inline inline ssize_t indexed_set<ELEMENT>::index(const ELEMENT& elem) const
{
  size_t start = detail::hashcode(detail::address(elem), sizeMinus1);
  size_t c = start;
  do
  {
    size_t v=hashtable[c];
    if (v == detail::EMPTY)
    {
      return npos; /* Not found. */
    }

    if (elem==m_keys[v])
    {
      return v;
    }

    c = (c+detail::STEP) & sizeMinus1;
  }
  while (c != start);

  return npos; /* Not found. */
}


template <class ELEMENT>
inline inline const ELEMENT& indexed_set<ELEMENT>::get(size_t index) const
{
  assert(m_keys.size()>index);
  return m_keys[index];
}


template <class ELEMENT>
inline void indexed_set<ELEMENT>::clear()
{
  for (size_t i=0; i<=sizeMinus1 ; i++)
  {
    hashtable[i] = detail::EMPTY;
  }

  m_keys.clear();
}


template <class ELEMENT>
inline std::pair<size_t, bool> indexed_set<ELEMENT>::put(const ELEMENT& key)
{
  const size_t n = hashPut(key,m_keys.size());
  if (n != m_keys.size())
  {
      return std::make_pair(n,false);
  }
  

  m_keys.push_back(key);
  if (m_keys.size() >= max_entries)
  {
    hashResizeSet(); 
  }

  return std::make_pair(n, true);
}


} // namespace atermpp 

#endif // MCRL2_ATERMPP_DETAIL_INDEXED_SET_H
