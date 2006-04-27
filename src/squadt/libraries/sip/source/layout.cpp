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

    element* element::read_structure(std::string& input) {
      xml2pp::text_reader r(input);

      r.read();

      return (read_structure(r));
    }

    element* element::read_structure(xml2pp::text_reader& r) {
      using namespace sip::layout::elements;

      element* new_element;

      std::string        name = r.element_name();
      object::identifier id;

      r.get_attribute(&id, "id");

      if (name == "label") {
        new_element = label::read_structure(r);
      }
      else if (name == "button") {
        new_element = button::read_structure(r);
      }
      else if (name == "radio-button") {
        new_element = radio_button::read_structure(r);
      }
      else if (name == "progress-bar") {
        new_element = progress_bar::read_structure(r);
      }
      else if (name == "text-field") {
        new_element = text_field::read_structure(r);
      }
      else if (name == "box-layout-manager") {
        std::string type;

        r.get_attribute(&type, "type");

        new_element = (type == "vertical") ? box< vertical >::read_structure(r) :
                                             box< horizontal >::read_structure(r);
      }

      new_element->id = id;

      return (new_element);
    }

    manager::sptr manager::read_structure(xml2pp::text_reader& r) {
      if (r.is_element("box-layout-manager")) {
        manager* new_manager;

        std::string type;

        r.get_attribute(&type, "type");

        if (type == "vertical") {
          new_manager = static_cast < manager* > (box< vertical >::read_structure(r));
        }
        else {
          new_manager = static_cast < manager* > (box< horizontal >::read_structure(r));
        }

        return (manager::sptr(new_manager));
      }
      else {
        return (manager::sptr());
      }
    }
  }
}

