// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/display.hpp

#ifndef TIPI_LAYOUT_DISPLAY_H
#define TIPI_LAYOUT_DISPLAY_H

#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/integer.hpp>

#include "tipi/layout_base.hpp"
#include "tipi/detail/event_handlers.hpp"

namespace tipi {

  /**
   * \brief Abstract container class for layout elements
   * 
   * Alements are associated and managed by a display. The display can be used
   * to create them and when the display is destroyed, all associated elements
   * are destroyed as well.
   **/
  class display : public ::utility::visitable, public ::tipi::layout::basic_event_handler {
    template < typename R, typename S >
    friend class ::utility::visitor;

    public:

      /** \brief Type for element identifiers */
      typedef boost::uint_t< (sizeof(::tipi::layout::element*) << 3) >::least                      element_identifier;

      /** \brief Type alias for a mapping from identifier to element */
      typedef std::map < const element_identifier, boost::shared_ptr < ::tipi::layout::element > > element_by_id;

    protected:

      /** \brief Whether or not the tool display is visible to the user */
      element_by_id m_element_by_id;

    protected:

      /** \brief Associate an id with an element */
      void associate(display::element_identifier const& id, boost::shared_ptr < tipi::layout::element > const& e) {
        assert(m_element_by_id.count(id) == 0);

        m_element_by_id[id] = e;
      }

      /** \brief Disassociate an id with an element */
      void disassociate(tipi::layout::element const*);

      /** \brief Disassociate an id with an element */
      void disassociate(tipi::display::element_identifier const&);

      /** \brief Factory function for layout elements of this display */
      template < typename T >
      void create(boost::shared_ptr< ::tipi::layout::element >&, ::tipi::display::element_identifier const&);

      /** \brief Factory function for layout elements of this display */
      template < typename T >
      T& create(const ::tipi::display::element_identifier);

    public:

      /** \brief Factory function for layout elements of this display */
      template < typename T >
      T& create();

      /** \brief Find an element by its id */
      template < typename T >
      T const* find(::tipi::display::element_identifier const&) const;

      /** \brief Find an element by its id */
      template < typename T >
      T* find(::tipi::display::element_identifier const&);

      /** \brief Find the id for an element */
      const ::tipi::display::element_identifier find(tipi::layout::element const*) const;

      /** \brief Pure virtual destructor */
      virtual ~display() {
      }
  };

  template < typename T >
  inline T& display::create() {
    boost::shared_ptr < T > new_t(::tipi::layout::element::create< T >());

    new_t->set_event_handler(*static_cast < tipi::layout::basic_event_handler* > (this));

    associate(reinterpret_cast < display::element_identifier > (new_t.get()), boost::static_pointer_cast < tipi::layout::element > (new_t));

    return *new_t;
  }

  /**
   * \param[in] id an identifier for the new element
   **/
  template < typename T >
  inline T& display::create(const display::element_identifier id) {
    boost::shared_ptr < T > new_t(::tipi::layout::element::create< T >());

    new_t->set_event_handler(*static_cast < tipi::layout::basic_event_handler* > (this));

    associate(id, boost::static_pointer_cast < tipi::layout::element > (new_t));

    return *new_t;
  }

  /**
   * \param[in] p a place to store the new element
   * \param[in] id an identifier for the new element
   **/
  template < typename T >
  inline void display::create(boost::shared_ptr < ::tipi::layout::element >& p, ::tipi::display::element_identifier const& id) {
    p = ::tipi::layout::element::create< T >();

    p->set_event_handler(*static_cast < tipi::layout::basic_event_handler* > (this));

    associate(id, p);
  }

  /**
   * \param[in] id the identifier of the element to find
   * \pre the element should be in the list
   * \throw false, when no element with this identifier is present
   **/
  template < typename T >
  inline T const* display::find(tipi::display::element_identifier const& id) const {
    element_by_id::const_iterator i = m_element_by_id.find(id);

    if (i == m_element_by_id.end()) {
      throw false;
    }

    return static_cast < T const* > (i->second.get());
  }

  /**
   * \param[in] id the identifier of the element to find
   * \pre the element should be in the list
   * \throw false, when no element with this identifier is present
   **/
  template < typename T >
  inline T* display::find(tipi::display::element_identifier const& id) {
    element_by_id::const_iterator i = m_element_by_id.find(id);

    if (i == m_element_by_id.end()) {
      throw false;
    }

    return const_cast < T* > (static_cast < T const* > (i->second.get()));
  }
}

#endif
