#ifndef LAYOUT_WIDGETS_H
#define LAYOUT_WIDGETS_H

#include <iostream>

#include <sip/detail/layout_base.h>
#include <sip/detail/layout_mediator.h>
#include <sip/detail/common.h>

namespace sip {
  namespace layout {
    namespace elements {

      using namespace sip::datatype;
     
      /** \brief A basic text label widget */
      class label : public layout::element {
        friend class layout::element;
     
        private:
     
          /** \brief The text to be displayed */
          std::string text;

        private:
     
          /** \brief Default constructor */
          inline label();

          /** \brief Write out the layout structure in XML format */
          void read_structure(xml2pp::text_reader&);
     
        public:
     
          /** \brief Constructor for a label */
          inline label(std::string);
     
          /** \brief Instantiate a layout element, through a mediator */
          inline layout::mediator::wrapper_aptr instantiate(layout::mediator*);

          /** \brief Synchronise with instantiation that is part of a (G)UI */
          inline void update(layout::mediator*, layout::mediator::wrapper*) const;

          /** \brief Write out the layout structure in XML format */
          void write_structure(std::ostream&) const;
      };
     
      inline label::label() {
      }

      /**
       * @param[in] c the text of the label
       **/
      inline label::label(std::string c) : text(c) {
      }

      /**
       * @param[in] m the mediator object to use
       **/
      inline layout::mediator::wrapper_aptr label::instantiate(layout::mediator* m) {
        return (m->build_label(this, text));
      }
     
      /**
       * @param[in] m the mediator object to use
       * @param[in] t pointer to the associated (G)UI object
       **/
      inline void label::update(layout::mediator* m, layout::mediator::wrapper* t) const {
        m->update_label(t, text);
      }
     
      /** \brief A basic button widget */
      class button : public layout::element {
        friend class layout::element;
     
        private:
          /** \brief The caption */
          std::string label;
     
        private:
     
          /** \brief Default constructor */
          inline button();

          /** \brief Write out the layout structure in XML format */
          void read_structure(xml2pp::text_reader&);
     
        public:
     
          /** \brief Constructor for a button */
          inline button(std::string);
     
          /** \brief Instantiate a layout element, through a mediator */
          inline layout::mediator::wrapper_aptr instantiate(layout::mediator*);

          /** \brief Synchronise with instantiation that is part of a (G)UI */
          inline void update(layout::mediator*, layout::mediator::wrapper*) const;

          /** \brief Write out the layout structure in XML format */
          void write_structure(std::ostream&) const;
      };
     
      inline button::button() {
      }

      /**
       * @param[in] c the label for the button
       **/
      inline button::button(std::string c) : label(c) {
      }
     
      /**
       * @param[in] m the mediator object to use
       **/
      inline layout::mediator::wrapper_aptr button::instantiate(layout::mediator* m) {
        return (m->build_button(this, label));
      }
     
      /**
       * @param[in] m the mediator object to use
       * @param[in] t pointer to the associated (G)UI object
       **/
      inline void button::update(layout::mediator* m, layout::mediator::wrapper* t) const {
        m->update_button(t, label);
      }

      /**
       * \brief A basic radio button widget
       *
       * Connected radio buttons allow at most one button to be pressed.  A radio
       * button p is connected to a radio button q if and only if :
       *  - q is connected to p, or
       *  - p has a connection reference to q, or
       *  - there exists a radio button r such that :
       *    - p is connected to r and
       *    - r is connected to q
       **/
      class radio_button : public layout::element {
        friend class layout::element;
     
        private:
          /** \brief The caption */
          std::string          label;
     
          /** \brief The connection reference */
          radio_button* const  connection;

          /** \brief Whether the radio button is selected or not */
          bool                 selected;
     
        private:
     
          /** \brief Default constructor */
          inline radio_button();

          /** \brief Write out the layout structure in XML format */
          void read_structure(xml2pp::text_reader&);
     
        public:
     
          /** \brief Alternative constructor for a button */
          inline radio_button(std::string, radio_button* = 0, bool = false);

          /** \brief Whether the button is selected or not */
          inline void set_selected(bool, bool = true);

          /** \brief Whether the radion button is selected or not */
          inline bool is_selected() const;
     
          /** \brief Instantiate a layout element, through a mediator */
          inline layout::mediator::wrapper_aptr instantiate(layout::mediator*);

          /** \brief Synchronise with instantiation that is part of a (G)UI */
          inline void update(layout::mediator*, layout::mediator::wrapper*) const;

          /** \brief Write out the layout structure in XML format */
          void write_structure(std::ostream&) const;
      };
     
      inline radio_button::radio_button() : connection(0) {
      }

      /**
       * @param[in] c the label for the button
       * @param[in] r pointer to a connected radio button
       * @param[in] s whether the button is selected or not
       **/
      inline radio_button::radio_button(std::string c, radio_button* r, bool s) : label(c), connection(r), selected(s) {
      }
     
      /**
       * @param[in] s the label for the button
       * @param[in] b whether or not to unselect connected radio buttons
       **/
      inline void radio_button::set_selected(bool s, bool b) {
        radio_button* temporary = connection;

        while (connection != 0 && connection != this) {
          temporary->set_selected(false, false);

          temporary = temporary->connection;
        }

        selected = true;
      }
     
      inline bool radio_button::is_selected() const {
        return (selected);
      }

      /**
       * @param[in] m the mediator object to use
       **/
      inline layout::mediator::wrapper_aptr radio_button::instantiate(layout::mediator* m) {
        return (m->build_radio_button(this, label, selected));
      }
     
      /**
       * @param[in] m the mediator object to use
       * @param[in] t pointer to the associated (G)UI object
       **/
      inline void radio_button::update(layout::mediator* m, layout::mediator::wrapper* t) const {
        m->update_radio_button(t, label, selected);
      }

      /**
       * \brief A checkbox widget
       **/
      class checkbox : public layout::element {
        friend class layout::element;
     
        private:

          /** \brief The caption */
          std::string          label;

          /** \brief The status of the checkbox */
          bool                 status;
     
        private:
     
          /** \brief Default constructor */
          inline checkbox();

          /** \brief Write out the layout structure in XML format */
          void read_structure(xml2pp::text_reader&);
     
        public:
     
          /** \brief Alternative constructor for a checkbox */
          inline checkbox(std::string, bool);
     
          /** \brief Set the status */
          inline void set_status(bool);

          /** \brief Gets the status */
          inline bool get_status() const;
     
          /** \brief Instantiate a layout element, through a mediator */
          inline layout::mediator::wrapper_aptr instantiate(layout::mediator*);

          /** \brief Synchronise with instantiation that is part of a (G)UI */
          inline void update(layout::mediator*, layout::mediator::wrapper*) const;

          /** \brief Write out the layout structure in XML format */
          void write_structure(std::ostream&) const;
      };
     
      inline checkbox::checkbox() {
      }

      /**
       * @param[in] c the label for the button
       * @param[in] s the status of the checkbox
       **/
      inline checkbox::checkbox(std::string c, bool s) : label(c), status(s) {
      }

      /**
       * @param[in] b the new status
       **/
      inline void checkbox::set_status(bool b) {
        status = b;
      }

      inline bool checkbox::get_status() const {
        return (status);
      }
     
      /**
       * @param[in] m the mediator object to use
       **/
      inline layout::mediator::wrapper_aptr checkbox::instantiate(layout::mediator* m){
        return (m->build_checkbox(this, label, status));
      }

      /**
       * @param[in] m the mediator object to use
       * @param[in] t pointer to the associated (G)UI object
       **/
      inline void checkbox::update(layout::mediator* m, layout::mediator::wrapper* t) const {
        m->update_checkbox(t, label, status);
      }

      /** \brief A basic button widget */
      class progress_bar : public layout::element {
        friend class layout::element;
     
        private:

          /** \brief The minimum value */
          unsigned int minimum;
     
          /** \brief The maximum value */
          unsigned int maximum;
     
          /** \brief The current value */
          unsigned int current;
     
        private:
     
          /** \brief Default constructor */
          inline progress_bar();

          /** \brief Write out the layout structure in XML format */
          void read_structure(xml2pp::text_reader&);
     
        public:
     
          /** \brief Constructor for a button */
          inline progress_bar(const unsigned int, const unsigned int, const unsigned int);

          /** \brief Gets the current value of the progess bar */
          inline unsigned int get_value() const;
     
          /** \brief Instantiate a layout element, through a mediator */
          inline layout::mediator::wrapper_aptr instantiate(layout::mediator*);

          /** \brief Synchronise with instantiation that is part of a (G)UI */
          inline void update(layout::mediator*, layout::mediator::wrapper*) const;

          /** \brief Write out the layout structure in XML format */
          void write_structure(std::ostream&) const;
      };
     
      inline progress_bar::progress_bar() {
      }

      /**
       * @param[in] min the minimum position
       * @param[in] max the maximum position
       * @param[in] c the current position
       **/
      inline progress_bar::progress_bar(const unsigned int min, const unsigned int max, const unsigned int c)
              : minimum(min), maximum(max), current(c) {
      }

      inline unsigned int progress_bar::get_value() const {
        return (current);
      }
     
      /**
       * @param[in] m the mediator object to use
       **/
      inline layout::mediator::wrapper_aptr progress_bar::instantiate(layout::mediator* m) {
        return (m->build_progress_bar(this, minimum, maximum, current));
      }
     
      /**
       * @param[in] m the mediator object to use
       * @param[in] t pointer to the associated (G)UI object
       **/
      inline void progress_bar::update(layout::mediator* m, layout::mediator::wrapper* t) const {
        m->update_progress_bar(t, minimum, maximum, current);
      }

      /**
       * \brief A text input field
       *
       * A datatype derived from basic_datatype can be specified for validation
       * purposes. By default any string is accepted.
       **/
      class text_field : public layout::element {
        friend class layout::element;
     
        private:
     
          /** \brief The text to be displayed initialy */
          std::string text;
     
          /** \brief Type for validation purposes */
          basic_datatype::sptr type;
     
        private:
     
          /** \brief Default constructor */
          inline text_field();

          /** \brief Write out the layout structure in XML format */
          void read_structure(xml2pp::text_reader&);

        public:
     
          /** \brief Constructor */
          inline text_field(const std::string& i, basic_datatype::sptr);

          /** \brief Constructor */
          inline text_field(const std::string& i, basic_datatype::sptr& = standard_string);

          /** \brief Set the text */
          inline void set_text(std::string);
     
          /** \brief Get the text */
          inline std::string get_text() const;
     
          /** \brief Instantiate a layout element, through a mediator */
          inline layout::mediator::wrapper_aptr instantiate(layout::mediator*);

          /** \brief Synchronise with instantiation that is part of a (G)UI */
          inline void update(layout::mediator*, layout::mediator::wrapper*) const;

          /** \brief Write out the layout structure in XML format */
          void write_structure(std::ostream&) const;
      };
     
      inline text_field::text_field() {
        type = standard_string;
      }

      /**
       * @param[in] s the initial content of the text control
       * @param[in] t the a type description object for validation purposes
       **/
      inline text_field::text_field(const std::string& s, basic_datatype::sptr& t) : text(s), type(t) {
      }
     
      /**
       * @param[in] s the initial content of the text control
       * @param[in] t the a type description object for validation purposes
       **/
      inline text_field::text_field(const std::string& s, basic_datatype::sptr t) : text(s), type(t) {
      }
     
      /**
       * @param[in] s the new text
       **/
      inline void text_field::set_text(std::string s) {
        text = s;
      }

      inline std::string text_field::get_text() const {
        return (text);
      }

      /**
       * @param[in] m the mediator object to use
       **/
      inline layout::mediator::wrapper_aptr text_field::instantiate(layout::mediator* m) {
        return (m->build_text_field(this, text));
      }

      /**
       * @param[in] m the mediator object to use
       * @param[in] t pointer to the associated (G)UI object
       **/
      inline void text_field::update(layout::mediator* m, layout::mediator::wrapper* t) const {
        m->update_text_field(t, text);
      }
    }
  }
}

#endif
