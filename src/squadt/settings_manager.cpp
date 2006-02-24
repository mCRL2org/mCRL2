#include "settings_manager.h"

namespace squadt {

  namespace bf = boost::filesystem;

  /* Deactivate default path validation for boost::filesystem */
  bool deactivate_path_validation() {
    bool b = boost::filesystem::path::default_name_check_writable();

    if (b) {
      boost::filesystem::path::default_name_check(bf::no_check);
    }

    return (b);
  }

  bool path_validation_active = deactivate_path_validation();

  const char* settings_manager::default_profile_directory    = ((bf::native(".squadt")) ? ".squadt" : "squadt");

  const char* settings_manager::schema_suffix                = ".xsd.gz";

  const char* settings_manager::tool_catalog_base_name       = "tool_catalog";

  const char* settings_manager::project_definition_base_name = "project";
}
