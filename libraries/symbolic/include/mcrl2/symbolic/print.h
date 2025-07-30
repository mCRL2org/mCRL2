// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_LPS_SYMBOLIC_PRINT_H
#define MCRL2_LPS_SYMBOLIC_PRINT_H

#ifdef MCRL2_ENABLE_SYLVAN


#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/undefined.h"
#include "mcrl2/symbolic/print.h"
#include "mcrl2/symbolic/alternative_relprod.h"
#include "mcrl2/symbolic/data_index.h"

#include <sylvan_ldd.hpp>

#include <vector>
#include <cmath>
#include <cfenv>

namespace mcrl2::symbolic {

namespace 
{

/// \brief Converts a state vector of indices to a vector of the corresponding data expressions. 
std::vector<data::data_expression> ldd2state(const std::vector<data_expression_index>& data_index, const std::vector<std::uint32_t>& x)
{
  std::vector<data::data_expression> result;
  for (std::size_t i = 0; i < x.size(); i++)
  {
    if (x[i] == relprod_ignore)
    {
      result.push_back(data::undefined_data_expression());
    }
    else
    {
      result.push_back(data_index[i][x[i]]);
    }
  }
  return result;
}

/// \brief Converts a state vector of indices projected on used to a vector of the corresponding data expressions. 
std::vector<data::data_expression> ldd2state(const std::vector<data_expression_index>& data_index, const std::vector<std::uint32_t>& x, const std::vector<std::size_t>& used)
{
  std::vector<data::data_expression> result;
  for (std::size_t i = 0; i < used.size(); i++)
  {
    if (x[i] == relprod_ignore)
    {
      result.push_back(data::undefined_data_expression());
    }
    else
    {
      result.push_back(data_index[used[i]][x[i]]);
    }
  }
  return result;
}

/// \brief Prints a state vector of indices as a list of the corresponding data expressions. 
inline
std::string print_state(const std::vector<data_expression_index>& data_index, const std::vector<std::uint32_t>& x)
{
  return core::detail::print_list(ldd2state(data_index, x));
}

/// \brief Prints a state vector of indices projected on used as a list of the corresponding data expressions. 
inline
std::string print_state(const std::vector<data_expression_index>& data_index, const std::vector<std::uint32_t>& x, const std::vector<std::size_t>& used)
{
  return core::detail::print_list(ldd2state(data_index, x, used));
}

} // internal

/// \brief Prints the set of state vectors represented by x.
inline
std::string print_states(const std::vector<data_expression_index>& data_index, const sylvan::ldds::ldd& x)
{
  std::ostringstream out;
  auto solutions = ldd_solutions(x);
  bool multiline = solutions.size() > 1;
  std::string sep = multiline ? ",\n" : ", ";

  out << "{" << (multiline ? "\n" : " ");
  for (std::size_t i = 0; i < solutions.size(); i++)
  {
    if (i > 0)
    {
      out << sep;
    }
    out << print_state(data_index, solutions[i]);
  }
  out << (multiline ? "\n" : " ") << "}";
  return out.str();
}

/// \brief Prints the set of state vectors represented by x projected on indices in used.
inline
std::string print_states(const std::vector<data_expression_index>& data_index, const sylvan::ldds::ldd& x, const std::vector<std::size_t>& used)
{
  std::ostringstream out;
  auto solutions = ldd_solutions(x);
  bool multiline = solutions.size() > 1;
  std::string sep = multiline ? ",\n" : ", ";

  out << "{" << (multiline ? "\n" : " ");
  for (std::size_t i = 0; i < solutions.size(); i++)
  {
    if (i > 0)
    {
      out << sep;
    }
    out << print_state(data_index, solutions[i], used);
  }
  out << (multiline ? "\n" : " ") << "}";
  return out.str();
}

namespace 
{

/// \brief Print a transition vector as 'x -> y' where x is the from vector and y the to vector.
/* This funcion is not used. 
std::string print_transition(const std::vector<data_expression_index>& data_index, const std::vector<std::uint32_t>& xy)
{
  std::size_t n = xy.size() / 2;
  std::vector<std::uint32_t> x(n);
  std::vector<std::uint32_t> y(n);
  for (std::size_t i = 0; i < n; i++)
  {
    x[i] = xy[2*i];
    y[i] = xy[2*i+1];
  }
  return print_state(data_index, x) + " -> " + print_state(data_index, y);
}
*/

} // internal

/// \brief Print a transition vector as 'x -> y' where x is the from vector and y the to vector based on the read and write indices.
inline std::string print_transition(const std::vector<data_expression_index>& data_index,
    const std::uint32_t* xy,
    const std::vector<std::size_t>& read,
    const std::vector<std::size_t>& write)
{
  std::vector<std::uint32_t> x;
  std::vector<std::uint32_t> y;
  auto ri = read.begin();
  auto wi = write.begin();
  auto xyi = xy;
  while (ri != read.end() && wi != write.end())
  {
    if (*ri <= *wi)
    {
      ri++;
      x.push_back(*xyi++);
    }
    else
    {
      wi++;
      y.push_back(*xyi++);
    }
  }
  while (ri != read.end())
  {
    ri++;
    x.push_back(*xyi++);
  }
  while (wi != write.end())
  {
    wi++;
    y.push_back(*xyi++);
  }
  return print_state(data_index, x, read) + " -> " + print_state(data_index, y, write);
}

/// \brief Prints a short vector transition relation R explicitly as 'x -> y' for every transition  where x is the from vector and y the to vector based on the read and write indices.
inline std::string print_relation(const std::vector<data_expression_index>& data_index,
    const sylvan::ldds::ldd& R,
    const std::vector<std::size_t>& read,
    const std::vector<std::size_t>& write)
{
  std::ostringstream out;
  for (const std::vector<std::uint32_t>& xy: ldd_solutions(R))
  {
    out << print_transition(data_index, xy.data(), read, write) << std::endl;
  }
  return out.str();
}

/// \brief Prints the number of elements represented by the ldd L and optionally also include the number of nodes of L.
inline std::string print_size(const sylvan::ldds::ldd& L, bool print_exact, bool print_nodecount)
{
  std::ostringstream out;
  if (print_exact)
  {
    // Use this ugly construct to figure out if the conversion succeeded, should have been an exception or sum type.
    std::fenv_t save_env;
    std::feholdexcept(&save_env);

    std::feclearexcept(FE_ALL_EXCEPT);
    long long exact = std::llround(satcount(L));
    if (std::fetestexcept(FE_INVALID))
    {
      //  the result of the rounding is outside the range of the return type.
      out << satcount(L);
    }
    else
    {
      out << exact;
    }
    
    std::feupdateenv(&save_env);
  }
  else
  {
    out << satcount(L);
  }

  if (print_nodecount)
  {
    out << "[" << nodecount(L) << "]";
  }
  return out.str();
}

} // namespace mcrl2::symbolic

#endif // MCRL2_ENABLE_SYLVAN

#endif // MCRL2_LPS_SYMBOLIC_PRINT_H
