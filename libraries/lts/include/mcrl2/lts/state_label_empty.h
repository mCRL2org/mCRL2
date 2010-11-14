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


#ifndef MCRL2_LTS_STATE_LABEL_EMPTY_H
#define MCRL2_LTS_STATE_LABEL_EMPTY_H

namespace mcrl2
{
namespace lts
{
namespace detail
{

  /** \brief Contains empty state values, used for lts's without state valued.
   *  \detail Empty state values are used in labelled transition
   *          systems which do not have state values, such as
   *          the .aut and the .bcg format. */
  class state_label_empty
  {
    public:
      bool operator ==(const state_label_empty &) const
      {
        return true;
      }

      bool operator !=(const state_label_empty &other) const
      {
        return !(*this==other);
      }
  };

} // namespace detail

} // namespace lts
} // namespace mcrl2

#endif


