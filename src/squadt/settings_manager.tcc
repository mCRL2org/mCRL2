#ifndef SETTINGS_MANAGER_TCC
#define SETTINGS_MANAGER_TCC

#include <iostream>
#include <cstdio>

#include "setup.h"

#include "exception.h"
#include "settings_manager.h"

namespace squadt {

  namespace bf = boost::filesystem;

  /**
   * @param[in] h the users home directory
   * @param[in] u the path relative to the home directory where user specific settings will be stored
   *
   * \throws boost::filesystem::filesystem_error if \p h, \p t or \p DATA_DIRECTORY are no valid paths
   **/
  template < typename T1, typename T2 >
  settings_manager::settings_manager(const T1& h, const T2& u) : home_directory(h, bf::no_check),
                user_settings_path((bf::path(h) / bf::path(u, bf::native))),
                system_settings_path(DATA_DIRECTORY, bf::no_check) {

    ensure_directories_exist();
  }

  /**
   * @param[in] h the users home directory
   *
   * \throws boost::filesystem::filesystem_error if \p h, \p t or \p DATA_DIRECTORY are no valid paths
   **/
  template < typename T >
  settings_manager::settings_manager(const T& h) : home_directory(h, bf::no_check),
                user_settings_path((bf::path(h) / bf::path(default_profile_directory, bf::native))),
                system_settings_path(DATA_DIRECTORY, bf::no_check) {

    ensure_directories_exist();
  }

  inline void settings_manager::ensure_directories_exist() {
    if (!bf::exists(user_settings_path)) {
      /* Create directories */
      bf::create_directory(user_settings_path);
    }
    else if (!is_directory(user_settings_path)) {
      throw (exception::exception(exception::cannot_access_user_settings_directory, user_settings_path.native_file_string()));
    }
  }

  /**
   * @param[in] node a complete path relative to the path to the user settings path
   *
   * \throws boost::filesystem::filesystem_error if node is not a valid name for a file
   **/
  template < typename T >
  inline std::string settings_manager::path_to_user_settings(const T& node) const {
    return ((user_settings_path / path(node)).native_file_string());
  }

  /**
   * @param[in] node a complete path relative to the path to the user settings path
   **/
  template < >
  inline std::string settings_manager::path_to_user_settings(const path& node) const {
    return ((user_settings_path / node).native_file_string());
  }

  inline std::string settings_manager::path_to_user_settings() const {
    return (user_settings_path.native_file_string());
  }

  /**
   * @param[in] n a complete path relative to the path to the system settings path
   *
   * \throws boost::filesystem::filesystem_error if node is not a valid name for a file
   **/
  template < typename T >
  inline std::string settings_manager::path_to_system_settings(const T& n) const {
    return ((system_settings_path / path(n)).native_file_string());
  }

  /**
   * @param[in] n a complete path relative to the path to the system settings path
   **/
  template < >
  inline std::string settings_manager::path_to_system_settings(const path& n) const {
    return ((system_settings_path / n).native_file_string());
  }

  inline std::string settings_manager::path_to_system_settings() const {
    return (system_settings_path.native_file_string());
  }

  inline std::string settings_manager::path_to_default_binaries() const {
    return (TOOL_DIRECTORY);
  }

  /**
   * @param[in] n a complete path, relative to the path to the images
   *
   * \throws boost::filesystem::filesystem_error if node is not a valid name for a file
   **/
  template < typename T >
  inline std::string settings_manager::path_to_images(const T& n) const {
    return ((system_settings_path / path("images") / path(n)).native_file_string());
  }

  /**
   * @param[in] n a complete path, relative to the path to the images
   **/
  template < >
  inline std::string settings_manager::path_to_images(const path& n) const {
    return ((system_settings_path / path("images") / n).native_file_string());
  }

  inline std::string settings_manager::path_to_images() const {
    return ((system_settings_path / path("images")).native_file_string());
  }

  /**
   * @param[in] n a complete path, relative to the path to the schemas
   *
   * \throws boost::filesystem::filesystem_error if node is not a valid name for a file
   **/
  template < typename T >
  inline std::string settings_manager::path_to_schemas(const T& n) const {
    return ((system_settings_path / path("schemas") / path(n)).native_file_string());
  }

  /**
   * @param[in] n a complete path, relative to the path to the schemas
   **/
  template < >
  inline std::string settings_manager::path_to_schemas(const path& n) const {
    return ((system_settings_path / path("schemas") / n).native_file_string());
  }

  inline std::string settings_manager::path_to_schemas() const {
    return ((system_settings_path / path("schemas")).native_file_string());
  }

  /**
   * @param[in] p a complete path
   */
  template < >
  inline std::string settings_manager::append_schema_suffix(const std::string& p) {
    return(p + schema_suffix);
  }

  /**
   * @param[in] p a complete path
   */
  template < typename T >
  inline std::string settings_manager::append_schema_suffix(const T& p) {
    return(std::string(p) + schema_suffix);
  }

  /**
   * @param[in] p1 an incomplete path
   * @param[in] p2 a relative path component
   **/
  template < typename T1, typename T2 >
  inline std::string settings_manager::path_concatenate(const T1 p1, const T2 p2) {
    return ((path(p1) / path(p2)).native_file_string());
  }
}
#endif

