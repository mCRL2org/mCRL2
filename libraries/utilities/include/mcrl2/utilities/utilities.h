// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/utilities.h
/// \brief Miscellaneous utility functions

#ifndef UTILITIES_H
#define UTILITIES_H

#include <cmath>

#include "aterm2.h"


/**
 * \brief Converts to textual representation for a boolean
 * \param[in] b the boolean to convert
 * \note for writing to stream consider using std::boolalpha
 **/
inline const char* bool_to_char_string(bool b) {
  return (b) ? "true" : "false";
}

inline
ATermAppl initAtermAppl(ATermAppl& f, ATermAppl v)
{
  ATprotectAppl(&f);
  return v;
}


// Eventually, these two functions should probably be moved into core
/**
 * \brief Creates an identifier for the for the ctau action
 **/
inline ATermAppl make_ctau_act_id() {
  static ATermAppl ctau_act_id = initAtermAppl(ctau_act_id, mcrl2::core::detail::gsMakeActId(ATmakeAppl0(ATmakeAFun("ctau", 0, ATtrue)), ATmakeList0()));

  assert(ctau_act_id);

  return ctau_act_id;
}

/**
 * \brief Creates the ctau action
 **/
inline ATermAppl make_ctau_action() {
  static ATermAppl ctau_action = initAtermAppl(ctau_action, mcrl2::core::detail::gsMakeAction(make_ctau_act_id(), ATmakeList0()));

  assert(ctau_action);

  return ctau_action;
}

#endif
