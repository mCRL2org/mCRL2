// Author(s): Bas Ploeger
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts/detail/tree_set.h

#ifndef _TREE_SET_H
#define _TREE_SET_H
#include <vector>

namespace mcrl2
{
namespace lts
{
  class tree_set_store {
    private:
      struct bucket {
        size_t child_l;
        size_t child_r;
        size_t tag;
        size_t next;
      };
      bucket *buckets;
      size_t buckets_size;
      size_t buckets_next;

      size_t *tags;
      size_t tags_size;
      size_t tags_next;

      size_t *hashtable;
      size_t hashmask;

      void check_tags();
      void check_buckets();
      size_t find_set(size_t child_l,size_t child_r);
      size_t build_set(size_t child_l,size_t child_r);
    public:
      tree_set_store();
      ~tree_set_store();
      size_t create_set(std::vector<size_t> &elems);
      size_t get_next_tag();
      size_t get_set(size_t tag);
      size_t get_set_child_left(size_t set);
      size_t get_set_child_right(size_t set);
      size_t get_set_size(size_t set);
      bool is_set_empty(size_t set);
      size_t set_set_tag(size_t set);
  };
}
}
#endif
