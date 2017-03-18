// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/** \file state_label_empty.h
 *
 * \brief This file contains the class with empty state labels.
 * \author Jan Friso Groote
 */


#ifndef MCRL2_LTS_STATE_LABEL_EMPTY_H
#define MCRL2_LTS_STATE_LABEL_EMPTY_H

#include <string>

namespace mcrl2
{
namespace lts
{

/** \brief Contains empty state values, used for lts's without state valued.
 *  \details Empty state values are used in labelled transition
 *          systems which do not have state values, such as
 *          the .aut format. */
class state_label_empty
{
  public:

    /* \brief Equality on empty state labels. Always returns true.
    */
    bool operator ==(const state_label_empty&) const
    {
      return true;
    }

    /* \brief Non equality on empty state labels. Always returns false.
    */
    bool operator !=(const state_label_empty& other) const
    {
      return !(*this==other);
    }

    /** \brief An operator to concatenate two state labels. */
    state_label_empty operator+(const state_label_empty& ) const
    {
      return state_label_empty();
    }

};

inline std::string pp(const state_label_empty& )
{
  return "empty";
}


} // namespace lts
} // namespace mcrl2

#endif


