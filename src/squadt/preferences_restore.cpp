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

  class preferences_read_visitor_impl : public utility::visitor< preferences_read_visitor, void, true > {

    private:

      std::auto_ptr < xml2pp::text_reader > m_reader;

    public:

      preferences_read_visitor_impl(boost::filesystem::path const&);

      /** \brief Reads state for objects of type T */
      template < typename T >
      void visit(T&);
  };

  /**
   * \param[in] b reference to a build_system instance
   * \param[in] p a path to the file from which to read
   **/
  preferences_read_visitor::preferences_read_visitor(boost::filesystem::path const& p) :
                        impl(new preferences_read_visitor_impl(p)) {
  }

  /**
   * \param[in] b reference to a build_system instance
   * \param[in] p a path to the file from which to read
   **/
  preferences_read_visitor_impl::preferences_read_visitor_impl(boost::filesystem::path const& p) {
  }

  template <>
  void preferences_read_visitor_impl::visit(tool& t) {
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
  void preferences_read_visitor_impl::visit(tool_manager& tm) {
    using namespace boost::filesystem;

    assert(m_reader->is_element("tool-catalog"));

    m_reader->next_element();

    while (!m_reader->is_end_element("tool-catalog")) {
      /* Add a new tool to the list of tools */
      boost::shared_ptr < tool > new_tool(new tool);

      new_tool->accept(*this);

      tm.tools.push_back(new_tool);
    }
  }

  template <>
  void preferences_read_visitor_impl::visit(executor& e) {
    assert(m_reader->is_element("execution-settings"));

    unsigned int maximum_instance_count = 3;

    while (!m_reader->is_end_element("execution-settings")) {
      m_reader->next_element();
    
      if (m_reader->is_element("concurrency-constraints")) {
        if (m_reader->get_attribute(&maximum_instance_count, "maximum-process-total")) {
          e.set_maximum_instance_count(maximum_instance_count);
        }
      }
    }

    m_reader->skip_end_element("execution-settings");
  }

  template <>
  void preferences_read_visitor_impl::visit(type_registry& r) {
    assert(m_reader->is_element("default-actions"));

    while (!m_reader->is_end_element("default-actions")) {
      m_reader->next_element();

      if (m_reader->is_element("associate-commands")) {
        while (!m_reader->is_end_element("associate-commands")) {
          mime_type t(m_reader->get_attribute_as_string("mime-type"));

          m_reader->next_element();

          if (m_reader->is_element("command")) {
            if (m_reader->get_attribute("system")) {
              r.register_command(t, type_registry::command_system);
            }
            else if (m_reader->get_attribute("none")) {
              r.register_command(t, type_registry::command_none);
            }
            else {
              m_reader->next_element();

              r.register_command(t, m_reader->get_value_as_string());

              m_reader->next_element();
            }

            m_reader->skip_end_element("command");
          }

          m_reader->skip_end_element("associate_commands");
        }
      }
    }
  }

  template <>
  void preferences_read_visitor_impl::visit(build_system& b) {
    using namespace boost::filesystem;

    const path tool_manager_file_name(global_build_system.get_settings_manager()->path_to_user_settings(settings_manager::tool_catalog_base_name));

    if (!exists(tool_manager_file_name)) {
      /* Write the default configuration */;
      boost::format f(" <tool name=\"%s\" location=\"%s\"/>\n");

      try {
        std::ofstream o(tool_manager_file_name.native_file_string().c_str());
       
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
        throw (exception::exception(exception::failed_loading_object, "tool catalog", tool_manager_file_name.native_file_string()));
      }
    }

    m_reader.reset(new xml2pp::text_reader(tool_manager_file_name));

    /* Read root element (tool-catalog) */
    m_reader->set_schema(boost::filesystem::path(
              global_build_system.get_settings_manager()->path_to_schemas(
                                  settings_manager::append_schema_suffix(
                                          settings_manager::tool_catalog_base_name))));

    b.get_tool_manager()->accept(*this);

    const path miscellaneous_file_name(global_build_system.get_settings_manager()->path_to_user_settings("preferences"));

    if (exists(miscellaneous_file_name)) {
      m_reader.reset(new xml2pp::text_reader(miscellaneous_file_name));

      m_reader->next_element();

      b.get_executor()->accept(*this);
      b.get_type_registry()->accept(*this);
    }
  }

  void preferences_read_visitor::restore(build_system& b, boost::filesystem::path const& p) {
    preferences_read_visitor v(p);
    
    v.impl->visit(b);
  }
}
