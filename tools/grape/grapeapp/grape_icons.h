// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file grape_icons.h
//
// Includes all icons used by the application

#ifndef GRAPE_ICONS_H
#define GRAPE_ICONS_H

#include <wx/bitmap.h>
#include <wx/hashmap.h>

WX_DECLARE_STRING_HASH_MAP( wxBitmap, IconMap );

/**
 * Initializes the mapping with array icons.
 */
void init_icons();

#endif // GRAPE_ICONS_H
