#ifndef LAYOUT_ELEMENTS_H
#define LAYOUT_ELEMENTS_H

#include <iosfwd>

#include <sip/layout_base.h>
#include <sip/detail/layout_mediator.h>
#include <sip/detail/basic_datatype.h>
#include <sip/detail/common.h>

namespace sip {
  namespace layout {
    namespace elements {

      using namespace sip::datatype;
     
      /** \brief A basic text label widget */
      class label : public layout::element_impl< label > {
        friend class sip::store_visitor_impl;
        friend class sip::restore_visitor_impl;
        friend class layout::element;
     
        private:
     
          /** \brief The text to be displayed */
          std::string m_text;

        private:
     
          /** \brief Default constructor */
          label();
     
        public:
     
          /** \brief Constructor for a label */
          label(std::string const&);
     
          /** \brief Change the text */
          void set_text(std::string const&);
     
          /** \brief Change the text, and send update */
          void set_text(std::string const&, tool::communicator* t);
     
          /** \brief Instantiate a layout element, through a mediator */
          layout::mediator::wrapper_aptr instantiate(layout::mediator*);

          /** \brief Synchronise with instantiation that is part of a (G)UI */
          void update(layout::mediator*, layout::mediator::wrapper*) const;
      };
     
      /** \brief A basic button widget */
      class button : public layout::element_impl< button > {
        friend class sip::store_visitor_impl;
        friend class sip::restore_visitor_impl;
        friend class layout::element;
     
        private:

          /** \brief The caption */
          std::string m_label;
     
        private:
     
          /** \brief Default constructor */
          button();
     
        public:
     
          /** \brief Constructor for a button */
          button(std::string const&);

          /** \brief Change the label */
          void set_label(std::string const&);
     
          /** \brief Change the label, and send update */
          void set_label(std::string const&, tool::communicator* t);
     
          /** \brief Instantiate a layout element, through a mediator */
          layout::mediator::wrapper_aptr instantiate(layout::mediator*);

          /** \brief Synchronise with instantiation that is part of a (G)UI */
          void update(layout::mediator*, layout::mediator::wrapper*) const;
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
      class radio_button : public layout::element_impl< radio_button > {
        friend class sip::store_visitor_impl;
        friend class sip::restore_visitor_impl;
        friend class layout::element;
     
        private:
          /** \brief The caption */
          std::string          m_label;
     
          /** \brief The connection reference */
          radio_button*        m_connection;

          /** \brief Whether the radio button is selected or not */
          bool                 m_selected;

          /** \brief Whether the radio button is the first in the group */
          bool                 m_first;
     
        private:
     
          /** \brief Default constructor */
          radio_button();
     
        public:
     
          /** \brief Alternative constructor for a radio button */
          radio_button(std::string const&);

          /** \brief Alternative constructor for a radio button */
          radio_button(std::string const&, radio_button*, bool = false);

          /** \brief Set state of the radio button */
          void set_selected(bool);

          /** \brief Set state of the radio button, and send update */
          void set_selected(tool::communicator*);

          /** \brief Returns the current label */
          std::string get_label() const;

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
      };
     
      /**
       * \brief A checkbox widget
       **/
      class checkbox : public layout::element_impl< checkbox > {
        friend class sip::store_visitor_impl;
        friend class sip::restore_visitor_impl;
        friend class layout::element;
     
        private:

          /** \brief The caption */
          std::string          m_label;

          /** \brief The status of the checkbox */
          bool                 m_status;
     
        private:
     
          /** \brief Default constructor */
          checkbox();

        public:
     
          /** \brief Alternative constructor for a checkbox */
          checkbox(std::string const&, bool = false);
     
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
      };
     
      /** \brief A basic progress bar widget */
      class progress_bar : public layout::element_impl< progress_bar > {
        friend class sip::store_visitor_impl;
        friend class sip::restore_visitor_impl;
        friend class layout::element;
     
        private:

          /** \brief The minimum value */
          unsigned int m_minimum;
     
          /** \brief The maximum value */
          unsigned int m_maximum;
     
          /** \brief The current value */
          unsigned int m_current;
     
        private:
     
          /** \brief Default constructor */
          progress_bar();

        public:
     
          /** \brief Constructor for a progress bar */
          progress_bar(const unsigned int, const unsigned int, const unsigned int);

          /** \brief Sets the current value of the progress bar */
          void set_value(unsigned int);
     
          /** \brief Sets the current value of the progress bar, and sends an update */
          void set_value(unsigned int, tool::communicator*);
     
          /** \brief Sets the minimum value of the progress bar */
          void set_minimum(unsigned int);
     
          /** \brief Sets the minimum value of the progress bar , and sends an update*/
          void set_minimum(unsigned int, tool::communicator*);
     
          /** \brief Sets the minimum value of the progress bar */
          void set_maximum(unsigned int);
     
          /** \brief Sets the minimum value of the progress bar , and sends an update*/
          void set_maximum(unsigned int, tool::communicator*);
     
          /** \brief Gets the current value of the progress bar */
          unsigned int get_value() const;
     
          /** \brief Instantiate a layout element, through a mediator */
          layout::mediator::wrapper_aptr instantiate(layout::mediator*);

          /** \brief Synchronise with instantiation that is part of a (G)UI */
          void update(layout::mediator*, layout::mediator::wrapper*) const;
      };
     
      /**
       * \brief A text input field
       *
       * A data type derived from basic_datatype can be specified for validation
       * purposes. By default any string is accepted.
       **/
      class text_field : public layout::element_impl< text_field > {
        friend class sip::store_visitor_impl;
        friend class sip::restore_visitor_impl;
        friend class layout::element;
     
        private:
     
          /** \brief The text to be displayed initially */
          std::string          m_text;
     
          /** \brief Type for validation purposes */
          basic_datatype::sptr m_type;
     
        private:
     
          /** \brief Default constructor */
          text_field();

        public:
     
          /** \brief Constructor */
          text_field(std::string const& i);

          /** \brief Constructor */
          text_field(std::string const& i, basic_datatype::sptr&);

          /** \brief Set the text */
          void set_text(std::string const&);
     
          /** \brief Set the text, and sends an update */
          void set_text(std::string const&, tool::communicator*);
     
          /** \brief Get the text */
          std::string get_text() const;
     
          /** \brief Instantiate a layout element, through a mediator */
          layout::mediator::wrapper_aptr instantiate(layout::mediator*);

          /** \brief Synchronise with instantiation that is part of a (G)UI */
          void update(layout::mediator*, layout::mediator::wrapper*) const;
      };
    }
  }
}

#endif
