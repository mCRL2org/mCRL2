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

namespace mcrl2::lps
{

/// \brief Options for linearisation
struct t_lin_options
{
  t_lin_method lin_method = lmRegular;
  bool no_intermediate_cluster = false;
  bool final_cluster = false;
  bool newstate = false;
  bool binary = false;
  bool statenames = false;
  bool norewrite = false;
  bool noglobalvars = false;
  bool nosumelm = false;
  bool nodeltaelimination = false;
  bool ignore_time = false;
  bool do_not_apply_constelm = false;
  bool apply_alphabet_axioms = false;
  bool balance_summands = false; // Used to balance long expressions of the shape p1 + p2 + ... + pn. By default the
                                 // parser delivers such expressions in a skewed form, causing stack overflow.
  mcrl2::data::rewriter::strategy rewrite_strategy = mcrl2::data::jitty;
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

} // namespace mcrl2::lps

#endif // MCRL2_LPS_LINEARISE_H
