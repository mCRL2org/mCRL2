// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/lin_std.h
/// \brief Add your file description here.

#ifndef MCRL2_LPS_LIN_STD_H
#define MCRL2_LPS_LIN_STD_H

#include <aterm2.h>
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/lin_types.h"

mcrl2::lps::specification linearise_std(ATermAppl spec, t_lin_options lin_options);
//Pre:  spec_term is an mCRL2 specification that adheres to the internal
//      structure after data implementation
//      lin_options represents the options that should be used during
//      linearisation
//Post: spec_term is linearised with the standard lineariser using options
//      lin_options
//Ret:  the linearised specification is everything went ok,
//      NULL, otherwise

/// \brief Function to initialize the global variables in the file lin_std.cpp.
/// Needed when the linearization algorithm is called more than once.
void lin_std_initialize_global_variables();

#endif // MCRL2_LPS_LIN_STD_H
