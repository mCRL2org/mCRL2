/*
 * Copyright 2011-2015 Formal Methods and Tools, University of Twente
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

/* Copyright 2020 Wieger Wesselink, Eindhoven University of Technology
 * Add an Mdd class
 */

#ifndef SYLVAN_MDD_OBJ_H
#define SYLVAN_MDD_OBJ_H

#include <string>
#include <vector>

#include <lace.h>
#include <sylvan.h>
#include <sylvan_ldd.h>

namespace sylvan
{

class Mdd
{
  friend class Sylvan;

  private:
    MDD mdd;

  public:
    Mdd()
    {
      mdd = lddmc_false;
      lddmc_protect(&mdd);
    }

    Mdd(const MDD other)
     : mdd(other)
    {
      lddmc_protect(&mdd);
    }

    Mdd(const Mdd& other)
     : mdd(other.mdd)
    {
      lddmc_protect(&mdd);
    }

    ~Mdd()
    {
      lddmc_unprotect(&mdd);
    }

    MDD get() const
    {
      return mdd;
    }

    std::uint32_t value() const
    {
      return lddmc_getvalue(mdd);
    }

    Mdd down() const
    {
      return Mdd(lddmc_getdown(mdd));
    }

    Mdd right() const
    {
      return Mdd(lddmc_getright(mdd));
    }

    Mdd follow(std::uint32_t value) const
    {
      return Mdd(lddmc_follow(mdd, value));
    }
};

inline
Mdd mdd_zero()
{
  return Mdd(lddmc_false);
}

inline
Mdd mdd_one()
{
  return Mdd(lddmc_true);
}

inline
Mdd make_node(std::uint32_t value, const Mdd& x, const Mdd& y)
{
  return Mdd(lddmc_makenode(value, x.get(), y.get()));
}

inline
Mdd extend(const Mdd& x, std::uint32_t value, const Mdd& y)
{
  return Mdd(lddmc_extendnode(x.get(), value, y.get()));
}

inline
Mdd union_(const Mdd& x, const Mdd& y)
{
  LACE_ME;
  return Mdd(lddmc_union(x.get(), y.get()));
}

inline
Mdd intersection(const Mdd& x, const Mdd& y)
{
  LACE_ME;
  return Mdd(lddmc_intersect(x.get(), y.get()));
}

inline
Mdd minus(const Mdd& x, const Mdd& y)
{
  LACE_ME;
  return Mdd(lddmc_minus(x.get(), y.get()));
}

//template <typename OutputIterator>
//void zip(const Mdd& x, const Mdd& y, OutputIterator result)
//{
//  LACE_ME;
//  lddmc_zip(x.get(), y.get(), result);
//}

inline
Mdd match(const Mdd& x, const Mdd& y, const Mdd& proj)
{
  LACE_ME;
  return Mdd(lddmc_match(x.get(), y.get(), proj.get()));
}

inline
long double sat_count(const Mdd& x)
{
  LACE_ME;
  return lddmc_satcount(x.get());
}

inline
Mdd pick_solution(const Mdd& x)
{
  return Mdd(lddmc_sat_one_mdd(x.get()));
}

} // namespace sylvan

#endif // SYLVAN_LDD_OBJ_H
