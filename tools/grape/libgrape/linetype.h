// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file linetype.h
//
// Declares the linetype enumeration.

#ifndef LIBGRAPE_LINETYPE_H
#define LIBGRAPE_LINETYPE_H

namespace grape
{
  namespace libgrape
  {
    /**
     * \short Linetype enumeration.
     * An enumeration that includes all possible line types ({straight, curved, zigzag}).
     */
    enum linetype
    {
      straight,
      curved,
      zigzag
    };

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_LINETYPE_H
