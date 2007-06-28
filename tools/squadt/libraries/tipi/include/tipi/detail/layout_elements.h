//  Copyright 2007 Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file include/tipi/detail/layout_elements.h

#ifndef LAYOUT_ELEMENTS_H
#define LAYOUT_ELEMENTS_H

#include <iosfwd>

#include <tipi/layout_base.h>
#include <tipi/detail/layout_mediator.h>
#include <tipi/basic_datatype.h>
#include <tipi/common.h>

namespace tipi {
  namespace layout {
    namespace elements {

      using namespace tipi::datatype;
     
      /** \brief A basic text label widget */
      class label : public layout::element {
        friend class layout::element;

        template < typename R, typename S >
        friend class ::utility::visitor;
     
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
     
          /** \brief Instantiate a layout element, through a mediator */
          layout::mediator::wrapper_aptr instantiate(layout::mediator*);

          /** \brief Synchronise with instantiation that is part of a (G)UI */
          void update(layout::mediator*, layout::mediator::wrapper*) const;
      };
     
      /** \brief A basic button widget */
      class button : public layout::element {
        friend class layout::element;

        template < typename R, typename S >
        friend class ::utility::visitor;
     
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
      class radio_button : public layout::element {
        friend class layout::element;
     
        template < typename R, typename S >
        friend class ::utility::visitor;

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
     
          /** \brief Set state of the radio button */
          void set_selected(bool = false);

        public:
     
          /** \brief Alternative constructor for a radio button */
          radio_button(std::string const&);

          /** \brief Alternative constructor for a radio button */
          radio_button(std::string const&, radio_button*, bool = false);

          /** \brief Returns the current label */
          std::string get_label() const;

          /** \brief set radio button */
          void select();

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
      class checkbox : public layout::element {
        friend class layout::element;
     
        template < typename R, typename S >
        friend class ::utility::visitor;

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

          /** \brief Gets the status */
          bool get_status() const;
     
          /** \brief Instantiate a layout element, through a mediator */
          layout::mediator::wrapper_aptr instantiate(layout::mediator*);

          /** \brief Synchronise with instantiation that is part of a (G)UI */
          void update(layout::mediator*, layout::mediator::wrapper*) const;
      };
     
      /** \brief A basic progress bar widget */
      class progress_bar : public layout::element {
        friend class layout::element;

        template < typename R, typename S >
        friend class ::utility::visitor;
     
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
     
          /** \brief Sets the minimum value of the progress bar */
          void set_minimum(unsigned int);
     
          /** \brief Sets the minimum value of the progress bar */
          void set_maximum(unsigned int);
     
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
      class text_field : public layout::element {
        friend class layout::element;

        template < typename R, typename S >
        friend class ::utility::visitor;
     
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
