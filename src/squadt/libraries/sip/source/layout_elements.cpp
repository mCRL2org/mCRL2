#include <sip/detail/layout_elements.h>
#include <sip/detail/event_handlers.h>

namespace sip {
  namespace layout {
    namespace elements {

      /**
       * @param[out] o the stream to which to write the result
       **/
      void label::write_structure(std::ostream& o) const {
        o << "<label id=\"" << id << "\" text=\"" << text << "\"/>";
      }

      /**
       * @param[in] r the xml2pp text reader from which to read
       *
       * \pre reader should point to a button element
       * \post reader points to after the associated end tag of the box
       **/
      void label::read_structure(xml2pp::text_reader& r) {
        r.get_attribute(&text, "text");

        r.read();

        current_event_handler->process(this);
      }

      /**
       * @param[out] o the stream to which to write the result
       **/
      void button::write_structure(std::ostream& o) const {
        o << "<button id=\"" << id << "\" label=\"" << label << "\"/>";
      }

      /**
       * @param[in] r the xml2pp text reader from which to read
       *
       * \pre reader should point to a button element
       * \post reader points to after the associated end tag of the box
       **/
      void button::read_structure(xml2pp::text_reader& r) {
        r.get_attribute(&label, "label");

        r.read();

        current_event_handler->process(this);
      }

      /**
       * @param[out] o the stream to which to write the result
       **/
      void radio_button::write_structure(std::ostream& o) const {
        o << "<radio-button id=\"" << id << "\" next=\""
          << connection->id << "\" label=\"" << label
          << "\" selected=\"" << selected << "\"/>";
      }

      /**
       * @param[in] r the xml2pp text reader from which to read
       *
       * \pre reader should point to a radio-button element
       * \post reader points to after the associated end tag of the box
       * \todo connect to associated radio buttons
       **/
      void radio_button::read_structure(xml2pp::text_reader& r) {
        r.get_attribute(&label, "label");

        r.read();

        current_event_handler->process(this);
      }

      /**
       * @param[out] o the stream to which to write the result
       **/
      void checkbox::write_structure(std::ostream& o) const {
        o << "<checkbox id=\"" << id << "\""
          << " label=\"" << label << "\" status=\"" << status << "\"/>";
      }

      /**
       * @param[in] r the xml2pp text reader from which to read
       *
       * \pre reader should point to a radio-button element
       * \post reader points to after the associated end tag of the box
       * \todo connect to associated radio buttons
       **/
      void checkbox::read_structure(xml2pp::text_reader& r) {
        r.get_attribute(&label, "label");

        status = r.get_attribute("status");

        r.read();

        current_event_handler->process(this);
      }

      /**
       * @param[out] o the stream to which to write the result
       **/
      void progress_bar::write_structure(std::ostream& o) const {
        o << "<progress-bar id=\"" << id << "\" minimum=\""
          << minimum << "\" maximum=\"" << maximum
          << " current=\"" << current <<  "\"/>";
      }

      /**
       * @param[in] r the xml2pp text reader from which to read
       *
       * \pre reader should point to a progress-bar element
       * \post reader points to after the associated end tag of the box
       **/
      void progress_bar::read_structure(xml2pp::text_reader& r) {
        r.get_attribute(&minimum, "minimum");
        r.get_attribute(&maximum, "maximum");
        r.get_attribute(&current, "current");

        r.read();

        current_event_handler->process(this);
      }

      /**
       * @param[out] o the stream to which to write the result
       **/
      void text_field::write_structure(std::ostream& o) const {
        o << "<text-field id=\"" << id << "\" text=\"" << text << "\">";

        type->write(o);

        o << "<text-field/>";
      }

      /**
       * @param[in] r the xml2pp text reader from which to read
       *
       * \pre reader should point to a text-field element
       * \post reader points to after the associated end tag of the box
       **/
      void text_field::read_structure(xml2pp::text_reader& r) {
        r.get_attribute(&text, "text");

        r.read(1);

        current_event_handler->process(this);
      }
    }
  }
}

