// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file INPUT_VALIDATION.cpp
//
// Validation of user input

#include "inputvalidation.h"

using namespace grape::grapeapp;

bool grape::grapeapp::identifier_valid( const wxString p_id )
{
  if (p_id.IsEmpty()) return true;
  
  bool isvalid = true;
  isvalid = isvalid && ((('a' <= p_id.GetChar(0)) && (p_id.GetChar(0) <= 'z')) || (('A' <= p_id.GetChar(0)) && (p_id.GetChar(0) <= 'Z')) || ('_' <= p_id.GetChar(0)));
    
  for (unsigned int i=1; i < p_id.Length(); ++i)
  {
    isvalid = isvalid && ((('a' <= p_id.GetChar(i)) && (p_id.GetChar(i) <= 'z')) || (('A' <= p_id.GetChar(i)) && (p_id.GetChar(i) <= 'Z')) || (('0' <= p_id.GetChar(i)) && (p_id.GetChar(i) <= '9')) || ('_' <= p_id.GetChar(i)));
  }
  return isvalid;
}

