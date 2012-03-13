// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/linearise.h
/// \brief Linearisation of process specifications

#ifndef MCRL2_LPS_LINEARISE_H
#define MCRL2_LPS_LINEARISE_H

#include <string>
#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/process/process_specification.h"
#include "mcrl2/process/parse.h"

namespace mcrl2
{

namespace lps
{

/// \brief The available linearisation methods
typedef enum { lmStack, lmRegular, lmRegular2 } t_lin_method;

/// \brief String representation of a linearisation method
/// \param[in] lin_method A linerisation method
/// \return
inline
std::string lin_method_to_string(t_lin_method lin_method)
{
  std::string method[] = {"stack","regular","regular2"};
  return method[lin_method];
}

inline
t_lin_method string_to_lin_method(const std::string& s)
{
  if(s == "stack")
  {
    return lmStack;
  }
  else if (s == "regular")
  {
    return lmRegular;
  }
  else if (s == "regular2")
  {
    return lmRegular2;
  }
  else
  {
    throw mcrl2::runtime_error("unknown linearisation strategy " + s);
  }
}

inline
std::istream& operator>>(std::istream& is, t_lin_method& l)
{
  std::string s;
  is >> s;

  try {
    l = string_to_lin_method(s);
  }
  catch (mcrl2::runtime_error&)
  {
    is.setstate(std::ios_base::failbit);
  }

  return is;
}

/// \brief Options for linearisation
struct t_lin_options
{
  t_lin_method lin_method;
  bool no_intermediate_cluster;
  bool final_cluster;
  bool newstate;
  bool binary;
  bool statenames;
  bool norewrite;
  bool noglobalvars;
  bool nosumelm;
  bool nodeltaelimination;
  bool add_delta;
  bool do_not_apply_constelm;
  mcrl2::data::basic_rewriter< mcrl2::data::data_expression >::strategy rewrite_strategy;
  std::string infilename;
  std::string outfilename;

  t_lin_options()
    : lin_method(lmRegular),
      no_intermediate_cluster(false),
      final_cluster(false),
      newstate(false),
      binary(false),
      statenames(false),
      norewrite(false),
      noglobalvars(false),
      nosumelm(false),
      nodeltaelimination(false),
      add_delta(false),
      do_not_apply_constelm(false),
      rewrite_strategy(mcrl2::data::basic_rewriter< mcrl2::data::data_expression >::jitty)
  {}
};

/// \brief Linearises a process specification
/// \param[in] spec A process specification
/// \param[in] lin_options options that should be used during linearisation
/// \return An LPS equivalent to spec, which is linearised using lin_options
/// \exception mcrl2::runtime_error Linearisation failed
mcrl2::lps::specification linearise(
  const mcrl2::process::process_specification& spec,
  mcrl2::lps::t_lin_options lin_options = t_lin_options());

/// \brief Linearises a process specification from a textual specification
/// \param[in] text A string containing a process specification
/// \param[in] lin_options options that should be used during linearisation
/// \return An LPS equivalent to the specification representing text, which is linearised using lin_options
/// \exception mcrl2::runtime_error Linearisation failed
inline mcrl2::lps::specification linearise(
  const std::string& text,
  mcrl2::lps::t_lin_options lin_options = t_lin_options())
{
  mcrl2::process::process_specification spec = mcrl2::process::parse_process_specification(text,true);
  return linearise(spec, lin_options);
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_LINEARISE_H
