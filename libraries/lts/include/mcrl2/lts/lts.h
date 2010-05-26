
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
#include <stdio.h>
#include <stdlib.h>

#include "boost/iterator/transform_iterator.hpp"

#include "aterm2.h"
#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/atermpp/container_utility.h"

#include "mcrl2/lts/transition.h"
#include "mcrl2/exception.h"


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

  namespace detail
  {
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
  } // namespace details

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
	detail::lts_dot_options dot_options;
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
      lts_extra(detail::lts_dot_options opts);

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
      detail::lts_dot_options get_dot_options();
  };

  /** \brief An empty lts_extra object. */
  extern lts_extra lts_no_extra;


  /** \brief Class for labelled transition systems.
   * \details This class provides an interface for creating and manipulating
   * labelled transition systems. */
  class lts 
  {
    private:
      lts_type type;
      std::string creator;

      unsigned int nstates;
      unsigned int nlabels;
      unsigned int init_state;
      std::vector<transition> transitions;
      atermpp::vector < ATerm > state_values;
      atermpp::vector < ATerm > label_values;
      std::vector < bool > taus; // A vector indicating which labels are to be viewed as tau's.

      ATerm extra_data;

      void init();

    public: // The items below are public, to allow restructuring of the code of the lts library.

      void clear_type();
      void clear_states();
      void clear();

    public:

      /** \brief Creates an empty LTS.
       */
      lts();

      /** \brief Creates an LTS and reads its data from a file.
       * \param[in] filename The name of the file from which the data will be
       * read.
       * \param[in] type The format of the file. If \a lts_none is passed then
       * an attempt is made to determine the format from the contents of the
       * file. */
      lts(std::string &filename, lts_type type = lts_none, lts_extra extra= lts_no_extra);

      /** \brief Creates an LTS and reads its data from an input stream.
       * \param[in] is The input stream from which the data will be read.
       * \param[in] type The format of the data. If \a lts_none is passed then
       * an attempt is made to determine the format from the contents of the
       * stream. */
      lts(std::istream &is, lts_type type = lts_none, lts_extra extra= lts_no_extra);

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

      /** \brief Swap this lts with the supplied supplied LTS.
       * \param[in] l The LTS to swap. */
      void swap(lts &l);

      /** \brief Writes LTS data to a file.
       * \param[in] filename The name of the file to which data will be written.
       * \param[in] type The format of the file.
       * \param[in] extra Additional information for the output.
       * \retval true if the write operation succeeded;
       * \retval false otherwise.*/
      void write_to(std::string const& filename, lts_type type = lts_mcrl2, lts_extra extra = lts_no_extra) const;

      /** \brief Writes LTS data to an output stream.
       * \details This is not supported for SVC and BCG files.
       * \param[in] os The output stream to which data will be written.
       * \param[in] type The format of the output stream.
       * \param[in] extra Additional information for the output.
       * \retval true if the write operation succeeded;
       * \retval false otherwise.*/
      void write_to(std::ostream &os, lts_type type = lts_mcrl2, lts_extra extra = lts_no_extra) const;

      /** \brief Gets the number of states of this LTS.
       * \return The number of states of this LTS. */
      unsigned int num_states() const
      {
        return nstates;
      }

      /** \brief Sets the number of states of this LTS.
       * \param[in] n The number of states of this LTS. */
      void set_num_states(const unsigned int n, const bool has_state_values=true)
      { 
        nstates=n;
        if (has_state_values)
        {
          if (state_values.size()>0)
          { state_values.resize(n);
          }
          else
          { 
            state_values=atermpp::vector <ATerm>();
          }
        }
        else
        { 
          state_values=atermpp::vector <ATerm>();
        }
      }

      /** \brief Gets the number of transitions of this LTS.
       * \return The number of transitions of this LTS. */
      unsigned int num_transitions() const
      { return transitions.size();
      }

      /** \brief Gets the number of labels of this LTS.
       * \return The number of labels of this LTS. */
      unsigned int num_labels() const
      { return nlabels;
      }

      /** \brief Sets the number of labels of this LTS.
       *  \param [in] n The new number of labels.
       *  \param [in] has_label_info An optional boolean indicating whether the lts
       *              has label values.
       */
      void set_num_labels(unsigned int n, bool has_label_info=true)
      { nlabels=n;
        taus.resize(n,false);
        if (has_label_info)
        { label_values.resize(n);
        }
        else
        {
          label_values=atermpp::vector < ATerm >();
        }
      } 

      /** \brief Gets the initial state number of this LTS.
       * \return The number of the initial state of this LTS. */
      unsigned int initial_state() const
      { return init_state;
      }

      /** \brief Sets the initial state number of this LTS.
       * \param[in] state The number of the state that will become the initial
       * state. */
      void set_initial_state(unsigned int state)
      { assert(state<nstates);
        init_state=state;
      }

      /** \brief Adds a state to this LTS.
       * \param[in] value The value of the state. If value is NULL, only the state, and
       *                  no state value is added. This is only allowed, if there are no
       *                  state_values.
       * \return The number of the added state. */
      unsigned int add_state(const ATerm value = NULL)
      { if (value!=NULL)
        { assert(nstates==state_values.size());
          state_values.push_back(value);
        }
        return nstates++;
      }

      /** \brief Adds a label to this LTS.
       * \param[in] value The value of the label.
       * \param[in] is_tau Indicates whether the label is a tau action.
       * \return The number of the added label. */
      unsigned int add_label(ATerm value = NULL, bool is_tau = false)
      { 
        // ATfprintf(stderr,"Add label %t %d %d\n",value,is_tau,nlabels);
        
        assert(nlabels==taus.size());
        taus.push_back(is_tau);
        
        if ( value!=NULL )
        { // std::cerr << "nlabels: " << nlabels << "  label_values size " << label_values.size() << "\n";
          assert(nlabels==label_values.size());
          label_values.push_back(value);
        }
        else
        { assert(label_values.size()==0);
        }
        return nlabels++;
      }


      /** \brief Adds a label to this LTS.
       * \param[in] is_tau Indicates whether the label is a tau action.
       * \return The number of the added label. */
      unsigned int add_label(bool is_tau)
      { 
        return  add_label(NULL,is_tau);
      }

      /** \brief Sets the value of a state.
       * \param[in] state The number of the state.
       * \param[in] value The value that will be assigned to the state. */
      void set_state_value(unsigned int state, ATerm value)
      { 
        assert(state<nstates);
        assert(nstates==state_values.size());
        state_values[state]=value;
      }

      /** \brief Sets the value of a label.
       * \param[in] label The number of the label.
       * \param[in] value The value that will be assigned to the label.
       * \param[in] is_tau Indicates whether the label is a tau action. */
      void set_label_value(unsigned int label, ATerm value, bool is_tau = false)
      { // ATfprintf(stderr,"Set label %t %d %d %d\n",value,is_tau,label,nlabels);
        assert(label<nlabels);
        assert(nlabels==label_values.size());
        label_values[label]=value;
        assert(nlabels==taus.size());
        taus[label]=is_tau;
      };

      /** \brief Gets the value of a state.
       * \param[in] state The number of the state.
       * \return The value of the state. */
      ATerm state_value(unsigned int state) const
      { 
        assert(state<nstates);
        assert(nstates==state_values.size());
        return state_values[state];
      }

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
      std::string label_value_str(unsigned int label) const;

      /** \brief Clear the transitions of an lts.
       * \details This resets the transition vector in an lts, but
       *          leaves other related items, such as state or 
       *          action labels untouched. */
      void clear_transitions()
      { transitions=std::vector<transition>();
      }

      /** \brief Clear the labels of an lts.
       * \details This resets the labels of an lts, but
       *          leaves the number of labels untouched.
       *          it also does not change the information 
       *          regarding to what actions are tau */
      void clear_labels()
      { // std::cerr << "Clear labels\n";
        label_values = atermpp::vector<ATerm>();
        taus = std::vector < bool >();
        nlabels = 0;
      }

      /** \brief Gets an iterator range to the transitions of this LTS.
       * \return A const iterator range to the transitions of this LTS. */
      transition_const_range get_transitions() const
      { return transition_const_range(transitions);
      }

      /** \brief Gets an iterator range to the transitions of this LTS.
       * \return An iterator range to the transitions of this LTS. 
       *         The transitions can be altered. */
      transition_range get_transitions() 
      { return transition_range(transitions);
      }

      /** \brief Add a transition to the lts. 
      */
      void add_transition(const transition &t)     
      { // std::cerr << "Add transition " << t.from() << "-" << t.label() << "->" << t.to() << "  #S " <<
        //       nstates << "#L " << nlabels << "\n";
        assert(t.from()<nstates && t.label()<nlabels && t.to()<nstates);
        transitions.push_back(t);
      }

      /** \brief Checks whether a label is a tau action.
       * \param[in] label The number of the label.
       * \retval true if the label is a tau action;
       * \retval false otherwise.  */
      bool is_tau(unsigned int label) const
      { assert(label<taus.size());
        return taus[label];
      }

      /** \brief Sets the tau attribute of a label.
       * \param[in] label The number of the label.
       * \param[in] is_tau Indicates whether the label should become a tau action. */
      void set_tau(unsigned int label, bool is_tau = true)
      { assert(label<taus.size());
        taus[label]=is_tau;
      }

      /** \brief Sets all labels with string that occurs in tau_actions to tau.
       *  \param[tau_actions] Vector with strings indicating which labels must be
       *       considered to be equal to tau's */
      bool hide_actions(const std::vector<std::string> &tau_actions);

      // /** \brief Checks whether this LTS has a creator.
      //  * \retval true if the label has a creator;
      //  * \retval false otherwise.  */
      bool has_creator();

      // /** \brief Gets the creator of this LTS.
      //  * \return The creator string.*/
      std::string get_creator();

      // /** \brief Sets the creator of this LTS.
      //  * \param[in] creator The creator string.*/
      void set_creator(std::string creator);

      /** \brief Gets the file type of the last successful call to load.
       * \details Note that this does not mean that the information
       * currently in the object is still in this format.
       * \return The format of this LTS. */
      lts_type get_type() const;

      /** \brief Sets the file type of this lts.
       * \param[in] t The format of this LTS. */
      void set_type(const lts_type t)
      { type=t;
      }

      /** \brief Checks whether this LTS has state values associated with its states.
       * \retval true if the LTS has state information;
       * \retval false otherwise.  */
      bool has_state_info() const
      { return state_values.size()>0;
      }

      /** \brief Checks whether this LTS has label values associated with its labels.
       * \retval true if the LTS has label information;
       * \retval false otherwise.  */
      bool has_label_info() const
      { return label_values.size()>0;
      }

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

  };
}
}

#endif
