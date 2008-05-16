#include <tipi/tool_display.hpp>
// Copyright: see the accompanying file COPYING.
#include <tipi/visitors.hpp>

namespace tipi {
  namespace layout {

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

