#include "settings_manager.h"

namespace squadt {

  namespace bf = boost::filesystem;

  class deactivate_path_validation {
    public:
      deactivate_path_validation() {
        /* Needed for settings manager */
        boost::filesystem::path::default_name_check(bf::no_check);
      }
  };

  deactivate_path_validation dummy = deactivate_path_validation();

  const char* settings_manager::default_profile_directory    = ((bf::native(".squadt")) ? ".squadt" : "squadt");

  const char* settings_manager::schema_suffix                = ".xsd.gz";

  const char* settings_manager::tool_catalog_base_name       = "tool_catalog";

  const char* settings_manager::project_definition_base_name = "project";
}
