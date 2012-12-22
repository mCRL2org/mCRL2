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
#include "mcrl2/atermpp/aterm_appl.h"
// #include "mcrl2/atermpp/aterm_balanced_tree.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/multi_action.h"
#include "mcrl2/lps/action_parse.h"
#include "mcrl2/lps/typecheck.h"
#include "mcrl2/lts/lts.h"
#include "mcrl2/lps/detail/multi_action_print.h"

namespace mcrl2
{
namespace lts
{
namespace detail
{

extern std::vector < atermpp::function_symbol > state_function_symbols;

/** \brief This class contains state labels for an labelled transition system in .lts format.
    \details A state label in .lts format consists of a term of the form
             STATE(t1,...,tn) where ti are the data expressions.
*/
class state_label_lts : public atermpp::aterm_appl
{
    friend struct atermpp::aterm_traits<state_label_lts>;

  protected:

    /** \brief We store functions symbols with varying arity and string "STATE"
        in this atermpp::aterm_appl array, as they are automatically protected.
    */

    /** \brief A function to get a protected STATE function symbol of the
               indicated arity.
    */

    static atermpp::function_symbol get_STATE_function_symbol(const size_t arity)
    {

      if (arity>=state_function_symbols.size())
      {
        state_function_symbols.resize(arity+1);
      }
      if (state_function_symbols[arity]==atermpp::function_symbol())
      {
        state_function_symbols[arity]=atermpp::function_symbol("STATE",arity);
      }
      return state_function_symbols[arity];
    }

  public:

    /** \brief Default constructor. The state label becomes some default aterm.
    */
    state_label_lts()
    {
    }

    /** \brief Constructor. Set the state label to the aterm a.
        \details The aterm a must have the shape "STATE(t1,...,tn).*/
    state_label_lts(const atermpp::aterm_appl& a):atermpp::aterm_appl(a)
    {
      // Take care that the STATE function symbol with the desired arity exists.
      const size_t arity=a.function().arity();
      get_STATE_function_symbol(arity);
    }

    /** \brief Construct a state label out of a data_expression_list.
    */
    state_label_lts(const mcrl2::data::data_expression_list& l):
      atermpp::aterm_appl(get_STATE_function_symbol(l.size()),l.begin(),l.end())
    {}

    /** \brief Construct a state label out of a data_expression_vector.
    */
    state_label_lts(const std::vector < mcrl2::data::data_expression > &l):
      atermpp::aterm_appl(get_STATE_function_symbol(l.size()),l.begin(),l.end())
    {}

    /** \brief Get the i-th element of this state label.
        \param[in] i The index of the state label. Must be smaller than the length of this state label.
        \return The data expression at position i of this state label.
    */
    mcrl2::data::data_expression operator [](const size_t i) const
    {
      assert(i<size());
      return mcrl2::data::data_expression((*this)(i));
    }

    /** \brief Set the i-th element of this state label to the indicated value.
        \param[in] e The expression to which the i-th element must be set.
        \param[in] i Index into this state label. */
    void set_element(const mcrl2::data::data_expression& e, const size_t i)
    {
      assert(i<this->size());
      set_argument(e,i);
    }
};

/** \brief Pretty print a state value of this LTS.
    \details The label l is printed as (t1,...,tn).
    \param[in] l  The state value to pretty print.
    \return           The pretty-printed representation of value. */

inline std::string pp(const state_label_lts l)
{
  std::string s;
  s = "(";
  for (size_t i=0; i<l.size(); ++i)
  {
    s += data::pp(l[i]);
    if (i+1<l.size())
    {
      s += ",";
    }
  }
  s += ")";
  return s;
}

/** \brief A class containing the values for action labels for the .lts format.
    \details An action label is a multi_action. */
class action_label_lts:public mcrl2::lps::multi_action
{
    friend struct atermpp::aterm_traits<action_label_lts>;

  public:

    /** \brief Default constructor. */
    action_label_lts()
    {}

    /** \brief Constructor. Sets action label to the multi_action a. */
    action_label_lts(const atermpp::aterm a):mcrl2::lps::multi_action(a)
    {
    }

    /** \brief Constructor. Transforms action label a to form a multi_action a. */
    action_label_lts(const lps::action a):mcrl2::lps::multi_action(a)
    {
    }

    /** \brief Constructor. */
    action_label_lts(const mcrl2::lps::multi_action a):mcrl2::lps::multi_action(a)
    {
    }

    /** \brief Returns this multi_action as an aterm without the time tag.
    */
    atermpp::aterm aterm_without_time() const
    {
      if (this->has_time())
      {
        throw mcrl2::runtime_error("Cannot transform multi action " +
                                   lps::detail::multi_action_print(*this) + " to an atermpp::aterm as it contains time.");
      }
      return mcrl2::core::detail::gsMakeMultAct(this->actions());
    }

    /** \brief Hide the actions with labels in tau_actions.
        \return Returns whether the hidden action becomes empty, i.e. a tau or hidden action.
    */
    bool hide_actions(const std::vector<std::string> &tau_actions)
    {
      using namespace std;
      using namespace mcrl2::lps;

      const action_list mas = this->actions();
      action_list new_action_list;
      for (action_list:: const_iterator i=mas.begin(); i!=mas.end(); ++i)
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

/** \brief Print the action label to string. */
inline std::string pp(const action_label_lts l)
{
  return lps::detail::multi_action_print(mcrl2::lps::multi_action(l));
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
  const lps::action_list& act_decls)
{
  // TODO: rewrite this cryptic code
  atermpp::aterm_appl t = mcrl2::lps::detail::multi_action_to_aterm(mcrl2::lps::parse_multi_action_new(multi_action_string));
  lps::multi_action ma=lps::action_list(atermpp::aterm_list(t(0)));
  lps::type_check(ma,data_spec,atermpp::aterm_cast<lps::action_label_list>(act_decls));
  lps::translate_user_notation(ma);
  lps::normalize_sorts(ma, data_spec);
  return action_label_lts(mcrl2::core::detail::gsMakeMultAct(ma.actions()));
}


} // namespace detail


/** \brief This class contains labelled transition systems in .lts format.
    \details In this .lts format, an action label is a multi action, and a
           state label is an expression of the form STATE(t1,...,tn) where
           ti are data expressions.
*/
class lts_lts_t : public lts< detail::state_label_lts, detail::action_label_lts >
{

  private:
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
    lts_lts_t(const atermpp::aterm &t);

    /** \brief Creates an object containing an mCRL2 specification.
     * \param[in] spec The mCRL2 specification that will be stored in the object. */
    lts_lts_t(lps::specification const& spec);

    /** \brief Copy constructor */
    lts_lts_t(const lts_lts_t& l):
      lts< detail::state_label_lts, detail::action_label_lts >(l),
      m_has_valid_data_spec(l.m_has_valid_data_spec),
      m_data_spec(l.m_data_spec),
      m_has_valid_parameters(l.m_has_valid_parameters),
      m_parameters(l.m_parameters),
      m_has_valid_action_decls(l.m_has_valid_action_decls),
      m_action_decls(l.m_action_decls)
    {}

    /** \brief Standard destructor for the class lts_lts
    */
    ~lts_lts_t()
    {}

    /** \brief Swap function for this lts. */
    void swap(lts_lts_t& l)
    {
      lts< detail::state_label_lts, detail::action_label_lts >::swap(l);

      {
        const bool aux=m_has_valid_data_spec;
        m_has_valid_data_spec=l.m_has_valid_data_spec;
        l.m_has_valid_data_spec=aux;
      }
      {
        const data::data_specification aux=m_data_spec;
        m_data_spec=l.m_data_spec;
        l.m_data_spec=aux;
      }
      {
        const bool aux=m_has_valid_parameters;
        m_has_valid_parameters=l.m_has_valid_parameters;
        l.m_has_valid_parameters=aux;
      }
      {
        const data::variable_list aux=m_parameters;
        m_parameters=l.m_parameters;
        l.m_parameters=aux;
      }
      {
        const bool aux=m_has_valid_action_decls;
        m_has_valid_action_decls=l.m_has_valid_action_decls;
        l.m_has_valid_action_decls=aux;
      }
      {
        const lps::action_label_list aux=m_action_decls;
        m_action_decls=l.m_action_decls;
        l.m_action_decls=aux;
      }
    }

    /** \brief Yields the type of this lts, in this case lts_lts. */
    lts_type type() const
    {
      return lts_lts;
    }

    /** \brief Returns the mCRL2 data specification of this LTS.
    */
    data::data_specification data() const
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
    lps::action_label_list action_labels() const
    {
      assert(m_has_valid_action_decls);
      return m_action_decls;
    }

    /** \brief Set the action label information for this LTS.
     * \param[in] decls  The action labels to be set in this lts.
    */
    void set_action_labels(const lps::action_label_list& decls)
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
    void set_data(data::data_specification const& spec)
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
    data::variable_list process_parameters() const
    {
      assert(m_has_valid_parameters);
      return m_parameters;
    }

    /** \brief Returns the i-th parameter of the state vectors stored in this LTS.
     * \return The state parameters stored in this LTS.
    */
    data::variable process_parameter(size_t i) const
    {
      assert(i<m_parameters.size());
      assert(m_has_valid_parameters);
      data::variable_list::const_iterator j=m_parameters.begin();
      for(size_t c=0; c!=i; ++j, ++c)
      {}
      return data::variable(*j);
    }


    /** \brief Set the state parameters for this LTS.
     * \param[in] params  The state parameters for this lts.
    */
    void set_process_parameters(const data::variable_list& params)
    {
      m_has_valid_parameters=true;
      m_parameters=params;
    }

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
