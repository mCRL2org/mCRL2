// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/3rdparty/sylvan/sylvan_mdd.hpp
/// \brief add your file description here.

#ifndef SYLVAN_LDD_HPP
#define SYLVAN_LDD_HPP

#include <string>
#include <vector>

#include <lace.h>
#include <sylvan.h>
#include <sylvan_ldd.h>

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

class Ldd
{
  friend class Sylvan;

  private:
    MDD mdd;

  public:
  Ldd()
    {
      mdd = lddmc_false;
      lddmc_protect(&mdd);
    }

    Ldd(const MDD other)
     : mdd(other)
    {
      lddmc_protect(&mdd);
    }

    Ldd(const Ldd& other)
     : mdd(other.mdd)
    {
      lddmc_protect(&mdd);
    }

    ~Ldd()
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
    Ldd down() const
    {
      return Ldd(lddmc_getdown(mdd));
    }

    // the right edge
    Ldd right() const
    {
      return Ldd(lddmc_getright(mdd));
    }

    bool operator==(const Ldd& other)
    {
      return mdd == other.mdd;
    }
};

// the leaf false
inline Ldd false_()
{
  return Ldd(lddmc_false);
}

// the leaf true
inline Ldd true_()
{
  return Ldd(lddmc_true);
}

// <undocumented>
inline Ldd make_node(std::uint32_t value, const Ldd& down, const Ldd& right)
{
  return Ldd(lddmc_makenode(value, down.get(), right.get()));
}

// <undocumented>
inline Ldd extend(const Ldd& A, std::uint32_t value, const Ldd& B)
{
  return Ldd(lddmc_extendnode(A.get(), value, B.get()));
}

// <undocumented>
Ldd follow(const Ldd& A, std::uint32_t value)
{
  return Ldd(lddmc_follow(A.get(), value));
}

// union(A,B) = A ∪ B
inline Ldd union_(const Ldd& A, const Ldd& B)
{
  LACE_ME;
  return Ldd(lddmc_union(A.get(), B.get()));
}

// minus(A,B) = A \ B
inline Ldd minus(const Ldd& A, const Ldd& B)
{
  LACE_ME;
  return Ldd(lddmc_minus(A.get(), B.get()));
}

// zip(A,B) = (X,Y) such that X = A U B, Y = B \ A
inline std::pair<Ldd, Ldd> zip(const Ldd& A, const Ldd& B)
{
  LACE_ME;
  MDD result[2];
  lddmc_zip(A.get(), B.get(), result);
  return {Ldd(result[0]), Ldd(result[1]) };
}

// project(A,p) = the projection of the set A according to the projection vector p
inline Ldd project(const Ldd& A, const Ldd& p)
{
  LACE_ME;
  return Ldd(lddmc_project(A.get(), p.get()));
}

// project_minus(A,p,B) = minus(project(A,p),B)
inline Ldd project_minus(const Ldd& A, const Ldd& p, const Ldd& B)
{
  LACE_ME;
  return Ldd(lddmc_project_minus(A.get(), p.get(), B.get()));
}

// intersect(A,B) = A ∩ B
inline Ldd intersect(const Ldd& A, const Ldd& B)
{
  LACE_ME;
  return Ldd(lddmc_intersect(A.get(), B.get()));
}

// match(A,B,meta) = A ∩ B, with B defined on subset of the variables of A according to meta
inline Ldd match(const Ldd& A, const Ldd& B, const Ldd& meta)
{
  LACE_ME;
  return Ldd(lddmc_match(A.get(), B.get(), meta.get()));
}

// join(A,B,pA,pB) = A ∩ B, but A and B are projections of the state vector, described by pA and pB
inline Ldd join(const Ldd& A, const Ldd& B, const Ldd& px, const Ldd& py)
{
  LACE_ME;
  return lddmc_join(A.get(), B.get(), px.get(), py.get());
}

// cube(v) = the singleton set containing the tuple with values in v
inline Ldd cube(const std::vector<std::uint32_t>& v)
{
  LACE_ME;
  return lddmc_cube(const_cast<std::uint32_t*>(v.data()), v.size());
}

// member_cube(A,v) = check if cube(v) is in the set A
inline Ldd member_cube(const Ldd& A, const std::vector<std::uint32_t>& v)
{
  LACE_ME;
  return lddmc_member_cube(A.get(), const_cast<std::uint32_t*>(v.data()), v.size());
}

// union_cube(A,v) = union_(A,cube(v))
inline Ldd union_cube(const Ldd& A, const std::vector<std::uint32_t>& v)
{
  LACE_ME;
  return lddmc_union_cube(A.get(), const_cast<std::uint32_t*>(v.data()), v.size());
}

// relprod(A,B,meta) = the successors of the states in A according to the transition relation B which is described by meta
inline Ldd relprod(const Ldd& A, const Ldd& B, const Ldd& meta)
{
  LACE_ME;
  return lddmc_relprod(A.get(), B.get(), meta.get());
}

// relprod_union(A,B,meta) = union_(A,relprod(A,B,meta))
// TODO: the parameter U is undocumented
inline Ldd relprod_union(const Ldd& A, const Ldd& B, const Ldd& meta, const Ldd& U)
{
  LACE_ME;
  return lddmc_relprod_union(A.get(), B.get(), meta.get(), U.get());
}

// relprev(A,B,meta,U) = the predecessors of the states in A according to the transition relation B which is described by meta, restricted to states in U
inline Ldd relprev(const Ldd& A, const Ldd& B, const Ldd& meta, const Ldd& U)
{
  LACE_ME;
  return lddmc_relprev(A.get(), B.get(), meta.get(), U.get());
}

// satcount(A) = the size of the set A
inline double satcount(const Ldd& A)
{
  LACE_ME;
  return lddmc_satcount(A.get());
}

// sat_one(A) = an arbitrary vector from the set A
// TODO: the parameter v and the result are undocumented
inline int sat_one(const Ldd& A, const std::vector<std::uint32_t>& v)
{
  LACE_ME;
  return lddmc_sat_one(A.get(), const_cast<std::uint32_t*>(v.data()), v.size());
}

// sat_all_nopar(A,cb) calls the callback function cb for each vector in A
inline
void sat_all(const Ldd& A, lddmc_enum_cb cb, void* context = nullptr)
{
  LACE_ME;
  lddmc_sat_all_nopar(A.get(), cb, context);
}

// sat_all_par(A,cb) = sat_all(A,cb), but parallelized
inline
void sat_all_nopar(const Ldd& A, lddmc_enum_cb cb, void* context = nullptr)
{
  LACE_ME;
  lddmc_sat_all_nopar(A.get(), cb, context);
}

// collect(A,cb) = sat_all_par(A,cb), but the callback cb returns some set encoded as an LDD, and all returned LDDs are combined using union
// TODO: this is probably wrong, because there are two more undocumented parameters
inline
Ldd collect(const Ldd& A, lddmc_collect_cb cb, void* context = nullptr)
{
  LACE_ME;
  return lddmc_collect(A.get(), cb, context);
}

// match_sat(A,B,m,cb) = sat_all_par(match(A,B,m),cb)
inline
void match_sat(const Ldd& A, const Ldd& B, const Ldd& meta, lddmc_enum_cb cb, void* context = nullptr)
{
  LACE_ME;
  lddmc_match_sat_par(A.get(), B.get(), meta.get(), cb, context);
}

inline
Ldd pick_cube(const Ldd& A)
{
  return Ldd(lddmc_pick_cube(A.get()));
}

inline
void print_dot(const std::string& filename, const Ldd& A)
{
  FILE* out = fopen(filename.c_str(), "w");
  if (!out)
  {
    throw std::runtime_error("Could not open file " + filename);
  }
  lddmc_fprintdot(out, A.get());
}

} // namespace sylvan

#endif // SYLVAN_LDD_OBJ_H
