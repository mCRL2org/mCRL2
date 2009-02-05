// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/layout_base.hpp
/// \brief Abstract interface for layout elements (display related)

#ifndef LAYOUT_BASE_H
#define LAYOUT_BASE_H

#include <memory>
#include <map>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include "tipi/detail/utility/generic_visitor.hpp"

#include "tipi/detail/event_handlers.hpp"

namespace tipi {
  /// \cond INTERNAL_DOCS
  namespace tool {
    class communicator;
  }
  namespace controller {
    class communicator_impl;
  }

  class display;
  /// \endcond

  namespace layout {
    class element;
    class manager;

    /** \brief Abstract base class for layout elements */
    class element : public ::utility::visitable, public boost::noncopyable {

      template < typename R, typename S >
      friend class ::utility::visitor;

      friend class ::tipi::layout::manager;
      friend class ::tipi::display;

      public:

        /** \brief Function type for event handlers */
        typedef boost::function < void () >    event_handler;

      private:

        /** \brief The global event handler for all element objects, unless they adopt another one */
        static basic_event_handler* get_default_event_handler() {
          static basic_event_handler event_handler;

          return &event_handler;
        }

      private:

        /** \brief The current event handler for this object */
        basic_event_handler*        m_event_handler;

        /** \brief Whether or not the element should grow */
        bool                        m_grow;

        /** \brief Whether or not the element is active */
        bool                        m_enabled;

      private:

        /**
         * \ brief Factory function for elements
         **/
        template < typename T >
        static boost::shared_ptr < T > create() {
          boost::shared_ptr < ::tipi::layout::element > p(new T());

          return boost::static_pointer_cast< T > (p);
        }

      protected:

        /** \brief Activate all handlers */
        void activate_handlers(bool b = true) {
          m_event_handler->process(this, b);
        }

        /** \brief Set the event handler object that will dispatch the events for this object */
        void set_event_handler(basic_event_handler& e) {
          m_event_handler->transfer(e, this);

          m_event_handler = &e;
        }

        /** \brief Get the event handler object that will dispatch the events for this object */
        basic_event_handler& get_event_handler() const {
          return *m_event_handler;
        }

        /** \brief Constructor */
        element() : m_event_handler(get_default_event_handler()), m_grow(true) {
        }

        /** \brief Set whether the element is allowed to grow or not
         * \param[in] b whether the element should grow to fill the available space
         **/
        inline void set_grow(bool b) {
          m_grow = b;
        }

        /** \brief Get whether the element is allowed to grow or not
         * \return whether the element grows to fill the available space
         **/
        inline bool get_grow() const {
          return m_grow;
        }

      public:

        /** \brief Enables user interaction
         * \param[in] b whether the element should be active
         **/
        inline void set_active(bool b) {
          m_enabled = b;
        }

        /** \brief Disables user interaction
         * \return whether the element is currently active
         **/
        inline bool is_active() const {
          return m_enabled;
        }

        /** \brief Awaits the next change event */
        void await_change() const {
          m_event_handler->await_change(this);
        }

        /** \brief Awaits the next change event */
        void on_change(boost::function < void (const void*) > h) const {
          m_event_handler->add(this, h);
        }

        /** \brief Abstract destructor */
        virtual ~element() {
        }
    };
  }
}

#endif

