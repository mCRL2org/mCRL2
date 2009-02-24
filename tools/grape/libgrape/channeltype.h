// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file channeltype.h
//
// Declares the channeltype enumeration.

#ifndef LIBGRAPE_CHANNELTYPE_H
#define LIBGRAPE_CHANNELTYPE_H

namespace grape
{
  namespace libgrape
  {
    /**
     * \short Channeltype enumeration.
     * An enumeration that includes all possible channel types ({channel_visible, channel_hidden, channel_blocked}).
     */
    enum channeltype
    {
      channel_visible, 
      channel_hidden, 
      channel_blocked
    };

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_CHANNELTYPE_H
