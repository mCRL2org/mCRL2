// Author(s): Jan Friso Groote (based on mcrl2/pbes/pbes_rewriter_type by Wieger Wesselink)
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/lps_rewriter_type.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_LPS_REWRITER_TYPE_H
#define MCRL2_LPS_LPS_REWRITER_TYPE_H

#include "mcrl2/utilities/exception.h"
#include <iostream>
#include <string>

namespace mcrl2 {

namespace lps {

/// \brief An enumerated type for the available lps rewriters
enum lps_rewriter_type
{
  simplify,
  quantifier_all,
  quantifier_finite,
  quantifier_inside,
  quantifier_one_point,
  prover,
  pfnf,
  ppg,
  bqnf_quantifier
};

/// \brief Parses a lps rewriter type
inline
lps_rewriter_type parse_lps_rewriter_type(const std::string& type)
{
  if (type == "simplify")
  {
    return simplify         ;
  }
  if (type == "quantifier-all")
  {
    return quantifier_all   ;
  }
  if (type == "quantifier-finite")
  {
    return quantifier_finite;
  }
  if (type == "quantifier-inside")
  {
    return quantifier_inside;
  }
  if (type == "quantifier-one-point")
  {
    return quantifier_one_point;
  }
  if (type == "prover")
  {
    return prover           ;
  }
  if (type == "pfnf")
  {
    return pfnf             ;
  }
  if (type == "ppg")
  {
    return ppg              ;
  }
  if (type == "bqnf-quantifier")
  {
    return bqnf_quantifier  ;
  }
  throw mcrl2::runtime_error("unknown lps rewriter option " + type);
}

/// \brief Prints a lps rewriter type
inline
std::string print_lps_rewriter_type(const lps_rewriter_type type)
{
  switch (type)
  {
    case simplify:
      return "simplify";
    case quantifier_all:
      return "quantifier-all";
    case quantifier_finite:
      return "quantifier-finite";
    case quantifier_inside:
      return "quantifier-inside";
    case quantifier_one_point:
      return "quantifier-one-point";
    case prover:
      return "prover";
    case pfnf:
      return "pfnf";
    case ppg:
      return "ppg";
    case bqnf_quantifier:
      return "bqnf-quantifier";
    default:
    return "unknown lps rewriter";
  }
}

/// \brief Returns a description of a lps rewriter
inline
std::string description(const lps_rewriter_type type)
{
  switch (type)
  {
    case simplify          :
      return "for simplification";
    case quantifier_all    :
      return "for eliminating all quantifiers";
    case quantifier_finite :
      return "for eliminating finite quantifier variables";
    case quantifier_inside :
      return "for pushing quantifiers inside";
    case quantifier_one_point :
      return "for one point rule quantifier elimination";
    case prover            :
      return "for rewriting using a prover";
    case pfnf              :
      return "for rewriting into PFNF normal form";
    case ppg               :
      return "for rewriting into Parameterised Parity Game form";
    case bqnf_quantifier   :
      return "for rewriting quantifiers over conjuncts to conjuncts of quantifiers (experimental)";
  }
  throw mcrl2::runtime_error("unknown lps rewriter");
}

/// \brief Stream operator for rewriter type
/// \param is An input stream
/// \param t A rewriter type
/// \return The input stream
inline
std::istream& operator>>(std::istream& is, lps_rewriter_type& t)
{
  std::string s;
  is >> s;
  try
  {
    t = parse_lps_rewriter_type(s);
  }
  catch (const mcrl2::runtime_error&)
  {
    is.setstate(std::ios_base::failbit);
  }
  return is;
}

inline
std::ostream& operator<<(std::ostream& os, const lps_rewriter_type t)
{
  os << print_lps_rewriter_type(t);
  return os;
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_LPS_REWRITER_TYPE_H
