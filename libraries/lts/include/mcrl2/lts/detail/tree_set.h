// Author(s): Bas Ploeger
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts/detail/tree_set.h

#ifndef _TREE_SET_H
#define _TREE_SET_H
#include <cstddef>
#include <vector>

namespace mcrl2::lts
{
class tree_set_store
{
  private:
    struct bucket
    {
      ptrdiff_t child_l;
      ptrdiff_t child_r;
      ptrdiff_t tag;
      ptrdiff_t next;
    };
    bucket* buckets;
    ptrdiff_t buckets_size;
    ptrdiff_t buckets_next;

    ptrdiff_t* tags;
    ptrdiff_t tags_size;
    ptrdiff_t tags_next;

    ptrdiff_t* hashtable;
    ptrdiff_t hashmask;

    void check_tags();
    void check_buckets();
    ptrdiff_t find_set(ptrdiff_t child_l,ptrdiff_t child_r);
    ptrdiff_t build_set(ptrdiff_t child_l,ptrdiff_t child_r);
  public:
    tree_set_store();
    ~tree_set_store();
    ptrdiff_t create_set(std::vector<ptrdiff_t> &elems);
    ptrdiff_t get_next_tag();
    ptrdiff_t get_set(ptrdiff_t tag);
    ptrdiff_t get_set_child_left(ptrdiff_t set);
    ptrdiff_t get_set_child_right(ptrdiff_t set);
    ptrdiff_t get_set_size(ptrdiff_t set);
    bool is_set_empty(ptrdiff_t set);
    ptrdiff_t set_set_tag(ptrdiff_t set);
};
}

#endif
