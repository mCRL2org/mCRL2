#include "preferences_visitors.h"

#include <fstream>

#include <boost/filesystem/operations.hpp>
#include <boost/foreach.hpp>

#include "build_system.h"
#include "tool_manager.h"
#include "settings_manager.h"
#include "type_registry.h"
#include "executor.h"

#include <utility/visitor.h>

namespace squadt {

  class preferences_write_visitor_impl : public utility::visitor< preferences_write_visitor, void > {
    private:

      std::ofstream m_output_stream;

    public:

      preferences_write_visitor_impl(boost::filesystem::path const&);

      /** \brief Writes state for objects of type T */
      template < typename T >
      void visit(T const&);
  };

  /**
   * \param[in] b reference to a build_system instance
   * \param[in] p a path to the file to write to
   **/
  preferences_write_visitor::preferences_write_visitor(boost::filesystem::path const& p) :
                        impl(new preferences_write_visitor_impl(p)) {
  }

  /**
   * \param[in] b reference to a build_system instance
   * \param[in] p a path to the file from which to read
   **/
  preferences_write_visitor_impl::preferences_write_visitor_impl(boost::filesystem::path const& p) {
  }

  template <>
  void preferences_write_visitor_impl::visit(tool const& t) {
    m_output_stream << "<tool name=\"" << t.get_name()
                    << "\" location=\"" << t.get_location() << "\"/>\n";
  }

  template <>
  void preferences_write_visitor_impl::visit(tool_manager const& tm) {
    /* Write header */
    m_output_stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                    << "<tool-catalog xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
                    << " xsi:noNamespaceSchemaLocation=\"tool_catalog.xsd\" version=\"1.0\">\n";
 
    BOOST_FOREACH(tool_manager::tool_list::value_type t, tm.get_tools()) {
      t->accept(*this);
    }
 
    /* Write footer */
    m_output_stream << "</tool-catalog>\n";
  }

  template <>
  void preferences_write_visitor_impl::visit(executor const& e) {
    m_output_stream << "<execution maximum-process-instances=\""
                    << e.get_maximum_instance_count()
                    << "\"/>";
  }

  template <>
  void preferences_write_visitor_impl::visit(type_registry const&) {
  }

  template <>
  void preferences_write_visitor_impl::visit(build_system const& b) {
    const boost::filesystem::path tool_catalog_file_name(global_build_system.get_settings_manager()->path_to_user_settings(settings_manager::tool_catalog_base_name));

    m_output_stream.open(tool_catalog_file_name.native_file_string().c_str(), std::ofstream::out|std::ofstream::trunc);

    b.get_tool_manager()->accept(*this);

    m_output_stream.close();

    const boost::filesystem::path miscellaneous_file_name(global_build_system.get_settings_manager()->path_to_user_settings("preferences"));

    m_output_stream.open(miscellaneous_file_name.native_file_string().c_str(), std::ofstream::out|std::ofstream::trunc);

    m_output_stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                    << "<squadt-preferences xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
                    << " xsi:noNamespaceSchemaLocation=\"tool_catalog.xsd\" version=\"1.0\">\n";

    b.get_executor()->accept(*this);

    b.get_type_registry()->accept(*this);

    m_output_stream << "</squadt-preferences>";

    m_output_stream.close();
  }

  /**
   * \param[in] p a path to the file from which to read
   **/
  void preferences_write_visitor::store(build_system& b, boost::filesystem::path const& p) {
    preferences_write_visitor v(p);
    
    v.impl->visit(b);
  }
}

