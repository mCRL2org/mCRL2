// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/tool_display.hpp

#ifndef TIPI_LAYOUT_TOOL_DISPLAY_H
#define TIPI_LAYOUT_TOOL_DISPLAY_H

#include <vector>

#include <boost/shared_ptr.hpp>

#include <tipi/display.hpp>
#include <tipi/layout_manager.hpp>

namespace tipi {
  namespace layout {

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

      public:

        /** \brief Convenience type alias to hide the shared pointer implementation */
        typedef boost::shared_ptr < tool_display >          sptr;

        /** \brief Convenience type for passing and returning a sequence of layout elements */
        typedef std::vector < tipi::layout::element const* > constant_elements;

      private:

        /** \brief the layout manager that contains all widgets for this display */
        boost::shared_ptr < layout::manager > m_manager;

        /** \brief Whether or not the tool display is visible to the user */
        bool                                  m_visible;

      public:

        using display::create;

        /** \brief Constructor */
        inline tool_display();

        /** \brief Factory function */
        inline static boost::shared_ptr < layout::tool_display > create();

        /** \brief Returns whether or not the contents should be visible */
        inline bool get_visibility() const;

        /** \brief Whether or not the tool display is shown */
        inline void show(bool);

        /** \brief Get the layout manager that contains all widgets for this display */
        inline layout::manager const* get_manager() const;

        /** \brief Set the layout manager that contains all widgets for this display */
        inline tool_display& set_manager(::tipi::layout::manager&);

        /** \brief Creates a (G)UI for this tool_display */
        mediator::wrapper_aptr instantiate(mediator* m) const;

        /** \brief Update (part of) the layout structure */
        void update(std::string const&, std::vector < tipi::layout::element const* >& elements);
    };

    inline tool_display::tool_display() : m_visible(true) {
    }

    inline boost::shared_ptr < tipi::layout::tool_display > tool_display::create() {
      boost::shared_ptr < tipi::layout::tool_display > p(new tool_display());

      return p;
    }

    inline layout::manager const* tool_display::get_manager() const {
      return (m_manager.get());
    }

    /**
     * \param[in] m the new top layout manager
     * 
     * \return *this
     **/
    inline tool_display& tool_display::set_manager(::tipi::layout::manager& m) {
      m_manager = boost::static_pointer_cast < ::tipi::layout::manager > (m_element_by_id[find(&m)]);

      return *this;
    }

    inline bool tool_display::get_visibility() const {
      return (m_visible);
    }

    /**
     * \param[in] s Whether or not the tool display must be visible
     **/
    inline void tool_display::show(bool s) {
      m_visible = s;
    }
  }
}

#endif

