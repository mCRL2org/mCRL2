// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/prover/solver_type.h
/// \brief Enumeration for the types of solvers

#ifndef MCRL2_DATA_DETAIL_PROVER_SOLVER_TYPE_H
#define MCRL2_DATA_DETAIL_PROVER_SOLVER_TYPE_H

#include <iostream>
#include <cstring>

namespace mcrl2
{
namespace data
{
namespace detail
{

/// \brief The enumaration type smt_solver_type enumerates all available SMT solvers.
enum smt_solver_type
{
  solver_type_cvc,
};

/// \brief standard conversion from stream to solver type
inline
std::istream& operator>>(std::istream& is, smt_solver_type& s)
{
  char solver_type[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  using namespace mcrl2::data::detail;

  /// no standard conversion available function, so implement on-the-spot
  is.readsome(solver_type, 10);

  s = solver_type_cvc;
  if (strncmp(solver_type, "cvc", 3) == 0)
  {
    if (solver_type[3] != '\0')
    {
      is.setstate(std::ios_base::failbit);
    }
  }
  else
  {
    is.setstate(std::ios_base::failbit);
  }

  return is;
}

/// \brief standard conversion from solvert type to stream
inline std::ostream& operator<<(std::ostream& os,smt_solver_type s)
{
  static char const* solvers[] =
  {
    "cvc",
  };

  os << solvers[s];

  return os;
}

} // namespace detail
} // namespace data
} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_PROVER_SOLVER_TYPE_H
