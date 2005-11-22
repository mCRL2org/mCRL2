// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : lpe/lpe_error.h
// date          : 14-7-2005
// version       : 0.2
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

#ifndef LPE_ERROR_H
#define LPE_ERROR_H

#include <iostream>
#include <cstdlib>

namespace lpe {

inline
void lpe_warning(const std::string& message)
{
  std::cerr << "LPE WARNING " << message << std::endl;
  std::exit(1);
}

inline
void lpe_error(const std::string& message)
{
  std::cerr << "LPE FATAL ERROR " << message << std::endl;
  std::exit(1);
}

} // namespace lpe

#endif // LPE_ERROR_H
