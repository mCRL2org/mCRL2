// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : mcrl2/mcrl2_error.h
// date          : 14-7-2005
// version       : 0.2
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

#ifndef MCRL2_ERROR_H
#define MCRL2_ERROR_H

#include <iostream>
#include <cstdlib>

namespace mcrl2 {

inline
void mcrl2_warning(const std::string& message)
{
  std::cerr << "MCRL2 WARNING " << message << std::endl;
  std::exit(1);
}

inline
void mcrl2_error(const std::string& message)
{
  std::cerr << "MCRL2 FATAL ERROR " << message << std::endl;
  std::exit(1);
}

} // namespace mcrl2

#endif // MCRL2_ERROR_H
