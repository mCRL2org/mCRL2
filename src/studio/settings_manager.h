#ifndef CONFIG_H
#define CONFIG_H

#include <string>

/* The default name of the profile directory */
#define PROFILE_DIRECTORY      ".studio"

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

    SettingsManager(const char* the_home_directory, const char* profile_directory = PROFILE_DIRECTORY);

    /* The personal directory for the user */
    void SetHomeDirectory(const std::string);

    /* Set the (basename) of the tool catalog file (name may not contain directory separators) */
    inline void SetToolCatalogName(const std::string name) {
      tool_catalog_name = name;
    }

    /* The (base)name of the directory in which settings are stored */
    void SetSettingsDirectory(const std::string);

    /* Get the the settings directory */
    const std::string GetSettingsDirectory() const;

    /* Get the path to the tool catalog file(s) */
    const std::string GetToolCatalogPath() const;

    /* Get the base name of a project file */
    const std::string GetProjectFileName() const;

    inline const std::string GetLogFileName() const {
      std::string return_value = settings_directory;

      return (return_value.append("/log"));
    }
};

#endif
