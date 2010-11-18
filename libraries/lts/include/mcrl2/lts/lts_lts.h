// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/** \file
 *
 * \brief This file contains a class that contains labelled transition systems in lts (mcrl2) format.
 * \details A labelled transition system in lts/mcrl2 format is a transition system 
 * with as state labels vectors of strings, and as transition labels strings.
 * \author Jan Friso Groote
 */


#ifndef MCRL2_LTS_LTS_MCRL2_H
#define MCRL2_LTS_LTS_MCRL2_H

#include <string>
#include <vector>
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_balanced_tree.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/multi_action.h"
#include "mcrl2/lps/typecheck.h"
#include "mcrl2/lts/lts.h"


namespace mcrl2
{
namespace lts
{
  namespace detail
  {

  class state_label_lts : public atermpp::aterm_appl
  {
    public:
      typedef mcrl2::data::data_expression element_type;

    protected: 
      // We just use an atermpp::aterm_appl of the form
      // STATE(t1,...,tn) where ti are the data expressions as
      // the state label for an lts. One might consider replacing
      // these by balanced trees, as these have a substantially 
      // smaller footprint.


      // We store functions symbols with varying arity and string "STATE"
      // in this ATermAppl array, as they are automatically protected.
      static atermpp::vector < ATermAppl > vector_templates;

      static AFun get_STATE_function_symbol(const unsigned int arity)
      { 
        if (arity>=vector_templates.size())
        { 
          vector_templates.resize(arity+1,NULL);
        }
        if (vector_templates[arity]==NULL)
        { 
          ATermAppl stub=ATmakeAppl0(ATmakeAFun("STUB",0,ATfalse));
          ATermList l=ATempty;
          for(unsigned int i=0;i<arity;++i)
          { 
            l=ATinsert(l,(ATerm)stub);
          }
          vector_templates[arity]=ATmakeApplList(ATmakeAFun("STATE",arity,ATfalse),l);
        }
        return ATgetAFun(vector_templates[arity]);
      }

    public:
 
      state_label_lts()
      {
      }

      state_label_lts(const ATermAppl &a):atermpp::aterm_appl(a)
      { 
        const unsigned int arity=ATgetArity(ATgetAFun(a));
        get_STATE_function_symbol(arity); // Create the STATE function symbol with the desired arity.
        assert(ATgetAFun(a)==ATgetAFun(vector_templates[arity]));
      }

      state_label_lts(const mcrl2::data::data_expression_list &l):
                            atermpp::aterm_appl(get_STATE_function_symbol(l.size()),atermpp::aterm_list(l))
      {} 

      state_label_lts(const atermpp::vector < element_type > &l):
                     atermpp::aterm_appl(get_STATE_function_symbol(l.size()),l.begin(),l.end())
      {} 

      element_type operator [](const unsigned int i) const
      {
        assert(i<size());
        return element_type(this->argument(i));
      }

      void set_element(const element_type &e, unsigned int i)
      {
        assert(i<this->size());
        set_argument(e,i);
      } 

      atermpp::aterm_appl aterm() const 
      { 
        return *this; 
      } 
  };

  /** \brief Pretty print a state value of this LTS.
   * \param[in] l  The state value to pretty print.
   * \return           The pretty-printed representation of value.
   * \pre              value occurs as state value in this LTS. */

  inline std::string pp(const state_label_lts l)
  {
    std::string s;
    s = "(";
    for(unsigned int i=0; i<l.size(); ++i)
    { 
      s += core::pp(l[i]);
      if ( i+1<l.size())
      { 
        s += ",";
      }
    }
    s += ")";
    return s;
  } 

  /** \brief Pretty print a state parameter value of this LTS.
   * \param[in] value  The state parameter value to pretty print.
   * \return           The pretty-printed representation of value.
   * \pre              value occurs as state parameter value in this LTS. */
  inline std::string pp(const state_label_lts::element_type &value)
  {
    return core::pp(value); 
  }


  /** \brief A class containing the values for action labels for the .lts format */
  class action_label_lts:public mcrl2::lps::multi_action
  {
    friend struct atermpp::aterm_traits<action_label_lts>;

    public:
      typedef mcrl2::lps::multi_action element_type;

      action_label_lts()
      {}

      action_label_lts(const ATerm a):mcrl2::lps::multi_action(a)
      { 
      }

      action_label_lts(const lps::action a):mcrl2::lps::multi_action(a)
      {
      }

      element_type label() const
      { 
        return *this; 
      }

      ATerm aterm() const
      { 
        assert(!this->has_time());
        return (ATerm)mcrl2::core::detail::gsMakeMultAct(this->actions());
      }

      bool hide_actions(const std::vector<std::string> &tau_actions)
      { 
        using namespace std;
        using namespace mcrl2::lps;

        const action_list mas = this->actions(); 
        action_list new_action_list;
        for(action_list:: const_iterator i=mas.begin(); i!=mas.end(); ++i)
        { 
          const action a=*i; 

          if (std::find(tau_actions.begin(),tau_actions.end(),
                      string(a.label().name()))==tau_actions.end())  // this action must not be hidden.
          { 
            new_action_list=push_front(new_action_list,a);
          }
        }
        const bool is_tau=new_action_list.empty(); 
        m_actions=new_action_list;

        return is_tau;
      }
  };


  inline std::string pp(const action_label_lts l)
  { 
    return pp(mcrl2::lps::multi_action(l));
  }

  inline action_label_lts parse_lts_action(
                     const std::string &multi_action_string,
                     const data::data_specification &data_spec,
                     const lps::action_list &act_decls)
  {
    std::stringstream ss(multi_action_string);
  
    ATermAppl t = mcrl2::core::parse_mult_act(ss);
    if ( t == NULL )
    {
      throw mcrl2::runtime_error("Fail to parse multi action " + multi_action_string);
    } 
    
    lps::multi_action ma=lps::action_list((ATermList)ATgetArgument(t,0));
    lps::type_check(ma,data_spec,act_decls);
  
    return action_label_lts((ATerm)mcrl2::core::detail::gsMakeMultAct(ma.actions()));
  } 


  } // namespace detail


  /** \brief Stores additional LTS information.
   * \details This class is provided for storing additional information with an
   * LTS. This can be either a muCRL specification, an mCRL2 specificiation, or
   * options for the Dot format. */

  class lts_lts_t : public lts< detail::state_label_lts, detail::action_label_lts >
  {

    private:
      // lts_type m_type;
      std::string m_creator; // Creator is also used for the name of the graph in a .dot file.
      
      bool m_has_valid_data_spec;
      data::data_specification m_data_spec;
      bool m_has_valid_parameters;
      data::variable_list m_parameters;
      bool m_has_valid_action_decls;
      lps::action_label_list m_action_decls;

    public:

      /** \brief Creates an object containing no information. */
      lts_lts_t():
           // m_type(lts_none),
           m_has_valid_data_spec(false),
           m_has_valid_parameters(false),
           m_has_valid_action_decls(false)
      {};

      /** \brief Creates an object containing a muCRL specification.
       * \param[in] t The muCRL specification that will be stored in the object. */
      lts_lts_t(ATerm t);

      /** \brief Creates an object containing an mCRL2 specification.
       * \param[in] spec The mCRL2 specification that will be stored in the object. */
      lts_lts_t(lps::specification const& spec);

      /** \brief Copy constructor */

      lts_lts_t(const lts_lts_t &l):
           lts< detail::state_label_lts, detail::action_label_lts >(l),
           // m_type(l.m_type),
           m_creator(l.m_creator),
           m_has_valid_data_spec(l.m_has_valid_data_spec),
           m_data_spec(l.m_data_spec),
           m_has_valid_parameters(l.m_has_valid_parameters),
           m_parameters(l.m_parameters),
           m_has_valid_action_decls(l.m_has_valid_action_decls),
           m_action_decls(l.m_action_decls)
      {
      }

      /** \brief Standard destructor for the class lts_lts
      */
      ~lts_lts_t()
      {}

      void swap(lts_lts_t &l)
      {
        lts< detail::state_label_lts, detail::action_label_lts >(l).swap(*this);
      
        // { const lts_type aux=m_type; m_type=l.m_type; l.m_type=aux; }
        m_creator.swap(l.m_creator);
        { const bool aux=m_has_valid_data_spec; m_has_valid_data_spec=l.m_has_valid_data_spec; l.m_has_valid_data_spec=aux; }
        { const data::data_specification aux=m_data_spec; m_data_spec=l.m_data_spec; l.m_data_spec=aux; }
        { const bool aux=m_has_valid_parameters; m_has_valid_parameters=l.m_has_valid_parameters; l.m_has_valid_parameters=aux; }
        { const data::variable_list aux=m_parameters; m_parameters=l.m_parameters; l.m_parameters=aux; }
        { const bool aux=m_has_valid_action_decls; m_has_valid_action_decls=l.m_has_valid_action_decls; l.m_has_valid_action_decls=aux; }
        { const lps::action_label_list aux=m_action_decls; m_action_decls=l.m_action_decls; l.m_action_decls=aux; }
      }

      lts_type type()
      {
        return lts_lts;
      }

      /** \brief Return the mCRL2 data specification of this LTS.
       * \return The mCRL2 data specification of this LTS.
       * \pre    This LTS has an mCRL2 data specification. 
      */
      data::data_specification data() const
      {
        assert(m_has_valid_data_spec);
        return m_data_spec; // data::data_specification((ATermAppl)ATgetArgument((ATermAppl) extra_data,0));
      }

      /** \brief Indicates whether the labels in lts_extra is valid
      */

      bool has_action_labels() const
      { 
        return m_has_valid_action_decls;
      }
      
      /** \brief Return action label declarations stored in this LTS.
       * \return The action labels stored in this LTS 
      */
      lps::action_label_list action_labels() const
      {
        assert(m_has_valid_action_decls);
        return m_action_decls;
      } 
      
      /** \brief Set the action label information for this LTS.
       * \param[in] decls  The action labels to be assigned to this lts.
      */
      void set_action_labels(const lps::action_label_list &decls)
      {
        m_has_valid_action_decls=true;
        m_action_decls=decls;
      } 

      
      /** \brief Indicates whether the data in lts_extra is valid
      */

      bool has_data() const
      { 
        return m_has_valid_data_spec;
      }
      
      /** \brief Set the mCRL2 data specification of this LTS.
       * \param[in] spec  The mCRL2 data specification for this LTS.
       * \pre             This is an mCRL2 LTS. 
      */
      void set_data(data::data_specification const& spec)
      {
        m_has_valid_data_spec=true;
        m_data_spec=spec;
      }


      /** \brief Checks whether this LTS has a creator.
       * \retval true if the label has a creator;
       * \retval false otherwise.  
      */
      bool has_creator() const
      {
        return !m_creator.empty();
      }
      
      /** \brief Gets the creator of this LTS.
       * \return The creator string.
      */
      std::string creator() const
      {
        return m_creator;
      }
      
      /** \brief Sets the creator of this LTS.
       * \param[in] creator The creator string.
      */
      void set_creator(const std::string &creator)
      {
        this->m_creator = creator;
      }

      /** \brief Indicates whether the process parameters are valid
      */

      bool has_process_parameters() const
      { 
        return m_has_valid_parameters;
      }

      /** \brief Return the parameters of the state vectors stored in this LTS.
       * \return The state parameters stored in this LTS.
      */
      data::variable_list process_parameters() const
      { 
        assert(m_has_valid_parameters);
        return m_parameters;
      } 
      
      /** \brief Returns the i-th parameter of the state vectors stored in this LTS.
       * \return The state parameters stored in this LTS.
      */
      data::variable process_parameter(unsigned int i) const
      { 
        assert(i<m_parameters.size());
        assert(m_has_valid_parameters);
        return data::variable(ATelementAt(m_parameters,i));
      } 
      

      /** \brief Set the state parameters for this LTS.
       * \param[in] params  The state parameters for this lts.
      */
      void set_process_parameters(const data::variable_list &params)
      { 
        // assert(num_state_parameters()==0 || num_state_parameters()==params.size());
        m_has_valid_parameters=true;
        m_parameters=params;
      } 
      
      /** \brief Save the labelled transition system to file.
       *  \details If the filename is empty, the result is read from stdout.
       *  \param[in] filename Name of the file from which this lts is read.
       */
      void load(const std::string &filename);

      /** \brief Save the labelled transition system to file.
       *  \details If the filename is empty, the result is written to stdout.
       *  \param[in] filename Name of the file to which this lts is written.
       */
      void save(const std::string &filename) const;
  };
} // namespace lts
} // namespace mcrl2

/// \cond INTERNAL_DOCS
namespace atermpp {
  template<>
  struct aterm_traits<mcrl2::lts::detail::action_label_lts>
  {
    typedef ATermAppl aterm_type;
    static void protect(mcrl2::lts::detail::action_label_lts t)   { t.protect(); }
    static void unprotect(mcrl2::lts::detail::action_label_lts t) { t.unprotect(); }
    static void mark(mcrl2::lts::detail::action_label_lts t)      { t.mark(); }
  // static ATerm term(mcrl2::lts::detail::action_label_lts t)     { return t.term(); }
  // static ATerm* ptr(mcrl2::lts::detail::action_label_lts& t)    { return &t.term(); }
  };
} // namespace atermpp
/// \endcond


#endif
