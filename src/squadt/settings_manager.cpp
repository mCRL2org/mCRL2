#include <iostream>
#include <cstdio>

#include "setup.h"

#include "exception.h"
#include "settings_manager.h"

namespace squadt {

  namespace bf = boost::filesystem;

  /** \brief Deactivates default path validation for boost::filesystem */
  bool deactivate_path_validation() {
    bool b = boost::filesystem::path::default_name_check_writable();

    if (b) {
      boost::filesystem::path::default_name_check(bf::no_check);
    }

    return (b);
  }

  /** \brief Triggers deactivation of default path validation during static initialisation */
  bool path_validation_active = deactivate_path_validation();

  const char* settings_manager::default_profile_directory    = ((bf::native(".squadt")) ? ".squadt" : "squadt");

  const char* settings_manager::schema_suffix                = ".xsd";

  const char* settings_manager::tool_catalog_base_name       = "tool_catalog";

  const char* settings_manager::project_definition_base_name = "project.xml";


  void settings_manager::ensure_directories_exist() {
    if (!bf::exists(user_settings_path)) {
      /* Create directories */
      bf::create_directory(user_settings_path);
    }
    else if (!is_directory(user_settings_path)) {
      throw (exception::exception(exception::cannot_access_user_settings_directory, user_settings_path.native_file_string()));
    }
  }

  std::string settings_manager::path_to_system_settings() const {
    return (system_settings_path.native_file_string());
  }

  std::string settings_manager::path_to_default_binaries() const {
    return (TOOL_DIRECTORY);
  }

  std::string settings_manager::path_to_images() const {
    return ((system_settings_path / path("images")).native_file_string());
  }

  std::string settings_manager::path_to_schemas() const {
    return ((system_settings_path / path("schemas")).native_file_string());
  }

  /**
   * \param[in] p1 an incomplete path
   * \param[in] p2 a relative path component
   **/
  std::string settings_manager::path_concatenate(path const& p1, path const& p2) {
    return ((path(p1) / path(p2)).native_file_string());
  }

  /**
   * \param[in] h the users home directory
   * \param[in] u the path relative to the home directory where user specific settings will be stored
   *
   * \throws boost::filesystem::filesystem_error if \p h, \p t or \p DATA_DIRECTORY are no valid paths
   **/
  settings_manager::settings_manager(path const& h, path const& u) : home_directory(h),
                user_settings_path((h / u)),
                system_settings_path(DATA_DIRECTORY, boost::filesystem::no_check) {

    ensure_directories_exist();
  }

  /**
   * \param[in] h the users home directory
   *
   * \throws boost::filesystem::filesystem_error if \p h, \p t or \p DATA_DIRECTORY are no valid paths
   **/
  settings_manager::settings_manager(path const& h) : home_directory(h),
                user_settings_path((h / path(default_profile_directory, boost::filesystem::native))),
                system_settings_path(DATA_DIRECTORY, boost::filesystem::no_check) {

    ensure_directories_exist();
  }

  /**
   * \param[in] h the users home directory
   * \param[in] u the path relative to the home directory where user specific settings will be stored
   **/
  std::auto_ptr < settings_manager > settings_manager::create(path const& h, path const& u) {
    return (std::auto_ptr < settings_manager > (new settings_manager(h, u)));
  }

  /**
   * \param[in] h the users home directory
   **/
  std::auto_ptr < settings_manager > settings_manager::create(path const& h) {
    return (std::auto_ptr < settings_manager > (new settings_manager(h)));
  }

  /**
   * \param[in] node a complete path relative to the path to the user settings path
   *
   * \throws boost::filesystem::filesystem_error if node is not a valid name for a file
   **/
  std::string settings_manager::path_to_user_settings(path const& node) const {
    return ((user_settings_path / node).native_file_string());
  }

  std::string settings_manager::path_to_user_settings() const {
    return (user_settings_path.native_file_string());
  }

  /**
   * \param[in] n a complete path, relative to the path to the images
   *
   * \throws boost::filesystem::filesystem_error if node is not a valid name for a file
   **/
  std::string settings_manager::path_to_images(path const& n) const {
    return ((system_settings_path / path("images") / n).native_file_string());
  }

  /**
   * \param[in] n a complete path, relative to the path to the schemas
   *
   * \throws boost::filesystem::filesystem_error if node is not a valid name for a file
   **/
  std::string settings_manager::path_to_schemas(path const& n) const {
    return ((system_settings_path / path("schemas") / n).native_file_string());
  }

  /**
   * \param[in] n a complete path relative to the path to the system settings path
   *
   * \throws boost::filesystem::filesystem_error if node is not a valid name for a file
   **/
  std::string settings_manager::path_to_system_settings(path const& n) const {
    return ((system_settings_path / n).native_file_string());
  }

  /**
   * \param[in] p a complete path
   */
  std::string settings_manager::append_schema_suffix(path const& p) {
    return(p.native_file_string() + schema_suffix);
  }
}
