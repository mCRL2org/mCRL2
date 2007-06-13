#include <iostream>
#include <functional>
#include <utility>

#include <boost/bind.hpp>

#include <sip/detail/layout_manager.h>
#include <sip/detail/layout_elements.h>
#include <sip/display.h>
#include <sip/object.h>
#include <sip/tool.h>

namespace sip {

  void display::disassociate(sip::layout::element const* e) {
    element_for_id::iterator i = std::find_if(m_element_by_id.begin(), m_element_by_id.end(),
                boost::bind(std::equal_to< sip::layout::element const* >(), e, boost::bind(&element_for_id::value_type::second, _1)));

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
    void tool_display::update(std::string const& s, std::vector < sip::layout::element const* >& elements) {

      /* Find the element that is to be changed */
      if (m_manager.get() != 0) {
        sip::visitors::restore(*this, elements, s);
      }
    }
  }
}

