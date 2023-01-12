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
#include "mcrl2/lps/multi_action.h"
#include "mcrl2/symbolic/alternative_relprod.h"
#include "mcrl2/symbolic/data_index.h"

#include <sylvan_bdd.hpp>
#include <sylvan_ldd.hpp>

#include <vector>
#include <cmath>
#include <cfenv>

namespace mcrl2::symbolic {

namespace 
{

/// \brief Converts a state vector of indices to a vector of the corresponding data expressions. 
inline 
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
inline 
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

// Print a container of containers on multiple lines, uses the given str function to obtain a string representation of each container.
template<typename T, typename F>
inline
std::string print_container_multiline(const T& containers, F str)
{
  std::ostringstream out;
  bool multiline = containers.size() > 1;
  std::string sep = multiline ? ",\n" : ", ";

  out << "{" << (multiline ? "\n" : " ");
  for (std::size_t i = 0; i < containers.size(); i++)
  {
    if (i > 0)
    {
      out << sep;
    }

    out << str(containers[i]);
  }
  out << (multiline ? "\n" : " ") << "}";
  return out.str();
}

/// \brief Prints the set of state vectors represented by x.
inline
std::string print_states(const std::vector<data_expression_index>& data_index, const sylvan::ldds::ldd& x)
{
  std::ostringstream out;
  auto solutions = ldd_solutions(x);
  return print_container_multiline(solutions, [&](const auto& container) { return core::detail::print_list(ldd2state(data_index, container)); } );
}

/// \brief Prints the set of state vectors represented by x projected on indices in used.
inline
std::string print_states(const std::vector<data_expression_index>& data_index, const sylvan::ldds::ldd& x, const std::vector<std::size_t>& used)
{
  std::ostringstream out;
  auto solutions = ldd_solutions(x);  
  return print_container_multiline(solutions, [&](const auto& container) { return core::detail::print_list(ldd2state(data_index, container, used)); } );
}

/// \brief Print a transition vector as 'x -> y' where x is the from vector and y the to vector based on the read and write indices.
inline 
std::string print_transition(const std::vector<data_expression_index>& data_index, 
  const utilities::indexed_set<lps::multi_action>& action_index, // No optional references apparently.
  const std::uint32_t* xy, 
  const std::vector<std::size_t>& read, 
  const std::vector<std::size_t>& write,
  bool has_action)
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

  if(has_action)
  {
    // Action always at the end.
    return print_state(data_index, x, read) + " -[" + pp(action_index[*xyi]) + "]-> " + print_state(data_index, y, write);
  }
  else
  {
    return print_state(data_index, x, read) + " -> " + print_state(data_index, y, write);
  }
}

inline 
std::string print_transition(const std::vector<data_expression_index>& data_index, 
  const std::uint32_t* xy, 
  const std::vector<std::size_t>& read, 
  const std::vector<std::size_t>& write)
{
  const utilities::indexed_set<lps::multi_action> action_index;

  std::ostringstream out;
  out << print_transition(data_index, action_index, xy, read, write, false) << std::endl;
  return out.str();  
}


/// \brief Prints a short vector transition relation R explicitly as 'x -> y' for every transition  where x is the from vector and y the to vector based on the read and write indices.
inline 
std::string print_relation(const std::vector<data_expression_index>& data_index, const sylvan::ldds::ldd& R, const std::vector<std::size_t>& read, const std::vector<std::size_t>& write)
{ 
  const utilities::indexed_set<lps::multi_action> action_index;
  std::ostringstream out;
  for (const std::vector<std::uint32_t>& xy: ldd_solutions(R))
  {
    out << print_transition(data_index, action_index, xy.data(), read, write, false) << std::endl;
  }
  return out.str();
}

/// \brief Prints a short vector transition relation R explicitly as 'x -[a]-> y' for every transition  where x is the from vector and y the to vector based on the read and write indices.
inline 
std::string print_relation(const std::vector<data_expression_index>& data_index, const utilities::indexed_set<lps::multi_action>& action_index, const sylvan::ldds::ldd& R, const std::vector<std::size_t>& read, const std::vector<std::size_t>& write)
{
  std::ostringstream out;
  for (const std::vector<std::uint32_t>& xy: ldd_solutions(R))
  {
    out << print_transition(data_index, action_index, xy.data(), read, write, true) << std::endl;
  }
  return out.str();
}

/// \brief Prints the number of elements represented by the ldd L and optionally also include the number of nodes of L.
std::string print_size(const sylvan::ldds::ldd& L, bool print_exact, bool print_nodecount)
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

inline
std::string print_vectors(const sylvan::ldds::ldd& L)
{    
  auto solutions = ldd_solutions(L);
  return print_container_multiline(solutions, [](const auto& container) { return core::detail::print_list(container); } );
}

// BDD related printing functionality.
inline
std::string print_vectors(const sylvan::bdds::bdd& L, const sylvan::bdds::bdd& variables)
{    
  std::ostringstream out;
  auto variables_vector = bdd_variables(variables, variables);
  assert(variables_vector.size() == 1); // Should be a singleton
  out << "variables: " << core::detail::print_list(variables_vector[0]);

  auto solutions = bdd_solutions(L, variables);
  return print_container_multiline(solutions, [](const auto& container) { return core::detail::print_list(container); } );
}

/// \brief Convert a bit sequence encoded in most significant first into a value.
inline std::uint32_t bits_to_value(const std::vector<std::uint32_t>& bits, std::uint32_t number_of_bits, std::uint32_t offset)
{
  std::uint32_t value = 0;
  for (std::uint32_t i = 0; i < number_of_bits; ++i)
  {
    value |= bits[i + offset] * 1 << (number_of_bits - i - 1); // Write every bit in the value based on their index and whether it is true in the BDD. (most significant bit first)
  }

  return value;
}

/// \brief Convert a bit sequence encoded in least significant first into a value.
inline std::uint32_t bits_to_value_lsb(const std::vector<std::uint32_t>& bits, std::uint32_t number_of_bits, std::uint32_t offset)
{
  std::uint32_t value = 0;
  for (std::uint32_t i = 0; i < number_of_bits; ++i)
  {
    value |= bits[i + offset] * 1 << i; // Write every bit in the value based on their index and whether it is true in the BDD. (least significant bit first)
  }

  return value;
}

// BDD related printing functionality.
inline
std::string print_states(const std::vector<data_expression_index>& data_index, const sylvan::bdds::bdd& L, const sylvan::bdds::bdd& variables, const std::vector<std::uint32_t>& bits)
{    
  std::ostringstream out;
  auto solutions = bdd_solutions(L, variables);
  std::vector<std::vector<data::data_expression>> vectors;

  for (const auto& solution : solutions)
  {
    std::vector<std::uint32_t> result;

    int offset  = 0;
    for (std::uint32_t number_of_bits : bits)
    {
      result.emplace_back(bits_to_value(solution, number_of_bits, offset));
      offset += number_of_bits;
    }

    vectors.emplace_back(ldd2state(data_index, result));
  }

  return print_container_multiline(vectors, [](const auto& container) { return core::detail::print_list(container); });
}

/// \brief Print a transition vector as 'x -> y' where x is the from vector and y the to vector based on the read and write indices.
inline 
std::string print_transition(const std::vector<data_expression_index>& data_index, 
  const utilities::indexed_set<lps::multi_action>& action_index, // No optional references apparently.
  const std::vector<std::uint32_t>& xy,  
  const std::vector<std::uint32_t>& bits,
  std::uint32_t bits_action_label,
  const std::vector<std::size_t>& read, 
  const std::vector<std::size_t>& write,
  bool has_action)
{
  std::vector<std::uint32_t> x;
  std::vector<std::uint32_t> y;
  auto ri = read.begin();
  auto wi = write.begin();
  std::size_t offset = 0;
  while (ri != read.end() && wi != write.end())
  {
    if (*ri <= *wi)
    {
      x.push_back(bits_to_value(xy, bits[*ri], offset));
      offset += bits[*ri];
      ri++;
    }
    else
    {
      y.push_back(bits_to_value(xy, bits[*wi], offset));
      offset += bits[*wi];
      wi++;
    }
  }
  while (ri != read.end())
  {
      x.push_back(bits_to_value(xy, bits[*ri], offset));
      offset += bits[*ri];
      ri++;
  }
  while (wi != write.end())
  {
      y.push_back(bits_to_value(xy, bits[*wi], offset));
      offset += bits[*wi];
      wi++;
  }
  
  if(has_action)
  {
    // Action always at the end.
    return print_state(data_index, x, read) + " -[" + pp(action_index[bits_to_value(xy, bits_action_label, offset)]) + "]-> " + print_state(data_index, y, write);
  }
  else
  {
    return print_state(data_index, x, read) + " -> " + print_state(data_index, y, write);
  }
}

/// \brief Prints a short vector transition relation R explicitly as 'x -> y' for every transition  where x is the from vector and y the to vector based on the read and write indices.
inline 
std::string print_relation(const std::vector<data_expression_index>& data_index,
  const sylvan::bdds::bdd& R, 
  const sylvan::bdds::bdd& variables,
  const std::vector<std::uint32_t>& bits,
  const std::vector<std::size_t>& read, 
  const std::vector<std::size_t>& write)
{ 
  const utilities::indexed_set<lps::multi_action> action_index;
  std::ostringstream out;
  for (const std::vector<std::uint32_t>& xy: bdd_solutions(R, variables))
  {
    out << print_transition(data_index, action_index, xy, bits, 0, read, write, false) << std::endl;
  }
  return out.str();
}

/// \brief Prints a short vector transition relation R explicitly as 'x -[a]-> y' for every transition  where x is the from vector and y the to vector based on the read and write indices.
inline 
std::string print_relation(const std::vector<data_expression_index>& data_index, 
  const utilities::indexed_set<lps::multi_action>& action_index, 
  const sylvan::bdds::bdd& R, 
  const sylvan::bdds::bdd& variables,
  const std::vector<std::uint32_t>& bits,
  std::uint32_t bits_action_label,
  const std::vector<std::size_t>& read, 
  const std::vector<std::size_t>& write)
{
  std::ostringstream out;
  for (const std::vector<std::uint32_t>& xy: bdd_solutions(R, variables))
  {
    out << print_transition(data_index, action_index, xy, bits, bits_action_label, read, write, true) << std::endl;
  }
  return out.str();
}


/// \brief Prints the number of elements represented by the BDD L and optionally also include the number of nodes of L.
inline 
std::string print_size(const sylvan::bdds::bdd& L, const sylvan::bdds::bdd& variables, bool print_nodecount)
{
  std::ostringstream out;
  out << sylvan::bdds::satcount(L, variables);
  if (print_nodecount)
  {
    out << "[" << L.node_count() << "]";
  }
  return out.str();
}

} // namespace mcrl2::symbolic

#endif // MCRL2_ENABLE_SYLVAN

#endif // MCRL2_LPS_SYMBOLIC_PRINT_H
