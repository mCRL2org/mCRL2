// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/** \file lts_lts.h
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
#include "mcrl2/core/detail/function_symbols.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/print.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/multi_action.h"
#include "mcrl2/process/action_parse.h"
#include "mcrl2/lps/typecheck.h"
#include "mcrl2/lps/state.h"
#include "mcrl2/lps/probabilistic_data_expression.h"
#include "mcrl2/lts/probabilistic_state.h"
#include "mcrl2/lts/probabilistic_lts.h"

namespace mcrl2
{
namespace lts
{

/** \brief This class contains state labels for an labelled transition system in .lts format.
    \details A state label in .lts format consists of lists of balanced tree of data expressions.
             These represent sets of state vectors. The reason for the sets is that states can 
             be merged by operations on state spaces, and if so, the sets of labels can easily 
             be joined. 
*/
class state_label_lts : public atermpp::term_list< atermpp::term_balanced_tree< data::data_expression > >
{
  public:
    typedef atermpp::term_balanced_tree< data::data_expression > single_label;
    typedef atermpp::term_list< single_label > super;

    /** \brief Default constructor. 
    */
    state_label_lts()
    {}

    /** \brief Copy constructor. */
    state_label_lts(const state_label_lts& other)=default;

    /** \brief Copy assignment. */
    state_label_lts& operator=(const state_label_lts& other)=default;

    /** \brief Construct a single state label out of the elements in a container.
    */
    template < class CONTAINER >
    explicit state_label_lts(const CONTAINER& l)
    {
      static_assert(std::is_same<typename CONTAINER::value_type, data::data_expression>::value,"Value type must be a data_expression");
      this->push_front(single_label(l.begin(),l.size()));
    }

    /** \brief Construct a state label out of a balanced tree of data expressions, representing a state label.
    */
    explicit state_label_lts(const single_label& l)
    {
      this->push_front(single_label(l.begin(),l.size()));
    }

    /** \brief Construct a state label out of list of balanced trees of data expressions, representing a state label.
    */
    explicit state_label_lts(const super& l)
      : super(l)
    {
    }

    /** \brief An operator to concatenate two state labels. */
    state_label_lts operator+(const state_label_lts& l) const
    {
      return state_label_lts(static_cast<super>(*this)+static_cast<super>(l));
    }
};

/** \brief Pretty print a state value of this LTS.
    \details The label l is printed as (t1,...,tn) if it consists of a single label.
             Otherwise the labels are printed with square brackets surrounding it. 
    \param[in] l  The state value to pretty print.
    \return           The pretty-printed representation of value. */

inline std::string pp(const state_label_lts& label)
{
  std::string s;
  if (label.size()!=1)
  {
    s += "[";
  }
  bool first=true;
  for(const state_label_lts::single_label& l: label) 
  {
    if (!first)
    {
      s += ", ";
    }
    first = false;
    s += "(";
    for (size_t i=0; i<l.size(); ++i)
    {
      s += data::pp(l[i]);
      if (i+1<l.size())
      {
        s += ",";
      }
    }
    s += ")";
  }
  if (label.size()!=1)
  {
    s += "]";
  }
  return s; 
}

/** \brief A class containing the values for action labels for the .lts format.
    \details An action label is a multi_action. */
class action_label_lts: public mcrl2::lps::multi_action
{
  public:

    /** \brief Default constructor. */
    action_label_lts()
    {}

    /** \brief Copy constructor. */
    action_label_lts(const action_label_lts& other)=default;

    /** \brief Copy assignment. */
    action_label_lts& operator=(const action_label_lts& other)=default;

    /** \brief Constructor. */
    explicit action_label_lts(const mcrl2::lps::multi_action& a):mcrl2::lps::multi_action(a)
    {
    }

    /** \brief Hide the actions with labels in tau_actions.
        \return Returns whether the hidden action becomes empty, i.e. a tau or hidden action.
    */
    void hide_actions(const std::vector<std::string>& tau_actions)
    {
      const process::action_list mas = this->actions();
      process::action_list new_action_list;
      for (process::action_list:: const_iterator i=mas.begin(); i!=mas.end(); ++i)
      {
        const process::action a=*i;

        if (std::find(tau_actions.begin(),tau_actions.end(),
                      std::string(a.label().name()))==tau_actions.end())  // this action must not be hidden.
        {
          new_action_list.push_front(a);
        }
      }
      m_actions=new_action_list;
    }

    /* \brief The action label that represents the internal action.
    */
    static const action_label_lts& tau_action()
    {
      static action_label_lts tau_action=action_label_lts();
      return tau_action;
    }

};

/** \brief Print the action label to string. */
inline std::string pp(const action_label_lts& l)
{
  return lps::pp(mcrl2::lps::multi_action(l));
}

/** \brief Parse a string into an action label.
    \details The string is typechecked against the data specification and
             list of declared actions. If parsing or type checking fails, an
             mcrl2::runtime_error is thrown.
    \param[in] multi_action_string The string to be parsed.
    \param[in] data_spec The data specification used for parsing.
    \param[in] act_decls Action declarations.
    \return The parsed and type checked multi action. */
inline action_label_lts parse_lts_action(
  const std::string& multi_action_string,
  const data::data_specification& data_spec,
  const process::action_label_list& act_decls)
{
  return action_label_lts(lps::parse_multi_action(multi_action_string, act_decls, data_spec));
}

namespace detail
{

/** \brief a base class for lts_lts_t and probabilistic_lts_t.
 */
class lts_lts_base
{
  protected:
    bool m_has_valid_data_spec;
    data::data_specification m_data_spec;
    bool m_has_valid_parameters;
    data::variable_list m_parameters;
    bool m_has_valid_action_decls;
    process::action_label_list m_action_decls;

  public:
    /// \brief Default constructor
    lts_lts_base()
    : m_has_valid_data_spec(false),
      m_has_valid_parameters(false),
      m_has_valid_action_decls(false)
    {}

    /// \brief Copy constructor
    lts_lts_base(const lts_lts_base& l)
    : m_has_valid_data_spec(l.m_has_valid_data_spec),
      m_data_spec(l.m_data_spec),
      m_has_valid_parameters(l.m_has_valid_parameters),
      m_parameters(l.m_parameters),
      m_has_valid_action_decls(l.m_has_valid_action_decls),
      m_action_decls(l.m_action_decls) 
    {}

    void swap(lts_lts_base& l)
    {
      bool aux=m_has_valid_data_spec;
      m_has_valid_data_spec=l.m_has_valid_data_spec;
      l.m_has_valid_data_spec=aux;
      
      const data::data_specification auxd=m_data_spec;
      m_data_spec=l.m_data_spec;
      l.m_data_spec=auxd;
      
      aux=m_has_valid_parameters;
      m_has_valid_parameters=l.m_has_valid_parameters;
      l.m_has_valid_parameters=aux;
      
      m_parameters.swap(l.m_parameters);
      
      aux=m_has_valid_action_decls;
      m_has_valid_action_decls=l.m_has_valid_action_decls;
      l.m_has_valid_action_decls=aux;
      
      m_action_decls.swap(l.m_action_decls);
    }

    /** \brief Yields the type of this lts, in this case lts_lts. */
    lts_type type() const
    {
      return lts_lts;
    }

    /** \brief Returns the mCRL2 data specification of this LTS.
    */
    const data::data_specification& data() const
    {
      assert(m_has_valid_data_spec);
      return m_data_spec;
    }

    /** \brief Indicates whether the labels in lts_extra is valid
    */
    bool has_action_labels() const
    {
      return m_has_valid_action_decls;
    }

    /** \brief Return action label declarations stored in this LTS.
    */
    const process::action_label_list& action_labels() const
    {
      assert(m_has_valid_action_decls);
      return m_action_decls;
    }

    /** \brief Set the action label information for this LTS.
     * \param[in] decls  The action labels to be set in this lts.
    */
    void set_action_labels(const process::action_label_list& decls)
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
    */
    void set_data(const data::data_specification& spec)
    {
      m_has_valid_data_spec=true;
      m_data_spec=spec;
    }

    /** \brief Indicates whether the process parameters are valid
    */
    bool has_process_parameters() const
    {
      return m_has_valid_parameters;
    }

    /** \brief Return the process parameters stored in this LTS.
    */
    const data::variable_list& process_parameters() const
    {
      assert(m_has_valid_parameters);
      return m_parameters;
    }

    /** \brief Returns the i-th parameter of the state vectors stored in this LTS.
     * \return The state parameters stored in this LTS.
    */
    const data::variable& process_parameter(size_t i) const
    {
      assert(i<m_parameters.size());
      assert(m_has_valid_parameters);
      data::variable_list::const_iterator j=m_parameters.begin();
      for(size_t c=0; c!=i; ++j, ++c)
      {}
      return *j;
    }


    /** \brief Set the state parameters for this LTS.
     * \param[in] params  The state parameters for this lts.
    */
    void set_process_parameters(const data::variable_list& params)
    {
      m_has_valid_parameters=true;
      m_parameters=params;
    }

};

} // namespace detail


/** \brief This class contains labelled transition systems in .lts format.
    \details In this .lts format, an action label is a multi action, and a
           state label is an expression of the form STATE(t1,...,tn) where
           ti are data expressions.
*/
class lts_lts_t : public lts< state_label_lts, action_label_lts, detail::lts_lts_base >
{
  public:
    typedef lts< state_label_lts, action_label_lts, detail::lts_lts_base > super;

    /** \brief Creates an object containing no information. */
    lts_lts_t()
    {}

    /** \brief Creates an object containing a muCRL specification.
     * \param[in] t The muCRL specification that will be stored in the object. */
    lts_lts_t(const atermpp::aterm& t);

    /** \brief Creates an object containing an mCRL2 specification.
     * \param[in] spec The mCRL2 specification that will be stored in the object. */
    lts_lts_t(lps::specification const& spec);

    /** \brief Copy constructor */
    lts_lts_t(const lts_lts_t& l):
      super(l)
    {}

    /** \brief Load the labelled transition system from file.
     *  \details If the filename is empty, the result is read from stdout.
     *  \param[in] filename Name of the file to which this lts is written.
     */
    void load(const std::string& filename);

    /** \brief Save the labelled transition system to file.
     *  \details If the filename is empty, the result is read from stdin.
     *  \param[in] filename Name of the file from which this lts is read.
     */
    void save(const std::string& filename) const;
};

/** \brief This class contains probabilistic labelled transition systems in .lts format.
    \details In this .lts format, an action label is a multi action, and a
           state label is an expression of the form STATE(t1,...,tn) where
           ti are data expressions.
*/
class probabilistic_lts_lts_t : 
         public probabilistic_lts< state_label_lts, 
                                   action_label_lts, 
                                   probabilistic_state<size_t, lps::probabilistic_data_expression>, 
                                   detail::lts_lts_base >
{
  public:
    typedef probabilistic_lts< state_label_lts, 
                               action_label_lts, 
                               probabilistic_state_t,
                               detail::lts_lts_base > super;

    /** \brief Creates an object containing no information. */
    probabilistic_lts_lts_t()
    {}

    /** \brief Creates an object containing a muCRL specification.
     * \param[in] t The muCRL specification that will be stored in the object. */
    probabilistic_lts_lts_t(const atermpp::aterm& t);

    /** \brief Creates an object containing an mCRL2 specification.
     * \param[in] spec The mCRL2 specification that will be stored in the object. */
    probabilistic_lts_lts_t(lps::specification const& spec);

    /** \brief Copy constructor */
    probabilistic_lts_lts_t(const probabilistic_lts_lts_t& l):
      super(l)
    {}

    /** \brief Load the labelled transition system from file.
     *  \details If the filename is empty, the result is read from stdout.
     *  \param[in] filename Name of the file to which this lts is written.
     */
    void load(const std::string& filename);

    /** \brief Save the labelled transition system to file.
     *  \details If the filename is empty, the result is read from stdin.
     *  \param[in] filename Name of the file from which this lts is read.
     */
    void save(const std::string& filename) const;
};
} // namespace lts
} // namespace mcrl2

#endif
