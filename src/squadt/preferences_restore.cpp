#include "preferences_visitors.h"

#include <fstream>

#include <boost/filesystem/operations.hpp>
#include <boost/foreach.hpp>

#include "build_system.h"
#include "settings_manager.h"
#include "tool_manager.tcc"
#include "type_registry.h"
#include "executor.tcc"

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
  void preferences_read_visitor_impl::visit(tool_manager_impl& tm) {
    using namespace boost::filesystem;

    assert(m_reader->is_element("tool-catalog"));

    m_reader->next_element();

    while (!m_reader->is_end_element("tool-catalog")) {
      /* Add a new tool to the list of tools */
      tool new_tool;

      new_tool.accept(*this);

      tm.add_tool(new_tool);
    }
  }

  template <>
  void preferences_read_visitor_impl::visit(executor_impl& e) {
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
            if (m_reader->is_end_element() || m_reader->is_empty_element()) {
              r.register_command(t, type_registry::command_system);
            }
            else {
              m_reader->next_element();

              r.register_command(t, m_reader->get_value_as_string());

              m_reader->next_element();
            }

            m_reader->skip_end_element("command");
          }
          else {
            m_reader->skip_end_element("no-command");
          }

          m_reader->skip_end_element("associate_commands");
        }
      }
    }

    m_reader->skip_end_element("default-actions");
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
       
        for (char const** t = tool_manager_impl::default_tools; *t != 0; ++t) {
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

    b.get_tool_manager_impl()->accept(*this);

    const path miscellaneous_file_name(global_build_system.get_settings_manager()->path_to_user_settings("preferences"));

    if (exists(miscellaneous_file_name)) {
      m_reader.reset(new xml2pp::text_reader(miscellaneous_file_name));

      m_reader->next_element();

      /// read execution preferences
      b.get_executor_impl()->accept(*this);

      /// read default actions
      b.get_type_registry()->accept(*this);

      /// read log settings
      if (m_reader->is_element("logging")) {
        unsigned int filter_level = 1;
      
        if (m_reader->get_attribute(&filter_level, "filter-level")) {
          sip::controller::communicator::get_standard_error_logger()->set_filter_level(static_cast< utility::logger::log_level > (filter_level));
        }
      }
    }
  }

  void preferences_read_visitor::restore(build_system& b, boost::filesystem::path const& p) {
    preferences_read_visitor v(p);
    
    v.impl->visit(b);
  }
}
