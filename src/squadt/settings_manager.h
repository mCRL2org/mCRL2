#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include <string>

#include <boost/noncopyable.hpp>
#include <boost/filesystem/operations.hpp>

namespace squadt {

  class boost::filesystem::path;

  /**
   * \brief Central storage component for paths to common resources
   *
   * Stores the runtime configuration settings for all components.
   *
   * \attention call boost::filesystem::path::default_name_check(bf::no_check) before instantiating
   **/
  class settings_manager : public boost::noncopyable {
    public:

      /** Convenience type to hide shared pointer implementation */
      typedef boost::shared_ptr < settings_manager > ptr;

    private:

      /** Convenience type for paths */
      typedef boost::filesystem::path path;

      /** \brief The home directory of the current user (either xsd or xsd.gz) */
      const path home_directory;

      /** \brief The absolute path the directory where specific configuration files can be found */
      const path user_settings_path;

      /** \brief The absolute path to the directory in which system wide configuration files can be found */
      const path system_settings_path;

    private:

      /** \brief Helper function for constructors that ensures that the appropriate directories exist */
      inline void ensure_directories_exist();

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

      /** \brief Constructor */
      template < typename T1, typename T2 >
      inline settings_manager(const T1&, const T2&);

      /** \brief Constructor with default relative path to profile directory */
      template < typename T >
      inline settings_manager(const T&);

      /** \brief Get the path to where the user independent and default configurations are stored */
      template < typename T >
      inline std::string path_to_user_settings(const T&) const;

      /** \brief Get the path to where the user independent and default configurations are stored */
      inline std::string path_to_user_settings() const;

      /** \brief Get the the settings directory */
      template < typename T >
      inline std::string path_to_system_settings(const T&) const;

      /** \brief Get the the settings directory */
      inline std::string path_to_system_settings() const;

      /** \brief Get the path to image, in the directory containing the image file(s) */
      template < typename T >
      inline std::string path_to_images(const T&) const;

      /** \brief Get the path to image, in the directory containing the image file(s) */
      inline std::string path_to_images() const;

      /** \brief Get the path to the directory containing the XML schema file(s) */
      template < typename T >
      inline std::string path_to_schemas(const T&) const;

      /** \brief Get the path to the directory containing the XML schema file(s) */
      inline std::string path_to_schemas() const;

      /** \brief Have the schema suffix appended to a complete path */
      template < typename T >
      static inline std::string append_schema_suffix(const T&);

      /** \brief Composes a new path by concatenating two paths */
      template < typename T1, typename T2 >
      static inline std::string path_concatenate(const T1 p1, const T2 p2);
  };
}

#endif
