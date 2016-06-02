// Author(s): Muck van Weerdenburg, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/** \file
 *
 * \brief Type recording the equivalence reductions supported by the LTS library.
 * \author Jan Friso Groote, Bas Ploeger, Muck van Weerdenburg, Jeroen Keiren
 */

#ifndef MCRL2_LTS_LTS_EQUIVALENCE_H
#define MCRL2_LTS_LTS_EQUIVALENCE_H

#include <iostream>
#include <string>
#include "mcrl2/utilities/exception.h"

namespace mcrl2
{
namespace lts
{

/** \brief LTS equivalence relations.
 * \details This enumerated type defines equivalence relations on
 * LTSs. They can be used to reduce an LTS or decide whether two LTSs
 * are equivalent. */
enum lts_equivalence
{
  lts_eq_none,             /**< Unknown or no equivalence */
  lts_eq_bisim,            /**< Strong bisimulation equivalence */
  lts_eq_bisim_gw,         /**< Strong bisimulation equivalence, using the Groote/Wijs almost-O(m log n) algorithm */
  lts_eq_bisim_gjkw,       /**< Strong bisimulation equivalence, using the Groote/Jansen/Keiren/Wijs O(m log n) algorithm */
  lts_eq_bisim_sigref,     /**< Strong bisimulation equivalence, using signature refinement */
  lts_eq_branching_bisim,  /**< Branching bisimulation equivalence */
  lts_eq_branching_bisim_gw,  /**< Branching bisimulation equivalence, using Groote/Wijs */
  lts_eq_branching_bisim_gjkw,/**< Branching bisimulation equivalence, using Groote/Jansen/Keiren/Wijs */
  lts_eq_branching_bisim_sigref, /**< Branching bisimulation equivalence, using signature refinement */
  lts_eq_divergence_preserving_branching_bisim, /**< Divergence preserving branching bisimulation equivalence */
  lts_eq_divergence_preserving_branching_bisim_gw,    /**< Divergence preserving branching bisimulation equivalence using the almost-O(m log n) Groote/Wijs algorithm */
  lts_eq_divergence_preserving_branching_bisim_gjkw,  /**< Divergence preserving branching bisimulation equivalence using the O(m log n) Groote/Jansen/Keiren/Wijs algorithm */
  lts_eq_divergence_preserving_branching_bisim_sigref, /** Divergence preserving branching bisimulation equivalence using the Blom/Orzan algorithm */
  lts_eq_weak_bisim,  /**< Weak bisimulation equivalence */
  lts_eq_weak_bisim_gw,     /**< Weak bisimulation equivalence, using the Groote/Wijs algorithm */
  lts_eq_weak_bisim_gjkw,   /**< Weak bisimulation equivalence, using the Groote/Jansen/Keiren/Wijs algorithm */
  lts_eq_weak_bisim_sigref, /**< Weak bisimulation equivalence, using signature refinement */
  lts_eq_divergence_preserving_weak_bisim, /**< Divergence preserving weak bisimulation equivalence */
  lts_eq_divergence_preserving_weak_bisim_gw, /**< Divergence preserving weak bisimulation equivalence, using the Groote/Wijs almost-O(m log n) algorithm */
  lts_eq_divergence_preserving_weak_bisim_gjkw, /**< Divergence preserving weak bisimulation equivalence, using the Groote/Jansen/Keiren/Wijs O(m log n) algorithm */
  lts_eq_divergence_preserving_weak_bisim_sigref, /**< Divergence preserving weak bisimulation equivalence, using signature refinement */
  lts_eq_sim,              /**< Strong simulation equivalence */
  lts_eq_trace,            /**< Strong trace equivalence*/
  lts_eq_weak_trace,       /**< Weak trace equivalence */
  lts_red_tau_star,        /**< Tau star reduction */
  lts_red_determinisation /**< Used for a determinisation reduction */
};

/** \brief Determines the equivalence from a string.
 * \details The following strings may be used:
 * \li "none" for identity equivalence;
 * \li "bisim" for strong bisimilarity;
 * \li "branching-bisim" for branching bisimilarity;
 * \li "branching-bisim_gw" for branching bisimilarity using Groote/Wijs;
 * \li "branching-bisim_gjkw" for branching bisimilarity using Groote/Jansen/Keiren/Wijs;
 * \li "dpbranching-bisim" for divergence preserving branching bisimilarity;
 * \li "weak-bisim" for weak bisimilarity;
 * \li "dpweak-bisim" for divergence preserving weak bisimilarity;
 * \li "sim" for strong simulation equivalence;
 * \li "trace" for strong trace equivalence;
 * \li "weak-trace" for weak trace equivalence;
 *
 * \param[in] s The string specifying the equivalence.
 * \return The equivalence type specified by \a s.
 * If \a s is none of the above values then \a lts_eq_none is returned. */
inline
lts_equivalence parse_equivalence(std::string const& s)
{
  if (s == "none")
  {
    return lts_eq_none;
  }
  else if (s == "bisim")
  {
    return lts_eq_bisim;
  }
  else if (s == "bisim-gw")
  {
    return lts_eq_bisim_gw;
  }
  else if (s == "bisim-gjkw")
  {
    return lts_eq_bisim_gjkw;
  }
  else if (s == "bisim-sig")
  {
    return lts_eq_bisim_sigref;
  }
  else if (s == "branching-bisim")
  {
    return lts_eq_branching_bisim;
  }
  else if (s == "branching-bisim-gw")
  {
      return lts_eq_branching_bisim_gw;
  }
  else if (s == "branching-bisim-gjkw")
  {
      return lts_eq_branching_bisim_gjkw;
  }
  else if (s == "branching-bisim-sig")
  {
    return lts_eq_branching_bisim_sigref;
  }
  else if (s == "dpbranching-bisim")
  {
    return lts_eq_divergence_preserving_branching_bisim;
  }
  else if (s == "dpbranching-bisim-gw")
  {
    return lts_eq_divergence_preserving_branching_bisim_gw;
  }
  else if (s == "dpbranching-bisim-gjkw")
  {
    return lts_eq_divergence_preserving_branching_bisim_gjkw;
  }
  else if (s == "dpbranching-bisim-sig")
  {
    return lts_eq_divergence_preserving_branching_bisim_sigref;
  }
  else if (s == "weak-bisim")
  {
    return lts_eq_weak_bisim;
  }
  else if (s == "weak-bisim-gw")
  {
    return lts_eq_weak_bisim_gw;
  }
  else if (s == "weak-bisim-gjkw")
  {
    return lts_eq_weak_bisim_gjkw;
  }
  else if (s == "weak-bisim-sig")
  {
    return lts_eq_weak_bisim_sigref;
  }
  else if (s == "dpweak-bisim")
  {
    return lts_eq_divergence_preserving_weak_bisim;
  }
  else if (s == "dpweak-bisim-gw")
  {
    return lts_eq_divergence_preserving_weak_bisim_gw;
  }
  else if (s == "dpweak-bisim-gjkw")
  {
    return lts_eq_divergence_preserving_weak_bisim_gjkw;
  }
  else if (s == "dpweak-bisim-sig")
  {
    return lts_eq_divergence_preserving_weak_bisim_sigref;
  }
  else if (s == "sim")
  {
    return lts_eq_sim;
  }
  else if (s == "trace")
  {
    return lts_eq_trace;
  }
  else if (s == "weak-trace")
  {
    return lts_eq_weak_trace;
  }
  else if (s == "tau-star")
  {
    return lts_red_tau_star;
  }
  else if (s == "determinisation")
  {
    return lts_red_determinisation;
  }
  else
  {
    throw mcrl2::runtime_error("Unknown equivalence " + s + ".");
  }
}

// \overload
inline
std::istream& operator>>(std::istream& is, lts_equivalence& eq)
{
  try
  {
    std::string s;
    is >> s;
    eq = parse_equivalence(s);
  }
  catch(mcrl2::runtime_error&)
  {
    is.setstate(std::ios_base::failbit);
  }
  return is;
}

/** \brief Gives the short name of an equivalence.
 * \param[in] eq The equivalence type.
 * \return A short string representing the equivalence specified by \a
 * eq. The returned value is one of the strings listed for
 * \ref parse_equivalence. */
inline std::string print_equivalence(const lts_equivalence eq)
{
  switch(eq)
  {
    case lts_eq_none:
      return "none";
    case lts_eq_bisim:
      return "bisim";
    case lts_eq_bisim_gw:
      return "bisim-gw";
    case lts_eq_bisim_gjkw:
      return "bisim-gjkw";
    case lts_eq_bisim_sigref:
      return "bisim-sig";
    case lts_eq_branching_bisim:
      return "branching-bisim";
    case lts_eq_branching_bisim_gw:
      return "branching-bisim-gw";
    case lts_eq_branching_bisim_gjkw:
      return "branching-bisim-gjkw";
    case lts_eq_branching_bisim_sigref:
      return "branching-bisim-sig";
    case lts_eq_divergence_preserving_branching_bisim:
      return "dpbranching-bisim";
    case lts_eq_divergence_preserving_branching_bisim_gw:
      return "dpbranching-bisim-gw";
    case lts_eq_divergence_preserving_branching_bisim_gjkw:
      return "dpbranching-bisim-gjkw";
    case lts_eq_divergence_preserving_branching_bisim_sigref:
      return "dpbranching-bisim-sig";
    case lts_eq_weak_bisim:
      return "weak-bisim";
    case lts_eq_weak_bisim_gw:
      return "weak-bisim-gw";
    case lts_eq_weak_bisim_gjkw:
      return "weak-bisim-gjkw";
    case lts_eq_weak_bisim_sigref:
      return "weak-bisim-sig";
    case lts_eq_divergence_preserving_weak_bisim:
      return "dpweak-bisim";
    case lts_eq_divergence_preserving_weak_bisim_gw:
      return "dpweak-bisim-gw";
    case lts_eq_divergence_preserving_weak_bisim_gjkw:
      return "dpweak-bisim-gjkw";
    case lts_eq_divergence_preserving_weak_bisim_sigref:
      return "dpweak-bisim-sig";
    case lts_eq_sim:
      return "sim";
    case lts_eq_trace:
      return "trace";
    case lts_eq_weak_trace:
      return "weak-trace";
    case lts_red_tau_star:
      return "tau-star";
    case lts_red_determinisation:
      return "deterministation";
    default:
      throw mcrl2::runtime_error("Unknown equivalence.");
  }
}

// \overload
inline
std::ostream& operator<<(std::ostream& os, const lts_equivalence eq)
{
  os << print_equivalence(eq);
  return os;
}

/** \brief Gives a description of an equivalence.
 * \param[in] eq The equivalence type.
 * \return A string describing the equivalence specified by \a
 * eq. */
inline std::string description(const lts_equivalence eq)
{
  switch(eq)
  {
    case lts_eq_none:
      return "identity equivalence";
    case lts_eq_bisim:
      return "strong bisimilarity";
    case lts_eq_bisim_gw:
      return "strong bisimilarity using the almost-O(m log n) Groote/Wijs algorithm";
    case lts_eq_bisim_gjkw:
      return "strong bisimilarity using the Groote/Jansen/Keiren/Wijs O(m log n) algorithm";
    case lts_eq_bisim_sigref:
      return "strong bisimilarity using signature refinement";
    case lts_eq_branching_bisim:
      return "branching bisimilarity";
    case lts_eq_branching_bisim_gw:
      return "branching bisimilarity using the almost-O(m log n) Groote/Wijs algorithm";
    case lts_eq_branching_bisim_gjkw:
      return "branching bisimilarity using the O(m log n) Groote/Keiren/Jansen/Wijs algorithm";
    case lts_eq_branching_bisim_sigref:
      return "branching bisimilarity using signature refinement";
    case lts_eq_divergence_preserving_branching_bisim:
      return "divergence preserving branching bisimilarity";
    case lts_eq_divergence_preserving_branching_bisim_gw:
      return "divergence preserving branching bisimilarity using the almost-O(m log n) Groote/Wijs algorithm";
    case lts_eq_divergence_preserving_branching_bisim_gjkw:
      return "divergence preserving branching bisimilarity using the O(m log n) Groote/Jansen/Keiren/Wijs algorithm";
    case lts_eq_divergence_preserving_branching_bisim_sigref:
      return "divergence preserving branching bisimilarity using signature refinement";
    case lts_eq_weak_bisim:
      return "weak bisimilarity";
    case lts_eq_weak_bisim_gw:
      return "weak bisimilarity using the almost-O(m log n) Groote/Wijs algorithm for strong bisimulation";
    case lts_eq_weak_bisim_gjkw:
      return "weak bisimilarity using the O(m log n) Groote/Jansen/Keiren/Wijs algorithm for strong bisimulation";
    case lts_eq_weak_bisim_sigref:
      return "weak bisimilarity using signature refinement";
    case lts_eq_divergence_preserving_weak_bisim:
      return "divergence preserving weak bisimilarity";
    case lts_eq_divergence_preserving_weak_bisim_gw:
      return "divergence preserving weak bisimilarity based on the O(m log n) Groote/Wijs algorithm for strong bisimulation";
    case lts_eq_divergence_preserving_weak_bisim_gjkw:
      return "divergence preserving weak bisimilarity based on the O(m log n) Groote/Jansen/Keiren/Wijs algorithm for strong bisimulation";
    case lts_eq_divergence_preserving_weak_bisim_sigref:
      return "divergence preserving weak bisimilarity using signature refinement";
    case lts_eq_sim:
      return "strong simulation equivalence";
    case lts_eq_trace:
      return "strong trace equivalence";
    case lts_eq_weak_trace:
      return "weak trace equivalence";
    case lts_red_tau_star:
      return "tau star reduction";
    case lts_red_determinisation:
      return "determinisation reduction";
    default:
      throw mcrl2::runtime_error("Unknown equivalence.");
  }
}

} // namespace lts
} // namespace mcrl2

#endif // MCRL2_LTS_LTS_EQUIVALENCE_H



