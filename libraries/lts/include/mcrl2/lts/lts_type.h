// Author(s): Muck van Weerdenburg, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/** \file lts_type.h
 *
 * \brief The file containing the types of labelled transition systems.
 * \author Muck van Weerdenburg, Jan Friso Groote
 */

#ifndef MCRL2_LTS_LTS_TYPE_H
#define MCRL2_LTS_LTS_TYPE_H

namespace mcrl2
{

/** \brief The main LTS namespace.
 * \details This namespace contains all data structures and members of the LTS
 * library.
 */
namespace lts
{

/** \brief The enumerated type lts_type contains an index for every type
 *   type of labelled transition system that is supported by the system.
 *   \details Every type has an associated labelled transition format. E.g.
 *   for lts_lts the type of the lts is lts_lts_t. Similarly, lts_aut_t,
 *   etc. are available. Files in which the lts's are stored have
 *   the name file.lts, file.aut, etc.
 */
enum lts_type
{
  lts_none,                  /**< unknown or no format */
  lts_lts,                   /**< mCRL2 SVC format */
  lts_aut,                   /**< Ald&eacute;baran format (CADP) */
  lts_fsm,                   /**< FSM format */
  lts_dot,                   /**< GraphViz format */
  lts_type_min=lts_none,
  lts_type_max=lts_dot
};

}
}

#endif // MCRL2_LTS_LTS_TYPE_H
