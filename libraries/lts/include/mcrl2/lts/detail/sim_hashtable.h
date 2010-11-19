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
#include <vector>

//typedef unsigned int size_t;

struct bucket2
{
  size_t x,y;
  int next;
};

struct bucket3
{
  size_t x,y,z;
  int next;
};

class hash_table2
{
  public:
    hash_table2(size_t initsize);
    ~hash_table2();
    void clear();
    void add(size_t x,size_t y);
    bool find(size_t x,size_t y);
    void remove(size_t x,size_t y);

  private:
    std::vector<bucket2> buckets;
    std::vector<int> table;
    size_t mask;
    size_t removed_count;
    size_t hash(size_t x,size_t y);
    int hfind(size_t h,size_t x,size_t y);
    bool check_table();

    friend class hash_table2_iterator;
};

class hash_table2_iterator
{
  public:
    hash_table2_iterator(hash_table2 *ht);
    bool is_end() { return (bucket_it == hash_table->buckets.end()); }
    void operator ++();
    size_t get_x() { return bucket_it->x; }
    size_t get_y() { return bucket_it->y; }
  private:
    std::vector<bucket2>::iterator bucket_it;
    hash_table2 *hash_table;
};


class hash_table3
{
  public:
    hash_table3(size_t initsize);
    ~hash_table3();
    void clear();
    void add(size_t x,size_t y,size_t z);
    bool find(size_t x,size_t y,size_t z);
    void remove(size_t x,size_t y,size_t z);
    size_t get_num_elements() { return buckets.size() - removed_count; }

  private:
    std::vector<bucket3> buckets;
    std::vector<int> table;
    size_t mask;
    size_t removed_count;
    size_t hash(size_t x,size_t y,size_t z);
    int hfind(size_t h,size_t x,size_t y,size_t z);
    bool check_table();

    friend class hash_table3_iterator;
};

class hash_table3_iterator
{
  public:
    hash_table3_iterator(hash_table3 *ht);
    bool is_end() { return (bucket_it == end); }
    void operator ++();
    size_t get_x() { return bucket_it->x; }
    size_t get_y() { return bucket_it->y; }
    size_t get_z() { return bucket_it->z; }
    void set(size_t i) { bucket_it = hash_table->buckets.begin() + i; }
    void set_end(size_t i) { end = hash_table->buckets.begin() + i; }
  private:
    std::vector<bucket3>::iterator bucket_it;
    std::vector<bucket3>::iterator end;
    hash_table3 *hash_table;
};

#endif
