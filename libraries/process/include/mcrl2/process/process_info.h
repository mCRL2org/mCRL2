// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/process_info.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_PROCESS_INFO_H
#define MCRL2_PROCESS_PROCESS_INFO_H

#include <iostream>
#include "mcrl2/process/process_specification.h"

namespace mcrl2 {

namespace process {

/// \brief Prints information about a process specification
inline
void process_info(const process_specification& procspec)
{
  std::cout << "Number of equations                 : " << procspec.equations().size() << std::endl;
  std::cout << "Number of declared global variables : " << procspec.global_variables().size() << std::endl;
  std::cout << "Number of declared action labels    : " << procspec.action_labels().size() << std::endl;
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_PROCESS_INFO_H
