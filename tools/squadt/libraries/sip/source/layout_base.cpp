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
#include <sip/detail/event_handlers.h>

namespace sip {
  namespace layout {

    basic_event_handler element::global_event_handler;

    element::element() : m_event_handler(&global_event_handler), m_grow(true) {
    }

    element::~element() {
    }

    void element::on_change(layout::basic_event_handler::handler_function h) const {
      m_event_handler->add(this, h);
    }

    /**
     * \param[in] b whether or not to execute non-element-specific handlers
     **/
    void element::activate_handlers(bool b) {
      m_event_handler->process(this, b);
    }

    /**
     * Blocks until the next change event or when the object is destroyed
     **/
    void element::await_change() const {
      m_event_handler->await_change(this);
    }

    /**
     * \param[in] e event handler object that will dispatch events for this object
     **/
    void element::set_event_handler(basic_event_handler* e) {
      m_event_handler->transfer(*e, this);
    }

    basic_event_handler* element::get_event_handler() const {
      return m_event_handler;
    }

    /**
     * \param[in] m the mediator object to use
     * \param[in] t pointer to the associated (G)UI object
     **/
    void element::update(layout::mediator* m, layout::mediator::wrapper* t) const {
      assert("No specific update method implemented!\n");
    }
  }
}

#endif
