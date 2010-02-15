// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/file_utility.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_FILE_UTILITY_H
#define MCRL2_CORE_FILE_UTILITY_H

#include <ctime>
#include <string>
#include <boost/lexical_cast.hpp>

namespace mcrl2 {

  namespace core {

    inline
    std::string create_filename(const std::string& prefix = "file", const std::string& extension = ".txt")
    {
      time_t seconds = time(0);
      return prefix + boost::lexical_cast<std::string>(seconds) + extension;
    }

  } // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_FILE_UTILITY_H
