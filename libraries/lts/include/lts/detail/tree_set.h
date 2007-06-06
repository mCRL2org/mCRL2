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
        int child_l;
        int child_r;
        int tag;
        int next;
      };
      bucket *buckets;
      unsigned int buckets_size;
      unsigned int buckets_next;

      int *tags;
      unsigned int tags_size;
      unsigned int tags_next;
      
      int *hashtable;
      unsigned int hashmask;
      
      void check_tags();
      void check_buckets();
      int find_set(int child_l,int child_r);
      int build_set(int child_l,int child_r);
    public:
      tree_set_store();
      ~tree_set_store();
      int create_set(std::vector<unsigned int> &elems);
      unsigned int get_next_tag();
      int get_set(int tag);
      int get_set_child_left(int set);
      int get_set_child_right(int set);
      int get_set_size(int set);
      bool is_set_empty(int set);
      int set_set_tag(int set);
  };
}
}
#endif
