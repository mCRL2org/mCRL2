#include <sip/detail/layout_manager.h>
#include <sip/detail/layout_elements.h>
#include <sip/detail/object.h>

#include <xml2pp/text_reader.h>

namespace sip {
  namespace layout {

    margins    manager::default_margins = {0,0,0,0};

    visibility manager::default_visibility = visible;

    box< vertical >::alignment     box< vertical >::default_alignment = box< vertical >::center;
    box< horizontal >::alignment   box< horizontal >::default_alignment = box< horizontal >::center;

    box< vertical >::constraints   box< vertical >::default_constraints(box< vertical >::default_alignment,
                                        manager::default_margins, manager::default_visibility);

    box< horizontal >::constraints box< horizontal >::default_constraints(box< horizontal >::default_alignment,
                                        manager::default_margins, manager::default_visibility);

    element::aptr element::static_read_structure(std::string& input) {
      xml2pp::text_reader r(input);

      r.read();

      /* Read structure data */
      return (static_read_structure(r));
    }

    element::aptr element::static_read_structure(xml2pp::text_reader& r) {
      using namespace sip::layout::elements;

      std::string name(r.element_name());

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

    manager::aptr manager::static_read_structure(xml2pp::text_reader& r) {
      std::string name(r.element_name());

      layout::manager::aptr new_element;

      if (name == "box-layout-manager") {
        std::string type;

        r.get_attribute(&type, "type");

        if (type == "vertical") {
          new_element = manager::aptr(new box_vertical::box());
        }
        else {
          new_element = manager::aptr(new box_horizontal::box());
        }
      }
      else {
        throw (new sip::exception(exception_identifier::unknown_layout_element, name));
      }

      /* Read abstract element specific data */
      new_element->read(r);
      new_element->read_structure(r);

      return (new_element);
    }
  }
}

