// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file store_visitor.cpp
/// \brief Add your file description here.

#include "wx.hpp" // precompiled headers

#include <fstream>
#include <exception>

#include <boost/thread.hpp> // workaround, because boost::thread includes errno.h
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#include "visitors.hpp"

#include "build_system.hpp"
#include "settings_manager.hpp"
#include "project_manager.ipp"
#include "tool_manager.ipp"
#include "type_registry.hpp"
#include "executor.ipp"
#include "processor.ipp"

#include <tipi/detail/utility/generic_visitor.hpp>

namespace squadt {

  /// \cond INTERNAL_DOCS
  class store_visitor_impl {

    friend class visitors;
    friend class store_visitor;

    protected:

      /** \brief The destination of output */
      std::ostream&   out;

    public:

      /** \brief Writes to stream */
      store_visitor_impl(std::ostream&);
  };

  class store_visitor_path_impl : public utility::visitor< store_visitor_impl > {

    private:

      /** \brief Storage for temporary std::ostream object */
      std::ofstream    m_help_stream;

    public:

      /** \brief Writes to file */
      store_visitor_path_impl(boost::filesystem::path const&);
  };

  class store_visitor_string_impl : public utility::visitor< store_visitor_impl > {

    private:

      /** \brief Storage for temporary std::ostream object */
      std::ostringstream  m_help_stream;

      /** \brief Storage for character data */
      std::string&        m_target_string;

    public:

      /** \brief Constructor for writing to string */
      store_visitor_string_impl(std::string&);

      /** \brief Writes to file */
      ~store_visitor_string_impl();
  };

  inline store_visitor_string_impl::store_visitor_string_impl(std::string& s) :
                                utility::visitor< store_visitor_impl >(m_help_stream),
                                m_target_string(s) {
  }

  inline store_visitor_string_impl::~store_visitor_string_impl() {
    m_target_string.assign(m_help_stream.str());
  }

  /**
   * \param[in] p a path to the file from which to read
   **/
  inline store_visitor_path_impl::store_visitor_path_impl(boost::filesystem::path const& p) :
                                utility::visitor< store_visitor_impl >(m_help_stream),
                                m_help_stream(p.string().c_str(), std::ios_base::out) {

    m_help_stream.exceptions(std::ofstream::failbit|std::ofstream::badbit);
  }

  inline store_visitor_impl::store_visitor_impl(std::ostream& o) : out(o) {
  }
  /// \endcond

  /**
   * \param[in] s a string to write to
   **/
  store_visitor::store_visitor(std::string& s) :
        utility::visitor_interface< store_visitor_impl >(boost::shared_ptr < utility::visitor< store_visitor_impl > > (new store_visitor_string_impl(s))) {
  }

  /**
   * \param[in] p a path to the file to write to
   **/
  store_visitor::store_visitor(boost::filesystem::path const& p) :
        utility::visitor_interface< store_visitor_impl >(boost::shared_ptr < utility::visitor< store_visitor_impl > > (new store_visitor_path_impl(p))) {
  }

  /**
   * \param[in] o a stream to write to
   **/
  store_visitor::store_visitor(std::ostream& o) :
        utility::visitor_interface< store_visitor_impl >(boost::shared_ptr < utility::visitor< store_visitor_impl > > (new utility::visitor < store_visitor_impl >(o))) {
  }
}

namespace utility {
  using namespace squadt;

  /**
   * \param[in] t the tool object to store
   **/
  template <>
  template <>
  void visitor< squadt::store_visitor_impl >::visit(tool const& t) {
    out << "<tool name=\"" << t.get_name()
                    << "\" location=\"" << t.get_location() << "\"/>\n";
  }

  /**
   * \param[in] t the tool manager to store
   **/
  template <>
  template <>
  void visitor< squadt::store_visitor_impl >::visit(tool_manager const& t) {
    do_visit(*t.impl);
  }

  /// \cond INTERNAL_DOCS
  /**
   * \param[in] t the tool manager object to store
   **/
  template <>
  template <>
  void visitor< squadt::store_visitor_impl >::visit(tool_manager_impl const& tm) {
    /* Write header */
    out << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                    << "<tool-catalog xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
                    << " xsi:noNamespaceSchemaLocation=\"tool_catalog.xsd\" version=\"1.0\">\n";

    BOOST_FOREACH(tool_manager::tool_list::value_type t, tm.get_tools()) {
      do_visit(*t);
    }

    /* Write footer */
    out << "</tool-catalog>\n";
  }
  /// \endcond

  /**
   * \param[in] t the executor object to store
   **/
  template <>
  template <>
  void visitor< squadt::store_visitor_impl >::visit(execution::executor const& t) {
    do_visit(*t.impl);
  }

  /// \cond INTERNAL_DOCS
  /**
   * \param[in] t the executor object to store
   **/
  template <>
  template <>
  void visitor< squadt::store_visitor_impl >::visit(executor_impl const& e) {
    out << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        << "<squadt-preferences xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" version=\"1.0\">\n";

    out << "<execution-settings>\n"
                    << " <concurrency-constraints maximum-process-total=\""
                    << e.get_maximum_instance_count() << "\"/>\n"
                    << "</execution-settings>\n";
  }
  /// \endcond

  /// \cond INTERNAL_DOCS
  /**
   * \param[in] t the type_registry object to store
   **/
  template <>
  template <>
  void visitor< squadt::store_visitor_impl >::visit(type_registry const& r) {

    out << "<default-actions>\n";

    BOOST_FOREACH(type_registry::actions_for_type::value_type c, r.command_for_type) {
      if (c.second != type_registry::command_none) {
        out << " <associate-commands mime-type=\"" << c.first << "\">\n";
        if (c.second == type_registry::command_system) {
          /// Associated command is specified by the system
          out << "  <command /";
        }
        else {
          /// A custom command is associated with this type
          out << "  <command><![CDATA[" << c.second << "]]></command";
        }

        out << ">\n </associate-commands>\n";
      }
    }

    out << "</default-actions>\n";

    out << "</squadt-preferences>";
  }
  /// \endcond

  /**
   * \param[in] p the processor object to store
   **/
  template <>
  template <>
  void visitor< squadt::store_visitor_impl >::visit(processor const& p) {
    do_visit(*p.impl);
  }

  /// \cond INTERNAL_DOCS
  /**
   * \param[in] p the processor object to store
   **/
  template <>
  template <>
  void visitor< squadt::store_visitor_impl >::visit(processor_impl const& p) {
    out << "<processor";

    if (p.tool_descriptor.get() != 0) {
      out << " tool-name=\"" << p.tool_descriptor->get_name() << "\"";
    }

    if (!p.output_directory.empty()) {
      out << " output-directory=\"" << p.output_directory << "\">";
    }
    else {
      out << ">";
    }

    if (p.selected_input_configuration != 0) {
      tipi::visitors::store(*p.selected_input_configuration, out);
    }

    /* The last received configuration from the tool */
    boost::shared_ptr < tipi::configuration > c = p.current_monitor->get_configuration();

    if (c.get() != 0) {
      tipi::visitors::store(*c, out);
    }

    /* The inputs */
    for (processor::input_list::const_iterator i = p.inputs.begin(); i != p.inputs.end(); ++i) {
      out << "<input id=\"" << std::dec << reinterpret_cast < unsigned long > (i->object.get())
          << "\" identifier=\"" << std::dec << i->identifier << "\"/>\n";
    }

    /* The outputs */
    for (processor::output_list::const_iterator i = p.outputs.begin(); i != p.outputs.end(); ++i) {
      processor_impl::object_descriptor& object(*boost::static_pointer_cast< processor_impl::object_descriptor >(i->object));

      out << "<output id=\"" << std::dec << reinterpret_cast < unsigned long > (&object)
        << "\" format=\"" << object.get_format()
        << "\" location=\"" << p.try_convert_to_store_relative(object.get_location());

      if (object.get_status() != processor::object_descriptor::original) {
        out << "\" identifier=\"" << std::dec << i->identifier;
      }

      out << "\" status=\"" << object.get_status()
          << "\" digest=\"" << object.get_checksum()
          << "\" timestamp=\"" << std::dec << object.get_timestamp() << "\"/>\n";
    }

    out << "</processor>\n";
  }
  /// \endcond

  /**
   * \param[in] p the project_manager object stored
   **/
  template <>
  template <>
  void visitor< squadt::store_visitor_impl >::visit(squadt::project_manager const& p) {
    do_visit(*p.impl);
  }

  /// \cond INTERNAL_DOCS
  /**
   * \param[in] p the processor implementation object to store
   **/
  template <>
  template <>
  void visitor< squadt::store_visitor_impl >::visit(squadt::project_manager_impl const& p) {
    /* Write header */
    out << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
      << "<squadt-project xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
      << " xsi:noNamespaceSchemaLocation=\"project.xsd\" version=\"1.0\" count=\""
      << p.count << "\">\n";

    if (!p.description.empty()) {
      out << "<description>" << p.description << "</description>\n";
    }

    BOOST_FOREACH(project_manager::processor_list::value_type i, p.processors) {
      do_visit(*i);
    }

    out << "</squadt-project>\n";
  }

  template <>
  bool visitor< squadt::store_visitor_impl >::initialise() {
    register_visit_method< const tool >();
    register_visit_method< const tool_manager >();
    register_visit_method< const tool_manager_impl >();
    register_visit_method< const executor >();
    register_visit_method< const executor_impl >();
    register_visit_method< const type_registry >();
    register_visit_method< const processor >();
    register_visit_method< const processor_impl >();
    register_visit_method< const project_manager >();
    register_visit_method< const project_manager_impl >();

    return true;
  }
  /// \endcond
}

