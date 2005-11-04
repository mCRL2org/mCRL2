#include <wx/filename.h>

#include "settings_manager.h"

SettingsManager::SettingsManager(const char* ahome_directory) {
  wxFileName new_name = wxFileName(wxString(ahome_directory, wxConvLocal), wxT(".studio"));

  home_directory     = ahome_directory;

  settings_directory = new_name.GetFullPath().fn_str();

  if (!new_name.DirExists()) {
    /* Create directories */
    new_name.Mkdir();

    /* TODO Copy default settings */
  }

  tool_catalog_name = "tool_catalog.xml";
  project_file_name = "project.xml";
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

