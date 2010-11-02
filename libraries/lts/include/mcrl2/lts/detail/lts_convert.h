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
 * \brief This file contains lts_convert routines that translate different lts formats into each other.
 * \details For each pair of lts formats there is a translation of one format into the other,
            if such a translation is possible.
 * \author Jan Friso Groote, Muck van Weerdenburg
 */


#ifndef MCRL2_LTS_DETAIL_LTS_CONVERT_H
#define MCRL2_LTS_DETAIL_LTS_CONVERT_H

#include <sstream>
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/lts/lts_aut.h"
#include "mcrl2/lts/lts_fsm.h"
#include "mcrl2/lts/lts_bcg.h"
#include "mcrl2/lts/lts_dot.h"
#include "mcrl2/lts/lts_svc.h"

namespace mcrl2
{
namespace lts
{
namespace detail
{
 
  template <class CONVERTOR, class LTS_IN_TYPE, class LTS_OUT_TYPE>
  inline void convert_core_lts(CONVERTOR &c,
                               const LTS_IN_TYPE &lts_in,
                               LTS_OUT_TYPE &lts_out);

  /** \brief Convert LTSs to each other. The input lts is not usable afterwards.
      \detail Converts the input lts into the output lts maintaining the nature
              of the lts as good as possible. If no translation can be provided
              because more information is required (such as data types), then
              a mcrl2::runtime error is thrown. This also happens if the particular
              translation is not implemented.
  */
  template < class LTS_IN_TYPE, class LTS_OUT_TYPE >
  inline void lts_convert(const LTS_IN_TYPE &lts_in, LTS_OUT_TYPE & lts_out)
  {
    throw mcrl2::runtime_error("Conversion between lts types is not defined (without extra information)");
  }

  /** \brief Convert LTSs to each other. The input lts is not usable afterwards.
      \details See lts_convert for an explanation. The extra information that is 
               provided, is used for the translation, provided that extra_data_is_defined
               is set to true. This extra boolean makes it possible to dynamically conclude
               whether the required data is available, and if not, this boolean can be set to
               false, and the extra data will not be used. If the extra information is not needed in 
               the translation, it is not used and a message is printed to stderr. 
  */
  template < class LTS_IN_TYPE, class LTS_OUT_TYPE >
  inline void lts_convert(
                 const LTS_IN_TYPE &lts_in, 
                 LTS_OUT_TYPE & lts_out,
                 const data::data_specification &data,
                 const lps::action_label_list &action_labels,
                 const data::variable_list &process_parameters,
                 const bool extra_data_is_defined=true)
  {
    if (!extra_data_is_defined)
    { 
      lts_convert(lts_in,lts_out);
    }
    else
    {
      throw mcrl2::runtime_error("Conversion between lts types is not defined (with extra information)");
    }
  }
// ================================================================
//
// Below the translations for labelled transition system formats
// to each other are provided. If a translation is not given,
// the routines above are used to indicate at runtime that a
// required translation does not exist.
//
// ================================================================


// ====================== mcrl2->mcrl2 =============================

  inline void lts_convert(
                const lts_lts_t &lts_in, 
                lts_lts_t &lts_out)
  {  
    lts_out=lts_in;
  }

  inline void lts_convert(
                const lts_lts_t &lts_in,
                lts_lts_t &lts_out,
                const data::data_specification &data,
                const lps::action_label_list &action_labels,
                const data::variable_list &process_parameters,
                const bool extra_data_is_defined=true)
  {
    if (extra_data_is_defined)
    {
      std::cerr << "While translating .mcrl2 to .mcrl2, additional information (data specification, action declarations and process parameters) are ignored.\n";
    }
    lts_convert(lts_in,lts_out);
  }

// ====================== mcrl2->fsm =============================

  class lts_fsm_convertor
  {
    private:
      std::vector < atermpp::map <data::data_expression , unsigned int > > state_element_values_sets;
      lts_fsm_t &lts_out;

    public:
      lts_fsm_convertor(unsigned int n, lts_fsm_t &l):
              state_element_values_sets(std::vector < atermpp::map <data::data_expression , unsigned int > >
                       (n,atermpp::map <data::data_expression , unsigned int >())),
              lts_out(l)
      {
      }

      action_label_string translate_label(const action_label_lts &l) const
      { 
        return pp(l);
      }

      state_label_fsm translate_state(const state_label_lts &l) 
      { 
      
        std::vector < unsigned int > result;
        for(unsigned int i=0; i<l.size(); ++i)
        { 
          const data::data_expression t=l[i];
          atermpp::map <data::data_expression , unsigned int >::const_iterator index=state_element_values_sets[i].find(t);
          if (index==state_element_values_sets[i].end())
          {
            const unsigned int element_index=state_element_values_sets[i].size();
            result.push_back(element_index);
            lts_out.add_state_element_value(i,core::pp(t));
            state_element_values_sets[i][t]=element_index;
          }
          else
          { 
            result.push_back(index->second);
          }
        }
        return result;
      }
  };

  inline void lts_convert(
                const lts_lts_t &lts_in, 
                lts_fsm_t &lts_out)
  {  
    lts_out.clear_process_parameters();
    for(data::variable_list::const_iterator i=lts_in.process_parameters().begin();
                 i!=lts_in.process_parameters().end(); ++i)
    { 
      lts_out.add_process_parameter(core::pp(*i),core::pp(i->sort()));
    }
    
    lts_fsm_convertor c(lts_in.process_parameters().size(),lts_out);
    convert_core_lts(c,lts_in,lts_out);
    
  }

  inline void lts_convert(
                const lts_lts_t &lts_in,
                lts_fsm_t &lts_out,
                const data::data_specification &data,
                const lps::action_label_list &action_labels,
                const data::variable_list &process_parameters,
                const bool extra_data_is_defined=true)
  {
    if (extra_data_is_defined)
    {
      std::cerr << "While translating .mcrl2 to .fsm, additional information (data specification, action declarations and process parameters) are ignored.\n";
    }
    lts_convert(lts_in,lts_out);
  }

// ====================== mcrl2->aut =============================

  class lts_aut_convertor
  {
    public:
      action_label_string translate_label(const action_label_lts &l) const
      { 
        return pp(l);
      }

      state_label_aut translate_state(const state_label_lts &l) const
      { 
        return state_label_aut();
      }
  };

  inline void lts_convert(
                const lts_lts_t &lts_in, 
                lts_aut_t &lts_out)
  {  
    lts_aut_convertor c;
    convert_core_lts(c,lts_in,lts_out);
  }

  inline void lts_convert(
                const lts_lts_t &lts_in,
                lts_aut_t &lts_out,
                const data::data_specification &data,
                const lps::action_label_list &action_labels,
                const data::variable_list &process_parameters,
                const bool extra_data_is_defined=true)
  {
    if (extra_data_is_defined)
    {
      std::cerr << "While translating .mcrl2 to .aut, additional information (data specification, action declarations and process parameters) are ignored.\n";
    }
    lts_convert(lts_in,lts_out);
  }

// ====================== mcrl2->dot =============================

  class lts_dot_convertor
  {
    unsigned int m_state_count;

    public:

      lts_dot_convertor():m_state_count(0)
      {}

      action_label_string translate_label(const action_label_lts &l) const
      { 
        return pp(l);
      }

      state_label_dot translate_state(const state_label_lts &l) 
      { 
        std::stringstream state_name;
        state_name << "s" << m_state_count;
        m_state_count++;
        return state_label_dot(state_name.str(),pp(l));
      }
  };

  inline void lts_convert(
                const lts_lts_t &lts_in, 
                lts_dot_t &lts_out)
  {  
    lts_out=lts_dot_t();
    lts_dot_convertor c;
    convert_core_lts(c,lts_in,lts_out);
  }

  inline void lts_convert(
                const lts_lts_t &lts_in,
                lts_dot_t &lts_out,
                const data::data_specification &data,
                const lps::action_label_list &action_labels,
                const data::variable_list &process_parameters,
                const bool extra_data_is_defined=true)
  {
    if (extra_data_is_defined)
    {
      std::cerr << "While translating .mcrl2 to .dot, additional information (data specification, action declarations and process parameters) are ignored.\n";
    }
    lts_convert(lts_in,lts_out);
  }

// ====================== mcrl2->bcg =============================

#ifdef USE_BCG
  inline void lts_convert(
                const lts_lts_t &lts_in, 
                lts_bcg_t &lts_out)
  {  
    lts_out=lts_in;
  }
 
#endif

// ====================== aut -> mcrl2 =============================
  class aut_lts_convertor
  {
    protected:
      const data::data_specification &m_data;
      const lps::action_label_list &m_action_labels;

    public:
      aut_lts_convertor(
                const data::data_specification &data,
                const lps::action_label_list &action_labels):
                     m_data(data),
                     m_action_labels(action_labels)
      {}

      action_label_lts translate_label(const action_label_string &l1) const
      {
        std::string l(l1);  
        action_label_lts al;
        // Remove quotes, if present in the action label string.
        if ((l.size()>=2) && 
            (l.substr(0,1)=="\"") && 
            (l.substr(l.size()-1,l.size())=="\""))
        {
          l=l.substr(1,l.size()-1);
        }
        
        try
        { 
          al=parse_lts_action(l,m_data,m_action_labels);
        }
        catch (mcrl2::runtime_error &e)
        {
          throw mcrl2::runtime_error("Parse error in action label " + l1 + ".\n" + e.what());
        }
        return al;
      }

      state_label_lts translate_state(const state_label_aut &l) const
      {
        // There is no state label. Use the default.
        return state_label_lts();
      }
  };

  inline void lts_convert(
                const lts_aut_t &lts_in,
                lts_lts_t &lts_out)
  {
    throw mcrl2::runtime_error("Cannot translate .aut into .lts format without additional information (data, action declarations and process parameters)");
  }

  inline void lts_convert(
                const lts_aut_t &lts_in,
                lts_lts_t &lts_out,
                const data::data_specification &data,
                const lps::action_label_list &action_labels,
                const data::variable_list &process_parameters,
                const bool extra_data_is_defined=true)
  {
    if (!extra_data_is_defined)
    {
      lts_convert(lts_in,lts_out);
    }
    else
    { 
      lts_out=lts_lts_t();
      lts_out.set_data(data);
      lts_out.set_action_labels(action_labels);
      lts_out.set_process_parameters(process_parameters);
      aut_lts_convertor c(data,action_labels);
      convert_core_lts(c,lts_in,lts_out);
    }
  }

// ====================== aut -> aut   =============================

  inline void lts_convert(
                const lts_aut_t &lts_in,
                lts_aut_t &lts_out)
  { 
    lts_out=lts_in;
  }

  inline void lts_convert(
                const lts_aut_t &lts_in,
                lts_aut_t &lts_out,
                const data::data_specification &data,
                const lps::action_label_list &action_labels,
                const data::variable_list &process_parameters,
                const bool extra_data_is_defined=true)
  {
    if (extra_data_is_defined)
    {
      std::cerr << "While translating .aut to .aut, additional information (data specification, action declarations and process parameters) are ignored.\n";
    }
    lts_convert(lts_in,lts_out);
  }

// ====================== aut -> fsm   =============================

  class aut_fsm_convertor
  {
    public:

      action_label_string translate_label(const action_label_string &l) const
      {
        return l;
      }

      state_label_fsm translate_state(const state_label_aut &l) const
      {
        return state_label_fsm();
      }
  };

  inline void lts_convert(
                const lts_aut_t &lts_in,
                lts_fsm_t &lts_out)
  { 
    //Reset lts_out
    lts_out=lts_fsm_t();
   
    aut_fsm_convertor c;
    convert_core_lts(c,lts_in,lts_out);

  }

  inline void lts_convert(
                const lts_aut_t &lts_in,
                lts_fsm_t &lts_out,
                const data::data_specification &data,
                const lps::action_label_list &action_labels,
                const data::variable_list &process_parameters,
                const bool extra_data_is_defined=true)
  {
    if (extra_data_is_defined)
    {
      std::cerr << "While translating .aut to .fsm, additional information (data specification, action declarations and process parameters) are ignored.\n";
    }
    lts_convert(lts_in,lts_out);
  }

// ====================== aut -> bcg    =============================

#ifdef USE_BCG
  inline void lts_convert(
                const lts_aut_t &lts_in, 
                lts_bcg_t &lts_out)
  {  
    lts_out=lts_in;
  }
 
#endif

// ====================== aut -> dot    =============================

  class aut_dot_convertor
  {
    public:

      action_label_string translate_label(const action_label_string &l) const
      {        return l;
      }

      state_label_dot translate_state(const state_label_aut &l) const
      {
        return state_label_dot();
      }
  };

  inline void lts_convert(
                const lts_aut_t &lts_in,
                lts_dot_t &lts_out)
  {
    //Reset lts_out
    lts_out=lts_dot_t();

    aut_dot_convertor c;
    convert_core_lts(c,lts_in,lts_out);

  }

  inline void lts_convert(
                const lts_aut_t &lts_in,
                lts_dot_t &lts_out,
                const data::data_specification &data,
                const lps::action_label_list &action_labels,
                const data::variable_list &process_parameters,
                const bool extra_data_is_defined=true)
  {
    if (extra_data_is_defined)
    {
      std::cerr << "While translating .aut to .dot, additional information (data specification, action declarations and process parameters) are ignored.\n";
    }
    lts_convert(lts_in,lts_out);
  }

// ====================== fsm -> mcrl2 =============================

// ====================== fsm -> aut   =============================

  class fsm_aut_convertor
  {
    public:
      action_label_string translate_label(const action_label_string &l) const
      {
        return l;
      }

      state_label_aut translate_state(const state_label_fsm &l) const
      {
        return state_label_aut();
      }
  };

  inline void lts_convert(
                const lts_fsm_t &lts_in,
                lts_aut_t &lts_out)
  {
    fsm_aut_convertor c;
    convert_core_lts(c,lts_in,lts_out);
  }

  inline void lts_convert(
                const lts_fsm_t &lts_in,
                lts_aut_t &lts_out,
                const data::data_specification &data,
                const lps::action_label_list &action_labels,
                const data::variable_list &process_parameters,
                const bool extra_data_is_defined=true)
  {
    if (extra_data_is_defined)
    {
      std::cerr << "While translating .fsm to .aut, additional information (data specification, action declarations and process parameters) are ignored.\n";
    }
    lts_convert(lts_in,lts_out);
  }

// ====================== fsm -> fsm   =============================


  inline void lts_convert(
                const lts_fsm_t &lts_in,
                lts_fsm_t &lts_out)
  {
    lts_out=lts_in;
  }

  inline void lts_convert(
                const lts_fsm_t &lts_in,
                lts_fsm_t &lts_out,
                const data::data_specification &data,
                const lps::action_label_list &action_labels,
                const data::variable_list &process_parameters,
                const bool extra_data_is_defined=true)
  {
    if (extra_data_is_defined)
    {
      std::cerr << "While translating .fsm to .fsm, additional information (data specification, action declarations and process parameters) are ignored.\n";
    }
    lts_convert(lts_in,lts_out);
  }

// ====================== fsm -> bcg    =============================
#ifdef USE_BCG

#endif
// ====================== fsm -> dot    =============================

  class fsm_dot_convertor
  {
    private: 
      unsigned int m_state_count;
      const lts_fsm_t &m_lts_in;
    
    public:
      fsm_dot_convertor(const lts_fsm_t &lts_in):
                 m_state_count(0),m_lts_in(lts_in)
      {}

      action_label_string translate_label(const action_label_string &l) const
      {
        return l;
      }

      state_label_dot translate_state(const state_label_fsm &l) 
      {
        std::stringstream state_name;
        state_name << "s" << m_state_count;
        m_state_count++;

        std::string state_label;
        if (!l.empty())
        { 
          state_label="(";
          for(unsigned int i=0; i<l.size(); ++i)
          {
            state_label=state_label + m_lts_in.state_element_value(i,l[i])+(i+1==l.size()?")":",");
          }
        }
        
        return state_label_dot(state_name.str(),state_label);
      }
  };

  inline void lts_convert(
                const lts_fsm_t &lts_in,
                lts_dot_t &lts_out)
  {
    fsm_dot_convertor c(lts_in);
    convert_core_lts(c,lts_in,lts_out);
  }

  inline void lts_convert(
                const lts_fsm_t &lts_in,
                lts_dot_t &lts_out,
                const data::data_specification &data,
                const lps::action_label_list &action_labels,
                const data::variable_list &process_parameters,
                const bool extra_data_is_defined=true)
  {
    if (extra_data_is_defined)
    {
      std::cerr << "While translating .fsm to .dot, additional information (data specification, action declarations and process parameters) are ignored.\n";
    }
    lts_convert(lts_in,lts_out);
  }

// ====================== bcg -> mcrl2 =============================
#ifdef USE_BCG

#endif
// ====================== bcg -> aut   =============================
#ifdef USE_BCG

#endif
// ====================== bcg -> fsm   =============================
#ifdef USE_BCG

#endif
// ====================== bcg -> bcg    =============================
#ifdef USE_BCG

  inline void lts_convert(
                const lts_bcg_t &lts_in,
                lts_bcg_t &lts_out)
  {
    lts_out=lts_in;
  }

  inline void lts_convert(
                const lts_bcg_t &lts_in,
                lts_bcg_t &lts_out,
                const data::data_specification &data,
                const lps::action_label_list &action_labels,
                const data::variable_list &process_parameters,
                const bool extra_data_is_defined=true)
  {
    if (extra_data_is_defined)
    {
      std::cerr << "While translating .bcg to .bcg, additional information (data specification, action declarations and process parameters) are ignored.\n";
    }
    lts_convert(lts_in,lts_out);
  }



#endif
// ====================== bcg -> dot    =============================
#ifdef USE_BCG

#endif
// ====================== dot -> mcrl2 =============================
// ====================== dot -> aut   =============================


  class dot_aut_convertor
  {
    public:
      action_label_string translate_label(const action_label_string &l) const
      {
        return l;
      }

      state_label_aut translate_state(const state_label_dot &l) const
      {
        return state_label_aut();
      }
  };

  inline void lts_convert(
                const lts_dot_t &lts_in,
                lts_aut_t &lts_out)
  {
    dot_aut_convertor c;
    convert_core_lts(c,lts_in,lts_out);
  }

  inline void lts_convert(
                const lts_dot_t &lts_in,
                lts_aut_t &lts_out,
                const data::data_specification &data,
                const lps::action_label_list &action_labels,
                const data::variable_list &process_parameters,
                const bool extra_data_is_defined=true)
  {
    if (extra_data_is_defined)
    {
      std::cerr << "While translating .dot to .aut, additional information (data specification, action declarations and process parameters) are ignored.\n";
    }
    lts_convert(lts_in,lts_out);
  }


// ====================== dot -> fsm   =============================


  class dot_fsm_convertor
  {
    private:
      std::vector < atermpp::map <std::string , unsigned int > > state_element_values_sets;
      lts_fsm_t &lts_out;

    public:
      dot_fsm_convertor(lts_fsm_t &l):
              state_element_values_sets(std::vector < atermpp::map <std::string , unsigned int > >
                       (2,atermpp::map <std::string , unsigned int >())),
              lts_out(l)
      {
      }

      action_label_string translate_label(const action_label_string &l) const
      {
        return l;
      }

      state_label_fsm translate_state(const state_label_dot &l)
      {
        std::vector < unsigned int > result;
        const std::string state=l.name();
        atermpp::map <std::string , unsigned int >::const_iterator index=state_element_values_sets[0].find(state);
        if (index==state_element_values_sets[0].end())
        {
          const unsigned int element_index=state_element_values_sets[0].size();
          result.push_back(element_index);
          lts_out.add_state_element_value(0,state);
          state_element_values_sets[0][state]=element_index;
        }
        else
        {
          result.push_back(index->second);
        }
        const std::string label=l.label();
        index=state_element_values_sets[1].find(state);
        if (index==state_element_values_sets[1].end())
        {
          const unsigned int element_index=state_element_values_sets[1].size();
          result.push_back(element_index);
          lts_out.add_state_element_value(1,state);
          state_element_values_sets[1][label]=element_index;
        }
        else
        {
          result.push_back(index->second);
        }
        
        return result;
      }
  };

  inline void lts_convert(
                const lts_dot_t &lts_in,
                lts_fsm_t &lts_out)
  {
    lts_out.clear_process_parameters();
    lts_out.add_process_parameter("s","State");
    lts_out.add_process_parameter("l","Label");
  
    dot_fsm_convertor c(lts_out);
    convert_core_lts(c,lts_in,lts_out);
  
  }

  inline void lts_convert(
                const lts_dot_t &lts_in,
                lts_fsm_t &lts_out,
                const data::data_specification &data,
                const lps::action_label_list &action_labels,
                const data::variable_list &process_parameters,
                const bool extra_data_is_defined=true)
  {
    if (extra_data_is_defined)
    {
      std::cerr << "While translating .mcrl2 to .fsm, additional information (data specification, action declarations and process parameters) are ignored.\n"; }
    lts_convert(lts_in,lts_out);
  }




// ====================== dot -> bcg   =============================
#ifdef USE_BCG

#endif
// ====================== dot -> dot   =============================


  inline void lts_convert(
                const lts_dot_t &lts_in,
                lts_dot_t &lts_out)
  {
    lts_out=lts_in;
  }

  inline void lts_convert(
                const lts_dot_t &lts_in,
                lts_dot_t &lts_out,
                const data::data_specification &data,
                const lps::action_label_list &action_labels,
                const data::variable_list &process_parameters,
                const bool extra_data_is_defined=true)
  {
    if (extra_data_is_defined)
    {
      std::cerr << "While translating .dot to .dot, additional information (data specification, action declarations and process parameters) are ignored.\n";
    }
    lts_convert(lts_in,lts_out);
  }



// ====================== convert_core_lts =============================

  template <class CONVERTOR, class LTS_IN_TYPE, class LTS_OUT_TYPE>
  inline void convert_core_lts(CONVERTOR &c,
                               const LTS_IN_TYPE &lts_in,
                               LTS_OUT_TYPE &lts_out)
  {
    if (lts_in.has_state_info())
    { 
      for(unsigned int i=0; i<lts_in.num_state_labels(); ++i)
      { 
        lts_out.add_state(c.translate_state(lts_in.state_value(i)));
      }
    }
    else 
    { 
      lts_out.set_num_states(lts_in.num_states(),false);
    }

    for(unsigned int i=0; i<lts_in.num_action_labels(); ++i)
    { 
      lts_out.add_label(c.translate_label(lts_in.label_value(i)));
      if (lts_in.is_tau(i))
      {
        lts_out.set_tau(i);
      }
    }

    for(transition_const_range r=lts_in.get_transitions(); !r.empty(); r.advance_begin(1))
    {
      lts_out.add_transition(r.front());
    }
    lts_out.set_initial_state(lts_in.initial_state());
  }


} // namespace detail
} // namespace lts
} // namespace mcrl2

#endif
