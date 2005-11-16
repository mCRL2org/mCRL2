#ifndef CONFIG_H
#define CONFIG_H

#include <string>

#define TOOL_CATALOG_SCHEMA    "tool_catalog.xsd.gz"
#define PROJECT_STORAGE_SCHEMA "project.xsd.gz"

#define TOOL_CATALOG_NAME      "tool_catalog.xml"
#define PROJECT_STORAGE_NAME   "project.xml"

/*
 * Stores the runtime configuration settings for all components.
 */
class SettingsManager {
  private:

    std::string home_directory;          /* Users home directory */
    std::string settings_directory;      /* Directory in which project independent settings are stored */
    std::string tool_catalog_name;       /* The name of the tool catalog file */
    std::string project_file_name;       /* The base name of a project file */

  public:

    SettingsManager(const char* the_home_directory);

    /* The personal directory for the user */
    void SetHomeDirectory(const std::string);

    /* The (base)name of the directory in which settings are stored */
    void SetSettingsDirectory(const std::string);

    /* Get the the settings directory */
    const std::string GetSettingsDirectory() const;

    /* Get the path to the tool catalog file(s) */
    const std::string GetToolCatalogPath() const;

    /* Get the base name of a project file */
    const std::string GetProjectFileName() const;
};

#endif
