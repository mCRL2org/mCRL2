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
 * \brief This file contains a class that contains labelled transition systems in aut format.
 * \details A labelled transition system in aut format is a transition system
 * with as state labels vectors of strings, and as transition labels strings.
 * \author Jan Friso Groote
 */


#ifndef MCRL2_LTS_ACTION_LABEL_STRING_H
#define MCRL2_LTS_ACTION_LABEL_STRING_H

#include <string>
#include <vector>
#include <iterator>
#include "mcrl2/exception.h"
#include "mcrl2/utilities/text_utility.h"

namespace mcrl2
{
namespace lts
{
namespace detail
{

/** \brief This class contains strings to be used as values for action labels in lts's.
 *  \details These action labels are used in the aut, fsm, bcg and dot format to represent
 *           values for the action labels in transitions.
 */
class action_label_string: public std::string
{
  public:

    /* \brief Default constructor. The label will contain the default string.
     */
    action_label_string()
    {}

    /* \brief A constructor, where the string s is taken to become the action label.
     */
    action_label_string(const std::string& s):std::string(s)
    {}

    /* \brief An auxiliary function to hide actions. As strings have no structure, hiding of
     *        actions is not possible, and this method will just throw an exception error. */
    bool hide_actions(const std::vector<std::string> &string_vector)
    {
      // Rename action label to tau
      for (std::vector<std::string>::const_iterator i = string_vector.begin(); i != string_vector.end(); ++i)
      {
        // Actions that have parameters have priority over actions without while renaming.
        std::string ns(utilities::regex_replace(*i + "\\([^\\)]*\\)" , "tau" , *this));
        ns = utilities::regex_replace(*i , "tau" , ns) ;
        assign(ns);
      }
      // Remove all "tau|", since this indicates that we are dealing with a multi-action
      std::string ns(utilities::regex_replace("tau\\|" , "" , *this));
      // Remove "|tau" from end of multi-action one still exists
      ns =  utilities::regex_replace("\\|tau$" , "" , ns) ;
      assign(ns);
      return ns=="tau";
    }

    /* \brief A comparison operator comparing the action_label_strings in the same way as strings.
    */
    bool operator<(const action_label_string& l) const
    {
      return std::string(*this)<std::string(l);
    }
};

/* \brief A pretty print operator on action labels, returning it as a string.
*/
inline std::string pp(const action_label_string& l)
{
  return l;
}

} // namespace detail

} // namespace lts
} // namespace mcrl2

#endif


