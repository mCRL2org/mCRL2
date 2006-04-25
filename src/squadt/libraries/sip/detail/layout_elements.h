#ifndef LAYOUT_WIDGETS_H
#define LAYOUT_WIDGETS_H

#include <sip/detail/layout_mediator.h>

namespace sip {
  namespace layout {
    namespace elements {

      using namespace sip::datatype;
     
      /** \brief A basic text label widget */
      class label : public layout::element {
     
        private:
     
          /** \brief The text to be displayed */
          std::string text;
     
        public:
     
          /** \brief Constructor for a label */
          inline label(std::string);
     
          /** \brief Instantiate a layout element, through a mediator */
          inline layout::mediator::wrapper_aptr instantiate(layout::mediator*);
      };
     
      /**
       * @param[in] c the text of the label
       **/
      inline label::label(std::string c) : text(c) {
      }
     
      /**
       * @param m the mediator object to use
       **/
      inline layout::mediator::wrapper_aptr label::instantiate(layout::mediator* m) {
        return (m->build_label(text));
      }
     
      /** \brief A basic button widget */
      class button : public layout::element {
        private:
          /** The caption */
          std::string label;
     
        public:
     
          /** \brief Constructor for a button */
          inline button(std::string);
     
          /** \brief Instantiate a layout element, through a mediator */
          inline layout::mediator::wrapper_aptr instantiate(layout::mediator*);
      };
     
      /**
       * @param[in] c the label for the button
       **/
      inline button::button(std::string c) : label(c) {
      }
     
      /**
       * @param m the mediator object to use
       **/
      inline layout::mediator::wrapper_aptr button::instantiate(layout::mediator* m) {
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
        private:
          /** The caption */
          std::string          label;
     
          /** The connection reference */
          const radio_button*  connection;
     
        public:
     
          /** \brief Constructor for a button */
          inline radio_button(std::string);
     
          /** \brief Alternative constructor for a button */
          inline radio_button(std::string, radio_button*);
     
          /** \brief Instantiate a layout element, through a mediator */
          inline layout::mediator::wrapper_aptr instantiate(layout::mediator*);
      };
     
      /**
       * @param[in] c the label for the button
       * @param[in] r pointer to a connected radio button
       **/
      inline radio_button::radio_button(std::string c, radio_button* r) : label(c), connection(r) {
      }
     
      /**
       * @param m the mediator object to use
       **/
      inline layout::mediator::wrapper_aptr radio_button::instantiate(layout::mediator* m) {
        return (m->build_radio_button(label));
      }
     
      /** \brief A basic button widget */
      class progress_bar : public layout::element {
     
        private:
          /** The minimum value */
          unsigned int minimum;
     
          /** The maximum value */
          unsigned int maximum;
     
          /** The current value */
          unsigned int current;
     
        public:
     
          /** \brief Constructor for a button */
          inline progress_bar(const unsigned int, const unsigned int, const unsigned int);
     
          /** \brief Instantiate a layout element, through a mediator */
          inline layout::mediator::wrapper_aptr instantiate(layout::mediator*);
      };
     
      /**
       * @param[in] min the minimum position
       * @param[in] max the maximum position
       * @param[in] c the current position
       **/
      inline progress_bar::progress_bar(const unsigned int min, const unsigned int max, const unsigned int c)
              : minimum(min), maximum(max), current(c) {
      }
     
      /**
       * @param m the mediator object to use
       **/
      inline layout::mediator::wrapper_aptr progress_bar::instantiate(layout::mediator* m) {
        return (m->build_progress_bar(minimum, maximum, current));
      }
     
      /**
       * \brief A text input field
       *
       * A datatype derived from basic_datatype can be specified for validation
       * purposes. By default any string is accepted.
       **/
      class text_field : public layout::element {
        private:
     
          /** \brief The text to be displayed initialy */
          std::string text;
     
          /** Type for validation purposes */
          basic_datatype* type;
     
        public:
     
          /** Constructor */
          inline text_field(const std::string& i, basic_datatype* = &standard_string);
     
          /** \brief Instantiate a layout element, through a mediator */
          inline layout::mediator::wrapper_aptr instantiate(layout::mediator*);
      };
     
      /**
       * @param[in] s the initial content of the text control
       * @param[in] t the a type description object for validation purposes
       **/
      inline text_field::text_field(const std::string& s, basic_datatype* t) : text(s), type(t) {
      }
     
      /**
       * @param m the mediator object to use
       **/
      inline layout::mediator::wrapper_aptr text_field::instantiate(layout::mediator* m) {
        return (m->build_text_field(text));
      }
    }
  }
}

#endif
