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
}
