// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/** \file mcrl2/lts/detail/lts_convert.h
 *
 * \brief This file contains lts_convert routines that translate different lts formats into each other.
 * \details For each pair of lts formats there is a translation of one format into the other,
            if such a translation is possible.
 * \author Jan Friso Groote, Muck van Weerdenburg
 */


#ifndef MCRL2_LTS_DETAIL_LTS_CONVERT_H
#define MCRL2_LTS_DETAIL_LTS_CONVERT_H

#include <sstream>
#include "mcrl2/utilities/logger.h"
#include "mcrl2/core/detail/function_symbols.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/lts/lts_aut.h"
#include "mcrl2/lts/lts_fsm.h"
#include "mcrl2/lts/lts_dot.h"

namespace mcrl2
{
namespace lts
{
namespace detail
{

template <class CONVERTOR, class LTS_IN_TYPE, class LTS_OUT_TYPE>
inline void convert_core_lts(CONVERTOR& c,
                             const LTS_IN_TYPE& lts_in,
                             LTS_OUT_TYPE& lts_out);

/** \brief Convert LTSs to each other. The input lts is not usable afterwards.
    \details Converts the input lts into the output lts maintaining the nature
            of the lts as good as possible. If no translation can be provided
            because more information is required (such as data types), then
            a mcrl2::runtime error is thrown. This also happens if the particular
            translation is not implemented.

template < class LTS_IN_TYPE, class LTS_OUT_TYPE >
inline void lts_convert(const LTS_IN_TYPE&, LTS_OUT_TYPE&)
{
  throw mcrl2::runtime_error("Conversion between lts types is not defined (without extra information).");
} */

/** \brief Convert LTSs to each other. The input lts is not usable afterwards.
    \details See lts_convert for an explanation. The extra information that is
             provided, is used for the translation, provided that extra_data_is_defined
             is set to true. This extra boolean makes it possible to dynamically conclude
             whether the required data is available, and if not, this boolean can be set to
             false, and the extra data will not be used. If the extra information is not needed in
             the translation, it is not used and a message is printed to stderr.
*/
/* Enabling this leads to selecting this class as the default, always causing the error messages.
 * template < class LTS_IN_TYPE, class LTS_OUT_TYPE >
inline void lts_convert(
  const LTS_IN_TYPE& lts_in,
  LTS_OUT_TYPE& lts_out,
  const data::data_specification&,
  const process::action_label_list&,
  const data::variable_list&,
  const bool extra_data_is_defined=true)
{
  if (!extra_data_is_defined)
  {
    lts_convert(lts_in,lts_out);
  }
  else
  {
    throw mcrl2::runtime_error("Conversion between lts types is not defined (with extra information).");
  }
} */

/** \brief Translate a fraction given as a data_expression to a representation
 *         with an arbitrary size fraction */

inline probabilistic_arbitrary_precision_fraction translate_probability_data_to_arbitrary_size_probability(const data::data_expression& d)
{
  const data::application& da=atermpp::down_cast<data::application>(d);
  if (!(data::sort_int::is_integer_constant(da[0]) && 
        data::sort_pos::is_positive_constant(da[1]) &&
        da.head()==data::sort_real::creal()))
  {
    throw mcrl2::runtime_error("Cannot convert the probability " + pp(d) + " to an arbitrary size denominator/enumerator pair.");
  }    
  return probabilistic_arbitrary_precision_fraction(pp(da[0]),pp(da[1]));
}

template <class PROBABILISTIC_STATE1, class PROBABILISTIC_STATE2> 
inline PROBABILISTIC_STATE2 lts_convert_probabilistic_state(const PROBABILISTIC_STATE1& ) 
{
  throw mcrl2::runtime_error("Translation of probabilistic states is not defined");
}

template <>
inline probabilistic_state<size_t, lps::probabilistic_data_expression> 
lts_convert_probabilistic_state<probabilistic_state<size_t, lps::probabilistic_data_expression>,
                               probabilistic_state<size_t, lps::probabilistic_data_expression> >(
             const probabilistic_state<size_t, lps::probabilistic_data_expression>& state_in) 
{
  return state_in;
}

template <>
inline probabilistic_state<size_t, mcrl2::lts::probabilistic_arbitrary_precision_fraction> 
lts_convert_probabilistic_state<probabilistic_state<size_t, mcrl2::lts::probabilistic_arbitrary_precision_fraction>,
                                probabilistic_state<size_t, mcrl2::lts::probabilistic_arbitrary_precision_fraction> >(
            const probabilistic_state<size_t, mcrl2::lts::probabilistic_arbitrary_precision_fraction>& state_in) 
{
  return state_in;
}

template <>
inline probabilistic_state<size_t, lps::probabilistic_data_expression> 
lts_convert_probabilistic_state<probabilistic_state<size_t, mcrl2::lts::probabilistic_arbitrary_precision_fraction>,
                                probabilistic_state<size_t, lps::probabilistic_data_expression> >(
            const probabilistic_state<size_t, mcrl2::lts::probabilistic_arbitrary_precision_fraction>& state_in) 
{
  std::vector<lps::state_probability_pair<size_t, lps::probabilistic_data_expression> > result;
  for(const lps::state_probability_pair<size_t, mcrl2::lts::probabilistic_arbitrary_precision_fraction>& p: state_in)
  {
    result.push_back(lps::state_probability_pair<size_t, lps::probabilistic_data_expression>(
                          p.state(), 
                          data::sort_real::creal(data::sort_int::int_(pp(p.probability().enumerator())),
                                                 data::sort_pos::pos(pp(p.probability().denominator())))));
  }
  return probabilistic_state<size_t, lps::probabilistic_data_expression>(result.begin(), result.end());
}

inline mcrl2::lts::probabilistic_arbitrary_precision_fraction translate_probability_data_prob(const data::data_expression& d)
{
  const data::application& da=atermpp::down_cast<data::application>(d);
  if (!(data::sort_int::is_integer_constant(da[0]) && 
        data::sort_pos::is_positive_constant(da[1]) &&
        da.head()==data::sort_real::creal()))
  {
    throw mcrl2::runtime_error("Cannot convert the probability " + pp(d) + " to an explicit denominator/enumerator pair.");
  }    
  return mcrl2::lts::probabilistic_arbitrary_precision_fraction(pp(da[0]),pp(da[1]));
} 

template <>
inline probabilistic_state<size_t, mcrl2::lts::probabilistic_arbitrary_precision_fraction> 
lts_convert_probabilistic_state<probabilistic_state<size_t, lps::probabilistic_data_expression>,
                                probabilistic_state<size_t, mcrl2::lts::probabilistic_arbitrary_precision_fraction> >(
            const probabilistic_state<size_t, lps::probabilistic_data_expression>& state_in) 
{
  std::vector<lps::state_probability_pair<size_t, mcrl2::lts::probabilistic_arbitrary_precision_fraction> > result;
  for(const lps::state_probability_pair<size_t, mcrl2::lps::probabilistic_data_expression>& p: state_in)
  {
    result.push_back(lps::state_probability_pair<size_t, mcrl2::lts::probabilistic_arbitrary_precision_fraction>(
                          p.state(),
                          translate_probability_data_prob(p.probability())));
  }
  return probabilistic_state<size_t, mcrl2::lts::probabilistic_arbitrary_precision_fraction>(result.begin(), result.end());
}



// ================================================================
//
// Below the translations for labelled transition system formats
// to each other are provided. If a translation is not given,
// the routines above are used to indicate at runtime that a
// required translation does not exist.
//
// ================================================================

// Default convertor, not doing anything.
template <class BASE_LTS_IN, class BASE_LTS_OUT>
class convertor
{
  public:
    // Constructor
    convertor(const BASE_LTS_IN& , const BASE_LTS_OUT& )
    {}

};

// ====================== convert_core_lts =============================

/* template <class CONVERTOR, class LTS_IN_TYPE, class LTS_OUT_TYPE>
inline void convert_core_lts(CONVERTOR& c,
                             const LTS_IN_TYPE& lts_in,
                             LTS_OUT_TYPE& lts_out)
{
  if (lts_in.has_state_info())
  {
    for (size_t i=0; i<lts_in.num_states(); ++i)
    {
      lts_out.add_state(c.translate_state(lts_in.state_label(i)));
    }
  }
  else
  {
    lts_out.set_num_states(lts_in.num_states(),false);
  }

  for (size_t i=0; i<lts_in.num_action_labels(); ++i)
  {
    lts_out.add_action(c.translate_label(lts_in.action_label(i)));
    if (lts_in.is_tau(i))
    {
      lts_out.set_tau(i);
    }
  }

  / * for (size_t i=0; i<lts_in.num_probabilistic_labels(); ++i)
  {
    lts_out.add_probabilistic_label(c.translate_probability_label(lts_in.probabilistic_label(i)));
  }
 
  for (size_t i=0; i<lts_in.num_states(); ++i)
  {
    if (lts_in.is_probabilistic(i))
    {
      lts_out.set_is_probabilistic(i,true); 
    }
  } * /

  const std::vector<transition> &trans=lts_in.get_transitions();
  for (std::vector<transition>::const_iterator r=trans.begin(); r!=trans.end(); ++r)
  {
    lts_out.add_transition(*r);
  }
  lts_out.set_initial_state(lts_in.initial_state());
} */

// =========================================================================   REWRITTEN CODE ==============

inline action_label_lts translate_label_aux(const action_label_string& l1, 
                                            const data::data_specification& data, 
                                            const process::action_label_list& action_labels)
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
    al=parse_lts_action(l,data,action_labels);
  }
  catch (mcrl2::runtime_error& e)
  {
    throw mcrl2::runtime_error("Parse error in action label " + l1 + ".\n" + e.what());
  }
  return al;
}

// ======================  lts -> lts  =============================

inline void lts_convert_base_class(const lts_lts_base& base_in, lts_lts_base& base_out) 
{
  base_out=base_in;
}

inline void lts_convert_base_class(const lts_lts_base& base_in, 
                            lts_lts_base& base_out,                
                            const data::data_specification& ,
                            const process::action_label_list& ,
                            const data::variable_list& ,
                            const bool extra_data_is_defined=true) 
{
  if (extra_data_is_defined)
  {
    mCRL2log(log::warning) << "While translating .lts to .lts, additional information (data specification, action declarations and process parameters) is ignored.\n";
  }
  lts_convert_base_class(base_in, base_out);
}

inline action_label_lts lts_convert_translate_label(const action_label_lts& l_in, convertor<lts_lts_base, lts_lts_base>& )
{
  return l_in;
}

inline void lts_convert_translate_state(const state_label_lts& state_label_in, state_label_lts& state_label_out, convertor<lts_lts_base, lts_lts_base>& /* c */)
{
  state_label_out=state_label_in;
}

// ======================  lts -> fsm  =============================

template<>
class convertor<lts_lts_base, lts_fsm_base>
{
  public:
    std::vector < std::map <data::data_expression , size_t > > state_element_values_sets;
    lts_fsm_base& lts_out;

    convertor(const lts_lts_base& lts_base_in, lts_fsm_base& lts_base_out):
      state_element_values_sets(std::vector < std::map <data::data_expression , size_t > >
                                (lts_base_in.process_parameters().size(), std::map <data::data_expression , size_t >())),
      lts_out(lts_base_out)
    {
    }
};

inline void lts_convert_base_class(const lts_lts_base& base_in, lts_fsm_base& base_out) 
{
  base_out.clear_process_parameters();

  for (data::variable_list::const_iterator i=base_in.process_parameters().begin();
       i!=base_in.process_parameters().end(); ++i)
  {
    base_out.add_process_parameter(data::pp(*i),data::pp(i->sort()));
  }
}

inline action_label_string lts_convert_translate_label(const action_label_lts& l_in, convertor<lts_lts_base, lts_fsm_base>& )
{
  return pp(l_in);
}

inline void lts_convert_base_class(const lts_lts_base& base_in, 
                            lts_fsm_base& base_out,                
                            const data::data_specification& ,
                            const process::action_label_list& ,
                            const data::variable_list& ,
                            const bool extra_data_is_defined=true) 
{
  if (extra_data_is_defined)
  {
    mCRL2log(log::warning) << "While translating .lts to .fsm, additional information (data specification, action declarations and process parameters) are ignored.\n";
  }
  lts_convert_base_class(base_in, base_out);
}

inline void lts_convert_translate_state(const state_label_lts& state_label_in, state_label_fsm& state_label_out, convertor<lts_lts_base, lts_fsm_base>& c)
{
  state_label_out.clear();
  size_t i=0;
  for (const data::data_expression& t: state_label_in)
  {
    std::map <data::data_expression , size_t >::const_iterator index=c.state_element_values_sets[i].find(t);
    if (index==c.state_element_values_sets[i].end())
    {
      const size_t element_index=c.state_element_values_sets[i].size();
      state_label_out.push_back(element_index);
      c.lts_out.add_state_element_value(i,data::pp(t));
      c.state_element_values_sets[i][t]=element_index;
    }
    else
    {
      state_label_out.push_back(index->second);
    }
    ++i;
  }
}

// ======================  lts -> aut  =============================

inline void lts_convert_base_class(const lts_lts_base& /* base_in */, lts_aut_base& /* base_out */) 
{
  // Nothing needs to be done.
}

inline void lts_convert_base_class(const lts_lts_base& base_in, 
                            lts_aut_base& base_out,                
                            const data::data_specification& ,
                            const process::action_label_list& ,
                            const data::variable_list& ,
                            const bool extra_data_is_defined=true) 
{
  if (extra_data_is_defined)
  {
    mCRL2log(log::warning) << "While translating .lts to .aut, additional information (data specification, action declarations and process parameters) are ignored.\n";
  }
  lts_convert_base_class(base_in, base_out);
}

inline action_label_string lts_convert_translate_label(const action_label_lts& l_in, convertor<lts_lts_base, lts_aut_base>& )
{
  return pp(l_in);
}

inline void lts_convert_translate_state(const state_label_lts& /* state_label_in */, state_label_empty& state_label_out, convertor<lts_lts_base, lts_aut_base>& /* c */)
{
  state_label_out=state_label_empty();
}

// ======================  lts -> dot  =============================

template<>
class convertor<lts_lts_base, lts_dot_base>
{
  public:
    size_t m_state_count;

    convertor(const lts_lts_base& /* lts_base_in */, lts_dot_base& /* lts_base_out */):
      m_state_count(0)
    {
    }
};

inline void lts_convert_base_class(const lts_lts_base& /* base_in */, lts_dot_base& /* base_out */) 
{
  // Nothing needs to be done.
}

inline void lts_convert_base_class(const lts_lts_base& base_in,
                            lts_dot_base& base_out,
                            const data::data_specification& ,
                            const process::action_label_list& ,
                            const data::variable_list& ,
                            const bool extra_data_is_defined=true)
{
  if (extra_data_is_defined)
  {
    mCRL2log(log::warning) << "While translating .lts to .dot, additional information (data specification, action declarations and process parameters) are ignored.\n";
  }
  lts_convert_base_class(base_in, base_out);
}

inline action_label_string lts_convert_translate_label(const action_label_lts& l_in, convertor<lts_lts_base, lts_dot_base>& )
{
  return pp(l_in);
}

inline void lts_convert_translate_state(const state_label_lts& state_label_in, state_label_dot& state_label_out, convertor<lts_lts_base, lts_dot_base>& c)
{
  std::stringstream state_name;
  state_name << "s" << c.m_state_count;
  c.m_state_count++;
  state_label_out=state_label_dot(state_name.str(),pp(state_label_in));
}

// ======================  fsm -> lts  =============================

template<>
class convertor<lts_fsm_base, lts_lts_base>

{
  public:
    const lts_fsm_base& m_lts_in;
    const lts_lts_base& m_lts_out;

    convertor(const lts_fsm_base& lts_base_in, lts_lts_base& lts_base_out):
      m_lts_in(lts_base_in), m_lts_out(lts_base_out)
    {
    }
};

inline void lts_convert_base_class(const lts_fsm_base& /* base_in */, lts_lts_base& /* base_out */) 
{
  throw mcrl2::runtime_error("Cannot translate .fsm into .lts format without additional LPS information (data, action declarations and process parameters).");
}

inline void lts_convert_base_class(const lts_fsm_base& base_in,
                            lts_lts_base& base_out,
                            const data::data_specification& data,
                            const process::action_label_list& action_labels,
                            const data::variable_list& process_parameters,
                            const bool extra_data_is_defined=true)
{
  if (!extra_data_is_defined)
  {
    base_out.set_data(data);
    base_out.set_action_labels(action_labels);
    base_out.set_process_parameters(process_parameters);
  }
  lts_convert_base_class(base_in,base_out);
}

inline action_label_lts lts_convert_translate_label(const action_label_string& l1, convertor<lts_fsm_base, lts_lts_base>& c) 
{
  return translate_label_aux(l1, c.m_lts_out.data(), c.m_lts_out.action_labels());
}

inline void lts_convert_translate_state(const state_label_fsm& state_label_in, state_label_lts& state_label_out, convertor<lts_fsm_base, lts_lts_base>& c)
{
  // If process_parameters are not empty, we use them to check that the sorts of its variables  match.
  std::vector < data::data_expression > state_label;
  size_t idx=0;
  const data::variable_list& parameters=c.m_lts_out.process_parameters();
  data::variable_list::const_iterator parameter_iterator=parameters.begin();
  for (state_label_fsm::const_iterator i=state_label_in.begin(); i!=state_label_in.end(); ++i, ++idx)
  {
    assert(parameters.empty() || parameter_iterator!=parameters.end());
    const data::data_expression d=data::parse_data_expression(c.m_lts_in.state_element_value(idx,*i),c.m_lts_out.data());
    if (!parameters.empty()  && (d.sort()!=parameter_iterator->sort()))
    {
      throw mcrl2::runtime_error("Sort of parameter " + pp(*parameter_iterator) + ":" +
                                 pp(parameter_iterator->sort()) + " does not match with that of actual value " +
                                 pp(d) + ".");
    }
    state_label.push_back(d);
    if (!parameters.empty())
    {
      ++parameter_iterator;
    }
  }
  assert(parameter_iterator==parameters.end());

  state_label_out=state_label_lts(state_label);
}

// ======================  fsm -> fsm  =============================

inline void lts_convert_base_class(const lts_fsm_base& base_in, lts_fsm_base& base_out) 
{
  base_out=base_in;
}

inline void lts_convert_base_class(const lts_fsm_base& base_in,
                            lts_fsm_base& base_out,
                            const data::data_specification& ,
                            const process::action_label_list& ,
                            const data::variable_list& ,
                            const bool extra_data_is_defined=true)
{
  if (extra_data_is_defined)
  {
    mCRL2log(log::warning) << "While translating .fsm to .fsm, additional information (data specification, action declarations and process parameters) are ignored.\n";
  }
  lts_convert_base_class(base_in,base_out);
}

inline void lts_convert_translate_state(const state_label_fsm& state_label_in, state_label_fsm& state_label_out, convertor<lts_fsm_base, lts_fsm_base>& /* c */) 
{
  state_label_out=state_label_in;
}

inline action_label_string lts_convert_translate_label(const action_label_string& l_in, convertor<lts_fsm_base, lts_fsm_base>& )
{
  return l_in;
}

// ======================  fsm -> aut  =============================

inline void lts_convert_base_class(const lts_fsm_base& /* base_in */, lts_aut_base& /* base_out */) 
{
  // Nothing needs to be done.
}

inline void lts_convert_base_class(const lts_fsm_base& base_in,
                            lts_aut_base& base_out,
                            const data::data_specification& ,
                            const process::action_label_list& ,
                            const data::variable_list& ,
                            const bool extra_data_is_defined=true)
{
  if (extra_data_is_defined)
  {
    mCRL2log(log::warning) << "While translating .fsm to .aut, additional information (data specification, action declarations and process parameters) are ignored.\n";
  }
  lts_convert_base_class(base_in,base_out);
}

inline void lts_convert_translate_state(const state_label_fsm&, state_label_empty& state_label_out, convertor<lts_fsm_base, lts_aut_base>& /* c */) 
{
  state_label_out=state_label_empty();
}

inline action_label_string lts_convert_translate_label(const action_label_string& l_in, convertor<lts_fsm_base, lts_aut_base>& )
{
  return l_in;
}


// ======================  fsm -> dot  =============================

template<>
class convertor<lts_fsm_base, lts_dot_base>
{
  public:
    size_t m_state_count;
    const lts_fsm_base& m_lts_in;

    convertor(const lts_fsm_base& lts_base_in, const lts_dot_base& /* lts_base_out */)
      : m_state_count(0), m_lts_in(lts_base_in)
    {
    }
};

inline void lts_convert_base_class(const lts_fsm_base& /* base_in */, lts_dot_base& /* base_out */) 
{
  // Nothing needs to be done. 
}

inline void lts_convert_base_class(const lts_fsm_base& base_in,
                            lts_dot_base& base_out,
                            const data::data_specification& ,
                            const process::action_label_list& ,
                            const data::variable_list& ,
                            const bool extra_data_is_defined=true)
{
  if (extra_data_is_defined)
  {
    mCRL2log(log::warning) << "While translating .fsm to .aut, additional information (data specification, action declarations and process parameters) are ignored.\n";
  }
  lts_convert_base_class(base_in,base_out);
}

inline void lts_convert_translate_state(const state_label_fsm& state_label_in, state_label_dot& state_label_out, convertor<lts_fsm_base, lts_dot_base>& c)
{
  std::stringstream state_name;
  state_name << "s" << c.m_state_count;
  c.m_state_count++;

  std::string state_label;
  if (!state_label_in.empty())
  {
    state_label="(";
    for (size_t i=0; i<state_label_in.size(); ++i)
    {
      state_label=state_label + c.m_lts_in.state_element_value(i,state_label_in[i])+(i+1==state_label_in.size()?")":",");
    }
  }

  state_label_out=state_label_dot(state_name.str(),state_label);
}

inline action_label_string lts_convert_translate_label(const action_label_string& l_in, convertor<lts_fsm_base, lts_dot_base>& )
{
  return l_in;
}


// ======================  aut -> lts  =============================

template<>
class convertor<lts_aut_base, lts_lts_base>
{
  public:
    const data::data_specification& m_data;
    const process::action_label_list& m_action_labels;

    convertor(const lts_aut_base& /* lts_base_in*/, const lts_lts_base& lts_base_out)
      : m_data(lts_base_out.data()),
        m_action_labels(lts_base_out.action_labels())
    {
    }
};

inline void lts_convert_base_class(const lts_aut_base& /* base_in */, lts_lts_base& /* base_out */) 
{
  throw mcrl2::runtime_error("Cannot translate .aut into .lts format without additional information (data, action declarations and process parameters)");
}

inline void lts_convert_base_class(const lts_aut_base& base_in,
                            lts_lts_base& base_out,
                            const data::data_specification& data,
                            const process::action_label_list& action_labels,
                            const data::variable_list& process_parameters,
                            const bool extra_data_is_defined=true)
{
  if (extra_data_is_defined)
  {
    base_out.set_data(data);
    base_out.set_action_labels(action_labels);
    base_out.set_process_parameters(process_parameters);
  }
  else
  {
    lts_convert_base_class(base_in,base_out);
  }
}

inline action_label_lts lts_convert_translate_label(const action_label_string& l_in, convertor<lts_aut_base, lts_lts_base>& c)
{
  return translate_label_aux(l_in, c.m_data, c.m_action_labels);
}

inline void lts_convert_translate_state(const state_label_empty& /* state_label_in */, state_label_lts& state_label_out, convertor<lts_aut_base, lts_lts_base>& /* c */)
{
  // There is no state label. Use the default.
  state_label_out=state_label_lts();
}

/* data::data_expression translate_probability_label(const probabilistic_arbitrary_precision_fraction& d)
{
  return detail::translate_probability_prob_data_arbitrary_size(d);
} */

// ======================  aut -> fsm  =============================

inline void lts_convert_base_class(const lts_aut_base& /* base_in */, lts_fsm_base& base_out) 
{
  //Reset lts_out
  base_out=lts_fsm_base();
}

inline void lts_convert_base_class(const lts_aut_base& base_in,
                            lts_fsm_base& base_out,
                            const data::data_specification& ,
                            const process::action_label_list& ,
                            const data::variable_list& ,
                            const bool extra_data_is_defined=true)
{
  if (extra_data_is_defined)
  {
    mCRL2log(log::warning) << "While translating .aut to .fsm, additional information (data specification, action declarations and process parameters) are ignored.\n";
  }
  lts_convert_base_class(base_in,base_out);
}

inline action_label_string lts_convert_translate_label(const action_label_string& l_in, convertor<lts_aut_base, lts_fsm_base>& )
{
  return l_in;
}

inline void lts_convert_translate_state(const state_label_empty& /* state_label_in */, state_label_fsm& state_label_out, convertor<lts_aut_base, lts_fsm_base>& /* c */)
{
  state_label_out=state_label_fsm();
}

// ======================  aut -> aut  =============================

inline void lts_convert_base_class(const lts_aut_base& base_in, lts_aut_base& base_out) 
{
  base_out=base_in;
}

inline void lts_convert_base_class(const lts_aut_base& base_in,
                            lts_aut_base& base_out,
                            const data::data_specification& ,
                            const process::action_label_list& ,
                            const data::variable_list& ,
                            const bool extra_data_is_defined=true)
{
  if (extra_data_is_defined)
  {
    mCRL2log(log::warning) << "While translating .aut to .aut, additional information (data specification, action declarations and process parameters) are ignored.\n";
  }
  lts_convert_base_class(base_in,base_out);
}

inline void lts_convert_translate_state(const state_label_empty& state_label_in, state_label_empty& state_label_out, convertor<lts_aut_base, lts_aut_base>& /* c */)
{
  state_label_out=state_label_in;
}

inline action_label_string lts_convert_translate_label(const action_label_string& l_in, convertor<lts_aut_base, lts_aut_base>& )
{
  return l_in;
}

// ======================  aut -> dot  =============================

inline void lts_convert_base_class(const lts_aut_base& /* base_in */, lts_dot_base& /* base_out */) 
{
  // Nothing needs to be done.
}

inline void lts_convert_base_class(const lts_aut_base& base_in,
                            lts_dot_base& base_out,
                            const data::data_specification& ,
                            const process::action_label_list& ,
                            const data::variable_list& ,
                            const bool extra_data_is_defined=true)
{
  if (extra_data_is_defined)
  {
    mCRL2log(log::warning) << "While translating .aut to .dot, additional information (data specification, action declarations and process parameters) are ignored.\n";
  }
  lts_convert_base_class(base_in,base_out);
}

inline void lts_convert_translate_state(const state_label_empty& /* state_label_in */, state_label_dot& state_label_out, convertor<lts_aut_base, lts_dot_base>& /* c */)
{
  state_label_out=state_label_dot();
}

inline action_label_string lts_convert_translate_label(const action_label_string& l_in, convertor<lts_aut_base, lts_dot_base>& )
{
  return l_in;
}


// ======================  END CONCRETE LTS FORMAT CONVERSIONS  =============================


// ======================  BEGIN ACTUAL CONVERSIONS  =============================


// Actual conversion without base class conversion.
template < class STATE_LABEL1, class ACTION_LABEL1, class LTS_BASE1,  class STATE_LABEL2, class ACTION_LABEL2, class LTS_BASE2>
inline void lts_convert_aux(const lts<STATE_LABEL1, ACTION_LABEL1, LTS_BASE1>& lts_in, 
                            lts<STATE_LABEL2, ACTION_LABEL2, LTS_BASE2>& lts_out)
{
  convertor<LTS_BASE1, LTS_BASE2> c(lts_in, lts_out);

  if (lts_in.has_state_info())
  {
    for (size_t i=0; i<lts_in.num_states(); ++i)
    {
      STATE_LABEL2 s;
      lts_convert_translate_state(lts_in.state_label(i), s, c);
      lts_out.add_state(s);
    }
  }
  else
  {
    lts_out.set_num_states(lts_in.num_states(),false);
  }

  for (size_t i=0; i<lts_in.num_action_labels(); ++i)
  {
    lts_out.add_action(lts_convert_translate_label(lts_in.action_label(i),c));
    /* if (lts_in.is_tau(i))
    {
      lts_out.set_tau(i);
    } */
  }

  const std::vector<transition> &trans=lts_in.get_transitions();
  for (std::vector<transition>::const_iterator r=trans.begin(); r!=trans.end(); ++r)
  {
    lts_out.add_transition(*r);
  }
  lts_out.set_initial_state(lts_in.initial_state());
}

// ======================  lts -> lts  =============================

template < class STATE_LABEL1, class ACTION_LABEL1, class LTS_BASE1,  class STATE_LABEL2, class ACTION_LABEL2, class LTS_BASE2>
inline void lts_convert(const lts<STATE_LABEL1, ACTION_LABEL1, LTS_BASE1>& lts_in, 
                       lts<STATE_LABEL2, ACTION_LABEL2, LTS_BASE2>& lts_out)
{
  lts_convert_base_class(static_cast<const LTS_BASE1&>(lts_in), static_cast<LTS_BASE2&>(lts_out));
  lts_convert_aux<STATE_LABEL1, ACTION_LABEL1, LTS_BASE1,STATE_LABEL2, ACTION_LABEL2, LTS_BASE2>(lts_in,lts_out);
}

template < class STATE_LABEL1, class ACTION_LABEL1, class LTS_BASE1,
           class STATE_LABEL2, class ACTION_LABEL2, class LTS_BASE2>
inline void lts_convert(const lts<STATE_LABEL1, ACTION_LABEL1, LTS_BASE1>& lts_in, 
                        lts<STATE_LABEL2, ACTION_LABEL2, LTS_BASE2>& lts_out,
                        const data::data_specification& ds,
                        const process::action_label_list& all,
                        const data::variable_list& vl,
                        const bool extra_data_is_defined=true)
{
  lts_convert_base_class(static_cast<const LTS_BASE1&>(lts_in), static_cast<LTS_BASE2&>(lts_out), ds, all, vl, extra_data_is_defined);
  lts_convert_aux<STATE_LABEL1, ACTION_LABEL1, LTS_BASE1,STATE_LABEL2, ACTION_LABEL2, LTS_BASE2>(lts_in,lts_out);
}

// ======================  probabilistic_lts -> lts  =============================

template < class STATE_LABEL1, class ACTION_LABEL1, class PROBABILISTIC_STATE1, class LTS_BASE1,  class STATE_LABEL2, class ACTION_LABEL2, class LTS_BASE2>
void remove_probabilities(const probabilistic_lts<STATE_LABEL1, ACTION_LABEL1, PROBABILISTIC_STATE1, LTS_BASE1>& lts_in,
                          lts<STATE_LABEL2, ACTION_LABEL2, LTS_BASE2>& lts_out)
{
  assert(lts_in.initial_probabilistic_state().size()!=0);
  if (lts_in.initial_probabilistic_state().size()==1)
  {
    lts_out.set_initial_state(lts_in.initial_probabilistic_state().begin()->state());
  }
  else
  {
    throw mcrl2::runtime_error("Initial state is probabilistic and cannot be transformed into a non probabilistic state.");
  }

  // Adapt the probabilistic target states to non probabilistic target states.
  size_t transition_number=1;
  for(transition& t: lts_out.get_transitions())
  {
    size_t probabilistic_target_state_number=t.to();
    assert(lts_in.probabilistic_state(probabilistic_target_state_number).size()!=0);
    if (lts_in.probabilistic_state(probabilistic_target_state_number).size()>1)
    {
      throw mcrl2::runtime_error("Transition " + std::to_string(transition_number) + " is probabilistic.");
    }
    else
    {
      t=transition(t.from(), t.label(), lts_in.probabilistic_state(probabilistic_target_state_number).begin()->state());
    }
    transition_number++;
  }
}
        
template < class STATE_LABEL1, class ACTION_LABEL1, class LTS_BASE1,  class PROBABILISTIC_STATE1, class STATE_LABEL2, class ACTION_LABEL2, class LTS_BASE2>
inline void lts_convert(const probabilistic_lts<STATE_LABEL1, ACTION_LABEL1, PROBABILISTIC_STATE1, LTS_BASE1>& lts_in, 
                        lts<STATE_LABEL2, ACTION_LABEL2, LTS_BASE2>& lts_out)
{
  lts_convert<STATE_LABEL1, ACTION_LABEL1, LTS_BASE1, STATE_LABEL2, ACTION_LABEL2, LTS_BASE2>(lts_in,lts_out);
  remove_probabilities(lts_in,lts_out);

}



template < class STATE_LABEL1, class ACTION_LABEL1, class PROBABILISTIC_STATE1, class LTS_BASE1,
           class STATE_LABEL2, class ACTION_LABEL2, class LTS_BASE2>
inline void lts_convert(const probabilistic_lts<STATE_LABEL1, ACTION_LABEL1, PROBABILISTIC_STATE1, LTS_BASE1>& lts_in, 
                        lts<STATE_LABEL2, ACTION_LABEL2, LTS_BASE2>& lts_out,
                        const data::data_specification& data,
                        const process::action_label_list& action_label_list,
                        const data::variable_list& process_parameters,
                        const bool extra_data_is_defined=true)
{
  lts_convert<STATE_LABEL1, ACTION_LABEL1, LTS_BASE1,STATE_LABEL2, ACTION_LABEL2, LTS_BASE2>(lts_in,lts_out,data,action_label_list,process_parameters,extra_data_is_defined);
  remove_probabilities(lts_in,lts_out);
}

// ======================  lts -> probabilistic_lts  =============================

template < class STATE_LABEL1, class ACTION_LABEL1, class LTS_BASE1,  
           class STATE_LABEL2, class ACTION_LABEL2, class PROBABILISTIC_STATE2, class LTS_BASE2>
inline void add_probabilities(const lts<STATE_LABEL1, ACTION_LABEL1, LTS_BASE1>& lts_in, 
                              probabilistic_lts<STATE_LABEL2, ACTION_LABEL2, PROBABILISTIC_STATE2, LTS_BASE2>& lts_out)
{ 
  lts_out.set_initial_probabilistic_state(PROBABILISTIC_STATE_T(lts_in.initial_state()));
  for(size_t i=0; i<lts_out.num_states(); ++i)
  {
    lts_out.add_probabilistic_state(PROBABILISTIC_STATE2::PROBABILISTIC_STATE_T(i));
  }
}


template < class STATE_LABEL1, class ACTION_LABEL1, class LTS_BASE1,  class STATE_LABEL2, class ACTION_LABEL2, class PROBABILISTIC_STATE2, class LTS_BASE2>
inline void lts_convert(const lts<STATE_LABEL1, ACTION_LABEL1, LTS_BASE1>& lts_in, 
                        probabilistic_lts<STATE_LABEL2, ACTION_LABEL2, PROBABILISTIC_STATE2, LTS_BASE2>& lts_out)
{
  lts_convert<STATE_LABEL1, ACTION_LABEL1, LTS_BASE1, STATE_LABEL2, ACTION_LABEL2, LTS_BASE2>(lts_in,lts_out);
  add_probabilities(lts_in,lts_out);
}

template < class STATE_LABEL1, class ACTION_LABEL1, class LTS_BASE1,
           class STATE_LABEL2, class ACTION_LABEL2, class PROBABILISTIC_STATE2, class LTS_BASE2>
inline void lts_convert(const lts<STATE_LABEL1, ACTION_LABEL1, LTS_BASE1>& lts_in, 
                        probabilistic_lts<STATE_LABEL2, ACTION_LABEL2, PROBABILISTIC_STATE2, LTS_BASE2>& lts_out,
                        const data::data_specification& data,
                        const process::action_label_list& action_label_list,
                        const data::variable_list& process_parameters,
                        const bool extra_data_is_defined=true)
{
  lts_convert<STATE_LABEL1, ACTION_LABEL1, LTS_BASE1,STATE_LABEL2, ACTION_LABEL2, LTS_BASE2>(lts_in,lts_out,data,action_label_list,process_parameters,extra_data_is_defined);
  add_probabilities(lts_in, lts_out);
}

// ======================  probabilistic_lts -> probabilistic_lts  =============================

template < class STATE_LABEL1, class ACTION_LABEL1, class PROBABILISTIC_STATE1, class LTS_BASE1,
           class STATE_LABEL2, class ACTION_LABEL2, class PROBABILISTIC_STATE2, class LTS_BASE2>
inline void translate_probability_labels(const probabilistic_lts<STATE_LABEL1, ACTION_LABEL1, PROBABILISTIC_STATE1, LTS_BASE1>& lts_in, 
                                         probabilistic_lts<STATE_LABEL2, ACTION_LABEL2, PROBABILISTIC_STATE2, LTS_BASE2>& lts_out)
{
  lts_out.set_initial_probabilistic_state(lts_convert_probabilistic_state<PROBABILISTIC_STATE1,PROBABILISTIC_STATE2>(lts_in.initial_probabilistic_state()));

  lts_out.clear_probabilistic_states();
  for(size_t i=0; i< lts_in.num_probabilistic_labels(); ++i)
  {
    lts_out.add_probabilistic_state(lts_convert_probabilistic_state<PROBABILISTIC_STATE1,PROBABILISTIC_STATE2>(lts_in.probabilistic_state(i)));
  }
}


template < class STATE_LABEL1, class ACTION_LABEL1, class PROBABILISTIC_STATE1, class LTS_BASE1,
           class STATE_LABEL2, class ACTION_LABEL2, class PROBABILISTIC_STATE2, class LTS_BASE2>
inline void lts_convert(const probabilistic_lts<STATE_LABEL1, ACTION_LABEL1, PROBABILISTIC_STATE1, LTS_BASE1>& lts_in, 
                       probabilistic_lts<STATE_LABEL2, ACTION_LABEL2, PROBABILISTIC_STATE2, LTS_BASE2>& lts_out)
{
  lts_convert(static_cast<const lts<STATE_LABEL1, ACTION_LABEL1, LTS_BASE1>& >(lts_in),
              static_cast<lts<STATE_LABEL2, ACTION_LABEL2, LTS_BASE2>& >(lts_out));
  translate_probability_labels(lts_in,lts_out);
}

template < class STATE_LABEL1, class ACTION_LABEL1, class PROBABILISTIC_STATE1, class LTS_BASE1,
           class STATE_LABEL2, class ACTION_LABEL2, class PROBABILISTIC_STATE2, class LTS_BASE2>
inline void lts_convert(const probabilistic_lts<STATE_LABEL1, ACTION_LABEL1, PROBABILISTIC_STATE1, LTS_BASE1>& lts_in, 
                        probabilistic_lts<STATE_LABEL2, ACTION_LABEL2, PROBABILISTIC_STATE2, LTS_BASE2>& lts_out,
                        const data::data_specification& data,
                        const process::action_label_list& action_label_list,
                        const data::variable_list& process_parameters,
                        const bool extra_data_is_defined=true)
{
 lts_convert(static_cast<const lts<STATE_LABEL1, ACTION_LABEL1, LTS_BASE1>& >(lts_in),
             static_cast<lts<STATE_LABEL2, ACTION_LABEL2, LTS_BASE2>& >(lts_out),
             data, action_label_list, process_parameters, extra_data_is_defined);
  translate_probability_labels(lts_in,lts_out);
}

// ======================  END ACTUAL CONVERSIONS  =============================

} // namespace detail
} // namespace lts
} // namespace mcrl2

#endif
