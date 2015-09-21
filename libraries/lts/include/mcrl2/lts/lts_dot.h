// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/** \file lts_dot.h
 *
 * \brief This file contains a class that contains labelled transition systems in dot format.
 * \details A labelled transition system in dot format is a transition system
 * with strings as state and transition labels.
 * \author Jan Friso Groote
 */


#ifndef MCRL2_LTS_LTS_DOT_H
#define MCRL2_LTS_LTS_DOT_H

#include <string>
#include <vector>
#include "mcrl2/utilities/logger.h"
#include "mcrl2/core/print.h"
#include "mcrl2/lts/probabilistic_arbitrary_size_label.h"
#include "mcrl2/lts/lts.h"
#include "mcrl2/lts/action_label_string.h"


namespace mcrl2
{
namespace lts
{
namespace detail
{
/** \brief This class contains labels for states in dot format.
   \details A dot state label consists of two strings, the name of a state and a separate label.
*/
class state_label_dot
{
  private:
    std::string m_state_name;
    std::string m_state_label;

  public:

    /** \brief The default constructor.
    */
    state_label_dot()
    {}

    /** \brief A constructor setting the name and label of this state label to the indicated values.
    */
    state_label_dot(const std::string& state_name,
                    const std::string& state_label):m_state_name(state_name),m_state_label(state_label)
    {}

    /** \brief This method sets the name of the state label to the string s.
    */
    void set_name(const std::string& s)
    {
      m_state_name=s;
    }

    /** \brief This method returns the string in the name field of a state label.
    */
    std::string name() const
    {
      return m_state_name;
    }

    /** \brief This method sets the label field of the state label to the string s.
    */
    void set_label(const std::string& s)
    {
      m_state_label=s;
    }

    /** \brief This method returns the label in the name field of a state label.
    */
    std::string label() const
    {
      return m_state_label;
    }

    /** \brief Standard comparison operator, comparing both the string in the name field, as well
              as the one in the label field.
    */
    bool operator ==(const state_label_dot& l) const
    {
      return m_state_name==l.name() && m_state_label==l.label();
    }

    /** \brief Standard inequality operator. Just the negation of equality
    */
    bool operator !=(const state_label_dot& l) const
    {
      return !(*this==l);
    }
};

/** \brief Pretty print function for a state_label_dot. Only prints the label field.
*/
inline std::string pp(const state_label_dot& l)
{
  return l.label();
}
} // namespace detail


/** \brief A class to contain labelled transition systems in graphviz format.
    \details Action labels are strings, and state labels are pairs with a name field
             and an action fields. */
class lts_dot_t : public lts< detail::state_label_dot, detail::action_label_string, detail::probabilistic_arbitrary_size_label >
{

  public:
    /** \brief The lts_type of state_label_dot. In this case lts_dot.
    */
    lts_type type() const
    {
      return lts_dot;
    }

    /** \brief Save the labelled transition system to file.
     *  \details If the filename is empty, the result is read from stdin.
     *  \param[in] filename Name of the file from which this lts is read.
     */
    void load(const std::string&)
    {
      throw mcrl2::runtime_error("The DOT file format is no longer supported as input file format.");
    }
    
    /** \brief Save the labelled transition system to a stream.
     *  \param[in] stream Stream which to write the lts to.
     */   
    void save(std::ostream& os) const;

    /** \brief Save the labelled transition system to a file.
     *  \details Throws an error when the file cannot be opened.
     *  \param[in] filename Name of the file to which this lts is written.
     */
    void save(const std::string& filename) const;
};



} // namespace lts
} // namespace mcrl2

#endif
