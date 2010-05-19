// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/layout_elements.hpp
/// \brief Concrete implementation of some layout elements (display related)

#ifndef LAYOUT_ELEMENTS_H
#define LAYOUT_ELEMENTS_H

#include <iosfwd>

#include "tipi/layout_base.hpp"
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
          inline label() {}

        public:

          /** \brief Change the text */
          inline std::string get_text() const {
            return m_text;
          }

          /**
           * \brief Change the text
           * \param[in] t the text of the label
           * \return *this
           **/
          inline label& set_text(std::string const& t) {
            m_text = t;

            activate_handlers();

            return *this;
          }
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
          inline button() {
            set_grow(false);
          }

        public:

          /** \brief Change the text */
          inline std::string get_label() const {
            return m_label;
          }

          /**
           * \brief Change the label
           * \param[in] l the label for the button
           * \return *this
           **/
          inline button& set_label(std::string const& l) {
            m_label = l;

            activate_handlers();

            return *this;
          }

          /** \brief Activates the button */
          void activate() {
            activate_handlers();
          }
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

          /**
           * \brief Set state of the radio button
           * \param[in] b whether or not to send an event
           **/
          void set_selected(bool b = false) {
            for (radio_button* r = m_connection; r != this; r = r->m_connection) {
              if (r->m_selected) {
                r->m_selected = false;

                break;
              }
            }

            m_selected = true;

            activate_handlers(b);
          }

          /** \brief Constructor */
          radio_button() : m_selected(true) {
            m_connection = this;
          }

        public:

          /**
           * \brief sets the label for the radio button
           * \param[in] l the new text of the label for the radio button
           * \return *this
           **/
          inline radio_button& set_label(std::string const& l) {
            m_label = l;

            return *this;
          }

          /** \brief Returns the current label */
          inline std::string get_label() const {
            return m_label;
          }

          /**
           * \brief Connects the button to another group
           * \param[in] r the button in the group to connect with
           **/
          inline radio_button& connect(radio_button& r) {
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

          /** \brief The next radio button in the group */
          inline radio_button const& connected_to() const {
            return *m_connection;
          }

          /** \brief The next radio button in the group */
          inline radio_button& connected_to() {
            return *m_connection;
          }

          /**
           * \brief set radio button
           * \return *this
           */
          inline radio_button& select() {
            if (!m_selected) {
              set_selected(true);
            }

            return *this;
          }

          /** \brief Gets a pointer to the radio button in the group that is selected */
          inline radio_button const& selected() const {
            radio_button const* r = this;

            while (!r->m_selected) {
              r = r->m_connection;
            }

            return *r;
          }

          /** \brief Whether the radion button is selected or not */
          inline bool is_selected() const {
            return m_selected;
          }
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
          inline checkbox() : m_status(false) {}

        public:

          /**
           * \brief Set the status
           * \param[in] b the new status
           * \return *this
           **/
          inline checkbox& set_status(bool b) {
            m_status = b;

            activate_handlers();

            return *this;
          }

          /** \brief sets the label for the radio button
           * \param[in] l the new text of the label for the radio button
           * \return *this
           **/
          inline checkbox& set_label(std::string const& l) {
            m_label = l;

            return *this;
          }

          /** \brief Returns the current label */
          inline std::string get_label() const {
            return m_label;
          }

          /** \brief Gets the status */
          inline bool get_status() const {
            return m_status;
          }
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
          inline progress_bar() : m_minimum(0), m_maximum(0), m_current(0) {
          }


        public:

          /**
           * \brief Sets the current value of the progress bar
           * \param[in] v the new value
           * \return *this
           * \pre minimum <= v <= maximum
           **/
          inline progress_bar& set_value(unsigned int v) {
            m_current = v;

            activate_handlers();

            return *this;
          }

          /** \brief Gets the current value of the progress bar */
          inline unsigned int get_value() const {
            return m_current;
          }

          /**
           * \brief Sets the minimum value of the progress bar
           * \param[in] v the new value
           * \return *this
           **/
          inline progress_bar& set_minimum(unsigned int v) {
            m_minimum = v;

            activate_handlers();

            return *this;
          }

          /** \brief Sets the minimum value of the progress bar */
          inline unsigned int get_minimum() const {
            return m_minimum;
          }

          /**
           * \brief Sets the minimum value of the progress bar
           * \param[in] v the new value
           * \return *this
           **/
          inline progress_bar& set_maximum(unsigned int v) {
            m_maximum = v;

            activate_handlers();

            return *this;
          }

          /** \brief Sets the minimum value of the progress bar */
          inline unsigned int get_maximum() const {
            return m_maximum;
          }
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
          std::string                         m_text;

          /** \brief Type for validation purposes */
          boost::shared_ptr< basic_datatype > m_type;

        private:

          /** \brief Default constructor */
          inline text_field() : m_text(""), m_type(new tipi::datatype::string()) {
          }

        public:

          /**
           * \brief Sets the text
           * \param[in] s the new text
           * \return *this
           **/
          inline text_field& set_text(std::string const& s) {
            m_text = s;

            activate_handlers();

            return *this;
          }

          /**
           * \brief Sets the type
           * \param[in] t the new type to validate against
           * \return *this
           **/
          inline text_field& set_type(boost::shared_ptr < basic_datatype >& t) {
            m_type = t;

            activate_handlers();

            return *this;
          }

          /** \brief Get the text */
          inline std::string get_text() const {
            return m_text;
          }
      };

      /**
       * \brief A file control picker input field
       *
       * A data type derived from basic_datatype can be specified for validation
       * purposes. By default any string is accepted.
       **/
      class file_control : public layout::element {
        friend class layout::element;

        template < typename R, typename S >
        friend class ::utility::visitor;

        private:

          /** \brief The text to be displayed initially */
          std::string                         m_text;

          /** \brief Type for validation purposes */
          boost::shared_ptr< basic_datatype > m_type;

        private:

          /** \brief Default constructor */
          inline file_control() : m_text(""), m_type(new tipi::datatype::string()) {
          }

        public:

          /**
           * \brief Sets the text
           * \param[in] s the new text
           * \return *this
           **/
          inline file_control& set_text(std::string const& s) {
            m_text = s;

            activate_handlers();

            return *this;
          }

          /**
           * \brief Sets the type
           * \param[in] t the new type to validate against
           * \return *this
           **/
          inline file_control& set_type(boost::shared_ptr < basic_datatype >& t) {
            m_type = t;

            activate_handlers();

            return *this;
          }

          /** \brief Get the text */
          inline std::string get_text() const {
            return m_text;
          }
      };


    }
  }
}

#endif
