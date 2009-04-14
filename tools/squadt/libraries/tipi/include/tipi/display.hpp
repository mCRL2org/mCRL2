// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/display.hpp
/// \brief Abstract interface for display implementations (protocol concept)

#ifndef TIPI_LAYOUT_DISPLAY_H
#define TIPI_LAYOUT_DISPLAY_H

#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/integer.hpp>

#include "tipi/common.hpp"
#include "tipi/layout_base.hpp"
#include "tipi/layout_manager.hpp"
#include "tipi/detail/event_handlers.hpp"

namespace tipi {

  /// \cond INTERNAL
  class display_impl : public ::tipi::layout::basic_event_handler {
    friend class tipi::display;

      /** \brief Type for element identifiers */
      typedef boost::uint_t< (sizeof(::tipi::layout::element*) << 3) >::least                      element_identifier;

      /** \brief Type alias for a mapping from identifier to element */
      typedef std::map < const element_identifier, boost::shared_ptr < ::tipi::layout::element > > element_by_id;

    protected:

      /** \brief the layout manager that contains all widgets for this display */
      boost::shared_ptr < layout::manager > m_manager;

      /** \brief Whether or not the tool display is visible to the user */
      element_by_id                         m_element_by_id;

    protected:

      /** \brief Associate an element with an id */
      void associate(element_identifier const& id, boost::shared_ptr < tipi::layout::element > const& e);

      /** \brief Disassociate an id with an element */
      void disassociate(tipi::layout::element const* e);

      /** \brief Disassociate an id with an element */
      void disassociate(element_identifier const& id);

      /** \brief Find an element by its id
       * \param[in] id the identifier of the element to find
       * \pre the element should be in the list
       * \throw false, when no element with this identifier is present
       **/
      tipi::layout::element const* find(element_identifier id) const;

      /** \brief Find an element by its id
       * \param[in] id the identifier of the element to find
       * \pre the element should be in the list
       * \throw false, when no element with this identifier is present
       **/
      tipi::layout::element* find(element_identifier id);

      /** \brief Find an element by its id
       * \pre the element should be in the list
       * \throw false, when the element is not present
       **/
      element_identifier find(::tipi::layout::element const* e);

    public:

      inline boost::shared_ptr< layout::manager >& get_manager() {
        return m_manager;
      }

      inline void set_manager(::tipi::layout::manager const& m) {
        m_manager = boost::static_pointer_cast < ::tipi::layout::manager > (
                        m_element_by_id[find(static_cast< ::tipi::layout::element const* > (&m))]);
      }
  };
  /// \endcond

  /**
   * \brief Abstract container class for layout elements
   *
   * Elements are associated and managed by a display. The display can be used
   * to create them and when the display is destroyed, all associated elements
   * are destroyed as well.
   **/
  class display : public ::utility::visitable {
    template < typename R, typename S >
    friend class ::utility::visitor;

    friend class ::tipi::tool::communicator_impl;
    friend class ::tipi::controller::communicator_impl;

    public:

      /** \brief Type for element identifiers */
      typedef boost::uint_t< (sizeof(::tipi::layout::element*) << 3) >::least element_identifier;

    protected:

      /** \brief Pointer to implementation object (handle body idiom) */
      boost::shared_ptr< display_impl > impl;

      /** \brief Factory function for layout elements */
      template < typename T >
      T& create(const display::element_identifier);

      /** \brief Factory function for layout elements */
      template < typename T >
      void create(boost::shared_ptr < ::tipi::layout::element >&, element_identifier const&);

      /** \brief Copy constructor */
      display(display const& other) : impl(other.impl) {
      }

      /** \brief Standard constructor */
      display() : impl(new display_impl) {
      }

    public:

      /** \brief Factory function for layout elements of this display */
      template < typename T >
      T& create();

      /** \brief Find the id for an element */
      ::tipi::display::element_identifier find(tipi::layout::element const* element) const {
        return impl->find(element);
      }

      /**
       * \param[in] id the identifier of the element to find
       * \pre the element should be in the list
       * \throw false, when no element with this identifier is present
       **/
      template < typename T >
      inline T const& find(tipi::display::element_identifier id) const {
        return static_cast < T const& > (*impl->find(id));
      }

      /**
       * \param[in] id the identifier of the element to find
       * \pre the element should be in the list
       * \throw false, when no element with this identifier is present
       **/
      template < typename T >
      inline T& find(tipi::display::element_identifier id) {
        return const_cast < T& > (*static_cast < T const* > (impl->find(id)));
      }
  };

  template < typename T >
  inline T& display::create() {
    boost::shared_ptr < T > new_t(::tipi::layout::element::create< T >());

    new_t->set_event_handler(*static_cast < tipi::layout::basic_event_handler* > (impl.get()));

    impl->associate(reinterpret_cast < display::element_identifier > (new_t.get()), boost::static_pointer_cast < tipi::layout::element > (new_t));

    return *new_t;
  }

  /**
   * \param[in] id an identifier for the new element
   **/
  template < typename T >
  inline T& display::create(const display::element_identifier id) {
    boost::shared_ptr < T > new_t(::tipi::layout::element::create< T >());

    new_t->set_event_handler(*static_cast < tipi::layout::basic_event_handler* > (impl.get()));

    impl->associate(id, boost::static_pointer_cast < tipi::layout::element > (new_t));

    return *new_t;
  }

  /**
   * \param[in] p a place to store the new element
   * \param[in] id an identifier for the new element
   **/
  template < typename T >
  inline void display::create(boost::shared_ptr < ::tipi::layout::element >& p, ::tipi::display::element_identifier const& id) {
    p = ::tipi::layout::element::create< T >();

    p->set_event_handler(*static_cast < tipi::layout::basic_event_handler* > (impl.get()));

    impl->associate(id, p);
  }
}

#endif
