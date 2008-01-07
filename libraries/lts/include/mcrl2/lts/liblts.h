// Author(s): Muck van Weerdenburg
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/** \file
 *
 * \brief The main header file of the LTS library.
 * \details This is the LTS library's main header file. It declares all publicly
 * accessible data structures that form the interface of the library.
 * \author Muck van Weerdenburg
 */

#ifndef __LIBLTS_H
#define __LIBLTS_H

#include <string>
#include <vector>
#include <iostream>
#include <aterm2.h>
#include "mcrl2/lps/specification.h"
#include "mcrl2/setup.h"

#ifdef MCRL2_BCG
#include "bcg_user.h"
#endif

namespace mcrl2
{
/** \brief The main LTS namespace.
 * \details This namespace contains all data structures and members of the LTS
 * library.
 */
namespace lts
{
  /** \brief LTS types/formats.
   * \details This enumerated type defines the various formats in which an LTS
   * can be stored.
   * */
  enum lts_type
  { 
    lts_none,  /**< unknown or no format */ 
    lts_mcrl2, /**< mCRL2 SVC format */
    lts_aut,   /**< Ald&eacute;baran format (CADP) */
    lts_mcrl,  /**< muCRL SVC format */
    lts_svc,   /**< SVC format */
    lts_fsm,   /**< FSM format */
    lts_dot    /**< GraphViz format */
#ifdef MCRL2_BCG
   ,lts_bcg /**< BCG format
              * \note Only available if the LTS library is built with BCG
              * support.*/
#endif
  };

  /** \brief LTS equivalence relations.
   * \details This enumerated type defines equivalence relations on LTSs.
   * They can be used to reduce an LTS or decide whether two LTSs are equivalent.
   */
  enum lts_equivalence
  {
    lts_eq_none,        /**< unknown or no equivalence */
    lts_eq_trace,       /**< Trace equivalence*/
    lts_eq_strong,      /**< Strong bisimulation equivalence */ 
    lts_eq_weak_trace,  /**< Weak trace equivalence */ 
    lts_eq_branch,      /**< Branching bisimulation equivalence */ 
    lts_eq_isomorph     /**< Isomorphism */
  };

  /** \brief Options for equivalence checking/reduction algorithms
   *  \details This class stores options for the algorithms that reduce an LTS
   *  modulo an equivalence or check the equivalence of two LTSs.
   */
  class lts_eq_options
  {
    public:
      /** Creates an object with the options for equivalence checking/reduction
       *  set to their default values.
       */
      lts_eq_options();

      struct {
        /**
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
        /** \details The vector of transition labels that the reduction algorithm
         * considers to be internal actions. (Besides those that are already
         * marked as being internal.)
         */
        std::vector<std::string> tau_actions;
      } reduce;
  };
  /** An empty lts_eq_options object */
  extern lts_eq_options lts_eq_no_options;

  /** \brief Typed of additional information for LTS operations.
   * \details This enumerated type defines types for additional information for
   * LTS operations.
   * \sa lts_extra
   */
  enum lts_extra_type
  {
    le_nothing, /**< no additional format */
    le_mcrl1,   /**< muCRL format */
    le_mcrl2,   /**< mCRL2 format */
    le_dot      /**< Dot format (GraphViz) */
  };

  /** \brief Options for the Dot format.
   * \details This struct stores options for the Dot format of the GraphViz
   * package.
   */
  typedef struct
  { 
    /** Points to the name that is given to the graph in the Dot file. */
    std::string const *name;
    /** If \a true, the states in the Dot file are labelled by the state values,
     * if available. 
     *
     * If \a false, the states are not labelled. */
    bool print_states;
  } lts_dot_options;

  /** Adds transition labels to the list of labels that are considered to be
   * internal actions.
   * \param[in,out] opts The object of which the list of internal actions will be
   * changed.
   * \param[in] act_names A string containing a comma-separated list of
   * transition labels that will be added to the list of internal actions.
   */
  void lts_reduce_add_tau_actions(lts_eq_options &opts, std::string act_names);

  /** Checks whether an action is a timed mCRL2 action.
   * \param[in] t The action for which the check has to be performed.
   * \retval true if the action is timed;
   * \retval false otherwise.
   */
  bool is_timed_pair(ATermAppl t);

  /** Creates a timed mCRL2 action.
   * \param[in] action The action part of the timed action that will be created.
   * \param[in] time The time part of the timed action that will be created.
   * \return A timed action with label \a action and time \a time.
   */
  ATermAppl make_timed_pair(ATermAppl action, ATermAppl time = NULL);
  
  /** \cond */
  #include "detail/liblts_private.h"
  /** \endcond */
  
  /** \brief Stores additional LTS information.
   * \details This class is provided for storing additional information with an
   * LTS. This can be either a muCRL specification, an mCRL2 specificiation, or
   * options for the Dot format.
   */
  class lts_extra : p_lts_extra
  {
    public:
      /** Creates an object containing no information. */
      lts_extra();
      /** Creates an object containing a muCRL specification.
       * \param[in] t The muCRL specification that will be stored in the object. */
      lts_extra(ATerm t);
      /** Creates an object containing an mCRL2 specification.
       * \param[in] spec The mCRL2 specification that will be stored in the object. */
      lts_extra(lps::specification *spec);
      /** Creates an object containing options for the Dot format.
       * \param[in] opts The options that will be stored in the object. */
      lts_extra(lts_dot_options opts);

      /** Gets the type of information that is stored in this object.
       * \retval le_nothing if this object contains no information;
       * \retval le_mcrl1 if this object contains a muCRL specification;
       * \retval le_mcrl2 if this object contains an mCRL2 specification;
       * \retval le_dot if this object contains options for the Dot format. */
      lts_extra_type get_type();
      /** Gets the muCRL specification stored in this object. 
       * \pre This object contains a muCRL specification.
       * \return The muCRL specification stored in this object.*/
      ATerm get_mcrl1_spec();
      /** Gets the mCRL2 specification stored in this object. 
       * \pre This object contains an mCRL2 specification.
       * \return The mCRL2 specification stored in this object.*/
      lps::specification *get_mcrl2_spec();
      /** Gets the Dot format options stored in this object. 
       * \pre This object contains options for the Dot format.
       * \return The Dot format options stored in this object.*/
      lts_dot_options get_dot_options();
  };
  /** An empty lts_extra object */
  extern lts_extra lts_no_extra;

  class lts;

  /** \brief Iterator over states of an LTS.
   * \details This class defines an iterator type for iterating over the states
   * of an LTS. */
  class state_iterator : p_state_iterator
  {
    public:
      /** Creates a state iterator for an LTS.
       * \param[in] l The LTS for which the state iterator will be created. */
      state_iterator(lts *l);

      /** Checks whether there are more states to iterate over.
       * \retval false if this iterator points to the last state of the LTS;
       * \retval true otherwise.*/
      bool more();

      /** Dereferences the iterator.
       * \return The state to which this iterator points.*/
      unsigned int operator *();
      /** Increments the iterator. */
      void operator ++();
  };

  /** \brief Iterator over transition labels of an LTS.
   * \details This class defines an iterator type for iterating over the
   * transition labels of an LTS. */
  class label_iterator : p_label_iterator
  {
    public:
      /** Creates a transition label iterator for an LTS.
       * \param[in] l The LTS for which the transition label iterator will be
       * created. */
      label_iterator(lts *l);
      
      /** Checks whether there are more transition labels to iterate over.
       * \retval false if this iterator points to the last transition label of
       * the LTS;
       * \retval true otherwise.*/
      bool more();

      /** Dereferences the iterator.
       * \return The transition label to which this iterator points.*/
      unsigned int operator *();
      /** Increments the iterator. */
      void operator ++();
  };

  /** \brief Iterator over transitions of an LTS.
   * \details This class defines an iterator type for iterating over the
   * transitions of an LTS. */
  class transition_iterator : p_transition_iterator
  {
    public:
      /** Creates a transition iterator for an LTS.
       * \param[in] l The LTS for which the transition iterator will be created.
       */
      transition_iterator(lts *l);
      
      /** Checks whether there are more transitions to iterate over.
       * \retval false if this iterator points to the last transition of the
       * LTS;
       * \retval true otherwise.*/
      bool more();

      /** Gets the transition's source state.
       * \return The source state of the transition to which this iterator points.*/
      unsigned int from();
      /** Gets the transition's label.
       * \return The label of the transition to which this iterator points.*/
      unsigned int label();
      /** Gets the transition's target state.
       * \return The target state of the transition to which this iterator points.*/
      unsigned int to();

      /** Increments the iterator. */
      void operator ++();
  };

  /** \brief Class for labelled transition systems.
   * \details This class provides an interface for creating and manipulating
   * labelled transition systems. */
  class lts : p_lts
  {
    public:

      /** Determines the LTS format from a filename by its extension.
       * \param[in] s The name of the file of which the format will be
       * determined.
       * \return The LTS format based on the extension of \a s.
       * If the extension is \p svc then the mCRL2 SVC format is assumed and
       * \a lts_mcrl2 is returned.
       * If no supported format can be determined from the extension then \a
       * lts_none is returned.  */
      static lts_type guess_format(std::string const& s);

      /** Determines the LTS format from a format specification string. This can
       * be any of the following strings:
       * \li "aut" for the Ald&eacute;baran format;
       * \li "mcrl" or "svc+mcrl" for the muCRL SVC format;
       * \li "mcrl2" or "svc+mcrl2" for the mCRL2 SVC format;
       * \li "svc" for the SVC format;
       * \li "fsm" for the FSM format;
       * \li "dot" for the GraphViz format;
       * \li "bcg" for the BCG format (only available if the LTS library is built
       * with BCG support).
       * \param[in] s The format specification string.
       * \return The LTS format based on the value of \a s.
       * If no supported format can be determined then \a lts_none is returned.
       */
      static lts_type parse_format(char const* s);

      /** Gives a string representation of an LTS format. (This is the "inverse"
       * of parse_format.)
       * \param[in] s The LTS format.
       * \return The name of the LTS format specified by \a s. */
      static char const* string_for_type(const lts_type s);

      /** Gives the filename extension associated with an LTS format.
       * \param[in] type The LTS format.
       * \return The filename extension of the LTS format specified by \a type.
       */
      static char const* extension_for_type(const lts_type type);

      /** Determines the equivalence from a string. The following strings may be
       * used:
       * \li "strong" for strong bisimilarity;
       * \li "branch" for branching bisimilarity;
       * \li "trace" for trace equivalence;
       * \li "wtrace" for weak trace equivalence;
       * \li "isomorph" for isomorphism.
       * \param[in] s The string specifying the equivalence.
       * \return The equivalence type specified by \a s.
       * If \a s is none of the above values then \a lts_eq_none is returned.
       */
      static lts_equivalence parse_equivalence(char const* s);
      
      /** Gives the short name of an equivalence.
       * \param[in] s The equivalence type.
       * \return A short string representing the equivalence specified by \a
       * s. The returned value is one of the strings listed for
       * \ref parse_equivalence. */
      static char const* string_for_equivalence(const lts_equivalence s);

      /** Gives the full name of an equivalence.
       * \param[in] s The equivalence type.
       * \return The full, descriptive name of the equivalence specified by \a s. */
      static char const* name_of_equivalence(const lts_equivalence s);

    public:
      /** Creates an empty LTS.
       * \param[in] type The LTS format.
       * \param[in] state_info Indicates whether state parameter information
       * will be present.
       * \param[in] label_info Indicates whether label information will be
       * present. */
      lts(lts_type type = lts_mcrl2, bool state_info = true, bool label_info = true);
      /** Creates an LTS and reads its data from a file.
       * \param[in] filename The name of the file from which the data will be
       * read.
       * \param[in] type The format of the file. If \a lts_none is passed then
       * an attempt is made to determine the format from the contents of the
       * file. */
      lts(std::string &filename, lts_type type = lts_none);
      /** Creates an LTS and reads its data from an input stream.
       * \param[in] is The input stream from which the data will be read.
       * \param[in] type The format of the data. If \a lts_none is passed then
       * an attempt is made to determine the format from the contents of the
       * stream. */
      lts(std::istream &is, lts_type type = lts_none);
      /** Frees the memory occupied by this LTS. */
      ~lts();

      /** Clears the LTS data. 
       * \param[in] type The new LTS format.
       * \param[in] state_info Indicates whether state parameter information
       * will be present.
       * \param[in] label_info Indicates whether label information will be
       * present. */
      void reset(lts_type type = lts_mcrl2, bool state_info = true, bool label_info = true);

      /** Reads LTS data from a file. This is not supported for Dot files.
       * \param[in] filename The name of the file from which data will be read.
       * \param[in] type The format of the file. If \a lts_none is passed then
       * an attempt is made to determine the format from the contents of the
       * file.
       * \param[in] extra Additional data to be stored with the LTS.
       * \retval true if the read operation succeeded;
       * \retval false otherwise.*/
      bool read_from(std::string const& filename, lts_type type = lts_none, lts_extra extra = lts_no_extra);
      /** Reads LTS data from an input stream. This is not supported for SVC,
       * Dot, and BCG files.
       * \param[in] is The input stream from which data will be read.
       * \param[in] type The format of the file. If \a lts_none is passed then
       * an attempt is made to determine the format from the contents of the
       * stream.
       * \param[in] extra Additional data to be stored with the LTS.
       * \retval true if the read operation succeeded;
       * \retval false otherwise.*/
      bool read_from(std::istream &is, lts_type type = lts_none, lts_extra extra = lts_no_extra);

      /** Writes LTS data to a file.
       * \param[in] filename The name of the file to which data will be written.
       * \param[in] type The format of the file.
       * \param[in] extra Additional information for the output.
       * \retval true if the write operation succeeded;
       * \retval false otherwise.*/
      bool write_to(std::string const& filename, lts_type type = lts_mcrl2, lts_extra extra = lts_no_extra);

      /** Writes LTS data to an output stream. This is not supported for SVC and
       * BCG files.
       * \param[in] os The output stream to which data will be written.
       * \param[in] type The format of the output stream.
       * \param[in] extra Additional information for the output.
       * \retval true if the write operation succeeded;
       * \retval false otherwise.*/
      bool write_to(std::ostream &os, lts_type type = lts_mcrl2, lts_extra extra = lts_no_extra);

      /** Gets the number of states of this LTS.
       * \return The number of states of this LTS. */
      unsigned int num_states();
      /** Gets the number of transitions of this LTS.
       * \return The number of transitions of this LTS. */
      unsigned int num_transitions();
      /** Gets the number of labels of this LTS.
       * \return The number of labels of this LTS. */
      unsigned int num_labels();
      
      /** Gets the number of the initial state of this LTS.
       * \return The number of the initial state of this LTS. */
      unsigned int initial_state();
      /** Sets the initial state of this LTS.
       * \param[in] state The number of the state that will become the initial
       * state. */
      void set_initial_state(unsigned int state);
      
      /** Adds a state to this LTS.
       * \param[in] value The value of the state. 
       * \return The number of the added state. */
      unsigned int add_state(ATerm value = NULL);
      /** Adds a label to this LTS.
       * \param[in] value The value of the label. 
       * \param[in] is_tau Indicates whether the label is a tau action.
       * \return The number of the added label. */
      unsigned int add_label(ATerm value = NULL, bool is_tau = false);
      /** Adds a label to this LTS.
       * \param[in] is_tau Indicates whether the label is a tau action.
       * \return The number of the added label. */
      unsigned int add_label(bool is_tau);
      /** Adds a transition to this LTS.
       * \param[in] from The number of the transition's source state.
       * \param[in] label The number of the transition's label.
       * \param[in] to The number of the transition's target state.
       * \return The number of the added transition. */
      unsigned int add_transition(unsigned int from,
                                  unsigned int label,
                                  unsigned int to);

      /** Sets the value of a state.
       * \param[in] state The number of the state.
       * \param[in] value The value that will be assigned to the state. */
      void set_state(unsigned int state, ATerm value);
      /** Sets the value of a label.
       * \param[in] label The number of the label.
       * \param[in] value The value that will be assigned to the label.
       * \param[in] is_tau Indicates whether the label is a tau action. */
      void set_label(unsigned int label, ATerm value, bool is_tau = false);

      /** Gets the value of a state.
       * \param[in] state The number of the state. 
       * \return The value of the state. */
      ATerm state_value(unsigned int state);
      /** Gets the value of a label.
       * \param[in] label The number of the label. 
       * \return The value of the label. */
      ATerm label_value(unsigned int label);
      /** Gets the value of a state as a string.
       * \param[in] state The number of the state. 
       * \return A string representation of the value of the state. */
      std::string state_value_str(unsigned int state);
      /** Gets the value of a label as a string.
       * \param[in] label The number of the label. 
       * \return A string representation of the value of the label. */
      std::string label_value_str(unsigned int label);
      /** Gets the source state of a transition.
       * \param[in] transition The number of the transition. 
       * \return The number of the transition's source state. */
      unsigned int transition_from(unsigned int transition);
      /** Gets the label of a transition.
       * \param[in] transition The number of the transition. 
       * \return The number of the transition's label. */
      unsigned int transition_label(unsigned int transition);
      /** Gets the target state of a transition.
       * \param[in] transition The number of the transition. 
       * \return The number of the transition's target state. */
      unsigned int transition_to(unsigned int transition);

      /** Gets an iterator to the states of this LTS.
       * \return An iterator to the states of this LTS. */
      state_iterator get_states();
      /** Gets an iterator to the labels of this LTS.
       * \return An iterator to the labels of this LTS. */
      label_iterator get_labels();
      /** Gets an iterator to the transitions of this LTS.
       * \return An iterator to the transitions of this LTS. */
      transition_iterator get_transitions();

      /** Checks whether a label is a tau action.
       * \param[in] label The number of the label. 
       * \retval true if the label is a tau action;
       * \retval false otherwise.  */
      bool is_tau(unsigned int label);
      /** Sets the tau attribute of a label.
       * \param[in] label The number of the label.
       * \param[in] is_tau Indicates whether the label should become a tau action. */
      void set_tau(unsigned int label, bool is_tau = true);

      
      /** Checks whether this LTS has a creator.
       * \retval true if the label has a creator;
       * \retval false otherwise.  */
      bool has_creator();
      /** Gets the creator of this LTS.
       * \return The creator string.*/
      std::string get_creator();
      /** Sets the creator of this LTS.
       * \param[in] creator The creator string.*/
      void set_creator(std::string creator);
      
      /** Gets the format of this LTS.
       * \return The format of this LTS. */
      lts_type get_type();

      /** Checks whether this LTS has state values associated with its states.
       * \retval true if the LTS has state information;
       * \retval false otherwise.  */
      bool has_state_info();
      /** Checks whether this LTS has label values associated with its labels.
       * \retval true if the LTS has label information;
       * \retval false otherwise.  */
      bool has_label_info();

      /** Removes the state values from all states. */
      void remove_state_values();

      /** Sorts the transitions first on source state number, then on label
       * number, then on target state number. */
      void sort_transitions();
      /** Gets an array specifying for each state, the range of transitions of
       * which that state is the source state.
       * \pre The transitions are sorted on source state number.
       * \return An array \e A of size \ref num_states()+1 such that for every state <em>s</em>:
       * [ \e A[\e s] .. \e A[<em>s</em>+1] ) are all transitions of which \e s is the source state. */
      unsigned int* get_transition_indices();
      
      /** Applies a reduction algorithm to this LTS.
       * \param[in] eq The equivalence with respect to which the LTS will be
       * reduced.
       * \param[in] opts The options that will be used for the reduction.
       * \retval true if the reduction succeeded;
       * \retval false otherwise. */
      bool reduce(lts_equivalence eq, lts_eq_options &opts = lts_eq_no_options);
      /** Checks whether this LTS is equivalent to another LTS.
       * \param[in] l The LTS to which this LTS will be compared.
       * \param[in] eq The equivalence with respect to which the LTSs will be
       * compared.
       * \param[in] opts The options that will be used for the comparison.
       * \retval true if the LTSs are found to be equivalent.
       * \retval false otherwise. */
      bool compare(lts &l, lts_equivalence eq, lts_eq_options &opts = lts_eq_no_options);
      /** Determinises this LTS. */
      void determinise();
      /** Checks whether all states in this LTS are reachable from the initial
       * state. Runs in O(\ref num_states * \ref num_transitions) time.
       * \param[in] remove_unreachable Indicates whether all unreachable states
       *            should be removed from the LTS. This option does not
       *            influence the return value; the return value is with
       *            respect to the original LTS.
       * \retval true if all states are reachable from the initial state;
       * \retval false otherwise. */
      bool reachability_check(bool remove_unreachable = false);
      /** Checks whether this LTS is deterministic.
       * \retval true if this LTS is deterministic;
       * \retval false otherwise. */
      bool is_deterministic();

      friend class state_iterator;
      friend class label_iterator;
      friend class transition_iterator;
  };

}
}

#endif
