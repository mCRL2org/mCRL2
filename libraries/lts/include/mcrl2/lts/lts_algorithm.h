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
 * \brief Algorithms for LTS, such as equivalence reductions, determinisation, etc.
 * \details This contains the main algorithms useful to manipulate with
 * labelled transition systems. Typically, it contains algorithms for bisimulation 
 * reduction, removal of tau loops, making an lts deterministic etc.
 * \author Jan Friso Groote, Bas Ploeger, Muck van Weerdenburg
 */

#ifndef __LIBLTSALGORITHM_H
#define __LIBLTSALGORITHM_H

#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <iostream>
#include <boost/bind.hpp>
#include "aterm2.h"
#include "mcrl2/atermpp/set.h"
#include "mcrl2/lts/lts.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef USE_BCG
#include <bcg_user.h>
#endif

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
    lts_eq_branching_bisim,  /**< Branching bisimulation equivalence */
    lts_eq_divergence_preserving_branching_bisim,  
                             /**< Divergence preserving branching bisimulation equivalence */
    lts_eq_sim,              /**< Strong simulation equivalence */
    lts_eq_trace,            /**< Strong trace equivalence*/
    lts_eq_weak_trace,       /**< Weak trace equivalence */
    lts_red_determinisation, /**< Used for a determinisation reduction */
    lts_equivalence_min = lts_eq_none,
    lts_equivalence_max = lts_red_determinisation,
  };

  /** \brief LTS preorder relations.
   * \details This enumerated type defines preorder relations on LTSs.
   * They can be used to decide whether one LTS is behaviourally
   * contained in another LTS. */
  enum lts_preorder
  {
    lts_pre_none,   /**< Unknown or no preorder */
    lts_pre_sim,    /**< Strong simulation preorder */
    lts_pre_trace,  /**< Strong trace preorder */
    lts_pre_weak_trace,   /**< Weak trace preorder */
    lts_preorder_min = lts_pre_none,
    lts_preorder_max = lts_pre_weak_trace
  };
 
  static const std::set<lts_equivalence> &initialise_supported_lts_equivalences()
  {
    static std::set<lts_equivalence> s;
    for (unsigned int i = lts_equivalence_min; i<1+(unsigned int)lts_equivalence_max; ++i)
    {
      if ( lts_eq_none != (lts_equivalence) i )
      {
        s.insert((lts_equivalence) i);
      }
    }
    return s;
  }

  /** \brief Gives the set of all supported equivalence on LTSs.
   * \return The set of all supported equivalences on LTSs. */
  inline const std::set<lts_equivalence> &supported_lts_equivalences()
  {
    static const std::set<lts_equivalence> &s = initialise_supported_lts_equivalences();
    return s;
  }
  
  static const std::set<lts_preorder> &initialise_supported_lts_preorders()
  {
    static std::set<lts_preorder> s;
    for (unsigned int i = lts_preorder_min; i<1+(unsigned int)lts_preorder_max; ++i)
    {
      if ( lts_pre_none != (lts_preorder) i )
      {
        s.insert((lts_preorder) i);
      }
    }
    return s;
  }

  /** \brief Gives the set of all supported preorders on LTSs.
   * \return The set of all supported preorders on LTSs. */

  inline const std::set<lts_preorder> &supported_lts_preorders()
  {
    static const std::set<lts_preorder> &s = initialise_supported_lts_preorders();
    return s;
  } 


  static std::string equivalence_desc_strings[] = 
  {
    "unknown equivalence",
    "strong bisimilarity",
    "branching bisimilarity",
    "divergence preserving branching bisimilarity",
    "strong simulation equivalence",
    "strong trace equivalence",
    "weak trace equivalence",
    "determinisation reduction"
  };
  
  /** \brief Gives the full name of an equivalence.
   * \param[in] eq The equivalence type.
   * \return The full, descriptive name of the equivalence specified by \a eq. */
  inline std::string name_of_equivalence(const lts_equivalence eq)
  {
    return equivalence_desc_strings[eq];
  }

  static std::string preorder_desc_strings[] = 
  {
    "unknown preorder",
    "strong simulation preorder",
    "strong trace preorder",
    "weak trace preorder"
  };
  
  /** \brief Gives the full name of a preorder.
   * \param[in] pre The preorder type.
   * \return The full, descriptive name of the preorder specified by \a pre. */
  inline std::string name_of_preorder(const lts_preorder pre)
  {
    return preorder_desc_strings[pre];
  }

  /** \brief Determines the equivalence from a string.
   * \details The following strings may be used:
   * \li "bisim" for strong bisimilarity;
   * \li "branching-bisim" for branching bisimilarity;
   * \li "dpbranching-bisim" for divergence preserving branching bisimilarity;
   * \li "sim" for strong simulation equivalence;
   * \li "trace" for strong trace equivalence;
   * \li "weak-trace" for weak trace equivalence;
   *
   * \param[in] s The string specifying the equivalence.
   * \return The equivalence type specified by \a s.
   * If \a s is none of the above values then \a lts_eq_none is returned. */
  inline lts_equivalence parse_equivalence(std::string const& s)
  {
    if ( s == "bisim" )
    {
      return lts_eq_bisim;
    } else if ( s == "branching-bisim" )
    {
      return lts_eq_branching_bisim;
    } else if ( s == "dpbranching-bisim" )
    {
      return lts_eq_divergence_preserving_branching_bisim;
    } else if ( s == "sim" )
    {
      return lts_eq_sim;
    } else if ( s == "trace" )
    {
      return lts_eq_trace;
    } else if ( s == "weak-trace" )
    {
      return lts_eq_weak_trace;
    } else if ( s == "determinisation" )
    {
      return lts_red_determinisation;
    } else {
      return lts_eq_none;
    }
  }
  
  static std::string equivalence_strings[] = 
  {
    "unknown",
    "bisim",
    "branching-bisim",
    "dpbranching-bisim",
    "sim",
    "trace",
    "weak-trace",
    "determinisation"
  };
  
  /** \brief Gives the short name of an equivalence.
   * \param[in] eq The equivalence type.
   * \return A short string representing the equivalence specified by \a
   * eq. The returned value is one of the strings listed for
   * \ref parse_equivalence. */
  inline std::string string_for_equivalence(const lts_equivalence eq)
  {
    return equivalence_strings[eq];
  }

  /** \brief Determines the preorder from a string.
   * \details The following strings may be used:
   * \li "sim" for strong simulation preorder;
   * \li "trace" for strong trace preorder;
   * \li "weak-trace" for weak trace preorder.
   *
   * \param[in] s The string specifying the preorder.
   * \return The preorder type specified by \a s.
   * If \a s is none of the above values then \a lts_pre_none is returned. */
  inline lts_preorder parse_preorder(std::string const& s)
  {
    if ( s == "sim" )
    {
      return lts_pre_sim;
    } else if ( s == "trace" ) {
      return lts_pre_trace;
    } else if ( s == "weak-trace" ) {
      return lts_pre_weak_trace;
    } else {
      return lts_pre_none;
    }
  }
  

  static std::string preorder_strings[] = 
  {
    "unknown",
    "sim",
    "trace",
    "weak-trace"
  };
  
  /** \brief Gives the short name of a preorder.
   * \param[in] pre The preorder type.
   * \return A short string representing the preorder specified by \a
   * pre. The returned value is one of the strings listed for
   * \ref parse_preorder. */
  inline std::string string_for_preorder(const lts_preorder pre)
  {
    return preorder_strings[pre];
  }

  /** \brief Options for equivalence checking/reduction algorithms
   *  \details This class stores options for the algorithms that reduce an LTS
   *  modulo an equivalence or check the equivalence of two LTSs. */
  class lts_eq_options
  {
    public:
      /** \brief Constructor.
       *  \details Creates an object with the options for equivalence checking/reduction
       *  set to their default values.
       */
      lts_eq_options();

      struct {
        /** \brief Indicates whether to add equivalence class
         * information to every state.
         * \details
         * If \a false, the reduction algorithm reduces the LTS by mapping
         * equivalent states onto each other.
         *
         * If \a true, the LTS is \b not reduced. Instead, the algorithm adds a
         * parameter to the state vector of every state. The value of this
         * parameter indicates the equivalence class to which that state belongs.
         *
         * This options only works with strong and branching bisimilarity.
         */
        bool add_class_to_state;
        
      } reduce; /**< \brief Contains the options data.*/
  };
  /** \brief An empty lts_eq_options object */
  extern lts_eq_options lts_eq_no_options;



 /** \brief Gives a textual list describing supported equivalences on LTSs.
  * \param[in] default_equivalence The equivalence that should be marked
  *                                as default (or \a lts_eq_none for no
  *                                default).
  * \param[in] supported           The equivalences that should be
  *                                considered supported.
  * \return                        A string containing lines of the form
  *                                "  'name' for <equivalence>". Every line
  *                                except the last is terminated with '\n'.
  */

 inline std::string supported_lts_equivalences_text(
                       lts_equivalence default_equivalence = lts_eq_none, 
                       const std::set<lts_equivalence> &supported = supported_lts_equivalences())
  {
    std::vector<lts_equivalence> types(supported.begin(),supported.end());
    std::sort(types.begin(),types.end(),boost::bind(lts_named_cmp<lts_equivalence>,equivalence_strings,_1,_2));
  
    std::string r;
    for (std::vector<lts_equivalence>::iterator i=types.begin(); i!=types.end(); i++)
    {
      r += "  '" + equivalence_strings[*i] + "' for " + equivalence_desc_strings[*i];
  
      if ( *i == default_equivalence )
      {
        r += " (default)";
      }
  
  
      if ( i+2 == types.end() )
      {
        r += ", or\n";
      } else if ( i+1 != types.end() )
      {
        r += ",\n";
      }
    }
  
    return r;
  }

 /** \brief Gives a textual list describing supported equivalences on LTSs.
  * \param[in] supported           The equivalences that should be
  *                                considered supported.
  * \return                        A string containing lines of the form
  *                                "  'name' for <equivalence>". Every line
  *                                except the last is terminated with '\n'.
  */
 inline std::string supported_lts_equivalences_text(const std::set<lts_equivalence> &supported)
  {
    return supported_lts_equivalences_text(lts_eq_none,supported);
  }
  

 /** \brief Gives a textual list describing supported preorders on LTSs.
  * \param[in] default_preorder    The preorder that should be marked
  *                                as default (or \a lts_pre_none for no
  *                                default).
  * \param[in] supported           The preorders that should be
  *                                considered supported.
  * \return                        A string containing lines of the form
  *                                "  'name' for <preorder>". Every line
  *                                except the last is terminated with '\n'.
  */
 inline std::string supported_lts_preorders_text(
                     lts_preorder default_preorder = lts_pre_none, 
                     const std::set<lts_preorder> &supported = supported_lts_preorders())
  {
    std::vector<lts_preorder> types(supported.begin(),supported.end());
    std::sort(types.begin(),types.end(),boost::bind(lts_named_cmp<lts_preorder>,preorder_strings,_1,_2));
  
    std::string r;
    for (std::vector<lts_preorder>::iterator i=types.begin(); i!=types.end(); i++)
    {
      r += "  '" + preorder_strings[*i] + "' for " + preorder_desc_strings[*i];
  
      if ( *i == default_preorder )
      {
        r += " (default)";
      }
  
  
      if ( i+2 == types.end() )
      {
        r += ", or\n";
      } else if ( i+1 == types.end() )
      {
        r += ",\n";
      }
    }
  
    return r;
  }
  
 /** \brief Gives a textual list describing supported preorders on LTSs.
  * \param[in] supported           The preorders that should be
  *                                considered supported.
  * \return                        A string containing lines of the form
  *                                "  'name' for <preorder>". Every line
  *                                except the last is terminated with '\n'.
  */
  inline std::string supported_lts_preorders_text(const std::set<lts_preorder> &supported)
  {
    return supported_lts_preorders_text(lts_pre_none,supported);
  } 


 /** \brief Applies a reduction algorithm to this LTS.
  * \param[in] eq The equivalence with respect to which the LTS will be
  * reduced.
  * \param[in] opts The options that will be used for the reduction.
  * \retval true if the reduction succeeded;
  * \retval false otherwise. */
 bool reduce(lts &l, lts_equivalence eq, lts_eq_options const&opts = lts_eq_no_options);

 /** \brief Checks whether this LTS is equivalent to another LTS.
  * \param[in] l The LTS to which this LTS will be compared.
  * \param[in] eq The equivalence with respect to which the LTSs will be
  * compared.
  * \param[in] opts The options that will be used for the comparison.
  * \retval true if the LTSs are found to be equivalent.
  * \retval false otherwise.
  * \warning This function alters the internal data structure of
  * both LTSs for efficiency reasons. After comparison, this LTS is
  * equivalent to the original LTS by equivalence \a eq, and
  * similarly for the LTS \a l.
  */
 bool destructive_compare(lts &l1, lts &l2, const lts_equivalence eq, lts_eq_options const&opts = lts_eq_no_options);

 /** \brief Checks whether this LTS is equivalent to another LTS.
  * \details The input labelled transition systems are duplicated in memory to carry
  *          out the comparison. When space efficiency is a concern, one can consider
  *          to use destructive_compare.
  * \param[in] l The LTS to which this LTS will be compared.
  * \param[in] eq The equivalence with respect to which the LTSs will be
  * compared.
  * \param[in] opts The options that will be used for the comparison.
  * \retval true if the LTSs are found to be equivalent.
  * \retval false otherwise.
  */
 bool compare(const lts &l1, const lts &l2, const lts_equivalence eq, lts_eq_options const&opts = lts_eq_no_options);

 /** \brief Checks whether this LTS is smaller than another LTS according
  * to a preorder. 
  * \param[in] l The LTS to which this LTS will be compared.
  * \param[in] pre The preorder with respect to which the LTSs will be
  * compared.
  * \param[in] opts The options that will be used for the comparison.
  * \retval true if this LTS is smaller than LTS \a l according to
  * preorder \a pre.
  * \retval false otherwise.
  * \warning This function alters the internal data structure of
  * both LTSs for efficiency reasons. After comparison, this LTS is
  * equivalent to the original LTS by equivalence \a eq, and
  * similarly for the LTS \a l, where \a eq is the equivalence
  * induced by the preorder \a pre (i.e. \f$eq = pre \cap
  * pre^{-1}\f$).
  */
 bool destructive_compare(lts &l1, lts &l2, const lts_preorder pre, lts_eq_options const &opts = lts_eq_no_options);

 /** \brief Checks whether this LTS is smaller than another LTS according
  * to a preorder.
  * \param[in] l The LTS to which this LTS will be compared.
  * \param[in] pre The preorder with respect to which the LTSs will be
  * compared.
  * \param[in] opts The options that will be used for the comparison.
  * \retval true if this LTS is smaller than LTS \a l according to
  * preorder \a pre.
  * \retval false otherwise.
  */
 bool compare(const lts &l1, const lts &l2, const lts_preorder pre, lts_eq_options const &opts = lts_eq_no_options);

 /** \brief Determinises this LTS. */
 void determinise(lts &l);

 /** \brief Checks whether all states in this LTS are reachable
  * from the initial state.
  * \details Runs in O(\ref num_states * \ref num_transitions) time.
  * \param[in] remove_unreachable Indicates whether all unreachable states
  *            should be removed from the LTS. This option does not
  *            influence the return value; the return value is with
  *            respect to the original LTS.
  * \retval true if all states are reachable from the initial state;
  * \retval false otherwise. */
 bool reachability_check(lts &l, bool remove_unreachable = false);

 /** \brief Checks whether this LTS is deterministic.
  * \retval true if this LTS is deterministic;
  * \retval false otherwise. */
 bool is_deterministic(const lts &l);

 /** \brief Removes tau cycles by mapping all the states on a cycle to one state.
  *  \details This routine is linear in the number of states and transitions.
  *  \param[in] l Transition system to be reduced.
  *  \param[in] preserve_divergence_loops If true leave a self loop on states that resided on a tau
  *            cycle in the original transition system. 
  */
 void scc_reduce(lts &l,const bool preserve_divergence_loops=false);

}
}

#endif


  
