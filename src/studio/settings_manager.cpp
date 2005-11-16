#include <iostream>

#include <boost/filesystem/operations.hpp>

#include "settings_manager.h"

/* Compile with the following macros defined:
 *
 *  The following specifies the data directory of the distribution (where xsd files can be found)
 *
 *   SCHEMA_DATA
 *
 *  The following specifies the data directory of the distribution (where initial tool catalog file can be found)
 *
 *   TOOL_DATA
 *
 */
SettingsManager::SettingsManager(const char* ahome_directory) {
  boost::filesystem::path settings_path(ahome_directory, &boost::filesystem::portable_posix_name);

  settings_path /= boost::filesystem::path(".studio", &boost::filesystem::portable_posix_name);

  home_directory     = ahome_directory;
  settings_directory = settings_path.string();

  if (!boost::filesystem::exists(settings_path)) {
    /* Create directories */
    boost::filesystem::create_directory(settings_path);
  }
  else if (!boost::filesystem::is_directory(settings_path)) {
    /* Perhaps a fallback mechanism should be put into place here */
    std::cerr << "Fatal: Cannot write to settings directory.\n";
  }

  settings_path /= boost::filesystem::path(TOOL_CATALOG_NAME);

  try {
    if (!boost::filesystem::exists(settings_path)) {
      /* Copy default settings */
      boost::filesystem::copy_file(boost::filesystem::path(SCHEMA_DATA)/boost::filesystem::path(TOOL_CATALOG_NAME), settings_path);
    }
  }
  catch (...) {
    std::cerr << "Fatal: Cannot load tool configuration.\n";

    exit(1);
  }

  tool_catalog_name = TOOL_CATALOG_NAME;
  project_file_name = PROJECT_STORAGE_NAME;
}

void SettingsManager::SetHomeDirectory(const std::string ahome_directory) {
  home_directory     = settings_directory.erase(0, home_directory.length());
  settings_directory = ahome_directory;

  settings_directory.append(home_directory);

  home_directory     = ahome_directory;
}

/* Sets the base name of the settings directory */
void SettingsManager::SetSettingsDirectory(const std::string asettings_directory) {
  settings_directory = asettings_directory;
}

/* Get the settings directory */
const std::string SettingsManager::GetSettingsDirectory() const {
  return (settings_directory);
}

/* Get the path to the tool catalog file(s) */
const std::string SettingsManager::GetToolCatalogPath() const {
  std::string path(settings_directory);

  return (path.append("/").append(tool_catalog_name));
}

const std::string SettingsManager::GetProjectFileName() const {
  return (project_file_name);
}

