// Author(s): Bas Ploeger
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file sim_hashtable.cpp
#include "mcrl2/lts/detail/sim_hashtable.h"

#define NOT_FOUND   (size_t)(-1)
#define END_OF_LIST (size_t)(-1)
#define REMOVED     (size_t)(-2)

/* ---------------- hash_table_iterator ----------------------------- */

hash_table2_iterator::hash_table2_iterator(hash_table2* ht)
  : hash_table(ht)
{
  bucket_it = hash_table->buckets.begin();
  while (bucket_it != hash_table->buckets.end()
         && (bucket_it->next == REMOVED))
  {
    ++bucket_it;
  }
}

void hash_table2_iterator::operator ++()
{
  if (bucket_it != hash_table->buckets.end())
  {
    ++bucket_it;
    while (bucket_it != hash_table->buckets.end()
           && (bucket_it->next == REMOVED))
    {
      ++bucket_it;
    }
  }
}

hash_table3_iterator::hash_table3_iterator(hash_table3* ht)
  : hash_table(ht)
{
  bucket_it = hash_table->buckets.begin();
  end = hash_table->buckets.end();
  while (bucket_it != end && bucket_it->next == REMOVED)
  {
    ++bucket_it;
  }
}

void hash_table3_iterator::operator ++()
{
  if (bucket_it != end)
  {
    ++bucket_it;
    while (bucket_it != end && bucket_it->next == REMOVED)
    {
      ++bucket_it;
    }
  }
}

/* ---------------- hash_table -------------------------------------- */

hash_table2::hash_table2(size_t initsize)
{
  mask = 1;
  while (mask < initsize)
  {
    mask = mask << 1;
  }
  --mask;
  clear();
}

hash_table2::~hash_table2()
{}

void hash_table2::clear()
{
  table.assign(mask+1,END_OF_LIST);
  buckets.clear();
  removed_count = 0;
}

void hash_table2::add(size_t x,size_t y)
{
  size_t h = hash(x,y);
  if (hfind(h,x,y) == NOT_FOUND)
  {
    if (check_table())
    {
      h = hash(x,y);
    }
    bucket2 new_bucket;
    new_bucket.x = x;
    new_bucket.y = y;
    new_bucket.next = table[h];
    table[h] = buckets.size();
    buckets.push_back(new_bucket);
  }
}

bool hash_table2::find(size_t x,size_t y)
{
  return (hfind(hash(x,y),x,y) != NOT_FOUND);
}

void hash_table2::remove(size_t x,size_t y)
{
  bucket2 b;
  size_t i, prev_i;
  size_t h = hash(x,y);
  i = table[h];
  if (i != END_OF_LIST)
  {
    b = buckets[i];
    if (b.x == x && b.y == y)
    {
      table[h] = b.next;
      buckets[i].next = REMOVED;
      ++removed_count;
      return;
    }
    prev_i = i;
    i = b.next;
  }
  while (i != END_OF_LIST)
  {
    b = buckets[i];
    if (b.x == x && b.y == y)
    {
      buckets[prev_i].next = buckets[i].next;
      buckets[i].next = REMOVED;
      ++removed_count;
      return;
    }
    prev_i = i;
    i = b.next;
  }
}

size_t hash_table2::hfind(size_t h,size_t x,size_t y)
{
  size_t i = table[h];
  bucket2 b;
  while (i != END_OF_LIST)
  {
    b = buckets[i];
    if (b.x == x && b.y == y)
    {
      return i;
    }
    i = b.next;
  }
  return NOT_FOUND;
}

bool hash_table2::check_table()
{
  if (4*(buckets.size() - removed_count) >= 3*table.size())
  {
    /* table is filled for at least 75%, so let's rehash! */
    mask = (2*mask) + 1;
    table.assign(mask+1,END_OF_LIST);
    size_t h;
    for (size_t i = 0; i < buckets.size(); ++i)
    {
      if (buckets[i].next != REMOVED)
      {
        h = hash(buckets[i].x,buckets[i].y);
        buckets[i].next = table[h];
        table[h] = i;
      }
    }
    return true;
  }
  return false;
}

size_t hash_table2::hash(size_t x, size_t y)
{
  return ((159403*x + 389651*y) & mask);
}

hash_table3::hash_table3(size_t initsize)
{
  mask = 1;
  while (mask < initsize)
  {
    mask = mask << 1;
  }
  --mask;
  clear();
}

hash_table3::~hash_table3()
{}

void hash_table3::clear()
{
  table.assign(mask+1,END_OF_LIST);
  buckets.clear();
  removed_count = 0;
}

void hash_table3::add(size_t x,size_t y,size_t z)
{
  size_t h = hash(x,y,z);
  if (hfind(h,x,y,z) == NOT_FOUND)
  {
    if (check_table())
    {
      h = hash(x,y,z);
    }
    bucket3 new_bucket;
    new_bucket.x = x;
    new_bucket.y = y;
    new_bucket.z = z;
    new_bucket.next = table[h];
    table[h] = buckets.size();
    buckets.push_back(new_bucket);
  }
}

bool hash_table3::find(size_t x,size_t y,size_t z)
{
  return (hfind(hash(x,y,z),x,y,z) != NOT_FOUND);
}

void hash_table3::remove(size_t x,size_t y,size_t z)
{
  bucket3 b;
  size_t i, prev_i;
  size_t h = hash(x,y,z);
  i = table[h];
  if (i != END_OF_LIST)
  {
    b = buckets[i];
    if (b.x == x && b.y == y && b.z == z)
    {
      table[h] = b.next;
      buckets[i].next = REMOVED;
      ++removed_count;
      return;
    }
    prev_i = i;
    i = b.next;
  }
  while (i != END_OF_LIST)
  {
    b = buckets[i];
    if (b.x == x && b.y == y && b.z == z)
    {
      buckets[prev_i].next = buckets[i].next;
      buckets[i].next = REMOVED;
      ++removed_count;
      return;
    }
    prev_i = i;
    i = b.next;
  }
}

size_t hash_table3::hfind(size_t h,size_t x,size_t y,size_t z)
{
  size_t i = table[h];
  bucket3 b;
  while (i != END_OF_LIST)
  {
    b = buckets[i];
    if (b.x == x && b.y == y && b.z == z)
    {
      return i;
    }
    i = b.next;
  }
  return NOT_FOUND;
}

bool hash_table3::check_table()
{
  if (4*(buckets.size() - removed_count) >= 3*table.size())
  {
    /* table is filled for at least 75%, so let's rehash! */
    mask = (2*mask) + 1;
    table.assign(mask+1,END_OF_LIST);
    size_t h;
    for (size_t i = 0; i < buckets.size(); ++i)
    {
      if (buckets[i].next != REMOVED)
      {
        h = hash(buckets[i].x,buckets[i].y,buckets[i].z);
        buckets[i].next = table[h];
        table[h] = i;
      }
    }
    return true;
  }
  return false;
}

size_t hash_table3::hash(size_t x, size_t y, size_t z)
{
  return ((159403*x + 389651*y + 521503*z) & mask);
}
