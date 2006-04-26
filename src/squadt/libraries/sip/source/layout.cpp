#include <sip/detail/layout_manager.h>

#include <xml2pp/text_reader.h>

namespace sip {
  namespace layout {

    margins    manager::default_margins = {0,0,0,0};

    visibility manager::default_visibility = visible;

    box< vertical >::alignment     box< vertical >::default_alignment = box< vertical >::center;

    box< horizontal >::alignment   box< horizontal >::default_alignment = box< horizontal >::center;

    inline void element::read_structure(std::string& input) {
      xml2pp::text_reader reader(input);

      r.read();

      std::string();

      if () {
      label
      button
      radio_button
      progress_bar
      text_field
    }
  }
}

