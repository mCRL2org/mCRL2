// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file inputvalidation.h
//
// Validation of user input

#ifndef INPUT_VALIDATION_H
#define INPUT_VALIDATION_H

#include <wx/string.h>

namespace grape
{
  namespace grapeapp
  {
   
    /**
     * Validation of an identifier
     * @param p_id identifier name
     * @return Returns is identifier valid
     */    
    bool identifier_valid( const wxString p_id );
  }
}

#endif // INPUT_VALIDATION_H
