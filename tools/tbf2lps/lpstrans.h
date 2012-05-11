// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpstrans.h

#include "mcrl2/aterm/aterm.h"


aterm_deprecated::ATermAppl translate(aterm_deprecated::ATermAppl spec, bool convert_bools, bool convert_funcs);
//Pre: spec is an mCRL LPE
//Ret: an equivalent mCRL2 LPS, in which sort Bool is converted based on the
//     values of convert_bools and convert_funcs

bool is_mCRL_spec(aterm_deprecated::ATermAppl spec);
//Ret: spec is a mCRL LPE

