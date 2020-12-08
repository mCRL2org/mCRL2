/*
 * Copyright 2011-2016 Formal Methods and Tools, University of Twente
 * Copyright 2016 Tom van Dijk, Johannes Kepler University Linz
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
// Copyright 2020 Wieger Wesselink
//
/// \file mcrl2/3rdparty/sylvan/sylvan_mdd.hpp
/// \brief add your file description here.

#ifndef SYLVAN_LDD_HPP
#define SYLVAN_LDD_HPP

#include <string>
#include <vector>

#include <lace.h>
#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_ldd.h>

extern llmsset_t nodes;

namespace sylvan::ldds
{

// From the PhD thesis of Tom van Dijk:
//
// A list decision diagram is a rooted directed acyclic graph with leaves 0 and
// 1. Each internal node has a value v and two outgoing edges labeled > and =,
// also called the "right" and the "down" edge. Along the "right" edges,
// values v
// are encountered in ascending order. The "down" edge never points to leaf 0
// and the "right" edge never points to leaf 1. Duplicate nodes are forbidden.
// LDD nodes have a property called a level (and its dual, depth), which is
// defined as follows: the root node is at the first level, nodes along
// "right" edges
// stay in the same level, while "down" edges lead to the next level. The depth
// of an LDD node is the number of "down" edges to leaf 1. All maximal paths
// from an LDD node have the same depth.

class ldd
{
  friend class Sylvan;

  private:
    MDD mdd;

  public:
    ldd()
     : mdd(lddmc_false)
    {

      lddmc_protect(&mdd);
    }

    explicit ldd(const MDD other)
     : mdd(other)
    {
      lddmc_protect(&mdd);
    }

    ldd(const ldd& other)
     : mdd(other.mdd)
    {
      lddmc_protect(&mdd);
    }

    ~ldd()
    {
      lddmc_unprotect(&mdd);
    }

    MDD get() const
    {
      return mdd;
    }

    // the value
    std::uint32_t value() const
    {
      return lddmc_getvalue(mdd);
    }

    // the down edge
    ldd down() const
    {
      return ldd(lddmc_getdown(mdd));
    }

    // the right edge
    // invariant: right() != true_()
    ldd right() const
    {
      return ldd(lddmc_getright(mdd));
    }

    bool operator==(const ldd& other) const
    {
      return mdd == other.mdd;
    }

    bool operator!=(const ldd& other) const
    {
      return !(*this == other);
    }
};

// the leaf false
inline ldd false_()
{
  return ldd(lddmc_false);
}

// the leaf true
inline ldd true_()
{
  return ldd(lddmc_true);
}

// create a node with the given value and down and right arrows
//
// The semantics of this operation are defined as follows:
// [{ε}] = {ε}
// [∅] = ∅
// [node(a, n1, n2)] = {a w | w ∈ [n1]} ∪ [n2]
inline ldd node(std::uint32_t value, const ldd& down = true_(), const ldd& right = false_())
{
  return ldd(lddmc_makenode(value, down.get(), right.get()));
}

// The function extend inserts an edge
//
// extend(false, w, z) = false
// extend(true, w, z) = true
// extend(node(v, x, y), w, z) = v < w -> node(v, x, extend(y, w, z))
//                               v = w -> node(w, z, y)
//                               v > w -> node(w, z, node(v, x, y))
inline ldd extend(const ldd& A, std::uint32_t value, const ldd& B)
{
  return ldd(lddmc_extendnode(A.get(), value, B.get()));
}

// The function follow searches for an edge that has a given value as target
//
// follow(false, w) = false
// follow(true, w) = true
// follow(node(v, x, y), w) = v = w -> x
//                            v > w -> false
//                            v < w -> follow(y, w)
ldd follow(const ldd& A, std::uint32_t value)
{
  return ldd(lddmc_follow(A.get(), value));
}

// union(A,B) = A ∪ B
inline ldd union_(const ldd& A, const ldd& B)
{
  LACE_ME;
  return ldd(lddmc_union(A.get(), B.get()));
}

// minus(A,B) = A \ B
inline ldd minus(const ldd& A, const ldd& B)
{
  LACE_ME;
  return ldd(lddmc_minus(A.get(), B.get()));
}

// zip(A,B) = (X,Y) such that X = A U B, Y = B \ A
inline std::pair<ldd, ldd> zip(const ldd& A, const ldd& B)
{
  LACE_ME;
  MDD result[2];
  lddmc_zip(A.get(), B.get(), result);
  return {ldd(result[0]), ldd(result[1]) };
}

// project(A,p) = the projection of the set A according to the projection vector p
inline ldd project(const ldd& A, const ldd& p)
{
  LACE_ME;
  return ldd(lddmc_project(A.get(), p.get()));
}

// project_minus(A,p,B) = minus(project(A,p),B)
inline ldd project_minus(const ldd& A, const ldd& p, const ldd& B)
{
  LACE_ME;
  return ldd(lddmc_project_minus(A.get(), p.get(), B.get()));
}

// intersect(A,B) = A ∩ B
inline ldd intersect(const ldd& A, const ldd& B)
{
  LACE_ME;
  return ldd(lddmc_intersect(A.get(), B.get()));
}

// match(A,B,meta) = A ∩ B, with B defined on subset of the variables of A according to meta
inline ldd match(const ldd& A, const ldd& B, const ldd& meta)
{
  LACE_ME;
  return ldd(lddmc_match(A.get(), B.get(), meta.get()));
}

// join(A,B,pA,pB) = A ∩ B, but A and B are projections of the state vector, described by pA and pB
inline ldd join(const ldd& A, const ldd& B, const ldd& px, const ldd& py)
{
  LACE_ME;
  return ldd(lddmc_join(A.get(), B.get(), px.get(), py.get()));
}

// cube(v) = the singleton set containing the tuple with values in v
inline ldd cube(const std::vector<std::uint32_t>& v)
{
  LACE_ME;
  return ldd(lddmc_cube(const_cast<std::uint32_t*>(v.data()), v.size()));
}

// member_cube(A,v) = check if cube(v) is in the set A
inline ldd member_cube(const ldd& A, const std::vector<std::uint32_t>& v)
{
  LACE_ME;
  return ldd(lddmc_member_cube(A.get(), const_cast<std::uint32_t*>(v.data()), v.size()));
}

// union_cube(A,v) = union_(A,cube(v))
inline ldd union_cube(const ldd& A, const std::vector<std::uint32_t>& v)
{
  LACE_ME;
  return ldd(lddmc_union_cube(A.get(), const_cast<std::uint32_t*>(v.data()), v.size()));
}

// <undocumented>
inline ldd cube_copy(const std::vector<std::uint32_t>& v, const std::vector<int>& copy)
{
  LACE_ME;
  return ldd(lddmc_cube_copy(const_cast<std::uint32_t*>(v.data()), const_cast<int*>(copy.data()), v.size()));
}

// <undocumented>
inline int member_cube_copy(const ldd& A, const std::vector<std::uint32_t>& v, const std::vector<int>& copy)
{
  LACE_ME;
  return lddmc_member_cube_copy(A.get(), const_cast<std::uint32_t*>(v.data()), const_cast<int*>(copy.data()), v.size());
}

// <undocumented>
inline ldd union_cube_copy(const ldd& A, const std::vector<std::uint32_t>& v, const std::vector<int>& copy)
{
  LACE_ME;
  return ldd(lddmc_union_cube_copy(A.get(), const_cast<std::uint32_t*>(v.data()), const_cast<int*>(copy.data()), v.size()));
}

// relprod(A,B,meta) = the successors of the states in A according to the transition relation B which is described by meta
// meta: -1 (end; rest not in rel), 0 (not in rel), 1 (read), 2 (write), 3 (only-read), 4 (only-write), 5 (action label)
inline ldd relprod(const ldd& A, const ldd& B, const ldd& meta)
{
  LACE_ME;
  return ldd(lddmc_relprod(A.get(), B.get(), meta.get()));
}

// relprod_union(A,B,meta) = union_(A,relprod(A,B,meta))
// TODO: the parameter U is undocumented
inline ldd relprod_union(const ldd& A, const ldd& B, const ldd& meta, const ldd& U)
{
  LACE_ME;
  return ldd(lddmc_relprod_union(A.get(), B.get(), meta.get(), U.get()));
}

// relprev(A,B,meta,U) = the predecessors of the states in A according to the transition relation B which is described by meta, restricted to states in U
inline ldd relprev(const ldd& A, const ldd& B, const ldd& meta, const ldd& U)
{
  LACE_ME;
  return ldd(lddmc_relprev(A.get(), B.get(), meta.get(), U.get()));
}

// satcount(A) = the size of the set A
inline double satcount(const ldd& A)
{
  LACE_ME;
  return lddmc_satcount(A.get());
}

// sat_one(A) = an arbitrary vector from the set A
// returns the value of the leaf node (0 or 1)
template <typename OutputIterator>
inline int sat_one(const ldd& A, OutputIterator to)
{
  typedef struct __attribute__((packed)) mddnode
  {
    uint64_t a, b;
  } * mddnode_t; // 16 bytes

  auto LDD_GETNODE = [](MDD mdd)
  {
    return ((mddnode_t)llmsset_index_to_ptr(nodes, mdd));
  };

  auto mddnode_getvalue = [](mddnode_t n)
  {
    return *(uint32_t*)((uint8_t*)n+6);
  };

  auto mddnode_getdown = [](mddnode_t n)
  {
    return n->b >> 17;
  };

  LACE_ME;
  MDD mdd = A.get();
  while (mdd != lddmc_false && mdd != lddmc_true)
  {
    mddnode_t n = LDD_GETNODE(mdd);
    *to++ = mddnode_getvalue(n);
    mdd = mddnode_getdown(n);
  }
  return mdd == lddmc_false ? 0 : 1;
}

inline
std::vector<std::uint32_t> sat_one_vector(const ldd& A)
{
  std::vector<std::uint32_t> result;
  sat_one(A, std::back_inserter(result));
  return result;
}

// sat_all_nopar(A,cb) calls the callback function cb for each vector in A
inline
void sat_all(const ldd& A, lddmc_enum_cb cb, void* context = nullptr)
{
  LACE_ME;
  lddmc_sat_all_nopar(A.get(), cb, context);
}

// sat_all_par(A,cb) = sat_all(A,cb), but parallelized
inline
void sat_all_nopar(const ldd& A, lddmc_enum_cb cb, void* context = nullptr)
{
  LACE_ME;
  lddmc_sat_all_nopar(A.get(), cb, context);
}

// collect(A,cb) = sat_all_par(A,cb), but the callback cb returns some set encoded as an LDD, and all returned LDDs are combined using union
// TODO: this is probably wrong, because there are two more undocumented parameters
inline ldd collect(const ldd& A, lddmc_collect_cb cb, void* context = nullptr)
{
  LACE_ME;
  return ldd(lddmc_collect(A.get(), cb, context));
}

// match_sat(A,B,m,cb) = sat_all_par(match(A,B,m),cb)
inline
void match_sat(const ldd& A, const ldd& B, const ldd& meta, lddmc_enum_cb cb, void* context = nullptr)
{
  LACE_ME;
  lddmc_match_sat_par(A.get(), B.get(), meta.get(), cb, context);
}

inline ldd pick_cube(const ldd& A)
{
  return ldd(lddmc_pick_cube(A.get()));
}

inline
void print_dot(const std::string& filename, const ldd& A)
{
  FILE* out = fopen(filename.c_str(), "w");
  if (!out)
  {
    throw std::runtime_error("Could not open file " + filename);
  }
  lddmc_fprintdot(out, A.get());
}

} // namespace sylvan::ldds

#endif // SYLVAN_LDD_OBJ_H
