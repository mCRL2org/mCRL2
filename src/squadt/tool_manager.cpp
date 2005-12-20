#include <iostream>
#include <fstream>

#include "tool_executor.h"
#include "tool_manager.h"
#include "settings_manager.h"
#include "xml_text_reader.h"
#include "ui_core.h"

#include <boost/filesystem/operations.hpp>

/* A tool executor per instance, will be replaced in the future */
ToolExecutor ToolManager::tool_executor;

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

inline bool ToolManager::Read(std::string file_name) {
  XMLTextReader reader(file_name.c_str());

  /* From here on assume that the XML file satisfies the project-catalog XML schema. */
  if (!reader.StreamIsOpened()) {
    /* TODO Errors should be logged somewhere, but for the time std::cerr suffices */
#ifndef NDEBUG
    std::cerr << "Fatal: Unable to open tool catalog file. (" << file_name << ")\n";
#endif

    return(false);
  }

#if defined(PARSER_SCHEMA_VALIDATION)
  if (!reader.SetSchemaForValidation("schemas/tool_catalog.xsd")) {
    /* TODO Errors should be logged somewhere, but for the time std::cerr suffices */
#ifndef NDEBUG
    std::cerr << "Error: schema is not usable.\n";
#endif

    return (false);
  }
#endif

  bool return_value = true;

  try {
    /* Read root element (tool-catalog) */
    reader.Read();
    reader.Read();
 
    while (!reader.IsEndElement()) {
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

  /* Clean up */
  reader.Close();
  reader.Destroy();

  return (return_value);
}

/* Loads tool configurations from XML file */
bool ToolManager::Load() throw (int) {
  using namespace boost::filesystem;

  std::string catalog_file = settings_manager.GetToolCatalogPath();

  if (!exists(path(catalog_file, no_check))) {
    path ghost_catalog(catalog_file + ".ghost", no_check);
 
    if (exists(ghost_catalog)) {
      /* Recover */
      rename(ghost_catalog, path(catalog_file, no_check));
    }
    else {
      throw (-1);
    }
  }

  return (Read(catalog_file));
}

/* TODO ensure atomicity */
bool ToolManager::Store() const {
  using namespace boost::filesystem;

  std::string   ghost_catalog_file = settings_manager.GetToolCatalogPath();
  std::ofstream catalog_stream;

  ghost_catalog_file.append(".ghost");

  catalog_stream.open(ghost_catalog_file.c_str(), std::ios::out | std::ios::trunc);

  bool return_value = Write(catalog_stream);

  catalog_stream.close();

  /* Replace original with newly generated copy */
  path original_catalog(settings_manager.GetToolCatalogPath(), no_check);
  path temporary(settings_manager.GetToolCatalogPath() + ".obsolete", no_check);
  path ghost_catalog(ghost_catalog_file, no_check);

  rename(original_catalog, temporary);
  rename(ghost_catalog, original_catalog);
  remove(temporary);

  return (return_value);
}

bool ToolManager::Execute(unsigned int tool_identifier, std::string arguments, Specification* plan) {
  using namespace boost::filesystem;

  try {
    std::string command = path(tool_manager.GetTool(tool_identifier)->GetLocation(), no_check).string();

    tool_executor.Execute(command.append(" ").append(arguments).c_str(), plan);
  }
  catch (ExecutionError* exception) {
    return (false);
  }
  catch (ExecutionDelayed* exception) {
    return (true);
  }

  return (true);
}

/* Have the tool executor terminate all running tools */
void ToolManager::TerminateAll() {
  tool_executor.TerminateAll();
}

