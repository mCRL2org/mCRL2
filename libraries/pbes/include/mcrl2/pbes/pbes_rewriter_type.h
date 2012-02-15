// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes_rewriter_type.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PBES_REWRITER_TYPE_H
#define MCRL2_PBES_PBES_REWRITER_TYPE_H

#include <iostream>
#include <string>
#include "mcrl2/exception.h"

namespace mcrl2 {

namespace pbes_system {

/// \brief An enumerated type for the available pbes rewriters
enum pbes_rewriter_type
{
  simplify,
  quantifier_all,
  quantifier_finite,
  quantifier_one_point,
  prover,
  pfnf
};

/// \brief Parses a pbes rewriter type
inline
pbes_rewriter_type parse_pbes_rewriter_type(const std::string& type)
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
  throw mcrl2::runtime_error("unknown pbes rewriter option " + type);
}

/// \brief Returns a description of a pbes rewriter
inline
std::string pbes_rewriter_description(pbes_rewriter_type type)
{
  switch (type)
  {
    case simplify          :
      return "  'simplify' for simplification";
    case quantifier_all    :
      return "  'quantifier-all' for eliminating all quantifiers";
    case quantifier_finite :
      return "  'quantifier-finite' for eliminating finite quantifier variables";
    case quantifier_one_point :
      return "  'quantifier-one-point' for one point rule quantifier elimination";
    case prover            :
      return "  'prover' for rewriting using a prover";
    case pfnf              :
      return "  'pfnf' for rewriting into PFNF normal form";
  }
  return "  unknown pbes rewriter";
}

/// \brief Stream operator for rewriter type
/// \param is An input stream
/// \param t A rewriter type
/// \return The input stream
inline
std::istream& operator>>(std::istream& is, pbes_rewriter_type& t)
{
  std::string s;
  is >> s;
  try
  {
    t = parse_pbes_rewriter_type(s);
  }
  catch (mcrl2::runtime_error)
  {
    is.setstate(std::ios_base::failbit);
  }
  return is;
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBES_REWRITER_TYPE_H
