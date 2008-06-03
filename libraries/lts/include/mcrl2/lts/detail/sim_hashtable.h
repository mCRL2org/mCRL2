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

typedef unsigned int uint;

struct bucket2
{
  uint x,y;
  int next;
};

struct bucket3
{
  uint x,y,z;
  int next;
};

class hash_table2
{
  public:
    hash_table2(uint initsize);
    ~hash_table2();
    void clear();
    void add(uint x,uint y);
    bool find(uint x,uint y);
    void remove(uint x,uint y);

  private:
    std::vector<bucket2> buckets;
    std::vector<int> table;
    uint mask;
    uint removed_count;
    uint hash(uint x,uint y);
    int hfind(uint h,uint x,uint y);
    bool check_table();

    friend class hash_table2_iterator;
};

class hash_table2_iterator
{
  public:
    hash_table2_iterator(hash_table2 *ht);
    bool is_end() { return (bucket_it == hash_table->buckets.end()); }
    void operator ++();
    uint get_x() { return bucket_it->x; }
    uint get_y() { return bucket_it->y; }
  private:
    std::vector<bucket2>::iterator bucket_it;
    hash_table2 *hash_table;
};


class hash_table3
{
  public:
    hash_table3(uint initsize);
    ~hash_table3();
    void clear();
    void add(uint x,uint y,uint z);
    bool find(uint x,uint y,uint z);
    void remove(uint x,uint y,uint z);
    uint get_num_elements() { return buckets.size() - removed_count; }

  private:
    std::vector<bucket3> buckets;
    std::vector<int> table;
    uint mask;
    uint removed_count;
    uint hash(uint x,uint y,uint z);
    int hfind(uint h,uint x,uint y,uint z);
    bool check_table();

    friend class hash_table3_iterator;
};

class hash_table3_iterator
{
  public:
    hash_table3_iterator(hash_table3 *ht);
    bool is_end() { return (bucket_it == end); }
    void operator ++();
    uint get_x() { return bucket_it->x; }
    uint get_y() { return bucket_it->y; }
    uint get_z() { return bucket_it->z; }
    void set(uint i) { bucket_it = hash_table->buckets.begin() + i; }
    void set_end(uint i) { end = hash_table->buckets.begin() + i; }
  private:
    std::vector<bucket3>::iterator bucket_it;
    std::vector<bucket3>::iterator end;
    hash_table3 *hash_table;
};

#endif
