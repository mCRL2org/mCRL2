// Author(s): Bas Ploeger
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file sim_hashtable.h
/// \brief Header file for hash table data structure used by the
/// simulation preorder algorithm.

/* This class implements a hash table for storing triples of unsigned
 * integers. Triples can be added, removed and checked for presence.
 * Note however that removing a triple does not free space and re-adding
 * a removed triple does not reuse the previously removed entry but
 * creates new entry.
 * Hence, although this does not lead to incorrect results, this hash
 * table implementation is intended for applications in which removed
 * triples will never (or rarely) be added again. */

#ifndef SIM_HASHTABLE_H
#define SIM_HASHTABLE_H
#include <cstddef>
#include <vector>

struct bucket2
{
  std::size_t x,y;
  std::size_t next;
};

struct bucket3
{
  std::size_t x,y,z;
  std::size_t next;
};

class hash_table2
{
  public:
    hash_table2(std::size_t initsize);
    ~hash_table2();
    void clear();
    void add(std::size_t x,std::size_t y);
    bool find(std::size_t x,std::size_t y);
    void remove(std::size_t x,std::size_t y);

  private:
    std::vector<bucket2> buckets;
    std::vector<std::size_t> table;
    std::size_t mask;
    std::size_t removed_count;
    std::size_t hash(std::size_t x,std::size_t y);
    std::size_t hfind(std::size_t h,std::size_t x,std::size_t y);
    bool check_table();

    friend class hash_table2_iterator;
};

class hash_table2_iterator
{
  public:
    hash_table2_iterator(hash_table2* ht);
    bool is_end()
    {
      return (bucket_it == hash_table->buckets.end());
    }
    void operator ++();
    std::size_t get_x()
    {
      return bucket_it->x;
    }
    std::size_t get_y()
    {
      return bucket_it->y;
    }
  private:
    std::vector<bucket2>::iterator bucket_it;
    hash_table2* hash_table;
};


class hash_table3
{
  public:
    hash_table3(std::size_t initsize);
    ~hash_table3();
    void clear();
    void add(std::size_t x,std::size_t y,std::size_t z);
    bool find(std::size_t x,std::size_t y,std::size_t z);
    void remove(std::size_t x,std::size_t y,std::size_t z);
    std::size_t get_num_elements()
    {
      return buckets.size() - removed_count;
    }

  private:
    std::vector<bucket3> buckets;
    std::vector<std::size_t> table;
    std::size_t mask;
    std::size_t removed_count;
    std::size_t hash(std::size_t x,std::size_t y,std::size_t z);
    std::size_t hfind(std::size_t h,std::size_t x,std::size_t y,std::size_t z);
    bool check_table();

    friend class hash_table3_iterator;
};

class hash_table3_iterator
{
  public:
    hash_table3_iterator(hash_table3* ht);
    bool is_end()
    {
      return (bucket_it == end);
    }
    void operator ++();
    std::size_t get_x()
    {
      return bucket_it->x;
    }
    std::size_t get_y()
    {
      return bucket_it->y;
    }
    std::size_t get_z()
    {
      return bucket_it->z;
    }
    void set(std::size_t i)
    {
      bucket_it = hash_table->buckets.begin() + i;
    }
    void set_end(std::size_t i)
    {
      end = hash_table->buckets.begin() + i;
    }
  private:
    std::vector<bucket3>::iterator bucket_it;
    std::vector<bucket3>::iterator end;
    hash_table3* hash_table;
};

#endif
