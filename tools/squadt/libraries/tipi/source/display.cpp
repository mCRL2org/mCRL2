//  Copyright 2007 Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file source/display.cpp

#include <utility>

#include "tipi/display.hpp"

namespace tipi {

  void display::disassociate(tipi::layout::element const* e) {
    for (element_by_id::iterator i = m_element_by_id.begin(); i != m_element_by_id.end(); ++i) {
      if (i->second.get() == e) {
        m_element_by_id.erase(i);

        break;
      }
    }
  }

  void display::disassociate(tipi::display::element_identifier const& id) {
    element_by_id::iterator i = m_element_by_id.find(id);
   
    if (i != m_element_by_id.end()) {
      m_element_by_id.erase(i);
    }
  }

  /**
   * \pre the element should be in the list
   * \throw false, when the element is not present
   **/
  const ::tipi::display::element_identifier display::find(tipi::layout::element const* e) const {
    for (element_by_id::const_iterator i = m_element_by_id.begin(); i != m_element_by_id.end(); ++i) {
      if (i->second.get() == e) {
        return i->first;

        break;
      }
    }

    throw false;
  }
}

