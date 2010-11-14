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
#include "mcrl2/exception.h"

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
  class action_label_string:
        public std::string
  { 
    public:

      action_label_string()
      {}

      action_label_string(const std::string &s):std::string(s)
      {}

      void hide_actions(const std::vector<std::string> &)
      {
        throw mcrl2::runtime_error("Cannot hide action labels, as actions are strings without structure");
      }
      
     bool operator<(const action_label_string &l) const
     { 
       return std::string(*this)<std::string(l);
     }
  };

  inline std::string pp(const action_label_string &l)
  {
    return l;
  }




} // namespace detail

} // namespace lts
} // namespace mcrl2

#endif


