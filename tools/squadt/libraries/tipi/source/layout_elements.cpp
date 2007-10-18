//  Copyright 2007 Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file source/layout_elements.cpp

#include "tipi/layout_base.hpp"
#include "tipi/layout_elements.hpp"

namespace tipi {
  namespace layout {
    namespace elements {

      label::label() {
      }

      std::string label::get_text() const {
        return m_text;
      }

      /**
       * \param[in] t the text of the label
       **/
      label& label::set_text(std::string const& t) {
        m_text = t;

        activate_handlers();

        return *this;
      }
     
      /**
       * \param[in] m the mediator object to use
       **/
      layout::mediator::wrapper_aptr label::instantiate(layout::mediator* m) {
        return (m->build(*this));
      }
     
      button::button() {
        set_grow(false);
      }
     
      std::string button::get_label() const {
        return m_label;
      }

      /**
       * \param[in] l the label for the button
       **/
      button& button::set_label(std::string const& l) {
        m_label = l;

        activate_handlers();

        return *this;
      }

      void button::activate() {
        activate_handlers();
      }

      /**
       * \param[in] m the mediator object to use
       **/
      layout::mediator::wrapper_aptr button::instantiate(layout::mediator* m) {
        return (m->build(*this));
      }
     
      radio_button::radio_button() : m_connection(this), m_selected(true) {
      }

      /**
       * \param[in] r the button in the group to connect with
       **/
      radio_button& radio_button::connect(radio_button& r) {

        if (&r != this) {
          // disconnect from group if it contains more than one button
          if (r.m_connection != &r) {
            radio_button* n = r.m_connection;

            while (n->m_connection != &r) {
              n = n->m_connection;
            }

            n->m_connection = r.m_connection;

            if (r.m_selected) {
              n->m_selected = true;
            }
          }

          r.m_selected   = false;
          r.m_connection = m_connection;
          m_connection   = &r;
        }

        return *this;
      }

      radio_button& radio_button::connected_to() {
        return *m_connection;
      }

      radio_button const& radio_button::connected_to() const {
        return *m_connection;
      }

      /**
       * \param[in] l the new text of the label for the radio button
       **/
      radio_button& radio_button::set_label(std::string const& l) {
        m_label = l;

        return *this;
      }
     
      std::string radio_button::get_label() const {
        return (m_label);
      }
     
      /**
       * \param[in] b whether or not to send an event
       **/
      void radio_button::set_selected(bool b) {
        for (radio_button* r = m_connection; r != this; r = r->m_connection) {
          if (r->m_selected) {
            r->m_selected = false;
       
            break;
          }
        }
       
        m_selected = true;
       
        activate_handlers(b);
      }

      radio_button& radio_button::select() {
        if (!m_selected) {
          set_selected(true);
        }

        return *this;
      }

      radio_button const& radio_button::get_selected() const {
        radio_button const* r = this;

        while (!r->m_selected) {
          r = r->m_connection;
        }

        return *r;
      }

      bool radio_button::is_selected() const {
        return (m_selected);
      }

      /**
       * \param[in] m the mediator object to use
       **/
      layout::mediator::wrapper_aptr radio_button::instantiate(layout::mediator* m) {
        return (m->build(*this));
      }
     
      checkbox::checkbox() {
      }

      /**
       * \param[in] l the new text of the label for the radio button
       **/
      checkbox& checkbox::set_label(std::string const& l) {
        m_label = l;

        return *this;
      }
     
      std::string checkbox::get_label() const {
        return (m_label);
      }
     
      /**
       * \param[in] b the new status
       **/
      checkbox& checkbox::set_status(bool b) {
        m_status = b;

        activate_handlers();

        return *this;
      }

      bool checkbox::get_status() const {
        return (m_status);
      }
     
      /**
       * \param[in] m the mediator object to use
       **/
      layout::mediator::wrapper_aptr checkbox::instantiate(layout::mediator* m){
        return (m->build(*this));
      }

      progress_bar::progress_bar() {
      }

      /**
       * \param[in] v the new value
       *
       * \pre minimum <= v <= maximum
       **/
      progress_bar& progress_bar::set_value(unsigned int v) {
        m_current = v;

        activate_handlers();

        return *this;
      }

      unsigned int progress_bar::get_value() const {
        return (m_current);
      }

      /**
       * \param[in] v the new value
       **/
      progress_bar& progress_bar::set_minimum(unsigned int v) {
        m_minimum = v;

        activate_handlers();

        return *this;
      }

      unsigned int progress_bar::get_minimum() const {
        return (m_minimum);
      }
     
      /**
       * \param[in] v the new value
       **/
      progress_bar& progress_bar::set_maximum(unsigned int v) {
        m_maximum = v;

        activate_handlers();

        return *this;
      }

      unsigned int progress_bar::get_maximum() const {
        return (m_maximum);
      }
     
      /**
       * \param[in] m the mediator object to use
       **/
      layout::mediator::wrapper_aptr progress_bar::instantiate(layout::mediator* m) {
        return (m->build(*this));
      }

      text_field::text_field() : m_text(""), m_type(new tipi::datatype::string()) {
      }

      /**
       * \param[in] t the new type to validate against
       **/
      text_field& text_field::set_type(boost::shared_ptr < basic_datatype >& t) {
        m_type = t;

        activate_handlers();

        return *this;
      }

      /**
       * \param[in] s the new text
       **/
      text_field& text_field::set_text(std::string const& s) {
        m_text = s;

        activate_handlers();

        return *this;
      }

      std::string text_field::get_text() const {
        return (m_text);
      }

      /**
       * \param[in] m the mediator object to use
       **/
      layout::mediator::wrapper_aptr text_field::instantiate(layout::mediator* m) {
        return (m->build(*this));
      }
    }
  }
}

