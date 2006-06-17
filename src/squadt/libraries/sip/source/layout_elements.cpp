#include <iostream>

#include <sip/detail/layout_elements.h>
#include <sip/detail/layout_base.h>
#include <sip/detail/event_handlers.h>

namespace sip {
  namespace layout {
    namespace elements {

      label::label() {
      }

      /**
       * @param[in] c the text of the label
       **/
      label::label(std::string c) : text(c) {
      }

      /**
       * @param[in] t the text of the label
       **/
      void label::set_text(std::string t) {
        text = t;
      }
     
      /**
       * @param[in] t the text of the label
       * @param[in] c the tool communicator to use for sending the update
       **/
      void label::set_text(std::string t, tool::communicator* c) {
        set_text(t);

        element::update(c, this);
      }
     
      /**
       * @param[in] m the mediator object to use
       **/
      layout::mediator::wrapper_aptr label::instantiate(layout::mediator* m) {
        return (m->build_label(this, text));
      }
     
      /**
       * @param[in] m the mediator object to use
       * @param[in] t pointer to the associated (G)UI object
       **/
      void label::update(layout::mediator* m, layout::mediator::wrapper* t) const {
        m->update_label(t, text);
      }
     
      /**
       * @param[out] o the stream to which to write the result
       **/
      void label::write_structure(std::ostream& o) const {
        o << "<label id=\"" << id << "\">" << text << "</label>";
      }

      /**
       * @param[in] r the read context with which to read
       *
       * \pre reader should point to a button element
       * \post reader points to after the associated end tag of the box
       **/
      void label::read_structure(read_context& r) {
        r.reader.next_element();

        if (!r.reader.is_empty_element() && !r.reader.is_element("label")) {
          r.reader.get_value(&text);

          r.reader.next_element();
        }

        r.reader.skip_end_element("label");

        current_event_handler->process(this);
      }

      button::button() {
      }

      /**
       * @param[in] c the label for the button
       **/
      button::button(std::string c) : label(c) {
      }
     
      /**
       * @param[in] l the label for the button
       **/
      void button::set_label(std::string l) {
        label = l;
      }

      /**
       * @param[in] l the label for the button
       * @param[in] t the tool communicator to use for sending the update
       **/
      void button::set_label(std::string l, tool::communicator* t) {
        set_label(l);

        element::update(t, this);
      }

      /**
       * @param[in] m the mediator object to use
       **/
      layout::mediator::wrapper_aptr button::instantiate(layout::mediator* m) {
        return (m->build_button(this, label));
      }
     
      /**
       * @param[in] m the mediator object to use
       * @param[in] t pointer to the associated (G)UI object
       **/
      void button::update(layout::mediator* m, layout::mediator::wrapper* t) const {
        m->update_button(t, label);
      }

      /**
       * @param[out] o the stream to which to write the result
       **/
      void button::write_structure(std::ostream& o) const {
        o << "<button id=\"" << id << "\" label=\"" << label << "\"/>";
      }

      /**
       * @param[in] r the read context with which to read
       *
       * \pre reader should point to a button element
       * \post reader points to after the associated end tag of the box
       **/
      void button::read_structure(read_context& r) {
        r.reader.get_attribute(&label, "label");

        r.reader.next_element();

        current_event_handler->process(this);
      }

      radio_button::radio_button() : connection(this), selected(true), first(false) {
      }

      /**
       * @param[in] c the label for the button
       **/
      radio_button::radio_button(std::string c) : label(c), connection(this), selected(true), first(true) {
      }

      /**
       * @param[in] c the label for the button
       * @param[in] r pointer to a connected radio button (may not be 0)
       * @param[in] s whether the button is selected or not
       **/
      radio_button::radio_button(std::string c, radio_button* r, bool s) :
                        label(c), selected(s), first(false) {

        radio_button* n = r;

        while (!n->connection->first) {
          n = n->connection;
        }

        connection = n->connection;
        n->connection = this;

        if (selected) {
          set_selected(true);
        }
      }
     
      /**
       * @param[in] b whether or not to unselect connected radio buttons
       **/
      void radio_button::set_selected(bool b) {
        for (radio_button* r = connection; r->connection != this; r = r->connection) {
          if (r->selected) {
            r->selected = false;

            break;
          }
        }

        selected = true;
      }

      radio_button const* radio_button::get_selected() const {
        radio_button const* r = this;

        while (!r->selected) {
          r = r->connection;
        }

        return (r);
      }

      bool radio_button::is_first_in_group() const {
        return (first);
      }

      /**
       * @param[in] t the tool communicator to use for sending the update
       **/
      void radio_button::set_selected(tool::communicator* t) {
        set_selected(true);

        element::update(t, this);
      }
     
      bool radio_button::is_selected() const {
        return (selected);
      }

      /**
       * @param[in] m the mediator object to use
       **/
      layout::mediator::wrapper_aptr radio_button::instantiate(layout::mediator* m) {
        return (m->build_radio_button(this, label, selected));
      }
     
      /**
       * @param[in] m the mediator object to use
       * @param[in] t pointer to the associated (G)UI object
       **/
      void radio_button::update(layout::mediator* m, layout::mediator::wrapper* t) const {
        m->update_radio_button(t, label, selected);
      }

      /**
       * @param[out] o the stream to which to write the result
       **/
      void radio_button::write_structure(std::ostream& o) const {
        o << "<radio-button id=\"" << id << "\" label=\"" << label
          << "\" connected=\"" << connection->id;

        if (selected) {
          o << "\" selected=\"" << selected;
        }
        if (first) {
          o << "\" first=\"true";
        }

        o << "\"/>";
      }

      /**
       * @param[in] r the read context with which to read
       *
       * \pre reader should point to a radio-button element
       * \post reader points to after the associated end tag of the box
       * \todo connect to associated radio buttons
       **/
      void radio_button::read_structure(read_context& r) {
        element::identifier connected_to = 0;

        r.reader.get_attribute(&label, "label");
        r.reader.get_attribute(&connected_to, "connected");

        first    = r.reader.get_attribute("first");
        selected = r.reader.get_attribute("selected");

        connection = static_cast < radio_button* > (r.element_for_id(connected_to));

        if (connection != 0) {
          for (radio_button* i = connection; i != this; i = i->connection) {
            i->connection = static_cast < radio_button* > (r.element_for_id(reinterpret_cast < element::identifier > (i->connection)));
          }
        }
        else {
          connection = reinterpret_cast < radio_button* > (connected_to);
        }

        r.reader.next_element();

        current_event_handler->process(this);
      }

      checkbox::checkbox() {
      }

      /**
       * @param[in] c the label for the button
       * @param[in] s the status of the checkbox
       **/
      checkbox::checkbox(std::string c, bool s) : label(c), status(s) {
      }

      /**
       * @param[in] b the new status
       **/
      void checkbox::set_status(bool b) {
        status = b;
      }

      /**
       * @param[in] b the new status
       * @param[in] t the tool communicator to use for sending the update
       **/
      void checkbox::set_status(bool b, tool::communicator* t) {
        set_status(b);

        element::update(t, this);
      }

      bool checkbox::get_status() const {
        return (status);
      }
     
      /**
       * @param[in] m the mediator object to use
       **/
      layout::mediator::wrapper_aptr checkbox::instantiate(layout::mediator* m){
        return (m->build_checkbox(this, label, status));
      }

      /**
       * @param[in] m the mediator object to use
       * @param[in] t pointer to the associated (G)UI object
       **/
      void checkbox::update(layout::mediator* m, layout::mediator::wrapper* t) const {
        m->update_checkbox(t, label, status);
      }

      /**
       * @param[out] o the stream to which to write the result
       **/
      void checkbox::write_structure(std::ostream& o) const {
        o << "<checkbox id=\"" << id << "\""
          << " label=\"" << label << "\" status=\"" << status << "\"/>";
      }

      /**
       * @param[in] r the read context with which to read
       *
       * \pre reader should point to a radio-button element
       * \post reader points to after the associated end tag of the box
       **/
      void checkbox::read_structure(read_context& r) {
        r.reader.get_attribute(&label, "label");

        status = r.reader.get_attribute("status");

        r.reader.next_element();

        current_event_handler->process(this);
      }

      progress_bar::progress_bar() {
      }

      /**
       * @param[in] min the minimum position
       * @param[in] max the maximum position
       * @param[in] c the current position
       **/
      progress_bar::progress_bar(const unsigned int min, const unsigned int max, const unsigned int c)
              : minimum(min), maximum(max), current(c) {
      }

      /**
       * @param[in] v the new value
       *
       * \pre minimum <= v <= maximum
       **/
      void progress_bar::set_value(unsigned int v) {
        current = v;
      }

      /**
       * @param[in] v the new value
       * @param[in] t the tool communicator to use for sending the update
       *
       * \pre minimum <= v <= maximum
       **/
      void progress_bar::set_value(unsigned int v, tool::communicator* t) {
        set_value(v);

        element::update(t, this);
      }

      /**
       * @param[in] v the new value
       **/
      void progress_bar::set_minimum(unsigned int v) {
        minimum = v;
      }

      /**
       * @param[in] v the new value
       * @param[in] t the tool communicator to use for sending the update
       **/
      void progress_bar::set_minimum(unsigned int v, tool::communicator* t) {
        set_minimum(v);

        element::update(t, this);
      }

      /**
       * @param[in] v the new value
       **/
      void progress_bar::set_maximum(unsigned int v) {
        maximum = v;
      }

      /**
       * @param[in] v the new value
       * @param[in] t the tool communicator to use for sending the update
       **/
      void progress_bar::set_maximum(unsigned int v, tool::communicator* t) {
        set_maximum(v);

        element::update(t, this);
      }

      unsigned int progress_bar::get_value() const {
        return (current);
      }
     
      /**
       * @param[in] m the mediator object to use
       **/
      layout::mediator::wrapper_aptr progress_bar::instantiate(layout::mediator* m) {
        return (m->build_progress_bar(this, minimum, maximum, current));
      }
     
      /**
       * @param[in] m the mediator object to use
       * @param[in] t pointer to the associated (G)UI object
       **/
      void progress_bar::update(layout::mediator* m, layout::mediator::wrapper* t) const {
        m->update_progress_bar(t, minimum, maximum, current);
      }

      /**
       * @param[out] o the stream to which to write the result
       **/
      void progress_bar::write_structure(std::ostream& o) const {
        o << "<progress-bar id=\"" << id << "\" minimum=\""
          << minimum << "\" maximum=\"" << maximum
          << "\" current=\"" << current <<  "\"/>";
      }

      /**
       * @param[in] r the read context with which to read
       *
       * \pre reader should point to a progress-bar element
       * \post reader points to after the associated end tag of the box
       **/
      void progress_bar::read_structure(read_context& r) {
        r.reader.get_attribute(&minimum, "minimum");
        r.reader.get_attribute(&maximum, "maximum");
        r.reader.get_attribute(&current, "current");

        r.reader.next_element();

        current_event_handler->process(this);
      }

      text_field::text_field() {
        type = string::standard;
      }

      /**
       * @param[in] s the initial content of the text control
       * @param[in] t the a type description object for validation purposes
       **/
      text_field::text_field(const std::string& s, basic_datatype::sptr& t) : text(s), type(t) {
      }
     
      /**
       * @param[in] s the new text
       **/
      void text_field::set_text(std::string s) {
        text = s;
      }

      /**
       * @param[in] s the new text
       * @param[in] t the tool communicator to use for sending the update
       **/
      void text_field::set_text(std::string s, tool::communicator* t) {
        set_text(s);

        element::update(t, this);
      }

      std::string text_field::get_text() const {
        return (text);
      }

      /**
       * @param[in] m the mediator object to use
       **/
      layout::mediator::wrapper_aptr text_field::instantiate(layout::mediator* m) {
        return (m->build_text_field(this, text));
      }

      /**
       * @param[in] m the mediator object to use
       * @param[in] t pointer to the associated (G)UI object
       **/
      void text_field::update(layout::mediator* m, layout::mediator::wrapper* t) const {
        m->update_text_field(t, text);
      }

      /**
       * @param[out] o the stream to which to write the result
       **/
      void text_field::write_structure(std::ostream& o) const {
        o << "<text-field id=\"" << id << "\">"
          << "<text>" << text << "</text>";

        type->write(o, text);

        o << "</text-field>";
      }

      /**
       * @param[in] r the read context with which to read
       *
       * \pre reader should point to a text-field element
       * \post reader points to after the associated end tag of the box
       **/
      void text_field::read_structure(read_context& r) {
        r.reader.next_element();

        if (r.reader.is_element("text")) {
          if (!r.reader.is_end_element()) {
            r.reader.next_element();

            if (!r.reader.is_end_element()) { 
              r.reader.get_value(&text);

              r.reader.next_element();
            }

            r.reader.skip_end_element("text");
          }
        }

        if (!r.reader.is_end_element("text")) {
          /* Assume datatype specification */
          type = basic_datatype::read(r.reader).first;
        }

        r.reader.skip_end_element("text-field");

        current_event_handler->process(this);
      }
    }
  }
}

