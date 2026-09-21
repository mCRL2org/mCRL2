// Author(s): Jan Friso Groote. Based on pbes/pbes_rewriter_type.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/pres_rewriter_type.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_PRES_REWRITER_TYPE_H
#define MCRL2_PRES_PRES_REWRITER_TYPE_H

#include <string>
#include "mcrl2/utilities/exception.h"

namespace mcrl2::pres_system
{

/// \brief An enumerated type for the available pres rewriters
enum class pres_rewriter_type
{
  simplify,
  quantifier_all,
  quantifier_finite,
  quantifier_inside,
  quantifier_one_point,
  prune_dataspec
};

/// \brief Parses a pres rewriter type
inline
pres_rewriter_type parse_pres_rewriter_type(const std::string& type)
{
  if (type == "simplify")
  {
    return pres_rewriter_type::simplify;
  }
  if (type == "quantifier-all")
  {
    return pres_rewriter_type::quantifier_all;
  }
  if (type == "quantifier-finite")
  {
    return pres_rewriter_type::quantifier_finite;
  }
  if (type == "quantifier-inside")
  {
    return pres_rewriter_type::quantifier_inside;
  }
  if (type == "quantifier-one-point")
  {
    return pres_rewriter_type::quantifier_one_point;
  }
  if (type == "prune-dataspec")
  {
    return pres_rewriter_type::prune_dataspec;
  }
  throw mcrl2::runtime_error("unknown pres rewriter option " + type);
}

/// \brief Prints a pres rewriter type
inline
std::string print_pres_rewriter_type(const pres_rewriter_type type)
{
  switch (type)
  {
    case pres_rewriter_type::simplify:
      return "simplify";
    case pres_rewriter_type::quantifier_all:
      return "quantifier-all";
    case pres_rewriter_type::quantifier_finite:
      return "quantifier-finite";
    case pres_rewriter_type::quantifier_inside:
      return "quantifier-inside";
    case pres_rewriter_type::quantifier_one_point:
      return "quantifier-one-point";
    case pres_rewriter_type::prune_dataspec:
      return "prune-dataspec";
    default:
      return "unknown pres rewriter";
  }
}

/// \brief Returns a description of a pres rewriter
inline
std::string description(const pres_rewriter_type type)
{
  switch (type)
  {
    case pres_rewriter_type::simplify          :
      return "for simplification";
    case pres_rewriter_type::quantifier_all    :
      return "for eliminating all quantifiers";
    case pres_rewriter_type::quantifier_finite :
      return "for eliminating finite quantifier variables";
    case pres_rewriter_type::quantifier_inside :
      return "for pushing quantifiers inside";
    case pres_rewriter_type::quantifier_one_point :
      return "for one point rule quantifier elimination";
    case pres_rewriter_type::prune_dataspec:
      return "for removing unused data equations and mappings";
  }
  throw mcrl2::runtime_error("unknown pres rewriter");
}

/// \brief Stream operator for rewriter type
/// \param is An input stream
/// \param t A rewriter type
/// \return The input stream
inline
std::istream& operator>>(std::istream& is, pres_rewriter_type& t)
{
  std::string s;
  is >> s;
  try
  {
    t = parse_pres_rewriter_type(s);
  }
  catch (const mcrl2::runtime_error&)
  {
    is.setstate(std::ios_base::failbit);
  }
  return is;
}

inline
std::ostream& operator<<(std::ostream& os, const pres_rewriter_type t)
{
  os << print_pres_rewriter_type(t);
  return os;
}

} // namespace mcrl2::pres_system

#endif // MCRL2_PRES_PRES_REWRITER_TYPE_H
