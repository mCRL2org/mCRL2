// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lps2lts.h

#ifndef _LPS2LTS_H
#define _LPS2LTS_H

#define NAME "lps2lts"

ATermAppl *parse_action_list(std::string const& s, int *len);

#endif
