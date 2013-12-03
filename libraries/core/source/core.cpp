// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file core.cpp

#include "mcrl2/atermpp/detail/utility.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/print.h"

namespace mcrl2 {

namespace core {

//--- start generated core overloads ---//
std::string pp(const core::identifier_string& x) { return core::pp< core::identifier_string >(x); }
std::string pp(const core::nil& x) { return core::pp< core::nil >(x); }
//--- end generated core overloads ---//

} // namespace core

} // namespace mcrl2
