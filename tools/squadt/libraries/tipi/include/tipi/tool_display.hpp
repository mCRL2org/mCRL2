// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/tool_display.hpp
/// \brief Type used to represent a tool display (protocol concept)

#ifndef TIPI_LAYOUT_TOOL_DISPLAY_H
#define TIPI_LAYOUT_TOOL_DISPLAY_H

#include <boost/shared_ptr.hpp>

#include "tipi/display.hpp"
#include "tipi/layout_manager.hpp"

namespace tipi {
  /**
   * \brief Basic container class for controller-side layout definitions
   *
   * A tool_display object contains a layout description for the tool display
   * facility on the side of the controller. Life time of the elements that
   * describe the layout of this display is managed by the tool display.
   **/
  class tool_display : public ::tipi::display {
    template < typename R, typename S >
    friend class ::utility::visitor;

    friend class ::tipi::tool::communicator_impl;
    friend class ::tipi::controller::communicator_impl;

    private:

      /** \brief Whether or not the tool display is visible to the user */
      bool m_visible;

    private:

      /** \brief Constructor */
      inline tool_display(tool_display& other) : display(other) {
      }

    public:

      using display::create;

      /** \brief Constructor */
      inline tool_display() : m_visible(true) {
      }

      /** \brief Returns whether or not the contents should be visible */
      inline bool visible() const {
        return m_visible;
      }

      /** \brief Whether or not the tool display is shown */
      inline void show(bool s) {
        m_visible = s;
      }

      /** \brief Get the layout manager that contains all widgets for this display */
      inline layout::manager const* manager() const {
        return impl->get_manager().get();
      }

      /** \brief Sets the layout manager containing all widgets of this display
       * \param[in] m the new top layout manager
       * \return *this
       **/
      inline tool_display& manager(::tipi::layout::manager& m) {
        impl->set_manager(m);

        return *this;
      }
  };
}

#endif

