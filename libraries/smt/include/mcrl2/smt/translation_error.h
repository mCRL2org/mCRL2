// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_SMT_TRANSLATION_ERROR_H
#define MCRL2_SMT_TRANSLATION_ERROR_H

#include <stdexcept>
#include <string>
#include <cassert>

namespace mcrl2
{

namespace smt
{

class translation_error : public std::runtime_error
{
  public:
    translation_error(const std::string& message) : std::runtime_error(message)
    { assert(false); }
};

}
}

#endif
