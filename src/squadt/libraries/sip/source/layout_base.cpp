#ifndef LAYOUT_BASE_TCC
#define LAYOUT_BASE_TCC

#include <iostream>
#include <sstream>
#include <memory>

#include <sip/layout_base.h>
#include <sip/detail/layout_elements.h>
#include <sip/detail/layout_manager.h>
#include <sip/detail/layout_mediator.h>
#include <sip/detail/object.h>

namespace sip {
  namespace layout {

    element::element() : id(reinterpret_cast < element::identifier > (this)),
                current_event_handler(&global_event_handler), grow(true) {
    }

    element::~element() {
    }

    /**
     * @param[in] o the stream to write to
     **/
    void element::write_attributes(std::ostream& o) const {
      if (id != 0) {
        o << " id=\"" << id << "\"";
      }
    }

    /**
     * @param[in] i the identifier of the wanted element
     **/
    void element::set_id(element::identifier i) {
      id = i;
    }

    element::identifier element::get_id() {
      return(id);
    }

    /**
     * @param[in] r the xml2pp text reader to read from
     **/
    void element::read(read_context& r) {
      r.reader.get_attribute(&id, "id");

      r.relate_id_to_element(id, this);
    }

    std::string element::get_state() const {
      std::ostringstream s;
    
      write_structure(s);
    
      return (s.str());
    }

    /**
     * @param[in] i the identifier of the wanted element
     **/
    element* element::find(element::identifier i) {
      return ((i == id) ? this : 0);
    }

    /**
     * @param[in] m the mediator object to use
     * @param[in] t pointer to the associated (G)UI object
     **/
    void element::update(layout::mediator* m, layout::mediator::wrapper* t) const {
      std::cerr << "No specific update method implemented!\n";
    }

    element::aptr element::static_read_structure(std::string& input) {
      xml2pp::text_reader r(input);
      read_context        c(r);

      /* Read structure data */
      return (static_read_structure(c));
    }

    element::aptr element::static_read_structure(read_context& r) {
      using namespace sip::layout::elements;

      std::string name(r.reader.element_name());

      layout::element::aptr new_element;

      if (name == "label") {
        new_element = element::aptr(new label());
      }
      else if (name == "button") {
        new_element = element::aptr(new button());
      }
      else if (name == "radio-button") {
        new_element = element::aptr(new radio_button());
      }
      else if (name == "checkbox") {
        new_element = element::aptr(new checkbox());
      }
      else if (name == "progress-bar") {
        new_element = element::aptr(new progress_bar());
      }
      else if (name == "text-field") {
        new_element = element::aptr(new text_field());
      }

      if (new_element.get() != 0) {
        /* Read abstract element specific data */
        new_element->read(r);
        new_element->read_structure(r);
      }
      else { /* Assume layout manager */
        new_element = manager::static_read_structure(r);
      }

      return (new_element);
    }
  }
}

#endif
