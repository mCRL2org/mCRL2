#include <boost/tuple/tuple.hpp>

#include <utilities/generic_visitor.tcc>

#include <sip/visitors.h>
#include <sip/report.h>
#include <sip/tool/capabilities.h>
#include <sip/controller/capabilities.h>
#include <sip/basic_datatype.h>
#include <sip/detail/layout_elements.h>
#include <sip/detail/layout_manager.h>
#include <sip/detail/layout_tool_display.h>
#include <sip/detail/event_handlers.h>
#include <sip/common.h>

namespace sip {

  /// \cond PRIVATE_PART
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
   * \param o the sip::object object to search in
   **/
  template <>
  template <>
  bool visitor< sip::search_visitor_impl, bool >::visit(sip::object& o) {
    /// \todo implement if need arises
    assert(false);

    return true;
  }

  /**
   * \param o the sip::option object to search in
   **/
  template <>
  template <>
  bool visitor< sip::search_visitor_impl, bool >::visit(sip::option& o) {
    /// \todo implement if need arises
    assert(false);

    return true;
  }

  /**
   * \param c the sip::configuration object to search in
   **/
  template <>
  template <>
  bool visitor< sip::search_visitor_impl, bool >::visit(sip::configuration& c) {
    /// \todo implement if need arises
    assert(false);

    return true;
  }

  /** \brief tuple type for searching an element by its identifier */
  typedef boost::tuple< sip::layout::element const*, sip::layout::element_identifier const& > element_by_id_tuple;

  /** \brief tuple type for searching the identifier for an element */
  typedef boost::tuple< sip::layout::element_identifier, sip::layout::element const* >        id_by_element_tuple;

  /**
   * \param[in] c the sip::layout::box object to search in
   * \param[in] t tuple to hold the potential result and the identifier of the element to look for
   **/
  template <>
  template <>
  bool visitor< sip::search_visitor_impl, bool >::visit(sip::layout::box& c, element_by_id_tuple& t) {
    sip::layout::element_identifier const& id = t.get< 1 >();

    for (sip::layout::box::children_list::const_iterator i = c.m_children.begin(); i != c.m_children.end(); ++i) {
      if (i->identifier == id) {
        t.get< 0 >() = i->layout_element;

        return true;
      }
    }

    for (sip::layout::box::children_list::const_iterator i = c.m_children.begin(); i != c.m_children.end(); ++i) {
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
   * \param[in] c the sip::layout::box object to search in
   * \param[in] t tuple to hold the potential result and the element to look for
   **/
  template <>
  template <>
  bool visitor< sip::search_visitor_impl, bool >::visit(sip::layout::box& c, id_by_element_tuple& t) {
    sip::layout::element const*& element = t.get< 1 >();

    for (sip::layout::box::children_list::const_iterator i = c.m_children.begin(); i != c.m_children.end(); ++i) {
      if (i->layout_element == element) {
        t.get< 0 >() = i->identifier;

        return true;
      }
    }

    for (sip::layout::box::children_list::const_iterator i = c.m_children.begin(); i != c.m_children.end(); ++i) {
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
   * \param[in] c the sip::layout::box object to search in
   * \param[in] t tuple to hold the potential result and the element to look for
   **/
  template <>
  template <>
  bool visitor< sip::search_visitor_impl, bool >::visit(sip::layout::horizontal_box& c, element_by_id_tuple& t) {
    return visit(static_cast < sip::layout::box& > (c), t);
  }

  /**
   * \param[in] c the sip::layout::box object to search in
   * \param[in] t tuple to hold the potential result and the identifier of the element to look for
   **/
  template <>
  template <>
  bool visitor< sip::search_visitor_impl, bool >::visit(sip::layout::horizontal_box& c, id_by_element_tuple& t) {
    return visit(static_cast < sip::layout::box& > (c), t);
  }

  /**
   * \param[in] c the sip::layout::box object to search in
   * \param[in] t tuple to hold the potential result and the element to look for
   **/
  template <>
  template <>
  bool visitor< sip::search_visitor_impl, bool >::visit(sip::layout::vertical_box& c, element_by_id_tuple& t) {
    return visit(static_cast < sip::layout::box& > (c), t);
  }

  /**
   * \param[in] c the sip::layout::box object to search in
   * \param[in] t tuple to hold the potential result and the identifier of the element to look for
   **/
  template <>
  template <>
  bool visitor< sip::search_visitor_impl, bool >::visit(sip::layout::vertical_box& c, id_by_element_tuple& t) {
    return visit(static_cast < sip::layout::box& > (c), t);
  }

  /**
   * \param[in] c the sip::layout::tool_display object to search in
   * \param[in] t tuple to hold the potential result and the element to look for
   **/
  template <>
  template <>
  bool visitor< sip::search_visitor_impl, bool >::visit(sip::layout::tool_display& c, element_by_id_tuple& t) {
    if (c.m_manager.get()) {
      return do_visit(*c.m_manager, t);
    }

    return false;
  }

  /**
   * \param[in] c the sip::layout::tool_display object to search in
   * \param[in] t tuple to hold the potential result and the identifier of the element to look for
   **/
  template <>
  template <>
  bool visitor< sip::search_visitor_impl, bool >::visit(sip::layout::tool_display& c, id_by_element_tuple& t) {
    if (c.m_manager.get()) {
      return do_visit(*c.m_manager, t);
    }

    return false;
  }

  /** Registers all available visit methods */
  template <>
  bool visitor< sip::search_visitor_impl, bool >::initialise() {
    register_visit_method< sip::object >();
    register_visit_method< sip::option >();
    register_visit_method< sip::configuration >();
    register_visit_method< sip::layout::tool_display, element_by_id_tuple >();
    register_visit_method< sip::layout::tool_display, id_by_element_tuple >();
    register_visit_method< sip::layout::horizontal_box, element_by_id_tuple >();
    register_visit_method< sip::layout::horizontal_box, id_by_element_tuple >();
    register_visit_method< sip::layout::vertical_box, element_by_id_tuple >();
    register_visit_method< sip::layout::vertical_box, id_by_element_tuple >();

    return true;
  }
}
