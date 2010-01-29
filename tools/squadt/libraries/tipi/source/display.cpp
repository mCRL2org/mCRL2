// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "tipi/display.hpp"

namespace tipi {

  /// \cond INTERNAL
  void display_impl::associate(element_identifier const& id, boost::shared_ptr < tipi::layout::element > const& e) {
    if(!(m_element_by_id.count(id) == 0)){
      throw mcrl2::runtime_error("Empty element");
    };

    m_element_by_id[id] = e;
  }

  /** \brief Disassociate an id with an element */
  void display_impl::disassociate(tipi::layout::element const* e) {
    for (element_by_id::iterator i = m_element_by_id.begin(); i != m_element_by_id.end(); ++i) {
      if (i->second.get() == e) {
        m_element_by_id.erase(i);

        break;
      }
    }
  }

  /** \brief Disassociate an id with an element */
  void display_impl::disassociate(element_identifier const& id) {
    element_by_id::iterator i = m_element_by_id.find(id);

    if (i != m_element_by_id.end()) {
      m_element_by_id.erase(i);
    }
  }

  /** \brief Find an element by its id
   * \param[in] id the identifier of the element to find
   * \pre the element should be in the list
   * \throw false, when no element with this identifier is present
   **/
  tipi::layout::element const* display_impl::find(element_identifier id) const {
    element_by_id::const_iterator i = m_element_by_id.find(id);

    if (i == m_element_by_id.end()) {
      throw false;
    }

    return i->second.get();
  }

  /** \brief Find an element by its id
   * \param[in] id the identifier of the element to find
   * \pre the element should be in the list
   * \throw false, when no element with this identifier is present
   **/
  tipi::layout::element* display_impl::find(element_identifier id) {
    element_by_id::iterator i = m_element_by_id.find(id);

    if (i == m_element_by_id.end()) {
      throw false;
    }

    return i->second.get();
  }

  /** \brief Find an element by its id
   * \pre the element should be in the list
   * \throw false, when the element is not present
   **/
  display_impl::element_identifier display_impl::find(::tipi::layout::element const* e) {
    for (element_by_id::const_iterator i = m_element_by_id.begin(); i != m_element_by_id.end(); ++i) {
      if (i->second.get() == e) {
        return i->first;

        break;
      }
    }

    throw false;
  }
  /// \endcond
}

