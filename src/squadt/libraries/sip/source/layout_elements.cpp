#include <sip/layout_base.h>
#include <sip/detail/layout_elements.h>

namespace sip {
  namespace layout {
    namespace elements {

      label::label() {
      }

      /**
       * \param[in] c the text of the label
       **/
      label::label(std::string const& c) : m_text(c) {
      }

      /**
       * \param[in] t the text of the label
       **/
      void label::set_text(std::string const& t) {
        m_text = t;
      }
     
      /**
       * \param[in] t the text of the label
       * \param[in] c the tool communicator to use for sending the update
       **/
      void label::set_text(std::string const& t, tool::communicator* c) {
        set_text(t);

        element::update(c, this);
      }
     
      /**
       * \param[in] m the mediator object to use
       **/
      layout::mediator::wrapper_aptr label::instantiate(layout::mediator* m) {
        return (m->build_label(this, m_text));
      }
     
      /**
       * \param[in] m the mediator object to use
       * \param[in] t pointer to the associated (G)UI object
       **/
      void label::update(layout::mediator* m, layout::mediator::wrapper* t) const {
        m->update_label(t, m_text);
      }

      button::button() {
        set_grow(false);
      }

      /**
       * \param[in] c the label for the button
       **/
      button::button(std::string const& c) : m_label(c) {
        set_grow(false);
      }
     
      /**
       * \param[in] l the label for the button
       **/
      void button::set_label(std::string const& l) {
        m_label = l;
      }

      /**
       * \param[in] l the label for the button
       * \param[in] t the tool communicator to use for sending the update
       **/
      void button::set_label(std::string const& l, tool::communicator* t) {
        set_label(l);

        element::update(t, this);
      }

      /**
       * \param[in] m the mediator object to use
       **/
      layout::mediator::wrapper_aptr button::instantiate(layout::mediator* m) {
        return (m->build_button(this, m_label));
      }
     
      /**
       * \param[in] m the mediator object to use
       * \param[in] t pointer to the associated (G)UI object
       **/
      void button::update(layout::mediator* m, layout::mediator::wrapper* t) const {
        m->update_button(t, m_label);
      }

      radio_button::radio_button() : m_connection(this), m_selected(true), m_first(false) {
      }

      /**
       * \param[in] c the label for the button
       **/
      radio_button::radio_button(std::string const& c) : m_label(c), m_connection(this), m_selected(true), m_first(true) {
      }

      /**
       * \param[in] c the label for the button
       * \param[in] r pointer to a connected radio button (may not be 0)
       * \param[in] s whether the button is selected or not
       **/
      radio_button::radio_button(std::string const& c, radio_button* r, bool s) :
                        m_label(c), m_selected(s), m_first(false) {

        radio_button* n = r;

        while (!n->m_connection->m_first) {
          n = n->m_connection;
        }

        m_connection = n->m_connection;
        n->m_connection = this;

        if (m_selected) {
          set_selected(true);
        }
      }

      std::string radio_button::get_label() const {
        return (m_label);
      }
     
      /**
       * \param[in] b whether or not to unselect connected radio buttons
       **/
      void radio_button::set_selected(bool b) {
        for (radio_button* r = m_connection; r != this; r = r->m_connection) {
          if (r->m_selected) {
            r->m_selected = false;

            break;
          }
        }

        m_selected = true;
      }

      radio_button const* radio_button::get_selected() const {
        radio_button const* r = this;

        while (!r->m_selected) {
          r = r->m_connection;
        }

        return (r);
      }

      bool radio_button::is_first_in_group() const {
        return (m_first);
      }

      /**
       * \param[in] t the tool communicator to use for sending the update
       **/
      void radio_button::set_selected(tool::communicator* t) {
        set_selected(true);

        element::update(t, this);
      }
     
      bool radio_button::is_selected() const {
        return (m_selected);
      }

      /**
       * \param[in] m the mediator object to use
       **/
      layout::mediator::wrapper_aptr radio_button::instantiate(layout::mediator* m) {
        return (m->build_radio_button(this, m_label, m_selected));
      }
     
      /**
       * \param[in] m the mediator object to use
       * \param[in] t pointer to the associated (G)UI object
       **/
      void radio_button::update(layout::mediator* m, layout::mediator::wrapper* t) const {
        m->update_radio_button(t, m_label, m_selected);
      }

      checkbox::checkbox() {
      }

      /**
       * \param[in] c the label for the button
       * \param[in] s the status of the checkbox
       **/
      checkbox::checkbox(std::string const& c, bool s) : m_label(c), m_status(s) {
      }

      /**
       * \param[in] b the new status
       **/
      void checkbox::set_status(bool b) {
        m_status = b;
      }

      /**
       * \param[in] b the new status
       * \param[in] t the tool communicator to use for sending the update
       **/
      void checkbox::set_status(bool b, tool::communicator* t) {
        set_status(b);

        element::update(t, this);
      }

      bool checkbox::get_status() const {
        return (m_status);
      }
     
      /**
       * \param[in] m the mediator object to use
       **/
      layout::mediator::wrapper_aptr checkbox::instantiate(layout::mediator* m){
        return (m->build_checkbox(this, m_label, m_status));
      }

      /**
       * \param[in] m the mediator object to use
       * \param[in] t pointer to the associated (G)UI object
       **/
      void checkbox::update(layout::mediator* m, layout::mediator::wrapper* t) const {
        m->update_checkbox(t, m_label, m_status);
      }

      progress_bar::progress_bar() {
      }

      /**
       * \param[in] min the minimum position
       * \param[in] max the maximum position
       * \param[in] c the current position
       **/
      progress_bar::progress_bar(const unsigned int min, const unsigned int max, const unsigned int c)
              : m_minimum(min), m_maximum(max), m_current(c) {
      }

      /**
       * \param[in] v the new value
       *
       * \pre minimum <= v <= maximum
       **/
      void progress_bar::set_value(unsigned int v) {
        m_current = v;
      }

      /**
       * \param[in] v the new value
       * \param[in] t the tool communicator to use for sending the update
       *
       * \pre minimum <= v <= maximum
       **/
      void progress_bar::set_value(unsigned int v, tool::communicator* t) {
        set_value(v);

        element::update(t, this);
      }

      /**
       * \param[in] v the new value
       **/
      void progress_bar::set_minimum(unsigned int v) {
        m_minimum = v;
      }

      /**
       * \param[in] v the new value
       * \param[in] t the tool communicator to use for sending the update
       **/
      void progress_bar::set_minimum(unsigned int v, tool::communicator* t) {
        set_minimum(v);

        element::update(t, this);
      }

      /**
       * \param[in] v the new value
       **/
      void progress_bar::set_maximum(unsigned int v) {
        m_maximum = v;
      }

      /**
       * \param[in] v the new value
       * \param[in] t the tool communicator to use for sending the update
       **/
      void progress_bar::set_maximum(unsigned int v, tool::communicator* t) {
        set_maximum(v);

        element::update(t, this);
      }

      unsigned int progress_bar::get_value() const {
        return (m_current);
      }
     
      /**
       * \param[in] m the mediator object to use
       **/
      layout::mediator::wrapper_aptr progress_bar::instantiate(layout::mediator* m) {
        return (m->build_progress_bar(this, m_minimum, m_maximum, m_current));
      }
     
      /**
       * \param[in] m the mediator object to use
       * \param[in] t pointer to the associated (G)UI object
       **/
      void progress_bar::update(layout::mediator* m, layout::mediator::wrapper* t) const {
        m->update_progress_bar(t, m_minimum, m_maximum, m_current);
      }

      text_field::text_field() : m_type(new sip::datatype::string()) {
      }

      text_field::text_field(std::string const& s) : m_text(s), m_type(new sip::datatype::string()) {
      }

      /**
       * \param[in] s the initial content of the text control
       * \param[in] t the a type description object for validation purposes
       **/
      text_field::text_field(std::string const& s, basic_datatype::sptr& t) : m_text(s), m_type(t) {
      }
     
      /**
       * \param[in] s the new text
       **/
      void text_field::set_text(std::string const& s) {
        m_text = s;
      }

      /**
       * \param[in] s the new text
       * \param[in] t the tool communicator to use for sending the update
       **/
      void text_field::set_text(std::string const& s, tool::communicator* t) {
        set_text(s);

        element::update(t, this);
      }

      std::string text_field::get_text() const {
        return (m_text);
      }

      /**
       * \param[in] m the mediator object to use
       **/
      layout::mediator::wrapper_aptr text_field::instantiate(layout::mediator* m) {
        return (m->build_text_field(this, m_text));
      }

      /**
       * \param[in] m the mediator object to use
       * \param[in] t pointer to the associated (G)UI object
       **/
      void text_field::update(layout::mediator* m, layout::mediator::wrapper* t) const {
        m->update_text_field(t, m_text);
      }
    }
  }
}

