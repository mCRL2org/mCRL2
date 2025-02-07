// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/linearise.h
/// \brief Linearisation of process specifications

#ifndef MCRL2_LPS_LINEARISE_H
#define MCRL2_LPS_LINEARISE_H

#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/linearisation_method.h"
#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/process/parse.h"

namespace mcrl2
{

namespace lps
{

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
  bool ignore_time;
  bool do_not_apply_constelm;
  bool apply_alphabet_axioms;
  bool balance_summands;      // Used to balance long expressions of the shape p1 + p2 + ... + pn. By default the parser delivers
                              // such expressions in a skewed form, causing stack overflow. 
  mcrl2::data::rewriter::strategy rewrite_strategy;

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
      ignore_time(false),
      do_not_apply_constelm(false),
      apply_alphabet_axioms(false),
      balance_summands(false),              
      rewrite_strategy(mcrl2::data::jitty)
  {}
};

/// \brief Linearises a process specification
/// \param[in] type_checked_spec A process specification
/// \param[in] lin_options options that should be used during linearisation
/// \return An LPS equivalent to spec, which is linearised using lin_options
/// \exception mcrl2::runtime_error Linearisation failed
mcrl2::lps::stochastic_specification linearise(
  const mcrl2::process::process_specification& type_checked_spec,
  const mcrl2::lps::t_lin_options& lin_options = t_lin_options());

/// \brief Linearises a process specification from a textual specification
/// \param[in] text A string containing a process specification
/// \param[in] lin_options options that should be used during linearisation
/// \return An LPS equivalent to the specification representing text, which is linearised using lin_options
/// \exception mcrl2::runtime_error Linearisation failed
inline mcrl2::lps::stochastic_specification linearise(
  const std::string& text,
  const mcrl2::lps::t_lin_options& lin_options = t_lin_options())
{
  mcrl2::process::process_specification spec =
      mcrl2::process::parse_process_specification(text);
  return linearise(spec, lin_options);
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_LINEARISE_H
