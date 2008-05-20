// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/layout_elements.hpp

#ifndef LAYOUT_ELEMENTS_H
#define LAYOUT_ELEMENTS_H

#include <iosfwd>

#include "tipi/layout_base.hpp"
#include "tipi/detail/layout_mediator.hpp"
#include "tipi/basic_datatype.hpp"

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
     
          /** \brief Change the text */
          std::string get_text() const;

          /** \brief Change the text */
          label& set_text(std::string const&);

          /** \brief Instantiate a layout element, through a mediator */
          layout::mediator::wrapper_aptr instantiate(layout::mediator*);
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
     
          /** \brief Change the text */
          std::string get_label() const;

          /** \brief Change the label */
          button& set_label(std::string const&);

          /** \brief Activates the button */
          void activate();
     
          /** \brief Instantiate a layout element, through a mediator */
          layout::mediator::wrapper_aptr instantiate(layout::mediator*);
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

        private:
     
          /** \brief Set state of the radio button */
          void set_selected(bool = false);

          /** \brief Constructor */
          radio_button();

        public:
     
          /** \brief sets the label for the radio button */
          radio_button& set_label(std::string const&);

          /** \brief Returns the current label */
          std::string get_label() const;

          /** \brief Connects the button to another group */
          radio_button& connect(radio_button&);

          /** \brief The next radio button in the group */
          radio_button const& connected_to() const;

          /** \brief The next radio button in the group */
          radio_button& connected_to();

          /** \brief set radio button */
          radio_button& select();

          /** \brief Gets a pointer to the radio button in the group that is selected */
          radio_button const& get_selected() const;

          /** \brief Whether the radion button is selected or not */
          bool is_selected() const;
     
          /** \brief Instantiate a layout element, through a mediator */
          layout::mediator::wrapper_aptr instantiate(layout::mediator*);
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
     
          /** \brief Set the status */
          checkbox& set_status(bool);

          /** \brief sets the label for the radio button */
          checkbox& set_label(std::string const&);

          /** \brief Returns the current label */
          std::string get_label() const;

          /** \brief Gets the status */
          bool get_status() const;
     
          /** \brief Instantiate a layout element, through a mediator */
          layout::mediator::wrapper_aptr instantiate(layout::mediator*);
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
     
          /** \brief Sets the current value of the progress bar */
          progress_bar& set_value(unsigned int);
     
          /** \brief Gets the current value of the progress bar */
          unsigned int get_value() const;
     
          /** \brief Sets the minimum value of the progress bar */
          progress_bar& set_minimum(unsigned int);
     
          /** \brief Sets the minimum value of the progress bar */
          unsigned int get_minimum() const;
     
          /** \brief Sets the minimum value of the progress bar */
          progress_bar& set_maximum(unsigned int);

          /** \brief Sets the minimum value of the progress bar */
          unsigned int get_maximum() const;
     
          /** \brief Instantiate a layout element, through a mediator */
          layout::mediator::wrapper_aptr instantiate(layout::mediator*);
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
     
          /** \brief Sets the text */
          text_field& set_text(std::string const&);
     
          /** \brief Sets the type */
          text_field& set_type(boost::shared_ptr < basic_datatype >&);

          /** \brief Get the text */
          std::string get_text() const;
     
          /** \brief Instantiate a layout element, through a mediator */
          layout::mediator::wrapper_aptr instantiate(layout::mediator*);
      };
    }
  }
}

#endif
