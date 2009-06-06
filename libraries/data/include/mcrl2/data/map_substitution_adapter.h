// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/map_substitution_adapter.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_MAP_SUBSTITUTION_ADAPTER_H
#define MCRL2_DATA_MAP_SUBSTITUTION_ADAPTER_H

#include "mcrl2/data/substitution.h"
#include "mcrl2/data/replace.h"

namespace mcrl2 {

namespace data {

  /// \brief Utility function for creating a map_substitution_adapter.
  template <typename MapContainer>
  map_substitution<MapContainer const&> make_map_substitution_adapter(const MapContainer& m)
  {
    return map_substitution<MapContainer const&>(m);
  }

  /// \brief Utility function for creating a map_substitution_adapter.
  template <typename MapContainer>
  map_substitution<MapContainer&> make_mutable_map_substitution_adapter(MapContainer& m)
  {
    return map_substitution<MapContainer&>(m);
  }

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_MAP_SUBSTITUTION_ADAPTER_H
