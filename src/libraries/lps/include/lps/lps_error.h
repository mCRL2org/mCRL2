// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : lps/lps_error.h
// date          : 14-7-2005
// version       : 0.2
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

#ifndef LPS_ERROR_H
#define LPS_ERROR_H

#include <iostream>
#include <cstdlib>

namespace lps {

inline
void lps_warning(const std::string& message)
{
  std::cerr << "LPS WARNING " << message << std::endl;
  std::exit(1);
}

inline
void lps_error(const std::string& message)
{
  std::cerr << "LPS FATAL ERROR " << message << std::endl;
  std::exit(1);
}

} // namespace lps

#endif // LPS_ERROR_H
