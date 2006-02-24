#include <iostream>
#include <cstdio>

#include "exception.tcc"
#include "settings.h"
#include "settings_manager.h"

namespace squadt {

  namespace bf = boost::filesystem;

  /**
   * @param h the users home directory
   * @param u the path relative to the home directory where user specific settings will be stored
   *
   * \throws boost::filesystem::filesystem_error if \p h, \p t or \p DATA_DIRECTORY are no valid paths
   **/
  template < typename T1, typename T2 >
  settings_manager::settings_manager(const T1& h, const T2& u) : home_directory(bf::path(h, bf::no_check)),
                user_settings_path((bf::path(h) / bf::path(u, bf::native))),
                system_settings_path(bf::path(DATA_DIRECTORY, bf::no_check)) {

    ensure_directories_exist();
  }

  /**
   * @param h the users home directory
   * @param u the path relative to the home directory where user specific settings will be stored
   *
   * \throws boost::filesystem::filesystem_error if \p h, \p t or \p DATA_DIRECTORY are no valid paths
   **/
  template < typename T >
  settings_manager::settings_manager(const T& h) : home_directory(bf::path(h, bf::no_check)),
                user_settings_path((bf::path(h) / bf::path(default_profile_directory, bf::native))),
                system_settings_path(bf::path(DATA_DIRECTORY)) {

    ensure_directories_exist();
  }

  inline void settings_manager::ensure_directories_exist() {
    if (!bf::exists(user_settings_path)) {
      /* Create directories */
      bf::create_directory(user_settings_path);
    }
    else if (!is_directory(user_settings_path)) {
      throw (exception(exception::cannot_access_user_settings_directory, user_settings_path.native_file_string()));
    }
   
    bf::path catalog_path = path_to_user_settings(tool_catalog_base_name);

    if (!bf::exists(catalog_path)) {
      /* Copy default settings */
      bf::copy_file(system_settings_path / bf::path("configuration") / bf::path(tool_catalog_base_name), catalog_path);
    }
  }

  /**
   * \param[in] node a complete path relative to the path to the user settings path
   *
   * \throws boost::filesystem::filesystem_error if node is not a valid name for a file
   **/
  template < typename T >
  inline std::string settings_manager::path_to_user_settings(const T& node) const {
    return ((user_settings_path / path(node)).native_file_string());
  }

  inline std::string settings_manager::path_to_user_settings() const {
    return (user_settings_path.native_file_string());
  }

  /**
   * \param[in] node a complete path relative to the path to the system settings path
   *
   * \throws boost::filesystem::filesystem_error if node is not a valid name for a file
   **/
  template < typename T >
  inline std::string settings_manager::path_to_system_settings(const T& node) const {
    return ((system_settings_path / path(node)).native_file_string());
  }

  inline std::string settings_manager::path_to_system_settings() const {
    return (system_settings_path.native_file_string());
  }

  /**
   * \param[in] node a complete path, relative to the path to the images
   *
   * \throws boost::filesystem::filesystem_error if node is not a valid name for a file
   **/
  template < typename T >
  inline std::string settings_manager::path_to_images(const T& node) const {
    return ((system_settings_path / path("images") / path(node)).native_file_string());
  }

  inline std::string settings_manager::path_to_images() const {
    return ((system_settings_path / path("images")).native_file_string());
  }

  /**
   * \param[in] node a complete path, relative to the path to the schemas
   *
   * \throws boost::filesystem::filesystem_error if node is not a valid name for a file
   **/
  template < typename T >
  inline std::string settings_manager::path_to_schemas(const T& node) const {
    return ((system_settings_path / path("schemas") / path(node)).native_file_string());
  }

  inline std::string settings_manager::path_to_schemas() const {
    return ((system_settings_path / path("schemas")).native_file_string());
  }

  /**
   * \param[in,out] path a complete path
   */
  inline std::string& settings_manager::append_schema_suffix(std::string& path) {
    path += schema_suffix;

    return(path);
  }

  /**
   * \param[in] p1 an incomplete path
   * \param[in] p2 a relative path component
   **/
  template < typename T1, typename T2 >
  inline std::string settings_manager::path_concatenate(const T1 p1, const T2 p2) {
    return ((path(p1) / path(p2)).native_file_string());
  }
}

