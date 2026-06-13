// Author(s): Bas Ploeger
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tree_set.cpp

#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/lts/detail/tree_set.h"
#include "mcrl2/utilities/exception.h"

#define EMPTY_SET (-1)
#define EMPTY_LIST (-1)
#define EMPTY_TAG (-1)

namespace mcrl2::lts
{

constexpr ptrdiff_t HASH_CLASS = 16;
constexpr ptrdiff_t TAGS_BLOCK = 15000;
constexpr ptrdiff_t BUCKETS_BLOCK = 25000;

// simple hash function; uses two large primes
constexpr ptrdiff_t hash(ptrdiff_t l, ptrdiff_t r, ptrdiff_t m)
{
  return (36425657*l + 77673689*r) & m;
}

tree_set_store::tree_set_store()
{
  buckets_size = 0;
  buckets_next = 0;

  tags_size = 0;
  tags_next = 0;

  hashmask = (static_cast<ptrdiff_t>(1) << HASH_CLASS) - 1;
  hashtable.assign(hashmask+1, EMPTY_LIST);
}

tree_set_store::~tree_set_store() = default;

void tree_set_store::check_tags()
{
  if (tags_next >= tags_size)
  {
    tags_size += TAGS_BLOCK;
    tags.resize(tags_size);
  }
}

void tree_set_store::check_buckets()
{
  if (buckets_next >= buckets_size)
  {
    buckets_size += BUCKETS_BLOCK;
    buckets.resize(buckets_size);
  }
  if (buckets_next*4 >= hashmask*3)
  {
    hashmask = hashmask + hashmask + 1;
    hashtable.assign(hashmask+1, EMPTY_LIST);
    for (ptrdiff_t i=0; i<buckets_next; ++i)
    {
      ptrdiff_t hc = hash(buckets[i].child_l,buckets[i].child_r,hashmask);
      buckets[i].next = hashtable[hc];
      hashtable[hc] = i;
    }
  }
}

ptrdiff_t tree_set_store::build_set(ptrdiff_t child_l,ptrdiff_t child_r)
{
  check_buckets();
  ptrdiff_t hc = hash(child_l,child_r,hashmask);
  buckets[buckets_next].child_l = child_l;
  buckets[buckets_next].child_r = child_r;
  buckets[buckets_next].tag     = EMPTY_TAG;
  buckets[buckets_next].next    = hashtable[hc];
  hashtable[hc] = buckets_next;
  return buckets_next++;
}

ptrdiff_t tree_set_store::find_set(ptrdiff_t child_l,ptrdiff_t child_r)
{
  ptrdiff_t hc = hash(child_l,child_r,hashmask);
  for (ptrdiff_t i=hashtable[hc]; i!=EMPTY_LIST; i=buckets[i].next)
  {
    if (buckets[i].child_l==child_l && buckets[i].child_r==child_r)
    {
      return i;
    }
  }
  return build_set(child_l,child_r);
}

ptrdiff_t tree_set_store::create_set(std::vector<ptrdiff_t> &elems)
{
  if (elems.size() == 0)
  {
    return EMPTY_SET;
  }

  ptrdiff_t* nodes = MCRL2_SPECIFIC_STACK_ALLOCATOR(ptrdiff_t,elems.size());
  std::size_t node_size = 0;
  std::size_t i;
  std::size_t j;
  for (i=0; i < elems.size(); ++i)
  {
    nodes[i]=find_set(elems[i],EMPTY_SET);
  }
  node_size = i;
  while (node_size > 1)
  {
    j = 0;
    for (i=0; i<node_size; i+=2)
    {
      if (i == node_size-1)
      {
        nodes[j] = nodes[i];
      }
      else
      {
        nodes[j] = find_set(nodes[i],nodes[i+1]);
      }
      ++j;
    }
    node_size = j;
  }
  ptrdiff_t r = nodes[0];
  return r;
}

ptrdiff_t tree_set_store::get_next_tag()
{
  return tags_next;
}

ptrdiff_t tree_set_store::get_set(ptrdiff_t tag)
{
  return tags[tag];
}

ptrdiff_t tree_set_store::get_set_child_left(ptrdiff_t set)
{
  return buckets[set].child_l;
}

ptrdiff_t tree_set_store::get_set_child_right(ptrdiff_t set)
{
  return buckets[set].child_r;
}

ptrdiff_t tree_set_store::get_set_size(ptrdiff_t set)
{
  if (buckets[set].child_r == EMPTY_SET)
  {
    return 1;
  }
  return get_set_size(buckets[set].child_l) +
         get_set_size(buckets[set].child_r);
}

bool tree_set_store::is_set_empty(ptrdiff_t set)
{
  return (set == EMPTY_SET);
}

ptrdiff_t tree_set_store::set_set_tag(ptrdiff_t set)
{
  if (buckets[set].tag != EMPTY_TAG)
  {
    return buckets[set].tag;
  }
  check_tags();
  tags[tags_next] = set;
  buckets[set].tag = tags_next;
  return tags_next++;
}

}

