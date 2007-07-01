//  Copyright 2007 Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file source/display.cpp

#include <iostream>
#include <functional>
#include <utility>

#include <boost/bind.hpp>

#include "tipi/detail/layout_manager.hpp"
#include "tipi/detail/layout_elements.hpp"
#include "tipi/display.hpp"
#include "tipi/object.hpp"
#include "tipi/tool.hpp"

namespace tipi {

  void display::disassociate(tipi::layout::element const* e) {
    element_for_id::iterator i = std::find_if(m_element_by_id.begin(), m_element_by_id.end(),
                boost::bind(std::equal_to< tipi::layout::element const* >(), e, boost::bind(&element_for_id::value_type::second, _1)));

    if (i != m_element_by_id.end()) {
      m_element_by_id.erase(i);
    }
  }

  namespace layout {

    const margins       manager::default_margins;

    const visibility    manager::default_visibility = visible;

    const properties    manager::default_properties(middle, left, manager::default_margins, manager::default_visibility);

    /**
     * \param[in] m a mediator to synchronise an element with the associated element in a (G)UI
     **/
    mediator::wrapper_aptr tool_display::instantiate(mediator* m) const {
      if (m_manager.get() != 0) {
        return (m_manager->instantiate(m));
      }

      return mediator::wrapper_aptr();
    }

    /**
     * \param[in] s string data with state descriptions
     * \param[in] elements a vector with the elements that should be updated
     *
     * Looks up an element by its id and calls the read_structure on this
     * member to read its new state from the text reader.
     *
     * \return vector of pointers to elements that have been updated
     **/
    void tool_display::update(std::string const& s, std::vector < tipi::layout::element const* >& elements) {

      /* Find the element that is to be changed */
      if (m_manager.get() != 0) {
        tipi::visitors::restore(*this, elements, s);
      }
    }
  }
}

