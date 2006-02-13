#include <iostream>
#include <cstdio>

#include <boost/filesystem/operations.hpp>

#include "settings_manager.h"

namespace squadt {

  #define TOOL_CATALOG_SCHEMA    "tool_catalog.xsd.gz"
  #define PROJECT_STORAGE_SCHEMA "project.xsd.gz"

  #define TOOL_CATALOG_NAME      "tool_catalog.xml"
  #define PROJECT_STORAGE_NAME   "project.xml"

  /* The default name of the profile directory */
  #if ! defined(PROFILE_DIRECTORY)
  # define PROFILE_DIRECTORY ".squadt"
  #endif

  /* Profile directory for braindead systems on which the other one is not a valid directory name */
  #if ! defined(YROTCERID_ELIFORP)
  # define YROTCERID_ELIFORP "squadt"
  #endif

  std::string SettingsManager::default_profile_directory = SettingsManager::GetDefaultProfileDirectory();

  std::string SettingsManager::GetDefaultProfileDirectory() {
    using namespace boost::filesystem;

    try {
      path profile_path(PROFILE_DIRECTORY);

      return (PROFILE_DIRECTORY);
    }
    catch (...) {
      return (YROTCERID_ELIFORP);
    }
  }

  /* Compile with the following macros defined:
   *
   *  The following specifies the data directory of the distribution (where xsd files can be found)
   *
   *   DATA_DIRECTORY where:
   *
   *    /schemas        schema files can be found
   *
   *    /pixmaps        image data can be found
   *
   *    /configuration       the initial tool catalog (that contain specifications for the mCRL2 toolset)
   *
   */
  SettingsManager::SettingsManager(const char* ahome_directory, const char* profile_directory) {
    using namespace boost::filesystem;

    if (ahome_directory != 0) {
      path settings_path(ahome_directory, no_check);

      if (profile_directory != 0) {
        settings_path /= default_profile_directory;
      }

      home_directory     = ahome_directory;
      settings_directory = settings_path.string();

      if (!exists(settings_path)) {
        /* Create directories */
        create_directory(settings_path);
      }
      else if (!is_directory(settings_path)) {
        /* Perhaps a fallback mechanism should be put into place here */
        std::cerr << "Fatal: Cannot write to settings directory.\n";
      }
   
      settings_path /= path(TOOL_CATALOG_NAME);
   
      try {
        if (!exists(settings_path)) {
          /* Copy default settings */
          copy_file(path(DATA_DIRECTORY, no_check)/ path ("configuration") / path(TOOL_CATALOG_NAME), settings_path);
        }
      }
      catch (...) {
        std::cerr << "Fatal: Cannot load tool configuration.\n";
   
        exit(1);
      }
    }
    else {
      home_directory     = "";
      settings_directory = "";
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

  /* Get the path to where the usersettings are stored */
  std::string SettingsManager::GetSettingsPath() const {
    return (settings_directory);
  }

  /* Get the path to where the images are stored */
  std::string SettingsManager::GetImagePath() const {
    using namespace boost::filesystem;

    return ((path(DATA_DIRECTORY, no_check) / path ("images")).string());
  }

  /* Get the path image in the directory where all images are stored */
  std::string SettingsManager::GetImagePath(const std::string image) const {
    using namespace boost::filesystem;

    return ((path(DATA_DIRECTORY, no_check) / path ("images") / path(image, no_check)).string());
  }

  /* Get the path to where the XML schemas are stored */
  std::string SettingsManager::GetSchemaPath() const {
    using namespace boost::filesystem;

    return ((path(DATA_DIRECTORY, no_check) / path ("schemas")).string());
  }

  /* Get the path to where the user independent and default configurations are stored */
  std::string SettingsManager::GetConfigurationPath() const {
    using namespace boost::filesystem;

    return ((path(DATA_DIRECTORY, no_check) / path ("configuration")).string());
  }

  /* Get the path to the tool catalog file(s) */
  std::string SettingsManager::GetToolCatalogPath() const {
    using namespace boost::filesystem;

    path apath(settings_directory, no_check);

    return ((apath / path(tool_catalog_name, no_check)).string());
  }

  std::string SettingsManager::GetProjectFileName() const {
    return (project_file_name);
  }

  const std::string SettingsManager::GetLogFileName() const {
    using namespace boost::filesystem;

    path a_path(settings_directory, no_check);

    return ((a_path / path("log")).string());
  }
}

