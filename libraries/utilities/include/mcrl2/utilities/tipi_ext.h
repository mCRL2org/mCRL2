// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
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

#include <tipi/display.hpp>
#include <tipi/layout_manager.hpp>
#include <tipi/layout_elements.hpp>

/**
 * \brief Name space for all mCRL2 toolset related functionality
 **/
namespace mcrl2 {
  /**
   * \brief Name space for all utility functionality
   **/
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
  
      /** \brief Helper class to work with tipi::radio_buttons
       *
       * A group of tipi::radio_button objects behave as a single control. The
       * tipi::radio_button interface also contains a function to return the
       * radio_button in the group that is selected. A very common use is to
       * map the radio buttons to the values of an enumerated type.
       *
       * This class helps to project the radio buttons in a group to instances
       * of a type T. For convenience it also takes over creation of the
       * radio_buttons in the group. So creation of the radio button and
       * association to a value of an enumerated type go hand-in-hand.
       **/
      template < typename T >
      class radio_button_helper {
  
        private:

          /** \brief Associates radio buttons with values */
          typedef std::map < tipi::layout::elements::radio_button const*, T > button_to_value_map;

        private:
  
          /** \brief Associates buttons with values */
          button_to_value_map                   selector;
  
          /** \brief The display for which to create the radio button objects */
          tipi::display&                        display;
  
          /** \brief The first button in the group */
          tipi::layout::elements::radio_button* first;
  
        public:
  
          /** \brief constructor */
          radio_button_helper(tipi::display&);
  
          /** \brief associate a radio button with a layout manager and a value */
          tipi::layout::elements::radio_button& associate(T const&, std::string const&, bool = false);
  
          /** \brief gets the first button associated with a value */
          tipi::layout::elements::radio_button& get_button(T const&);
  
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
        using tipi::layout::elements::radio_button;

        radio_button& button = display.create< radio_button >();
  
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
  
      /**
       * \pre there must be a button that is associated with the value
       * \param[in] t the value for which to find the associated button
       **/
      template < typename T >
      inline tipi::layout::elements::radio_button& radio_button_helper< T >::get_button(T const& t) {
        using tipi::layout::elements::radio_button;

        for (typename button_to_value_map::const_iterator i = selector.begin(); i != selector.end(); ++i) {
          if (i->second == t) {
            return const_cast < radio_button& > (*(i->first));
          }
        }
  
        return *first;
      }
  
      /**
       * \param[in] t the value for which to select the associated button
       **/
      template < typename T >
      inline void radio_button_helper< T >::set_selection(T const& t) {
        using tipi::layout::elements::radio_button;

        for (typename button_to_value_map::const_iterator i = selector.begin(); i != selector.end(); ++i) {
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
