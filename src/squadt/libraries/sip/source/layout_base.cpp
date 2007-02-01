#ifndef LAYOUT_BASE_TCC
#define LAYOUT_BASE_TCC

#include <iostream>
#include <sstream>
#include <memory>

#include <sip/layout_base.h>
#include <sip/object.h>
#include <sip/detail/layout_elements.h>
#include <sip/detail/layout_manager.h>
#include <sip/detail/layout_mediator.h>

namespace sip {
  namespace layout {

    element::element() : m_event_handler(&global_event_handler), m_grow(true) {
    }

    element::~element() {
    }

    /**
     * \param[in] m the mediator object to use
     * \param[in] t pointer to the associated (G)UI object
     **/
    void element::update(layout::mediator* m, layout::mediator::wrapper* t) const {
      std::cerr << "No specific update method implemented!\n";
    }
  }
}

#endif
