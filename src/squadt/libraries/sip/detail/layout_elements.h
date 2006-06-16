#ifndef LAYOUT_ELEMENTS_H
#define LAYOUT_ELEMENTS_H

#include <iosfwd>

#include <sip/detail/layout_base.h>
#include <sip/detail/layout_mediator.h>
#include <sip/detail/basic_datatype.h>
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
          label();

          /** \brief Write out the layout structure in XML format */
          void read_structure(element::read_context&);
     
        public:
     
          /** \brief Constructor for a label */
          label(std::string);
     
          /** \brief Change the text */
          void set_text(std::string);
     
          /** \brief Change the text, and send update */
          void set_text(std::string, tool::communicator* t);
     
          /** \brief Instantiate a layout element, through a mediator */
          layout::mediator::wrapper_aptr instantiate(layout::mediator*);

          /** \brief Synchronise with instantiation that is part of a (G)UI */
          void update(layout::mediator*, layout::mediator::wrapper*) const;

          /** \brief Write out the layout structure in XML format */
          void write_structure(std::ostream&) const;
      };
     
      /** \brief A basic button widget */
      class button : public layout::element {
        friend class layout::element;
     
        private:
          /** \brief The caption */
          std::string label;
     
        private:
     
          /** \brief Default constructor */
          button();

          /** \brief Write out the layout structure in XML format */
          void read_structure(element::read_context&);
     
        public:
     
          /** \brief Constructor for a button */
          button(std::string);

          /** \brief Change the label */
          void set_label(std::string);
     
          /** \brief Change the label, and send update */
          void set_label(std::string, tool::communicator* t);
     
          /** \brief Instantiate a layout element, through a mediator */
          layout::mediator::wrapper_aptr instantiate(layout::mediator*);

          /** \brief Synchronise with instantiation that is part of a (G)UI */
          void update(layout::mediator*, layout::mediator::wrapper*) const;

          /** \brief Write out the layout structure in XML format */
          void write_structure(std::ostream&) const;
      };
     
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
          radio_button*        connection;

          /** \brief Whether the radio button is selected or not */
          bool                 selected;

          /** \brief Whether the radio button is the first in the group */
          bool                 first;
     
        private:
     
          /** \brief Default constructor */
          radio_button();

          /** \brief Write out the layout structure in XML format */
          void read_structure(element::read_context&);
     
        public:
     
          /** \brief Alternative constructor for a button */
          radio_button(std::string);

          /** \brief Alternative constructor for a button */
          radio_button(std::string, radio_button*, bool = false);

          /** \brief Set state of the radio button */
          void set_selected(bool);

          /** \brief Set state of the radio button, and send update */
          void set_selected(tool::communicator*);

          /** \brief Gets a pointer to the radio button in the group that is selected */
          radio_button const* get_selected() const;

          /** \brief Whether the radion button is selected or not */
          bool is_selected() const;
     
          /** \brief Whether the radio button is the first in a group */
          bool is_first_in_group() const;

          /** \brief Instantiate a layout element, through a mediator */
          layout::mediator::wrapper_aptr instantiate(layout::mediator*);

          /** \brief Synchronise with instantiation that is part of a (G)UI */
          void update(layout::mediator*, layout::mediator::wrapper*) const;

          /** \brief Write out the layout structure in XML format */
          void write_structure(std::ostream&) const;
      };
     
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
          checkbox();

          /** \brief Write out the layout structure in XML format */
          void read_structure(element::read_context&);
     
        public:
     
          /** \brief Alternative constructor for a checkbox */
          checkbox(std::string, bool);
     
          /** \brief Set the status */
          void set_status(bool);

          /** \brief Set the status, and send an update */
          void set_status(bool, tool::communicator* t);

          /** \brief Gets the status */
          bool get_status() const;
     
          /** \brief Instantiate a layout element, through a mediator */
          layout::mediator::wrapper_aptr instantiate(layout::mediator*);

          /** \brief Synchronise with instantiation that is part of a (G)UI */
          void update(layout::mediator*, layout::mediator::wrapper*) const;

          /** \brief Write out the layout structure in XML format */
          void write_structure(std::ostream&) const;
      };
     
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
          progress_bar();

          /** \brief Write out the layout structure in XML format */
          void read_structure(element::read_context&);
     
        public:
     
          /** \brief Constructor for a button */
          progress_bar(const unsigned int, const unsigned int, const unsigned int);

          /** \brief Sets the current value of the progess bar */
          void set_value(unsigned int);
     
          /** \brief Sets the current value of the progess bar, and sends an update */
          void set_value(unsigned int, tool::communicator*);
     
          /** \brief Sets the minimum value of the progess bar */
          void set_minimum(unsigned int);
     
          /** \brief Sets the minimum value of the progess bar , and sends an update*/
          void set_minimum(unsigned int, tool::communicator*);
     
          /** \brief Sets the minimum value of the progess bar */
          void set_maximum(unsigned int);
     
          /** \brief Sets the minimum value of the progess bar , and sends an update*/
          void set_maximum(unsigned int, tool::communicator*);
     
          /** \brief Gets the current value of the progess bar */
          unsigned int get_value() const;
     
          /** \brief Instantiate a layout element, through a mediator */
          layout::mediator::wrapper_aptr instantiate(layout::mediator*);

          /** \brief Synchronise with instantiation that is part of a (G)UI */
          void update(layout::mediator*, layout::mediator::wrapper*) const;

          /** \brief Write out the layout structure in XML format */
          void write_structure(std::ostream&) const;
      };
     
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
          text_field();

          /** \brief Write out the layout structure in XML format */
          void read_structure(element::read_context&);

        public:
     
          /** \brief Constructor */
          text_field(const std::string& i, basic_datatype::sptr& = string::standard);

          /** \brief Set the text */
          void set_text(std::string);
     
          /** \brief Set the text, and sends an update */
          void set_text(std::string, tool::communicator*);
     
          /** \brief Get the text */
          std::string get_text() const;
     
          /** \brief Instantiate a layout element, through a mediator */
          layout::mediator::wrapper_aptr instantiate(layout::mediator*);

          /** \brief Synchronise with instantiation that is part of a (G)UI */
          void update(layout::mediator*, layout::mediator::wrapper*) const;

          /** \brief Write out the layout structure in XML format */
          void write_structure(std::ostream&) const;
      };
    }
  }
}

#endif
