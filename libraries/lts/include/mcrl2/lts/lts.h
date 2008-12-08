// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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
#include <mcrl2/atermpp/set.h>
#include <mcrl2/data/data_specification.h>
#include <mcrl2/lps/specification.h>

#ifdef USE_BCG
#include <bcg_user.h>
#endif

namespace mcrl2
{
/** \brief The main LTS namespace.
 * \details This namespace contains all data structures and members of the LTS
 * library.
 */
namespace lts
{
  /** \brief LTS file types/formats.
   * \details This enumerated type defines the various formats in which an LTS
   * can be stored. */
  enum lts_type
  { 
    lts_none,  /**< unknown or no format */ 
    lts_mcrl2, /**< mCRL2 SVC format */
    lts_aut,   /**< Ald&eacute;baran format (CADP) */
    lts_mcrl,  /**< muCRL SVC format */
    lts_svc,   /**< SVC format */
    lts_fsm,   /**< FSM format */
    lts_dot    /**< GraphViz format */
#ifdef USE_BCG
   ,lts_bcg /**< BCG format
              * \note Only available if the LTS library is built with BCG
              * support.*/
#endif
  };

  /** \brief LTS equivalence relations.
   * \details This enumerated type defines equivalence relations on 
   * LTSs. They can be used to reduce an LTS or decide whether two LTSs
   * are equivalent. */
  enum lts_equivalence
  {
    lts_eq_none,             /**< Unknown or no equivalence */
    lts_eq_bisim,            /**< Strong bisimulation equivalence */
    lts_eq_branching_bisim,  /**< Branching bisimulation equivalence */
    lts_eq_sim,              /**< Strong simulation equivalence */
    lts_eq_trace,            /**< Strong trace equivalence*/
    lts_eq_weak_trace,       /**< Weak trace equivalence */
    lts_eq_isomorph          /**< Isomorphism */
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
    lts_pre_weak_trace   /**< Weak trace preorder */
  };

  /** \brief Transition sort styles.
   * \details This enumerated type defines sort styles for transitions.
   * They can be used to sort the transitions of an LTS based on various
   * criteria. */
  enum transition_sort_style
  {
    src_lbl_tgt, /**< Sort first on source state, then on label, then on target state */
    lbl_tgt_src  /**< Sort first on label, then on target state, then on source state*/
  };

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
        /** \brief List of internal action labels.
         * \details The vector of transition labels that the reduction algorithm
         * considers to be internal actions. (Besides those that are already
         * marked as being internal.)
         */
        std::vector<std::string> tau_actions;
      } reduce; /**< \brief Contains the options data.*/
  };
  /** \brief An empty lts_eq_options object */
  extern lts_eq_options lts_eq_no_options;

  /** \brief Types of additional information for LTS operations.
   * \details This enumerated type defines types for additional information for
   * LTS operations.
   * \sa lts_extra */
  enum lts_extra_type
  {
    le_nothing, /**< no additional format */
    le_mcrl1,   /**< muCRL format */
    le_mcrl2,   /**< mCRL2 format */
    le_dot      /**< Dot format (GraphViz) */
  };

  /** \brief Options for the Dot format.
   * \details This struct stores options for the Dot format of the GraphViz
   * package. */
  typedef struct
  { 
    /** \brief Name of the graph. 
     * \details Points to the name that is given to the graph in the Dot file. */
    std::string const *name;
    /** \brief Indicates whether to label states in the graph.
     * \details
     * If \a true, the states in the Dot file are labelled by the state values,
     * if available. 
     *
     * If \a false, the states are not labelled. */
    bool print_states;
  } lts_dot_options;

  /** \brief Adds transition labels to the list of internal actions.
   * \param[in,out] opts The object of which the list of internal actions will be
   * changed.
   * \param[in] act_names A string containing a comma-separated list of
   * transition labels that will be added to the list of internal actions.
   */
  void lts_reduce_add_tau_actions(lts_eq_options& opts, std::string const& act_names);

  /** \brief Checks whether an action is a timed mCRL2 action.
   * \param[in] t The action for which the check has to be performed.
   * \retval true if the action is timed;
   * \retval false otherwise. */
  bool is_timed_pair(ATermAppl t);

  /** \brief Creates a timed mCRL2 action.
   * \param[in] action The action part of the timed action that will be created.
   * \param[in] time The time part of the timed action that will be created.
   * \return A timed action with label \a action and time \a time. */
  ATermAppl make_timed_pair(ATermAppl action, ATermAppl time = NULL);

  /** \brief Add an mCRL2 data specification, parameter list and action
   *         specification to a mCRL2 LTS in SVC format.
   * \param[in] filename   The file name of the mCRL2 LTS.
   * \param[in] data_spec  The data specification to add in mCRL2 internal
   *                       format (or NULL for none).
   * \param[in] params     The list of state(/process) parameters in mCRL2
   *                       internal format (or NULL for none).
   * \param[in] act_spec   The action specification to add in mCRL2 internal
   *                       format (or NULL for none).
   * \pre                  The LTS in filename is a mCRL2 SVC without extra
   *                       information. */
  void add_extra_mcrl2_svc_data(std::string const &filename, ATermAppl data_spec, ATermList params, ATermAppl act_spec);
  
  /** \cond */
  #include "detail/liblts_private.h"
  /** \endcond */
  
  /** \brief Stores additional LTS information.
   * \details This class is provided for storing additional information with an
   * LTS. This can be either a muCRL specification, an mCRL2 specificiation, or
   * options for the Dot format. */
  class lts_extra : p_lts_extra
  {
    public:
      /** \brief Creates an object containing no information. */
      lts_extra();
      /** \brief Creates an object containing a muCRL specification.
       * \param[in] t The muCRL specification that will be stored in the object. */
      lts_extra(ATerm t);
      /** \brief Creates an object containing an mCRL2 specification.
       * \param[in] spec The mCRL2 specification that will be stored in the object. */
      lts_extra(lps::specification *spec);
      /** \brief Creates an object containing options for the Dot format.
       * \param[in] opts The options that will be stored in the object. */
      lts_extra(lts_dot_options opts);

      /** \brief Gets the type of information that is stored in this object.
       * \retval le_nothing if this object contains no information;
       * \retval le_mcrl1 if this object contains a muCRL specification;
       * \retval le_mcrl2 if this object contains an mCRL2 specification;
       * \retval le_dot if this object contains options for the Dot format. */
      lts_extra_type get_type();
      /** \brief Gets the muCRL specification stored in this object. 
       * \pre This object contains a muCRL specification.
       * \return The muCRL specification stored in this object.*/
      ATerm get_mcrl1_spec();
      /** \brief Gets the mCRL2 specification stored in this object. 
       * \pre This object contains an mCRL2 specification.
       * \return The mCRL2 specification stored in this object.*/
      lps::specification *get_mcrl2_spec();
      /** \brief Gets the Dot format options stored in this object. 
       * \pre This object contains options for the Dot format.
       * \return The Dot format options stored in this object.*/
      lts_dot_options get_dot_options();
  };

  /** \brief An empty lts_extra object. */
  extern lts_extra lts_no_extra;

  class lts;

  /** \brief Iterator over states of an LTS.
   * \details This class defines an iterator type for iterating over the states
   * of an LTS. */
  class state_iterator : p_state_iterator
  {
    public:
      /** \brief Creates a state iterator for an LTS.
       * \details The created iterator will point to the first state of
       * the LTS.
       * \param[in] l The LTS for which the state iterator will be created. */
      state_iterator(lts *l);

      /** \brief Checks whether there are more states to iterate over.
       * \retval true if there are more states to iterate over;
       * \retval false otherwise. */
      bool more();

      /** \brief Dereferences the iterator.
       * \return The state to which this iterator points.*/
      unsigned int operator *();
      /** \brief Increments the iterator. 
       * \details Makes this iterator point to the next state of the LTS. */
      void operator ++();
  };

  /** \brief Iterator over transition labels of an LTS.
   * \details This class defines an iterator type for iterating over the
   * transition labels of an LTS. */
  class label_iterator : p_label_iterator
  {
    public:
      /** \brief Creates a transition label iterator for an LTS.
       * \details The created iterator will point to the first label of
       * the LTS.
       * \param[in] l The LTS for which the transition label iterator will be
       * created. */
      label_iterator(lts *l);
      
      /** \brief Checks whether there are more transition labels to iterate over.
       * \retval true if there are more labels to iterate over;
       * \retval false otherwise. */
      bool more();

      /** \brief Dereferences the iterator.
       * \return The transition label to which this iterator points.*/
      unsigned int operator *();
      /** \brief Increments the iterator.
       * \details Makes this iterator point to the next label of the LTS. */
      void operator ++();
  };

  /** \brief Iterator over transitions of an LTS.
   * \details This class defines an iterator type for iterating over the
   * transitions of an LTS. */
  class transition_iterator : p_transition_iterator
  {
    public:
      /** \brief Creates a transition iterator for an LTS.
       * \details The created iterator will point to the first
       * transition of the LTS.
       * \param[in] l The LTS for which the transition iterator will be created.
       */
      transition_iterator(lts *l);
      
      /** \brief Checks whether there are more transitions to iterate over.
       * \retval true if there are more transitions to iterate over;
       * \retval false otherwise. */
      bool more();

      /** \brief Gets the transition's source state.
       * \return The source state of the transition to which this iterator points.*/
      unsigned int from();
      /** \brief Gets the transition's label.
       * \return The label of the transition to which this iterator points.*/
      unsigned int label();
      /** \brief Gets the transition's target state.
       * \return The target state of the transition to which this iterator points.*/
      unsigned int to();

      /** \brief Increments the iterator.
       * \details Makes this iterator point to the next transition of the LTS. */
      void operator ++();
  };

  /** \brief Class for labelled transition systems.
   * \details This class provides an interface for creating and manipulating
   * labelled transition systems. */
  class lts : p_lts
  {
    public:

      /** \brief Determines the LTS format from a filename by its extension.
       * \param[in] s The name of the file of which the format will be
       * determined.
       * \return The LTS format based on the extension of \a s.
       * If the extension is \p svc then the mCRL2 SVC format is assumed and
       * \a lts_mcrl2 is returned.
       * If no supported format can be determined from the extension then \a
       * lts_none is returned.  */
      static lts_type guess_format(std::string const& s);

      /** \brief Determines the LTS format from a format specification string.
       * \details This can be any of the following strings:
       * \li "aut" for the Ald&eacute;baran format;
       * \li "mcrl" or "svc+mcrl" for the muCRL SVC format;
       * \li "mcrl2" or "svc+mcrl2" for the mCRL2 SVC format;
       * \li "svc" for the SVC format;
       * \li "fsm" for the FSM format;
       * \li "dot" for the GraphViz format;
       * \li "bcg" for the BCG format (only available if the LTS library is built
       * with BCG support).
       *
       * \param[in] s The format specification string.
       * \return The LTS format based on the value of \a s.
       * If no supported format can be determined then \a lts_none is returned.
       */
      static lts_type parse_format(std::string const& s);

      /** \brief Gives a string representation of an LTS format. 
       * \details This is the "inverse" of \ref parse_format.
       * \param[in] type The LTS format.
       * \return The name of the LTS format specified by \a type. */
      static std::string string_for_type(const lts_type type);

      /** \brief Gives the filename extension associated with an LTS format.
       * \param[in] type The LTS format.
       * \return The filename extension of the LTS format specified by \a type.
       */
      static std::string extension_for_type(const lts_type type);

      /** \brief Determines the equivalence from a string.
       * \details The following strings may be used:
       * \li "bisim" for strong bisimilarity;
       * \li "branching-bisim" for branching bisimilarity;
       * \li "sim" for strong simulation equivalence;
       * \li "trace" for strong trace equivalence;
       * \li "weak-trace" for weak trace equivalence;
       * \li "isomorph" for isomorphism.
       *
       * \param[in] s The string specifying the equivalence.
       * \return The equivalence type specified by \a s.
       * If \a s is none of the above values then \a lts_eq_none is returned. */
      static lts_equivalence parse_equivalence(std::string const& s);
      
      /** \brief Gives the short name of an equivalence.
       * \param[in] eq The equivalence type.
       * \return A short string representing the equivalence specified by \a
       * eq. The returned value is one of the strings listed for
       * \ref parse_equivalence. */
      static std::string string_for_equivalence(const lts_equivalence eq);

      /** \brief Gives the full name of an equivalence.
       * \param[in] eq The equivalence type.
       * \return The full, descriptive name of the equivalence specified by \a eq. */
      static std::string name_of_equivalence(const lts_equivalence eq);

      /** \brief Determines the preorder from a string.
       * \details The following strings may be used:
       * \li "sim" for strong simulation preorder;
       * \li "trace" for strong trace preorder;
       * \li "weak-trace" for weak trace preorder.
       *
       * \param[in] s The string specifying the preorder.
       * \return The preorder type specified by \a s.
       * If \a s is none of the above values then \a lts_pre_none is returned. */
      static lts_preorder parse_preorder(std::string const& s);
      
      /** \brief Gives the short name of a preorder.
       * \param[in] pre The preorder type.
       * \return A short string representing the preorder specified by \a
       * pre. The returned value is one of the strings listed for
       * \ref parse_preorder. */
      static std::string string_for_preorder(const lts_preorder pre);

      /** \brief Gives the full name of a preorder.
       * \param[in] pre The preorder type.
       * \return The full, descriptive name of the preorder specified by \a pre. */
      static std::string name_of_preorder(const lts_preorder pre);

    public:

      /** \brief Creates an empty LTS.
       * \param[in] state_info Indicates whether state parameter information
       * will be present.
       * \param[in] label_info Indicates whether label information will be
       * present. */
      lts(bool state_info = true, bool label_info = true);

      /** \brief Creates an LTS and reads its data from a file.
       * \param[in] filename The name of the file from which the data will be
       * read.
       * \param[in] type The format of the file. If \a lts_none is passed then
       * an attempt is made to determine the format from the contents of the
       * file. */
      lts(std::string &filename, lts_type type = lts_none);

      /** \brief Creates an LTS and reads its data from an input stream.
       * \param[in] is The input stream from which the data will be read.
       * \param[in] type The format of the data. If \a lts_none is passed then
       * an attempt is made to determine the format from the contents of the
       * stream. */
      lts(std::istream &is, lts_type type = lts_none);

      /** \brief Creates a copy of the supplied LTS.
       * \param[in] l The LTS to copy. */
      lts(lts const &l);

      /** \brief Frees the memory occupied by this LTS. */
      ~lts();

      /** \brief Clears the LTS data. 
       * \param[in] state_info Indicates whether state parameter information
       * will be present.
       * \param[in] label_info Indicates whether label information will be
       * present. */
      void reset(bool state_info = true, bool label_info = true);

      /** \brief Reads LTS data from a file.
       * \details This is not supported for Dot files.
       * \param[in] filename The name of the file from which data will be read.
       * \param[in] type The format of the file. If \a lts_none is passed then
       * an attempt is made to determine the format from the contents of the
       * file.
       * \param[in] extra Additional data to be stored with the LTS.
       * \retval true if the read operation succeeded;
       * \retval false otherwise.*/
      bool read_from(std::string const& filename, lts_type type = lts_none, lts_extra extra = lts_no_extra);

      /** \brief Reads LTS data from an input stream.
       * \details This is not supported for SVC, Dot, and BCG files.
       * \param[in] is The input stream from which data will be read.
       * \param[in] type The format of the file. If \a lts_none is passed then
       * an attempt is made to determine the format from the contents of the
       * stream.
       * \param[in] extra Additional data to be stored with the LTS.
       * \retval true if the read operation succeeded;
       * \retval false otherwise.*/
      bool read_from(std::istream &is, lts_type type = lts_none, lts_extra extra = lts_no_extra);

      /** \brief Writes LTS data to a file.
       * \param[in] filename The name of the file to which data will be written.
       * \param[in] type The format of the file.
       * \param[in] extra Additional information for the output.
       * \retval true if the write operation succeeded;
       * \retval false otherwise.*/
      bool write_to(std::string const& filename, lts_type type = lts_mcrl2, lts_extra extra = lts_no_extra);

      /** \brief Writes LTS data to an output stream.
       * \details This is not supported for SVC and BCG files.
       * \param[in] os The output stream to which data will be written.
       * \param[in] type The format of the output stream.
       * \param[in] extra Additional information for the output.
       * \retval true if the write operation succeeded;
       * \retval false otherwise.*/
      bool write_to(std::ostream &os, lts_type type = lts_mcrl2, lts_extra extra = lts_no_extra);

      /** \brief Gets the number of states of this LTS.
       * \return The number of states of this LTS. */
      unsigned int num_states();

      /** \brief Gets the number of transitions of this LTS.
       * \return The number of transitions of this LTS. */
      unsigned int num_transitions();

      /** \brief Gets the number of labels of this LTS.
       * \return The number of labels of this LTS. */
      unsigned int num_labels();
      
      /** \brief Gets the initial state number of this LTS.
       * \return The number of the initial state of this LTS. */
      unsigned int initial_state();

      /** \brief Sets the initial state number of this LTS.
       * \param[in] state The number of the state that will become the initial
       * state. */
      void set_initial_state(unsigned int state);
      
      /** \brief Adds a state to this LTS.
       * \param[in] value The value of the state. 
       * \return The number of the added state. */
      unsigned int add_state(ATerm value = NULL);

      /** \brief Adds a label to this LTS.
       * \param[in] value The value of the label. 
       * \param[in] is_tau Indicates whether the label is a tau action.
       * \return The number of the added label. */
      unsigned int add_label(ATerm value = NULL, bool is_tau = false);

      /** \brief Adds a label to this LTS.
       * \param[in] is_tau Indicates whether the label is a tau action.
       * \return The number of the added label. */
      unsigned int add_label(bool is_tau);

      /** \brief Adds a transition to this LTS.
       * \param[in] from The number of the transition's source state.
       * \param[in] label The number of the transition's label.
       * \param[in] to The number of the transition's target state.
       * \return The number of the added transition. */
      unsigned int add_transition(unsigned int from,
                                  unsigned int label,
                                  unsigned int to);

      /** \brief Sets the value of a state.
       * \param[in] state The number of the state.
       * \param[in] value The value that will be assigned to the state. */
      void set_state(unsigned int state, ATerm value);

      /** \brief Sets the value of a label.
       * \param[in] label The number of the label.
       * \param[in] value The value that will be assigned to the label.
       * \param[in] is_tau Indicates whether the label is a tau action. */
      void set_label(unsigned int label, ATerm value, bool is_tau = false);

      /** \brief Gets the value of a state.
       * \param[in] state The number of the state. 
       * \return The value of the state. */
      ATerm state_value(unsigned int state);

      /** \brief Gets the value of a label.
       * \param[in] label The number of the label. 
       * \return The value of the label. */
      ATerm label_value(unsigned int label);

      /** \brief Gets the value of a state as a string.
       * \param[in] state The number of the state. 
       * \return A string representation of the value of the state. */
      std::string state_value_str(unsigned int state);

      /** \brief Gets the value of a label as a string.
       * \param[in] label The number of the label. 
       * \return A string representation of the value of the label. */
      std::string label_value_str(unsigned int label);

      /** \brief Gets the source state of a transition.
       * \param[in] transition The number of the transition. 
       * \return The number of the transition's source state. */
      unsigned int transition_from(unsigned int transition);

      /** \brief Gets the label of a transition.
       * \param[in] transition The number of the transition. 
       * \return The number of the transition's label. */
      unsigned int transition_label(unsigned int transition);

      /** \brief Gets the target state of a transition.
       * \param[in] transition The number of the transition. 
       * \return The number of the transition's target state. */
      unsigned int transition_to(unsigned int transition);

      /** \brief Gets an iterator to the states of this LTS.
       * \return An iterator to the states of this LTS. */
      state_iterator get_states();

      /** \brief Gets an iterator to the labels of this LTS.
       * \return An iterator to the labels of this LTS. */
      label_iterator get_labels();

      /** \brief Gets an iterator to the transitions of this LTS.
       * \return An iterator to the transitions of this LTS. */
      transition_iterator get_transitions();

      /** \brief Checks whether a label is a tau action.
       * \param[in] label The number of the label. 
       * \retval true if the label is a tau action;
       * \retval false otherwise.  */
      bool is_tau(unsigned int label);

      /** \brief Sets the tau attribute of a label.
       * \param[in] label The number of the label.
       * \param[in] is_tau Indicates whether the label should become a tau action. */
      void set_tau(unsigned int label, bool is_tau = true);

      
      /** \brief Checks whether this LTS has a creator.
       * \retval true if the label has a creator;
       * \retval false otherwise.  */
      bool has_creator();

      /** \brief Gets the creator of this LTS.
       * \return The creator string.*/
      std::string get_creator();

      /** \brief Sets the creator of this LTS.
       * \param[in] creator The creator string.*/
      void set_creator(std::string creator);
      
      /** \brief Gets the file type of the last successful call to load.
       * \details Note that this does not mean that the information
       * currently in the object is still in this format.
       * \return The format of this LTS. */
      lts_type get_type();

      /** \brief Checks whether this LTS has state values associated with its states.
       * \retval true if the LTS has state information;
       * \retval false otherwise.  */
      bool has_state_info();

      /** \brief Checks whether this LTS has label values associated with its labels.
       * \retval true if the LTS has label information;
       * \retval false otherwise.  */
      bool has_label_info();

      /** \brief Removes the state values from all states. */
      void remove_state_values();

      /** \brief Checks whether states are (recognised) vectors of parameters.
       * \retval true If states are vectors of parameters.
       * \retval false If states are not vectors of parameters. */
      bool has_state_parameters();

      /** \brief Returns the number of parameters of states.
       * \return The number of parameters of states.
       * \pre    States have parameters. */
      unsigned int num_state_parameters();

      /** \brief Returns the name of a parameter.
       * \param[in] idx  The index of the parameter.
       * \return         The name of the idx'th parameter if it is known and
       *                 'p<idx>' otherwise.
       * \pre            States have parameters. */
      ATerm state_parameter_name(unsigned int idx);

      /** \brief Returns the name of a parameter as a string.
       * \param[in] idx  The index of the parameter.
       * \return         The name of the idx'th parameter as string if it is
       *                 known and 'p<idx>' otherwise.
       * \pre            States have parameters. */
      std::string state_parameter_name_str(unsigned int idx);

      /** \brief Returns the sort of a parameter.
       * \param[in] idx  The index of the parameter.
       * \return         The sort of the idx'th parameter if it is known and
       *                 'D<idx>' otherwise.
       * \pre            States have parameters. */
      ATerm state_parameter_sort(unsigned int idx);

      /** \brief Returns the sort of a parameter as a string.
       * \param[in] idx  The index of the parameter.
       * \return         The sort of the idx'th parameter as string if it is
       *                 known and 'D<idx>' otherwise.
       * \pre            States have parameters. */
      std::string state_parameter_sort_str(unsigned int idx);

      /** \brief Returns a parameter value of a state.
       * \param[in] state  The state from which the parameter value must be
       *                   taken.
       * \param[in] idx    The index of the parameter.
       * \return           The value of the idx'th parameter of state.
       * \pre              States have parameters. */
      ATerm get_state_parameter_value(unsigned int state, unsigned int idx);

      /** \brief Returns the string representation of a parameter value of a
       *         state.
       * \param[in] state  The state from which the parameter value must be
       *                   taken.
       * \param[in] idx    The index of the parameter.
       * \return           The value of the idx'th parameter of state as a
       *                   string.
       * \pre              States have parameters. */
      std::string get_state_parameter_value_str(unsigned int state, unsigned int idx);

      /** \brief Returns a list of all unique label values that occur in this
       *         LTS.
       * \return A list of all unique label values that occur in this LTS.
       * \pre    This LTS has label information. */
      atermpp::set<ATerm> get_label_values();

      /** \brief Returns a list of all unique state values that occur in this
       *         LTS.
       * \return A list of all unique state values that occur in this LTS.
       * \pre    This LTS has state information. */
      atermpp::set<ATerm> get_state_values();

      /** \brief Returns a list of all unique state parameters values that
       *         occur in this LTS.
       * \param[in] idx  The index of the state parameter.
       * \return         A list of all unique state parameters values that
       *                 occur in this LTS as idx'th parameter.
       * \pre            States have parameters. */
      atermpp::set<ATerm> get_state_parameter_values(unsigned int idx);

      /** \brief Pretty print a label value of this LTS.
       * \param[in] value  The label value to pretty print.
       * \return           The pretty-printed representation of value.
       * \pre              value occurs as label value in this LTS. */
      std::string pretty_print_label_value(ATerm value);

      /** \brief Pretty print a state value of this LTS.
       * \param[in] value  The state value to pretty print.
       * \return           The pretty-printed representation of value.
       * \pre              value occurs as state value in this LTS. */
      std::string pretty_print_state_value(ATerm value);

      /** \brief Pretty print a state parameter value of this LTS.
       * \param[in] value  The state parameter value to pretty print.
       * \return           The pretty-printed representation of value.
       * \pre              value occurs as state parameter value in this LTS. */
      std::string pretty_print_state_parameter_value(ATerm value);


      /** \brief Return the extra information stored in this LTS.
       * \return The extra information stored in this LTS or NULL if there
       *         is none. */
      ATerm get_extra_data();
      
      /** \brief Set (or remove) the extra information for this LTS.
       * \param[in] data  The extra information for this LTS or NULL to
       *                  remove any possible extra information. */
      void set_extra_data(ATerm data);
      
      /** \brief Check whether this LTS has an mCRL2 data specification.
       * \return A boolean indicating whether this LTS has an mCRL2 data
       *         specification. */
      bool has_data_specification();
      
      /** \brief Return the mCRL2 data specification of this LTS.
       * \return The mCRL2 data specification of this LTS.
       * \pre    This LTS has an mCRL2 data specification. */
      data::data_specification get_data_specification();
      
      /** \brief Set the mCRL2 data specification of this LTS.
       * \param[in] spec  The mCRL2 data specification for this LTS.
       * \pre             This is an mCRL2 LTS. */
      void set_data_specification(data::data_specification spec);

      /** \brief Sorts the transitions using a sort style.
       * \param[in] ts The sort style to use. */
      void sort_transitions(transition_sort_style ts = src_lbl_tgt);

      /** \brief Gets the outgoing transitions for each state.
       * \details Gets an array specifying for each state, the range of
       * transitions of which that state is the source state.
       * \pre The transitions are sorted on source state number.
       * \return An array \e A of size \ref num_states()+1 such that 
       * for every state <em>s</em>: 
       * [ \e A[\e s] .. \e A[<em>s</em>+1] ) 
       * are all transitions of which \e s is the source state. */
      unsigned int* get_transition_indices();

      /** \brief Gets the predecessors of each state, per label. 
       * \details Gets a table specifying for each state and label number, the
       * range of transitions having that label and target state.
       * \pre The transitions are sorted first on label number, then on
       * target state number.
       * \return A table \e A of size \ref num_labels() * (\ref num_states()+1) 
       * such that for every label <em>l</em> and state <em>t</em>: 
       * [ \e A[\e l][\e t] .. \e A[\e l][<em>t</em>+1] ) 
       * are all <em>l</em>-labelled transitions of which \e t is the target state. */
      unsigned int** get_transition_pre_table();
      
      /** \brief Applies a reduction algorithm to this LTS.
       * \param[in] eq The equivalence with respect to which the LTS will be
       * reduced.
       * \param[in] opts The options that will be used for the reduction.
       * \retval true if the reduction succeeded;
       * \retval false otherwise. */
      bool reduce(lts_equivalence eq, lts_eq_options const&opts = lts_eq_no_options);

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
      bool compare(lts &l, lts_equivalence eq, lts_eq_options const&opts = lts_eq_no_options);
      
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
      bool compare(lts &l, lts_preorder pre, lts_eq_options const&opts = lts_eq_no_options);

      /** \brief Determinises this LTS. */
      void determinise();
      
      /** \brief Checks whether all states in this LTS are reachable
       * from the initial state. 
       * \details Runs in O(\ref num_states * \ref num_transitions) time.
       * \param[in] remove_unreachable Indicates whether all unreachable states
       *            should be removed from the LTS. This option does not
       *            influence the return value; the return value is with
       *            respect to the original LTS.
       * \retval true if all states are reachable from the initial state;
       * \retval false otherwise. */
      bool reachability_check(bool remove_unreachable = false);

      /** \brief Checks whether this LTS is deterministic.
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
