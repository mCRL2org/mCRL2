// Author(s): Bas Ploeger
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tree_set.cpp

#include <cstdlib>
#include <vector>
#include <boost/scoped_array.hpp>
#include "mcrl2/core/messaging.h"
#include "mcrl2/lts/detail/tree_set.h"
#include "mcrl2/exception.h"

using namespace mcrl2::core;

#define EMPTY_SET (-1)
#define EMPTY_LIST (-1)
#define EMPTY_TAG (-1)
#define HASH_CLASS 16
#define TAGS_BLOCK 15000
#define BUCKETS_BLOCK 25000
// simple hash function; uses two large primes
#define hash(l,r,m) (36425657*l + 77673689*r) & m

using namespace std;

namespace mcrl2
{
namespace lts
{

tree_set_store::tree_set_store() {
  buckets = NULL;
  buckets_size = 0;
  buckets_next = 0;

  tags = NULL;
  tags_size = 0;
  tags_next = 0;

  hashmask = (1 << HASH_CLASS) - 1;
  hashtable = (int*)malloc((hashmask+1)*sizeof(int));
  if (hashtable == NULL) {
    throw mcrl2::runtime_error("Out of memory.");
  }
  for (unsigned int i=0; i<=hashmask; ++i) {
  	hashtable[i] = EMPTY_LIST;
  }
}

tree_set_store::~tree_set_store() {
  if (tags != NULL) {
    free(tags);
    tags = NULL;
  }
  if (buckets != NULL) {
    free(buckets);
    buckets = NULL;
  }
  free(hashtable);
  hashtable = NULL;
}

void tree_set_store::check_tags() {
  if (tags_next >= tags_size) {
    tags_size += TAGS_BLOCK;
    tags = (int*)realloc(tags,tags_size*sizeof(int));
    if (tags == NULL) {
      throw mcrl2::runtime_error("Out of memory.");
    }
  }
}

void tree_set_store::check_buckets() {
  if (buckets_next >= buckets_size) {
    buckets_size += BUCKETS_BLOCK;
    buckets = (bucket*)realloc(buckets,buckets_size*sizeof(bucket));
    if (buckets == NULL) {
      throw mcrl2::runtime_error("Out of memory.");
    }
  }
  if (buckets_next*4 >= hashmask*3) {
    hashmask = hashmask + hashmask + 1;
    hashtable = (int*)realloc(hashtable,(hashmask+1)*sizeof(int));
    if (hashtable == NULL) {
      throw mcrl2::runtime_error("Out of memory.");
    }
    unsigned int i,hc;
    for (i=0; i<=hashmask; ++i) {
      hashtable[i] = EMPTY_LIST;
    }
    for (i=0; i<buckets_next; ++i) {
      hc = hash(buckets[i].child_l,buckets[i].child_r,hashmask);
      buckets[i].next = hashtable[hc];
      hashtable[hc] = i;
    }
  }
}

int tree_set_store::build_set(int child_l,int child_r) {
	check_buckets();
	int hc = hash(child_l,child_r,hashmask);
	buckets[buckets_next].child_l = child_l;
	buckets[buckets_next].child_r = child_r;
	buckets[buckets_next].tag	    = EMPTY_TAG;
	buckets[buckets_next].next    = hashtable[hc];
	hashtable[hc] = buckets_next;
	return buckets_next++;
}

int tree_set_store::find_set(int child_l,int child_r) {
	int hc = hash(child_l,child_r,hashmask);
	for (int i=hashtable[hc]; i!=EMPTY_LIST; i=buckets[i].next) {
		if (buckets[i].child_l==child_l && buckets[i].child_r==child_r) {
			return i;
		}
	}
	return build_set(child_l,child_r);
}

int tree_set_store::create_set(vector<unsigned int> &elems) {
	if (elems.size() == 0) {
		return EMPTY_SET;
	}
        boost::scoped_array< int > nodes(new int[elems.size()]);
	unsigned int node_size = 0;
	unsigned int i,j;
	for (i=0; i < elems.size(); ++i) {
		nodes[i] = find_set(elems[i],EMPTY_SET);
	}
	node_size = i;
	while (node_size > 1) {
		j = 0;
		for (i=0; i<node_size; i+=2) {
			if (i == node_size-1) {
				nodes[j] = nodes[i];
			}
			else {
				nodes[j] = find_set(nodes[i],nodes[i+1]);
			}
			++j;
		}
		node_size = j;
	}
	unsigned int r = nodes[0];
	return r;
}

unsigned int tree_set_store::get_next_tag() {
  return tags_next;
}

int tree_set_store::get_set(int tag) {
  return tags[tag];
}

int tree_set_store::get_set_child_left(int set) {
  return buckets[set].child_l;
}

int tree_set_store::get_set_child_right(int set) {
  return buckets[set].child_r;
}

int tree_set_store::get_set_size(int set) {
  if (buckets[set].child_r == EMPTY_SET) {
    return 1;
  }
 	return get_set_size(buckets[set].child_l) +
 				 get_set_size(buckets[set].child_r);
}

bool tree_set_store::is_set_empty(int set) {
  return (set == EMPTY_SET);
}

int tree_set_store::set_set_tag(int set) {
  if (buckets[set].tag != EMPTY_TAG) {
    return buckets[set].tag;
  }
  check_tags();
  tags[tags_next] = set;
  buckets[set].tag = tags_next;
  return tags_next++;
}

}
}
