#include <iostream>
#include <functional>
#include <utility>

#include <boost/bind.hpp>

#include <sip/detail/layout_manager.h>
#include <sip/detail/layout_elements.h>
#include <sip/detail/layout_tool_display.h>
#include <sip/object.h>
#include <sip/tool.h>

#include <sip/detail/event_handlers.h>

namespace sip {

  void display::disassociate(sip::layout::element const* e) {
    element_for_id::iterator i = std::find_if(m_element_by_id.begin(), m_element_by_id.end(),
                boost::bind(std::equal_to< sip::layout::element const* >(), e, boost::bind(&element_for_id::value_type::second, _1)));

    if (i != m_element_by_id.end()) {
      m_element_by_id.erase(i);
    }
  }

  namespace layout {

    basic_event_handler element::global_event_handler;

    const margins       manager::default_margins;

    const visibility    manager::default_visibility = visible;

    const properties    manager::default_properties(middle, left, manager::default_margins, manager::default_visibility);

    /**
     * Blocks until the next change event or when the object is destroyed
     *
     * \return Whether there was a change, otherwise the object was destroyed
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

    /**
     * \param[in] t the tool communicator object to use
     * \param[in] e the layout element of which to send the state
     **/
    void element::update(tool::communicator* t, layout::element const* e) {
      t->send_display_data(e);
    }

    /**
     * \param[in] m a mediator to synchronise an element with the associated element in a (G)UI
     **/
    mediator::wrapper_aptr tool_display::instantiate(mediator* m) const {
      if (m_manager.get() != 0) {
        return (m_manager->instantiate(m));
      }

      return mediator::wrapper_aptr();
    }

    /** \todo remove this class after new generic visitor has been added */
    class elements_wrapper {
      private:

        tool_display&                                m_object;

        std::vector < sip::layout::element const* >& m_elements;

      public:

        elements_wrapper(tool_display& t, std::vector < sip::layout::element const* >& e) : m_object(t), m_elements(e) {
        }

        template < typename V >
        void accept(V& visitor) {
          visitor.visit(m_object, m_elements);
        }
    };

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
        elements_wrapper w(*this, elements);

        sip::visitors::restore(w, s);
      }
    }
  }
}

