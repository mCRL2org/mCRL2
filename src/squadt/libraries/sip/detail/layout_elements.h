#ifndef LAYOUT_WIDGETS_H
#define LAYOUT_WIDGETS_H

#include <iostream>

#include <sip/detail/layout_base.h>
#include <sip/detail/layout_mediator.h>

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
     
          /** Default constructor */
          inline label();

          /** \brief Write out the layout structure in XML format */
          void read_structure(xml2pp::text_reader&);
     
        public:
     
          /** \brief Constructor for a label */
          inline label(std::string);
     
          /** \brief Instantiate a layout element, through a mediator */
          inline layout::mediator::wrapper_aptr instantiate(layout::mediator*) const;

          /** \brief Write out the layout structure in XML format */
          inline void write_structure(std::ostream&);
      };
     
      inline label::label() {
      }

      /**
       * @param[in] c the text of the label
       **/
      inline label::label(std::string c) : text(c) {
      }

      /**
       * @param[out] o the stream to which to write the result
       **/
      inline void label::write_structure(std::ostream& o) {
        o << "<label id=\"" << id << "\" text=\"" << text << "\"/>";
      }

      /**
       * @param[in] o the xml2pp text reader from which to read
       *
       * \pre reader should point to a button element
       * \post reader points to after the associated end tag of the box
       **/
      inline void label::read_structure(xml2pp::text_reader& r) {
        r.get_attribute(&text, "text");

        r.read();
      }

      /**
       * @param m the mediator object to use
       **/
      inline layout::mediator::wrapper_aptr label::instantiate(layout::mediator* m) const {
        return (m->build_label(text));
      }
     
      /** \brief A basic button widget */
      class button : public layout::element {
        friend class layout::element;
     
        private:
          /** The caption */
          std::string label;
     
        private:
     
          /** Default constructor */
          inline button();

          /** \brief Write out the layout structure in XML format */
          void read_structure(xml2pp::text_reader&);
     
        public:
     
          /** \brief Constructor for a button */
          inline button(std::string);
     
          /** \brief Instantiate a layout element, through a mediator */
          inline layout::mediator::wrapper_aptr instantiate(layout::mediator*) const;

          /** \brief Write out the layout structure in XML format */
          inline void write_structure(std::ostream&);
      };
     
      inline button::button() {
      }

      /**
       * @param[in] c the label for the button
       **/
      inline button::button(std::string c) : label(c) {
      }
     
      /**
       * @param[out] o the stream to which to write the result
       **/
      inline void button::write_structure(std::ostream& o) {
        o << "<button id=\"" << id << "\" label=\"" << label << "\"/>";
      }

      /**
       * @param[in] o the xml2pp text reader from which to read
       *
       * \pre reader should point to a button element
       * \post reader points to after the associated end tag of the box
       **/
      inline void button::read_structure(xml2pp::text_reader& r) {
        r.get_attribute(&label, "label");

        r.read();
      }

      /**
       * @param m the mediator object to use
       **/
      inline layout::mediator::wrapper_aptr button::instantiate(layout::mediator* m) const {
        return (m->build_button(label));
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
          /** The caption */
          std::string          label;
     
          /** The connection reference */
          const radio_button*  connection;
     
        private:
     
          /** Default constructor */
          inline radio_button();

          /** \brief Write out the layout structure in XML format */
          void read_structure(xml2pp::text_reader&);
     
        public:
     
          /** \brief Constructor for a button */
          inline radio_button(std::string);
     
          /** \brief Alternative constructor for a button */
          inline radio_button(std::string, radio_button*);
     
          /** \brief Instantiate a layout element, through a mediator */
          inline layout::mediator::wrapper_aptr instantiate(layout::mediator*) const;

          /** \brief Write out the layout structure in XML format */
          inline void write_structure(std::ostream&);
      };
     
      inline radio_button::radio_button() {
      }

      /**
       * @param[in] c the label for the button
       **/
      inline radio_button::radio_button(std::string c) : label(c) {
      }
     
      /**
       * @param[in] c the label for the button
       * @param[in] r pointer to a connected radio button
       **/
      inline radio_button::radio_button(std::string c, radio_button* r) : label(c), connection(r) {
      }
     
      /**
       * @param[out] o the stream to which to write the result
       **/
      inline void radio_button::write_structure(std::ostream& o) {
        o << "<radio-button id=\"" << id << "\" next=\""
          << connection->id << "\" label=\"" << label << "\"/>";
      }

      /**
       * @param[in] o the xml2pp text reader from which to read
       *
       * \pre reader should point to a radio-button element
       * \post reader points to after the associated end tag of the box
       * \todo connect to associated radio buttons
       **/
      inline void radio_button::read_structure(xml2pp::text_reader& r) {
        r.get_attribute(&label, "text");

        r.read();
      }

      /**
       * @param m the mediator object to use
       **/
      inline layout::mediator::wrapper_aptr radio_button::instantiate(layout::mediator* m) const {
        return (m->build_radio_button(label));
      }
     
      /** \brief A basic button widget */
      class progress_bar : public layout::element {
        friend class layout::element;
     
        private:

          /** The minimum value */
          unsigned int minimum;
     
          /** The maximum value */
          unsigned int maximum;
     
          /** The current value */
          unsigned int current;
     
        private:
     
          /** Default constructor */
          inline progress_bar();

          /** \brief Write out the layout structure in XML format */
          void read_structure(xml2pp::text_reader&);
     
        public:
     
          /** \brief Constructor for a button */
          inline progress_bar(const unsigned int, const unsigned int, const unsigned int);
     
          /** \brief Instantiate a layout element, through a mediator */
          inline layout::mediator::wrapper_aptr instantiate(layout::mediator*) const;

          /** \brief Write out the layout structure in XML format */
          inline void write_structure(std::ostream&);
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
     
      /**
       * @param[out] o the stream to which to write the result
       **/
      inline void progress_bar::write_structure(std::ostream& o) {
        o << "<progress-bar id=\"" << id << "\" minimum=\""
          << minimum << "\" maximum=\"" << maximum
          << " current=\"" << current <<  "\"/>";
      }

      /**
       * @param[in] o the xml2pp text reader from which to read
       *
       * \pre reader should point to a progress-bar element
       * \post reader points to after the associated end tag of the box
       **/
      inline void progress_bar::read_structure(xml2pp::text_reader& r) {
        r.get_attribute(&minimum, "minimum");
        r.get_attribute(&maximum, "maximum");
        r.get_attribute(&current, "current");

        r.read();
      }

      /**
       * @param m the mediator object to use
       **/
      inline layout::mediator::wrapper_aptr progress_bar::instantiate(layout::mediator* m) const {
        return (m->build_progress_bar(minimum, maximum, current));
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
     
          /** Type for validation purposes */
          basic_datatype* type;
     
        private:
     
          /** Default constructor */
          inline text_field();

          /** \brief Write out the layout structure in XML format */
          void read_structure(xml2pp::text_reader&);
     
        public:
     
          /** Constructor */
          inline text_field(const std::string& i, basic_datatype* = &standard_string);
     
          /** \brief Instantiate a layout element, through a mediator */
          inline layout::mediator::wrapper_aptr instantiate(layout::mediator*) const;

          /** \brief Write out the layout structure in XML format */
          inline void write_structure(std::ostream&);
      };
     
      inline text_field::text_field() {
      }

      /**
       * @param[in] s the initial content of the text control
       * @param[in] t the a type description object for validation purposes
       **/
      inline text_field::text_field(const std::string& s, basic_datatype* t) : text(s), type(t) {
      }
     
      /**
       * @param[out] o the stream to which to write the result
       **/
      inline void text_field::write_structure(std::ostream& o) {
        o << "<text-field id=\"" << id << "\" text=\"" << text << "\">";

        type->write(o);

        o << "<text-field/>";
      }

      /**
       * @param[in] o the xml2pp text reader from which to read
       *
       * \pre reader should point to a text-field element
       * \post reader points to after the associated end tag of the box
       **/
      inline void text_field::read_structure(xml2pp::text_reader& r) {
        r.get_attribute(&text, "text");

        r.read(1);
      }

      /**
       * @param m the mediator object to use
       **/
      inline layout::mediator::wrapper_aptr text_field::instantiate(layout::mediator* m) const {
        return (m->build_text_field(text));
      }
    }
  }
}

#endif
