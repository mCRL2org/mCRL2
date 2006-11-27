#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include <string>

#include <boost/noncopyable.hpp>
#include <boost/filesystem/operations.hpp>

namespace squadt {

  /**
   * \brief Central storage component for paths to common resources
   *
   * Stores the runtime configuration settings for all components.
   *
   * \attention call boost::filesystem::path::default_name_check(boost::filesystem::no_check) before instantiating
   **/
  class settings_manager : public boost::noncopyable {
    public:

      /** Convenience type to hide shared pointer implementation */
      typedef boost::shared_ptr < settings_manager > ptr;

    private:

      /** Convenience type for paths */
      typedef boost::filesystem::path path;

      /** \brief The home directory of the current user */
      const path home_directory;

      /** \brief The absolute path the directory where user specific configuration files can be found */
      const path user_settings_path;

      /** \brief The absolute path to the directory in which system wide configuration files can be found */
      const path system_settings_path;

    private:

      /** \brief Helper function for constructors that ensures that the appropriate directories exist */
      void ensure_directories_exist();

    public:

      /** \brief The suffix of a schema file (either xsd or xsd.gz)*/
      static const char* schema_suffix;

      /** \brief The base name of a catalog file (as well as the XML schema file for a tool catalog) */
      static const char* tool_catalog_base_name;

      /** \brief The base name of a project definition file (as well as the XML schema file for a project definition) */
      static const char* project_definition_base_name;

      /** \brief The name of the profile directory, for user specific settings (located in a users home directory) */
      static const char* default_profile_directory;

    public:

      /** \brief Factory function */
      static std::auto_ptr < settings_manager > create(path const&, path const&);

      /** \brief Factory function */
      static std::auto_ptr < settings_manager > create(path const&);

      /** \brief Constructor */
      settings_manager(path const&, path const&);

      /** \brief Constructor with default relative path to profile directory */
      settings_manager(path const&);

      /** \brief Get the path to where the user independent and default configurations are stored */
      std::string path_to_user_settings(path const& T) const;

      /** \brief Get the path to where the user independent and default configurations are stored */
      std::string path_to_user_settings() const;

      /** \brief Get the settings directory */
      std::string path_to_system_settings(path const&) const;

      /** \brief Get the settings directory */
      std::string path_to_system_settings() const;

      /** \brief Get the path where the default executable binaries are installed */
      std::string path_to_default_binaries() const;

      /** \brief Get the path to image, in the directory containing the image file(s) */
      std::string path_to_images(path const&) const;

      /** \brief Get the path to image, in the directory containing the image file(s) */
      std::string path_to_images() const;

      /** \brief Get the path to the directory containing the XML schema file(s) */
      std::string path_to_schemas(path const&) const;

      /** \brief Get the path to the directory containing the XML schema file(s) */
      std::string path_to_schemas() const;

      /** \brief Have the schema suffix appended to a complete path */
      static std::string append_schema_suffix(path const&);

      /** \brief Composes a new path by concatenating two paths */
      static std::string path_concatenate(path const& p1, path const& p2);
  };
}

#endif
