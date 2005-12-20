#ifndef CONFIG_H
#define CONFIG_H

#include <string>

/*
 * Stores the runtime configuration settings for all components.
 */
class SettingsManager {
  private:

    std::string home_directory;          /* Users home directory */
    std::string settings_directory;      /* Directory in which project independent settings are stored */
    std::string tool_catalog_name;       /* The name of the tool catalog file */
    std::string project_file_name;       /* The base name of a project file */

    static std::string default_profile_directory;

  protected:

    /* Determine the default name that is used for the profile directory (depends on host system) */
    static std::string SettingsManager::GetDefaultProfileDirectory();

  public:

    SettingsManager(const char* the_home_directory, const char* profile_directory = default_profile_directory.c_str());

    /* The personal directory for the user */
    void SetHomeDirectory(const std::string);

    /* Set the (basename) of the tool catalog file (name may not contain directory separators) */
    inline void SetToolCatalogName(const std::string name) {
      tool_catalog_name = name;
    }

    /* The (base)name of the directory in which settings are stored */
    void SetSettingsDirectory(const std::string);

    /* Get the the settings directory */
    std::string GetSettingsPath() const;

    /* Get the path to the directory containing the image file(s) */
    std::string GetImagePath() const;

    /* Get the path to image, in the directory containing the image file(s) */
    std::string GetImagePath(const std::string image) const;

    /* Get the path to the directory containing the XML schema file(s) */
    std::string GetSchemaPath() const;

    /* Get the path to where the user independent and default configurations are stored */
    std::string GetConfigurationPath() const;

    /* Get the path to the tool catalog file(s) */
    std::string GetToolCatalogPath() const;

    /* Get the base name of a project file */
    std::string GetProjectFileName() const;

    const std::string GetLogFileName() const;
};

#endif
