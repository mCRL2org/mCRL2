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

  class state_label_mcrl2
  {
    public:
      typedef ATermAppl element_type;

    protected: 
      ATermAppl m_state_label_mcrl2;

      // We store functions symbols with varying arity and string "STATE"
      // in this ATermAppl array, as they are automatically protected.

      static atermpp::vector < ATermAppl > vector_templates;

      static void make_vector_template_if_required(const unsigned int arity)
      { 
        if (arity>=vector_templates.size())
        { 
          vector_templates.resize(arity+1);
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

      }

    public:
 
      state_label_mcrl2():m_state_label_mcrl2(NULL)
      {}

      state_label_mcrl2(ATermAppl a):m_state_label_mcrl2(a)
      { 
        assert(ATgetType(m_state_label_mcrl2)==AT_APPL);
      }

      template <class ITERATOR>
      state_label_mcrl2(const ITERATOR begin, const ITERATOR end)
      { 
        unsigned int arity=0;
        ATermList l=ATempty;
        for(ITERATOR i=begin; i!=end; ++i)
        { 
          // TODO. This should be more efficient.
          ATinsert(l,*i); 
          arity++;
        }
        make_vector_template_if_required(arity);

        m_state_label_mcrl2=(ATermAppl)ATmakeApplList(ATgetAFun(vector_templates[arity]),ATreverse(l));
      }

      state_label_mcrl2(const ATermList l)
      {
        const unsigned int arity=ATgetLength(l);
        make_vector_template_if_required(arity);
        m_state_label_mcrl2=(ATermAppl)ATmakeApplList(ATgetAFun(vector_templates[arity]),l);
      }

      unsigned int size() const
      { 
        return ATgetArity(ATgetAFun(m_state_label_mcrl2));
      }
      
      element_type operator [](const unsigned int i) const
      {
        assert(i<size());
        return element_type(ATgetArgument(m_state_label_mcrl2,i));
      }

      bool operator ==(const state_label_mcrl2 &other) const
      { 
        return m_state_label_mcrl2==other.m_state_label_mcrl2;
      }

      bool operator !=(const state_label_mcrl2 &other) const
      { 
        return m_state_label_mcrl2!=other.m_state_label_mcrl2;
      }

      void set_element(const element_type &e, unsigned int i)
      {
        assert(i<size());
        m_state_label_mcrl2=ATsetArgument(m_state_label_mcrl2,(ATerm)e,i);
      } 

      ATermAppl aterm() const
      {
        return m_state_label_mcrl2;
      }
  };

  /** \brief Pretty print a state value of this LTS.
   * \param[in] l  The state value to pretty print.
   * \return           The pretty-printed representation of value.
   * \pre              value occurs as state value in this LTS. */

  inline std::string pp(const state_label_mcrl2 l)
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
  inline std::string pp(const state_label_mcrl2::element_type &value)
  {
    AFun head = ATgetAFun(value);
    if ((ATgetArity(head) == 0) && (ATisQuoted(head) == ATtrue)) // Type of parameter is a string.
    {
      std::string s = ATwriteToString((ATerm) value);
      return s.substr(1,s.size()-2);
    }
    if ((ATgetArity(head) == 0) && (ATisQuoted(head) == ATfalse)) // Type of parameter is a string.
    {
      std::string s = ATwriteToString((ATerm) value);
      return s;
    }
    return core::pp(value); //PrintPart_CXX(value,ppDefault);
  }


  class action_label_mcrl2;
  inline std::string pp(const action_label_mcrl2 l);
  ATerm sort_multi_action(ATerm ma);

  class action_label_mcrl2
  {
    public:
      typedef mcrl2::lps::multi_action element_type;

    protected: 
      element_type m_action_label_mcrl2;

    public:
      action_label_mcrl2():m_action_label_mcrl2()
      {}

      action_label_mcrl2(const ATerm a):m_action_label_mcrl2(a)
      { 
      }

      element_type label() const
      { 
        return m_action_label_mcrl2;
      }

      ATerm aterm() const
      { 
        assert(!m_action_label_mcrl2.has_time());
        return (ATerm)mcrl2::core::detail::gsMakeMultAct(m_action_label_mcrl2.actions());
      }

      bool operator ==(const action_label_mcrl2 &other) const
      {
        return m_action_label_mcrl2==other.m_action_label_mcrl2;
      }

      bool operator !=(const action_label_mcrl2 &other) const
      {
        return m_action_label_mcrl2!=other.m_action_label_mcrl2;
      }

      bool operator<(const action_label_mcrl2 &l) const
      { 
        return this->m_action_label_mcrl2<l.m_action_label_mcrl2;
      }


      bool hide_actions(const std::vector<std::string> &tau_actions)
      { 
        using namespace std;
        std::cerr << "Hide actions in .lts file is not yet re-implemented \n";
        return true;

        /* const bool is_string=((ATgetArity(ATgetAFun(m_action_label_mcrl2)) == 0) && (ATisQuoted(ATgetAFun(m_action_label_mcrl2)) == ATtrue));
        string s=detail::pp(m_action_label_mcrl2);
        stringstream ss(s);
        ATermAppl t=mcrl2::core::parse_mult_act(ss);

        if ( t == NULL )
        {
          mcrl2::runtime_error("Cannot reconstruct multi action " + s + " (parsing)");
        }

        ATermList new_multi_action=ATempty;
        for(ATermList mas=(ATermList)ATgetArgument(t,0); mas!=ATempty; mas=ATgetNext(mas))
        { 
          ATermAppl multiaction=(ATermAppl)ATgetFirst(mas);

          if (std::find(tau_actions.begin(),tau_actions.end(),
                      string(ATgetName(ATgetAFun(ATgetArgument(multiaction,0)))))==tau_actions.end())  // this action must not be hidden.
          { 
            new_multi_action=ATinsert(new_multi_action,(ATerm)multiaction);
          }
        }
        const bool is_tau=new_multi_action==ATempty; 
        m_action_label_mcrl2=sort_multi_action((ATerm)mcrl2::core::detail::gsMakeMultAct(new_multi_action));

        if (is_string)
        { 
          m_action_label_mcrl2=(ATerm)ATmakeAppl0(ATmakeAFun(ATwriteToString(m_action_label_mcrl2),0,ATtrue));
        }
        
        return is_tau;
        */
      }
  };


  inline std::string pp(const action_label_mcrl2 l)
  { 
    return l.label().to_string();

    /* using namespace mcrl2::core;
    if ((ATgetArity(ATgetAFun(l.label())) == 0) && (ATisQuoted(ATgetAFun(l.label())) == ATtrue))
    {
      std::string s=ATwriteToString(l.label());
      return s.substr(1,s.length()-2);
    }
    if ((ATgetArity(ATgetAFun(l.label())) == 0) && (ATisQuoted(ATgetAFun(l.label())) == ATfalse))
    {
      std::string s=ATwriteToString(l.label());
      return s;
    }

    return PrintPart_CXX((ATerm)l.label(),ppDefault); */
  }

  inline action_label_mcrl2 parse_mcrl2_action(
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
  
    return action_label_mcrl2((ATerm)mcrl2::core::detail::gsMakeMultAct(ma.actions()));
  } 


  } // namespace detail


  /** \brief Stores additional LTS information.
   * \details This class is provided for storing additional information with an
   * LTS. This can be either a muCRL specification, an mCRL2 specificiation, or
   * options for the Dot format. */

  class lts_svc_t : public lts_< detail::state_label_mcrl2, detail::action_label_mcrl2 >
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
      lts_svc_t():
           // m_type(lts_none),
           m_has_valid_data_spec(false),
           m_has_valid_parameters(false),
           m_has_valid_action_decls(false)
      {};

      /** \brief Creates an object containing a muCRL specification.
       * \param[in] t The muCRL specification that will be stored in the object. */
      lts_svc_t(ATerm t);

      /** \brief Creates an object containing an mCRL2 specification.
       * \param[in] spec The mCRL2 specification that will be stored in the object. */
      lts_svc_t(lps::specification const& spec);

      /** \brief Copy constructor */

      lts_svc_t(const lts_svc_t &l):
           lts_< detail::state_label_mcrl2, detail::action_label_mcrl2 >(l),
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

      /** \brief Standard destructor for the class lts_svc
      */
      ~lts_svc_t()
      {}

      void swap(lts_svc_t &l)
      {
        lts_< detail::state_label_mcrl2, detail::action_label_mcrl2 >(l).swap(*this);
      
        // { const lts_type aux=m_type; m_type=l.m_type; l.m_type=aux; }
        m_creator.swap(l.m_creator);
        { const bool aux=m_has_valid_data_spec; m_has_valid_data_spec=l.m_has_valid_data_spec; l.m_has_valid_data_spec=aux; }
        { const data::data_specification aux=m_data_spec; m_data_spec=l.m_data_spec; l.m_data_spec=aux; }
        { const bool aux=m_has_valid_parameters; m_has_valid_parameters=l.m_has_valid_parameters; l.m_has_valid_parameters=aux; }
        { const data::variable_list aux=m_parameters; m_parameters=l.m_parameters; l.m_parameters=aux; }
        { const bool aux=m_has_valid_action_decls; m_has_valid_action_decls=l.m_has_valid_action_decls; l.m_has_valid_action_decls=aux; }
        { const lps::action_label_list aux=m_action_decls; m_action_decls=l.m_action_decls; l.m_action_decls=aux; }
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

#endif
