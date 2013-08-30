// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/** \file lts_bcg.h
 *
 * \brief This file contains a class that contains labelled transition systems in bcg format.
 * \details A labelled transition system in bcg format is a transition system
 * with as state labels vectors of strings, and as transition labels strings.
 * \author Jan Friso Groote
 */


#ifndef MCRL2_LTS_LTS_BCG_H
#define MCRL2_LTS_LTS_BCG_H

#ifdef USE_BCG
#include "mcrl2/lts/state_label_empty.h"
#include "mcrl2/lts/action_label_string.h"
#include "mcrl2/lts/lts.h"

namespace mcrl2
{
namespace lts
{
/** \brief This class contains labelled transition systemis corresponding to the BCG labelled transition system format
          used in the Ceasar/Aldebaran toolset developed at INRIA in Grenoble.
   \details In this lts format there are no state labels, and action labels are strings.
*/
class lts_bcg_t : public lts< detail::state_label_empty, detail::action_label_string >
{
  public:

    /** \brief Provides the type of this lts, in casu lts_bcg.
    */
    lts_type type() const
    {
      return lts_bcg;
    }

    /** \brief Save the labelled transition system to file.
     *  \details If the filename is empty, the result is read from stdin.
     *  \param[in] filename Name of the file from which this lts is read.
     */
    void load(const std::string& filename);

    /** \brief Save the labelled transition system to file.
     *  \details If the filename is empty, the result is written to stdout.
     *  \param[in] filename Name of the file to which this lts is written.
     */
    void save(const std::string& filename) const;
};

} // namespace lts
} // namespace mcrl2

#endif
#endif
