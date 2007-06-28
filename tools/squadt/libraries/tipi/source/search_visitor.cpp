//  Copyright 2007 Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file source/search_visitor.cpp

#include <boost/tuple/tuple.hpp>

#include <tipi/utility/generic_visitor.tcc>
#include <tipi/visitors.h>
#include <tipi/report.h>
#include <tipi/tool/capabilities.h>
#include <tipi/controller/capabilities.h>
#include <tipi/basic_datatype.h>
#include <tipi/detail/layout_elements.h>
#include <tipi/detail/layout_manager.h>
#include <tipi/display.h>
#include <tipi/detail/event_handlers.h>
#include <tipi/common.h>

namespace tipi {

  /// \cond INTERNAL_DOCS
  class search_visitor_impl : private boost::noncopyable {
    friend class visitors;
    friend class find_visitor;
  };
  /// \endcond

  /** Default constructor */
  search_visitor::search_visitor() {
  }
}

namespace utility {

  /**
   * \param o the tipi::object object to search in
   **/
  template <>
  template <>
  bool visitor< tipi::search_visitor_impl, bool >::visit(tipi::object& o) {
    /// \todo implement if need arises
    assert(false);

    return true;
  }

  /**
   * \param o the tipi::option object to search in
   **/
  template <>
  template <>
  bool visitor< tipi::search_visitor_impl, bool >::visit(tipi::option& o) {
    /// \todo implement if need arises
    assert(false);

    return true;
  }

  /**
   * \param c the tipi::configuration object to search in
   **/
  template <>
  template <>
  bool visitor< tipi::search_visitor_impl, bool >::visit(tipi::configuration& c) {
    /// \todo implement if need arises
    assert(false);

    return true;
  }

  /** \brief tuple type for searching an element by its identifier */
  typedef boost::tuple< tipi::layout::element const*, tipi::layout::element_identifier const& > element_by_id_tuple;

  /** \brief tuple type for searching the identifier for an element */
  typedef boost::tuple< tipi::layout::element_identifier, tipi::layout::element const* >        id_by_element_tuple;

  /**
   * \param[in] c the tipi::layout::box object to search in
   * \param[in] t tuple to hold the potential result and the identifier of the element to look for
   **/
  template <>
  template <>
  bool visitor< tipi::search_visitor_impl, bool >::visit(tipi::layout::box& c, element_by_id_tuple& t) {
    tipi::layout::element_identifier const& id = t.get< 1 >();

    for (tipi::layout::box::children_list::const_iterator i = c.m_children.begin(); i != c.m_children.end(); ++i) {
      if (i->identifier == id) {
        t.get< 0 >() = i->layout_element;

        return true;
      }
    }

    for (tipi::layout::box::children_list::const_iterator i = c.m_children.begin(); i != c.m_children.end(); ++i) {
      try {
        if (do_visit(*i->layout_element, t)) {
          return true;
        }
      }
      catch (...) {
        // Catch exceptions for missing visit methods
      }
    }

    return false;
  }

  /**
   * \param[in] c the tipi::layout::box object to search in
   * \param[in] t tuple to hold the potential result and the element to look for
   **/
  template <>
  template <>
  bool visitor< tipi::search_visitor_impl, bool >::visit(tipi::layout::box& c, id_by_element_tuple& t) {
    tipi::layout::element const*& element = t.get< 1 >();

    for (tipi::layout::box::children_list::const_iterator i = c.m_children.begin(); i != c.m_children.end(); ++i) {
      if (i->layout_element == element) {
        t.get< 0 >() = i->identifier;

        return true;
      }
    }

    for (tipi::layout::box::children_list::const_iterator i = c.m_children.begin(); i != c.m_children.end(); ++i) {
      try {
        if (do_visit(*i->layout_element, t)) {
          return true;
        }
      }
      catch (...) {
        // Catch exceptions for missing visit methods
      }
    }

    return false;
  }

  /**
   * \param[in] c the tipi::layout::box object to search in
   * \param[in] t tuple to hold the potential result and the element to look for
   **/
  template <>
  template <>
  bool visitor< tipi::search_visitor_impl, bool >::visit(tipi::layout::horizontal_box& c, element_by_id_tuple& t) {
    return visit(static_cast < tipi::layout::box& > (c), t);
  }

  /**
   * \param[in] c the tipi::layout::box object to search in
   * \param[in] t tuple to hold the potential result and the identifier of the element to look for
   **/
  template <>
  template <>
  bool visitor< tipi::search_visitor_impl, bool >::visit(tipi::layout::horizontal_box& c, id_by_element_tuple& t) {
    return visit(static_cast < tipi::layout::box& > (c), t);
  }

  /**
   * \param[in] c the tipi::layout::box object to search in
   * \param[in] t tuple to hold the potential result and the element to look for
   **/
  template <>
  template <>
  bool visitor< tipi::search_visitor_impl, bool >::visit(tipi::layout::vertical_box& c, element_by_id_tuple& t) {
    return visit(static_cast < tipi::layout::box& > (c), t);
  }

  /**
   * \param[in] c the tipi::layout::box object to search in
   * \param[in] t tuple to hold the potential result and the identifier of the element to look for
   **/
  template <>
  template <>
  bool visitor< tipi::search_visitor_impl, bool >::visit(tipi::layout::vertical_box& c, id_by_element_tuple& t) {
    return visit(static_cast < tipi::layout::box& > (c), t);
  }

  /**
   * \param[in] c the tipi::layout::tool_display object to search in
   * \param[in] t tuple to hold the potential result and the element to look for
   **/
  template <>
  template <>
  bool visitor< tipi::search_visitor_impl, bool >::visit(tipi::layout::tool_display& c, element_by_id_tuple& t) {
    if (c.m_manager.get()) {
      return do_visit(*c.m_manager, t);
    }

    return false;
  }

  /**
   * \param[in] c the tipi::layout::tool_display object to search in
   * \param[in] t tuple to hold the potential result and the identifier of the element to look for
   **/
  template <>
  template <>
  bool visitor< tipi::search_visitor_impl, bool >::visit(tipi::layout::tool_display& c, id_by_element_tuple& t) {
    if (c.m_manager.get()) {
      return do_visit(*c.m_manager, t);
    }

    return false;
  }

  /** Registers all available visit methods */
  template <>
  bool visitor< tipi::search_visitor_impl, bool >::initialise() {
    register_visit_method< tipi::object >();
    register_visit_method< tipi::option >();
    register_visit_method< tipi::configuration >();
    register_visit_method< tipi::layout::tool_display, element_by_id_tuple >();
    register_visit_method< tipi::layout::tool_display, id_by_element_tuple >();
    register_visit_method< tipi::layout::horizontal_box, element_by_id_tuple >();
    register_visit_method< tipi::layout::horizontal_box, id_by_element_tuple >();
    register_visit_method< tipi::layout::vertical_box, element_by_id_tuple >();
    register_visit_method< tipi::layout::vertical_box, id_by_element_tuple >();

    return true;
  }
}
