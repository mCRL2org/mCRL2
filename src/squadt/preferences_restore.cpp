#include "preferences_visitors.h"

#include <fstream>

#include <boost/filesystem/operations.hpp>
#include <boost/foreach.hpp>

#include "build_system.h"
#include "tool_manager.h"
#include "settings_manager.h"
#include "type_registry.h"
#include "executor.h"

#include <xml2pp/text_reader.h>

#include <utility/visitor.h>

namespace squadt {

  class read_preferences_visitor_impl : public utility::visitor< read_preferences_visitor, void, true > {

    private:

      std::auto_ptr < xml2pp::text_reader > m_reader;

    public:

      read_preferences_visitor_impl(boost::filesystem::path const&);

      /** \brief Reads state for objects of type T */
      template < typename T >
      void visit(T&);
  };

  /**
   * \param[in] b reference to a build_system instance
   * \param[in] p a path to the file from which to read
   **/
  read_preferences_visitor::read_preferences_visitor(boost::filesystem::path const& p) :
                        impl(new read_preferences_visitor_impl(p)) {
  }

  /**
   * \param[in] b reference to a build_system instance
   * \param[in] p a path to the file from which to read
   **/
  read_preferences_visitor_impl::read_preferences_visitor_impl(boost::filesystem::path const& p) {
  }

  template <>
  void read_preferences_visitor_impl::visit(tool& t) {
    if (!(m_reader->get_attribute(&t.name, "name") && m_reader->get_attribute(&t.location, "location"))) {
      throw (exception::exception(exception::required_attributes_missing, "tool"));
    }

    if (!m_reader->is_end_element()) {
      m_reader->next_element();

      t.set_capabilities(sip::tool::capabilities::read(*m_reader));
    }

    m_reader->skip_end_element("tool");
  }

  template <>
  void read_preferences_visitor_impl::visit(tool_manager& tm) {
    using namespace boost::filesystem;

    const path file_name(global_build_system.get_settings_manager()->path_to_user_settings(settings_manager::tool_catalog_base_name));

    if (!exists(file_name)) {
      /* Write the default configuration */;
      boost::format f(" <tool name=\"%s\" location=\"%s\"/>\n");

      try {
        std::ofstream o(file_name.native_file_string().c_str());
       
        o << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
          << "<tool-catalog xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"tool_catalog.xsd\" version=\"1.0\">\n";
       
        path default_path(global_build_system.get_settings_manager()->path_to_default_binaries());
       
        for (char const** t = tool_manager::default_tools; *t != 0; ++t) {
          o << (f % *t % (default_path / path(*t)).native_file_string());
        }
       
        o << "</tool-catalog>\n";
       
        o.close();
      }
      catch (...) {
        throw (exception::exception(exception::failed_loading_object, "tool catalog", file_name.native_file_string()));
      }
    }

    m_reader.reset(new xml2pp::text_reader(file_name));

    m_reader->set_schema(boost::filesystem::path(
              global_build_system.get_settings_manager()->path_to_schemas(
                                  settings_manager::append_schema_suffix(
                                          settings_manager::tool_catalog_base_name))));

    /* Read root element (tool-catalog) */
    m_reader->next_element();

    while (!m_reader->is_end_element("tool-catalog")) {
      /* Add a new tool to the list of tools */
      boost::shared_ptr < tool > new_tool(new tool);

      new_tool->accept(*this);

      tm.tools.push_back(new_tool);
    }
  }

  template <>
  void read_preferences_visitor_impl::visit(executor& e) {
  }

  template <>
  void read_preferences_visitor_impl::visit(type_registry& r) {
  }

  template <>
  void read_preferences_visitor_impl::visit(build_system& b) {
    b.get_tool_manager()->accept(*this);
    b.get_executor()->accept(*this);
    b.get_type_registry()->accept(*this);
  }

  void read_preferences_visitor::restore(build_system& b, boost::filesystem::path const& p) {
    read_preferences_visitor v(p);
    
    v.impl->visit(b);
  }
}
