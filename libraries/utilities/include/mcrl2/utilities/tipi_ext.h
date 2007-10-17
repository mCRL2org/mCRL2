// Author(s): Jeroen van der Wulp
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file include/mcrl2/utilities/tipi_ext.h
/// \brief Add your file description here.

#ifndef SQUADT_UTILITY_H_
#define SQUADT_UTILITY_H_

#include <memory>
#include <string>

#include <tipi/tool.hpp>

namespace mcrl2 {
  namespace utilities {
    namespace squadt {

      /** \brief Helper function for showing/hiding an element based on widget state changes */
      template < typename T >
      void change_visibility_on_toggle(T& r, tipi::layout::manager* m, tipi::layout::element*& c);
  
      /** \brief Helper function for showing/hiding an element based on radio_button widget state changes */
      template <>
      inline void change_visibility_on_toggle(tipi::layout::elements::radio_button& r, tipi::layout::manager* m, tipi::layout::element*& c) {
        if (r.is_selected()) {
          m->show(c);
        }
        else {
          m->hide(c);
        }
      }
  
      /** \brief Helper function for showing/hiding an element based on checkbox widget state changes */
      template <>
      inline void change_visibility_on_toggle(tipi::layout::elements::checkbox& r, tipi::layout::manager* m, tipi::layout::element*& c) {
        if (r.get_status()) {
          m->show(c);
        }
        else {
          m->hide(c);
        }
      }
  
      /** Helper class to project the selected radio button in a group to instances of a type T */
      template < typename T >
      class radio_button_helper {
  
        typedef tipi::layout::elements::radio_button radio_button;
  
        private:
  
          std::map < radio_button const*, T > selector;
  
        public:

          /** \brief The display for which to create the radio button objects */
          tipi::display& display;
  
          /** \brief The first button in the group */
          radio_button* first;
  
        public:
  
          /** \brief constructor */
          radio_button_helper(tipi::display&);
  
          /** \brief associate a radio button with a layout manager and a value */
          radio_button& associate(T const&, std::string const&, bool = false);
  
          /** \brief gets the button associated with a value */
          radio_button& get_button(T const&);
  
          /** \brief sets the selection for the group of radio buttons */
          void set_selection(T const&);
  
          /** \brief get the value for the selected radio button */
          T get_selection();
  
          /** \brief gets the label of the selected radio button */
          std::string get_selection_label() const;
      };
  
      /**
       * \param[in] d the display for which the radio button objects will be created
       **/
      template < typename T >
      inline radio_button_helper< T >::radio_button_helper(tipi::display& d) : display(d), first(0) {
      }
  
      /**
       * \param[in] v the value to associate the button with
       * \param[in] s the label of the radio button
       * \param[in] b whether the new button should be selected
       **/
      template < typename T >
      inline tipi::layout::elements::radio_button& radio_button_helper< T >::associate(T const& v, std::string const& s, bool b) {
        radio_button& button = display.create< tipi::layout::elements::radio_button >();;
  
        if (first == 0) {
          first = &button;
        }
        else {
          first->connect(button);
        }

        if (b) {
          button.select();
        }

        button.set_label(s);

        selector[&button] = v;
  
        return button;
      }
  
      template < typename T >
      inline tipi::layout::elements::radio_button& radio_button_helper< T >::get_button(T const& t) {
        for (typename std::map < radio_button const*, T >::iterator i = selector.begin(); i != selector.end(); ++i) {
          if (i->second == t) {
            return const_cast < radio_button& > (*(i->first));
          }
        }
  
        return *first;
      }
  
      template < typename T >
      inline void radio_button_helper< T >::set_selection(T const& t) {
        for (typename std::map < radio_button const*, T >::iterator i = selector.begin(); i != selector.end(); ++i) {
          if (i->second == t) {
            const_cast < radio_button* > (i->first)->select();
          }
        }
      }
  
      template < typename T >
      inline T radio_button_helper< T >::get_selection() {
        return selector[&first->get_selected()];
      }
  
      template < typename T >
      std::string radio_button_helper< T >::get_selection_label() const {
        return first->get_selected().get_label();
      }
    }
  }
}
 
#endif
