#include <iostream>
#include <fstream>

#include <boost/bind.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#include <xml2pp/detail/text_reader.tcc>

#include "tool_manager.h"
#include "settings_manager.tcc"
#include "ui_core.h"

#include "setup.h"

namespace squadt {

  tool_manager::tool_manager() {
  }

  tool_manager::~tool_manager() {
    terminate();
  }

  void tool_manager::write(std::ostream& stream) const {
    const tool_list::const_iterator b = tools.end();
          tool_list::const_iterator i = tools.begin();
 
    /* Write header */
    stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
           << "<tool-catalog xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
           << " xsi:noNamespaceSchemaLocation=\"tool_catalog.xsd\" version=\"1.0\">\n";
 
    while (i != b) {
      (*i)->write(stream);
 
      ++i;
    }
 
    /* Write footer */
    stream << "</tool-catalog>\n";
  }

  tool_manager::ptr tool_manager::read() {
    return (read(global_settings_manager->path_to_user_settings(settings_manager::tool_catalog_base_name)));
  }

  /**
   * @param n the name of the file to read from
   **/
  tool_manager::ptr tool_manager::read(const std::string& n) {
    xml2pp::text_reader::file_name< std::string > f(n);

    if (!bf::exists(bf::path(f.get()))) {
      throw (exception(exception_identifier::failed_loading_object, "squadt tool catalog", f.get()));
    }

    xml2pp::text_reader reader(f);

    reader.set_schema(xml2pp::text_reader::file_name< std::string >(
                            global_settings_manager->path_to_schemas(
                                    settings_manager::append_schema_suffix(
                                            settings_manager::tool_catalog_base_name))));
    return (read(reader));
  }

  /**
   * @param r an XML text reader to use to read data from
   **/
  tool_manager::ptr tool_manager::read(xml2pp::text_reader& r) {

    tool_manager::ptr new_tool_manager(new tool_manager());
 
    /* Read root element (tool-catalog) */
    r.read();
    r.read();
  
    while (!r.is_end_element()) {
      /* Add a new tool to the list of tools */
      new_tool_manager->tools.push_back(tool::read(r));
    }
 
    return (new_tool_manager);
  }

  /**
   * @param t the tool that is to be run
   * @param p the processor that should be passed the feedback of execution
   **/
  void tool_manager::run(tool& t, processor* p) const {
    using namespace boost::filesystem;
 
    std::string command = t.get_location();
 
    /* TODO contact executor to execute */
  }

  void tool_manager::terminate() {
    /* TODO signal executor to terminate the processes related to this tool manager */
  }
}

