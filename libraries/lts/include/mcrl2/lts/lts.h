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
#include <set>
#include <iostream>
#include "aterm2.h"
#include "mcrl2/atermpp/set.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef USE_BCG
#include <bcg_user.h>
#endif

namespace mcrl2
{
namespace data
{
  class data_specification;
}
namespace lps
{
  class specification;
}
/** \brief The main LTS namespace.
 * \details This namespace contains all data structures and members of the LTS
 * library.
 */
namespace lts
{
  template<typename T>
  static bool lts_named_cmp(std::string N[], T a, T b)
  {
    return N[a] < N[b];
  }

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
    lts_dot,   /**< GraphViz format */
#ifdef USE_BCG
    lts_bcg,   /**< BCG format
                * \note Only available if the LTS library is built with BCG
                * support.*/
#endif
    lts_type_min = lts_none,
#ifdef USE_BCG
    lts_type_max = lts_bcg
#else
    lts_type_max = lts_dot
#endif
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
  struct lts_dot_options
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
  };

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
  void add_extra_mcrl2_svc_data(std::string const &filename, ATermAppl data_spec, ATermList params, ATermList act_spec);

  /** \brief Stores additional LTS information.
   * \details This class is provided for storing additional information with an
   * LTS. This can be either a muCRL specification, an mCRL2 specificiation, or
   * options for the Dot format. */
  class lts_extra 
  {
    private:
      lts_extra_type type;
      union {
        ATerm mcrl1_spec;
        ATermAppl mcrl2_spec;
	lts_dot_options dot_options;
      } content;

    public:
      /** \brief Creates an object containing no information. */
      lts_extra();
      /** \brief Creates an object containing a muCRL specification.
       * \param[in] t The muCRL specification that will be stored in the object. */
      lts_extra(ATerm t);
      /** \brief Creates an object containing an mCRL2 specification.
       * \param[in] spec The mCRL2 specification that will be stored in the object. */
      lts_extra(lps::specification const& spec);
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
      lps::specification get_mcrl2_spec();
      /** \brief Gets the Dot format options stored in this object.
       * \pre This object contains options for the Dot format.
       * \return The Dot format options stored in this object.*/
      lts_dot_options get_dot_options();
  };

  /** \brief An empty lts_extra object. */
  extern lts_extra lts_no_extra;

  struct transition
  {
    unsigned int from;
    unsigned int label;
    unsigned int to;

    transition():from(0),label(0),to(0)
    {}

    transition(const unsigned int f, 
               const unsigned int l,
               const unsigned int t):from(f),label(l),to(t)
    {}
    
     bool operator <(const transition &t) const
     { return from < t.from ||
              (from==t.from && (label<t.label ||
                   (label==t.label && to<t.to)));
     }

  };

  class lts;

  /** \brief Iterator over states of an LTS.
   * \details This class defines an iterator type for iterating over the states
   * of an LTS. */
  class state_iterator 
  {
    private:
      const lts &l;
      unsigned int pos;
      unsigned int max;

    public:
      /** \brief Creates a state iterator for an LTS.
       * \details The created iterator will point to the first state of
       * the LTS.
       * \param[in] l The LTS for which the state iterator will be created. */
      state_iterator(const lts &l);

      /** \brief Checks whether there are more states to iterate over.
       * \retval true if there are more states to iterate over;
       * \retval false otherwise. */
      bool more() const;

      /** \brief Dereferences the iterator.
       * \return The state to which this iterator points.*/
      unsigned int operator *() const;
      /** \brief Increments the iterator.
       * \details Makes this iterator point to the next state of the LTS. */
      void operator ++();
  };


  /** \brief Const iterator over states of an LTS.
   * \details This class defines an iterator type for iterating over the states
   * of an LTS. */
  class const_state_iterator 
  {
    private:
      const lts &l;
      unsigned int pos;
      unsigned int max;

    public:
      /** \brief Creates a state iterator for an LTS.
       * \details The created iterator will point to the first state of
       * the LTS.
       * \param[in] l The LTS for which the state iterator will be created. */
      const_state_iterator(const lts &l);

      /** \brief Checks whether there are more states to iterate over.
       * \retval true if there are more states to iterate over;
       * \retval false otherwise. */
      bool more() const;

      /** \brief Dereferences the iterator.
       * \return The state to which this iterator points.*/
      unsigned int operator *() const;

      /** \brief Increments the iterator.
       * \details Makes this iterator point to the next state of the LTS. */
      void operator ++();
  };


  /** \brief Iterator over transition labels of an LTS.
   * \details This class defines an iterator type for iterating over the
   * transition labels of an LTS. */
  class label_iterator 
  {
    private:
      const lts &l;
      unsigned int pos;
      unsigned int max;

    public:
      /** \brief Creates a transition label iterator for an LTS.
       * \details The created iterator will point to the first label of
       * the LTS.
       * \param[in] l The LTS for which the transition label iterator will be
       * created. */
      label_iterator(const lts &l);

      /** \brief Checks whether there are more transition labels to iterate over.
       * \retval true if there are more labels to iterate over;
       * \retval false otherwise. */
      bool more() const;

      /** \brief Dereferences the iterator.
       * \return The transition label to which this iterator points.*/
      unsigned int operator *() const;

      /** \brief Increments the iterator.
       * \details Makes this iterator point to the next label of the LTS. */
      void operator ++();

      /** \brief Equality on label iterators
       * \details Indicates whether the current iterator is equal to the argument. */
      bool operator ==(const label_iterator &i) const;
  
      /** \brief Inequality on label iterators
       * \details Indicates whether the current iterator is equal to the argument. */
      bool operator !=(const label_iterator &i) const;
  };

  /** \brief Const iterator over transition labels of an LTS.
   * \details This class defines an iterator type for iterating over the
   * transition labels of an LTS. */
  class const_label_iterator 
  {
    private:
      const lts &l;
      unsigned int pos;
      unsigned int max;

    public:
      /** \brief Creates a transition label iterator for an LTS.
       * \details The created iterator will point to the first label of
       * the LTS.
       * \param[in] l The LTS for which the transition label iterator will be
       * created. */
      const_label_iterator(const lts &l);

      /** \brief Checks whether there are more transition labels to iterate over.
       * \retval true if there are more labels to iterate over;
       * \retval false otherwise. */
      bool more() const;

      /** \brief Dereferences the iterator.
       * \return The transition label to which this iterator points.*/
      unsigned int operator *() const;

      /** \brief Increments the iterator.
       * \details Makes this iterator point to the next label of the LTS. */
      void operator ++();

      /** \brief Equality on label iterators
       * \details Indicates whether the current iterator is equal to the argument. */
      bool operator ==(const const_label_iterator &i) const;
  
      /** \brief Inequality on label iterators
       * \details Indicates whether the current iterator is equal to the argument. */
      bool operator !=(const const_label_iterator &i) const;
  };

  /** \brief Iterator over transitions of an LTS.
   * \details This class defines an iterator type for iterating over the
   * transitions of an LTS. */
  class transition_iterator 
  {
    private:
      const lts &l;
      unsigned int pos;
      unsigned int max;

    public:
      /** \brief Creates a transition iterator for an LTS.
       * \details The created iterator will point to the first
       * transition of the LTS.
       * \param[in] l The LTS for which the transition iterator will be created.
       */
      transition_iterator(const lts &l);

      /** \brief Checks whether there are more transitions to iterate over.
       * \retval true if there are more transitions to iterate over;
       * \retval false otherwise. */
      bool more() const;

      /** \brief Gets the transition's source state.
       * \return The source state of the transition to which this iterator points.*/
      unsigned int from() const;
      /** \brief Gets the transition's label.
       * \return The label of the transition to which this iterator points.*/
      unsigned int label() const;
      /** \brief Gets the transition's target state.
       * \return The target state of the transition to which this iterator points.*/
      unsigned int to() const;

      /** \brief Increments the iterator.
       * \details Makes this iterator point to the next transition of the LTS. */
      void operator ++();

      /** \brief Dereferences the iterator.
       * \return The state to which this iterator points.*/
      unsigned int operator *() const;

  };

  /** \brief Iterator over transitions of an LTS.
   * \details This class defines an iterator type for iterating over the
   * transitions of an LTS. */
  class const_transition_iterator 
  {
    private:
      const lts &l;
      unsigned int pos;
      unsigned int max;

    public:
      /** \brief Creates a transition iterator for an LTS.
       * \details The created iterator will point to the first
       * transition of the LTS.
       * \param[in] l The LTS for which the transition iterator will be created.
       */
      const_transition_iterator(const lts &l);

      /** \brief Checks whether there are more transitions to iterate over.
       * \retval true if there are more transitions to iterate over;
       * \retval false otherwise. */
      bool more() const;

      /** \brief Gets the transition's source state.
       * \return The source state of the transition to which this iterator points.*/
      unsigned int from() const; 
      /** \brief Gets the transition's label.
       * \return The label of the transition to which this iterator points.*/
      unsigned int label() const;
      /** \brief Gets the transition's target state.
       * \return The target state of the transition to which this iterator points.*/
      unsigned int to() const;

      /** \brief Increments the iterator.
       * \details Makes this iterator point to the next transition of the LTS. */
      void operator ++();

      /** \brief Dereferences the iterator.
       * \return The state to which this iterator points.*/
      unsigned int operator *() const;

  };


  /** \brief Class for labelled transition systems.
   * \details This class provides an interface for creating and manipulating
   * labelled transition systems. */
  class lts 
  {
    private:
      lts *lts_object;
      lts_type type;
      std::string creator;
      unsigned int labels_size;
      // unsigned int transitions_size;
      unsigned int nstates;
      unsigned int nlabels;
      unsigned int ntransitions;
      unsigned int init_state;
      ATerm extra_data;

      lts_type detect_type(std::string const& filename);
      lts_type detect_type(std::istream &is);

      void clear_labels();

      bool read_from_aut(std::string const& filename);
      bool read_from_aut(std::istream &is);
      bool write_to_aut(std::string const& filename);
      bool write_to_aut(std::ostream& os);

      static lps::specification const& empty_specification();

      bool read_from_svc(std::string const& filename, lts_type type);
      bool write_to_svc(std::string const& filename, lts_type type, lps::specification const& spec = empty_specification());

      lts_type fsm_get_lts_type();
      bool read_from_fsm(std::string const& filename, lts_type type, lps::specification const& spec = empty_specification());
      bool read_from_fsm(std::string const& filename, ATerm lps);
      bool read_from_fsm(std::string const& filename, lps::specification const& spec = empty_specification());
      bool read_from_fsm(std::istream& is, lts_type type, lps::specification const& spec = empty_specification());
      bool read_from_fsm(std::istream& is, ATerm lps);
      bool read_from_fsm(std::istream& is, lps::specification const& spec = empty_specification());
      bool write_to_fsm(std::string const& filename, lts_type type, ATermList params);
      bool write_to_fsm(std::string const& filename, ATerm lps);
      bool write_to_fsm(std::string const& filename, lps::specification const& spec = empty_specification());
      bool write_to_fsm(std::ostream& os, lts_type type, ATermList params);
      bool write_to_fsm(std::ostream& os, ATerm lps);
      bool write_to_fsm(std::ostream& os, lps::specification const& spec = empty_specification());

      bool read_from_dot(std::string const& filename);
      bool read_from_dot(std::istream &is);
      bool write_to_dot(std::ostream& os, lts_dot_options opts);
      bool write_to_dot(std::string const& filename, lts_dot_options opts);

#ifdef USE_BCG
      bool read_from_bcg(std::string const& filename);
      bool write_to_bcg(std::string const& filename);
#endif

    public: // The items below are public, to allow restructuring of the code of the lts library.
      
      void clear_type();
      void clear_transitions();
      void clear_states();
      unsigned int transitions_size;
      void merge(const lts &l);
      void clear(bool state_info = true, bool label_info = true);
      void init(bool state_info = true, bool label_info = true);
      void init(lts const &l);
      transition *transitions;
      bool state_info;
      ATerm *state_values;
      bool *taus;
      ATerm *label_values;
      bool label_info;
      unsigned int states_size;

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

      /** \brief Gives the MIME type associated with an LTS format.
       * \param[in] type The LTS format.
       * \return The MIME type of the LTS format specified by \a type.
       */
      static std::string mime_type_for_type(const lts_type type);

      /** \brief Gives the set of all supported LTS formats.
       * \return The set of all supported LTS formats. */
      static const std::set<lts_type> &supported_lts_formats();

      /** \brief Gives a textual list describing supported LTS formats.
       * \param[in] default_format The format that should be marked as default
       *                           (or \a lts_none for no default).
       * \param[in] supported      The formats that should be considered
       *                           supported.
       * \return                   A string containing lines of the form
       *                           "  'name' for the ... format". Every line
       *                           except the last is terminated with '\n'. */
      static std::string supported_lts_formats_text(lts_type default_format = lts_none, const std::set<lts_type> &supported = supported_lts_formats());

      /** \brief Gives a textual list describing supported LTS formats.
       * \param[in] supported      The formats that should be considered
       *                           supported.
       * \return                   A string containing lines of the form
       *                           "  'name' for the ... format". Every line
       *                           except the last is terminated with '\n'. */
      static std::string supported_lts_formats_text(const std::set<lts_type> &supported);

      /** \brief Gives a list of extensions for supported LTS formats.
       * \param[in] sep       The separator to use between each extension.
       * \param[in] supported The formats that should be considered supported.
       * \return              A string containing a list of extensions of the
       *                      formats in \a supported, separated by \a sep.
       *                      E.g. "*.aut,*.lts" */
      static std::string lts_extensions_as_string(const std::string &sep = ",", const std::set<lts_type> &supported = supported_lts_formats());

      /** \brief Gives a list of extensions for supported LTS formats.
       * \param[in] supported The formats that should be considered supported.
       * \return              A string containing a list of extensions of the
       *                      formats in \a supported, separated by \a ','.
       *                      E.g. "*.aut,*.lts" */
      static std::string lts_extensions_as_string(const std::set<lts_type> &supported);

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

      /** \brief Creates an LTS from a string in aut format.
       * \param[in] is The input stream from which the data will be read.
       * an attempt is made to determine the format from the contents of the
       * stream. */
      lts(const std::string &s);

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
      unsigned int num_states() const;

      /** \brief Sets the number of states of this LTS.
       * \param[in] n The number of states of this LTS. */
      void set_num_states(const unsigned int n)
      { nstates=n;
      }

      /** \brief Gets the number of transitions of this LTS.
       * \return The number of transitions of this LTS. */
      unsigned int num_transitions() const;

      /** \brief Sets the number of transitions of this LTS.
       */
      void set_num_transitions(unsigned int n) 
      { ntransitions=n;
      }

      /** \brief Gets the number of labels of this LTS.
       * \return The number of labels of this LTS. */
      unsigned int num_labels() const;

      /** \brief Sets the number of labels of this LTS.
       */
      void set_num_labels(unsigned int n)
      { nlabels=n;
      }

      /** \brief Gets the initial state number of this LTS.
       * \return The number of the initial state of this LTS. */
      unsigned int initial_state() const;

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
      ATerm state_value(unsigned int state) const;

      /** \brief Gets the value of a label.
       * \param[in] label The number of the label.
       * \return The value of the label. */
      ATerm label_value(unsigned int label) const;

      /** \brief Gets the value of a state as a string.
       * \param[in] state The number of the state.
       * \return A string representation of the value of the state. */
      std::string state_value_str(unsigned int state) const;

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

      /** \brief Sets the source state of a transition.
       * \param[in] transition The number of the transition.
       * \param[in] from An index of the new from state */
      void set_transition_from(const unsigned int transition, const unsigned int from);

      /** \brief Sets the label of a transition.
       * \param[in] transition The number of the transition.
       * \param[in] label An index of the new label */
      void set_transition_label(const unsigned int transition, const unsigned int label);

      /** \brief Sets the target state of a transition.
       * \param[in] transition The number of the transition.
       * \param[in] to An index of the new to state. */
      void set_transition_to(const unsigned int transition, const unsigned int to);

      /** \brief Gets an iterator to the states of this LTS.
       * \return An iterator to the states of this LTS. */
      state_iterator get_states();

      /** \brief Gets an iterator to the states of this LTS.
       * \return An iterator to the states of this LTS. */
      const_state_iterator get_states() const;

      /** \brief Gets an iterator to the labels of this LTS.
       * \return An iterator to the labels of this LTS. */
      label_iterator get_labels();

      /** \brief Gets an iterator to the labels of this LTS.
       * \return An iterator to the labels of this LTS. */
      const_label_iterator get_labels() const;

      /** \brief Gets an iterator to the transitions of this LTS.
       * \return An iterator to the transitions of this LTS. */
      transition_iterator get_transitions();

      /** \brief Gets an iterator to the transitions of this LTS.
       * \return An iterator to the transitions of this LTS. */
      const_transition_iterator get_transitions() const;

      /** \brief Sets the transitions to t malloced array of transitions t.
       *  The existing transitions are freed if necessary. If t is NULL the
       *  transitions are set to NULL. ntransitions and transition_size are
       *  set to new_number_of_transitions and new_transitions_size respectively */
      void set_transitions(transition *t, const unsigned int new_number_of_transitions, const unsigned int new_transitions_size)
      { if (transitions!=NULL)
        { free(transitions);
        }
        transitions=t;
        ntransitions=new_number_of_transitions;
        transitions_size=new_transitions_size;
      }

      /** \brief Checks whether a label is a tau action.
       * \param[in] label The number of the label.
       * \retval true if the label is a tau action;
       * \retval false otherwise.  */
      bool is_tau(unsigned int label) const;


      /** \brief Sets the tau attribute of a label.
       * \param[in] label The number of the label.
       * \param[in] is_tau Indicates whether the label should become a tau action. */
      void set_tau(unsigned int label, bool is_tau = true);

      /** \brief Sets all labels with string that occurs in tau_actions to tau.
       *  \param[tau_actions] Vector with strings indicating which labels must be
       *       considered to be equal to tau's */
      bool hide_actions(const std::vector<std::string> &tau_actions);

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
      lts_type get_type() const;

      /** \brief Checks whether this LTS has state values associated with its states.
       * \retval true if the LTS has state information;
       * \retval false otherwise.  */
      bool has_state_info() const;

      /** \brief Checks whether this LTS has label values associated with its labels.
       * \retval true if the LTS has label information;
       * \retval false otherwise.  */
      bool has_label_info() const;

      /** \brief Removes the state values from all states. */
      void remove_state_values();

      /** \brief Checks whether states are (recognised) vectors of parameters.
       * \retval true If states are vectors of parameters.
       * \retval false If states are not vectors of parameters. */
      bool has_state_parameters() const;

      /** \brief Returns the number of parameters of states.
       * \return The number of parameters of states.
       * \pre    States have parameters. */
      unsigned int num_state_parameters() const;

      /** \brief Returns the name of a parameter.
       * \param[in] idx  The index of the parameter.
       * \return         The name of the idx'th parameter if it is known and
       *                 'p<idx>' otherwise.
       * \pre            States have parameters. */
      ATerm state_parameter_name(unsigned int idx) const;

      /** \brief Returns the name of a parameter as a string.
       * \param[in] idx  The index of the parameter.
       * \return         The name of the idx'th parameter as string if it is
       *                 known and 'p<idx>' otherwise.
       * \pre            States have parameters. */
      std::string state_parameter_name_str(unsigned int idx) const;

      /** \brief Returns the sort of a parameter.
       * \param[in] idx  The index of the parameter.
       * \return         The sort of the idx'th parameter if it is known and
       *                 'D<idx>' otherwise.
       * \pre            States have parameters. */
      ATerm state_parameter_sort(unsigned int idx) const;

      /** \brief Returns the sort of a parameter as a string.
       * \param[in] idx  The index of the parameter.
       * \return         The sort of the idx'th parameter as string if it is
       *                 known and 'D<idx>' otherwise.
       * \pre            States have parameters. */
      std::string state_parameter_sort_str(unsigned int idx) const;

      /** \brief Returns a parameter value of a state.
       * \param[in] state  The state from which the parameter value must be
       *                   taken.
       * \param[in] idx    The index of the parameter.
       * \return           The value of the idx'th parameter of state.
       * \pre              States have parameters. */
      ATerm get_state_parameter_value(unsigned int state, unsigned int idx) const;

      /** \brief Returns the string representation of a parameter value of a
       *         state.
       * \param[in] state  The state from which the parameter value must be
       *                   taken.
       * \param[in] idx    The index of the parameter.
       * \return           The value of the idx'th parameter of state as a
       *                   string.
       * \pre              States have parameters. */
      std::string get_state_parameter_value_str(unsigned int state, unsigned int idx) const;

      /** \brief Returns a list of all unique label values that occur in this
       *         LTS.
       * \return A list of all unique label values that occur in this LTS.
       * \pre    This LTS has label information. */
      atermpp::set<ATerm> get_label_values() const;

      /** \brief Returns a list of all unique state values that occur in this
       *         LTS.
       * \return A list of all unique state values that occur in this LTS.
       * \pre    This LTS has state information. */
      atermpp::set<ATerm> get_state_values() const;

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
      std::string pretty_print_state_value(ATerm value) const;

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
      void set_data_specification(data::data_specification const& spec);

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

      friend class state_iterator;

      friend class label_iterator;

      friend class transition_iterator;
  };

}
}

#endif
