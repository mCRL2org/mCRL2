#include <iostream>
#include <fstream>

#include <boost/bind.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#include <xml2pp/detail/text_reader.tcc>

#include "tool.h"
#include "executor.h"
#include "specification.h"
#include "tool_manager.h"
#include "settings_manager.tcc"
#include "ui_core.h"

namespace squadt {

  /* A tool executor per instance, will be replaced in the future */
  executor ToolManager::tool_executor;

  ToolManager::ToolManager() {
  }

  ToolManager::~ToolManager() {
    const std::list < Tool* >::const_iterator b = tools.end();
          std::list < Tool* >::const_iterator i = tools.begin();
 
    while (i != b) {
      delete *i;
 
      ++i;
    }
  }

  /* Write project information to stream */
  void ToolManager::Print(std::ostream& stream) const {
    const std::list < Tool* >::const_iterator b = tools.end();
          std::list < Tool* >::const_iterator i = tools.begin();
          unsigned int                       n = 0;
 
    while (i != b) {
      stream << "Configured as tool <" << n++ << "> :" << std::endl << std::endl;
 
      (*i)->Print(stream);
 
      ++i;
    }
  }

  inline bool ToolManager::Write(std::ostream& stream) const {
    const std::list < Tool* >::const_iterator b = tools.end();
          std::list < Tool* >::const_iterator i = tools.begin();
 
    /* Write header */
    stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
           << "<tool-catalog xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
           << " xsi:noNamespaceSchemaLocation=\"tool_catalog.xsd\" version=\"1.0\">\n";
 
    while (i != b) {
      (*i)->Write(stream);
 
      ++i;
    }
 
    /* Write footer */
    stream << "</tool-catalog>\n";
 
    return (true);
  }

  inline bool ToolManager::Read(const std::string& name) {
    xml2pp::text_reader::file_name< std::string > f(name);

    xml2pp::text_reader reader(f);

    reader.set_schema(xml2pp::text_reader::file_name< std::string >(
                            global_settings_manager->path_to_schemas(
                                    settings_manager::append_schema_suffix(
                                            settings_manager::tool_catalog_base_name))));
 
    bool return_value = true;
 
    try {
      /* Read root element (tool-catalog) */
      reader.read();
      reader.read();
  
      while (!reader.is_end_element()) {
        Tool* new_tool = new Tool();
 
        new_tool->Read(reader);
 
        /* Add a new tool to the list of tools */
        tools.push_back(new_tool);
      }
    }
    catch(int status) {
      /* Process error, or end of file */
      if (status != 0) {
        std::cerr << "Fatal: Parse error(s) or unknown exception with parsing tool_catalog.xsd.\n";
 
        return_value = false;
      }
    }
 
    return (return_value);
  }

  /* Loads tool configurations from XML file */
  bool ToolManager::Load() throw () {
    using namespace boost::filesystem;
 
    std::string catalog_file = global_settings_manager->path_to_user_settings(settings_manager::tool_catalog_base_name);

    if (!exists(path(catalog_file, no_check))) {
      path ghost_catalog(catalog_file + ".ghost", no_check);
  
      if (exists(ghost_catalog)) {
        /* Recover */
        rename(ghost_catalog, path(catalog_file, no_check));
      }
      else {
        throw (exception(exception_identifier::cannot_load_tool_configuration, catalog_file));
      }
    }
 
    return (Read(catalog_file));
  }

  /* TODO ensure atomicity */
  bool ToolManager::Store() const {
    using namespace boost::filesystem;
 
    std::string   catalog_file = global_settings_manager->path_to_user_settings(settings_manager::tool_catalog_base_name);
    std::ofstream catalog_stream(catalog_file.c_str(), std::ios::out | std::ios::trunc);
    path          old_catalog_path = path(catalog_file + ".ghost");
 
    rename(path(catalog_file), old_catalog_path);

    bool return_value = Write(catalog_stream);
 
    catalog_stream.close();
 
    remove(old_catalog_path);
 
    return (return_value);
  }

  void ToolManager::Execute(unsigned int tool_identifier, std::string arguments, Specification* p) const {
    using namespace boost::filesystem;
 
    std::string command = path(tool_manager.GetTool(tool_identifier)->GetLocation(), no_check).string();
 
    tool_executor.execute(command.append(" ").append(arguments), p);
  }

  /* Have the tool executor terminate all running tools */
  void ToolManager::TerminateAll() {
    tool_executor.terminate();
  }
}

