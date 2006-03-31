#include "gui_main.h"
#include "gui_project.h"
#include "project_manager.h"

namespace squadt {
  namespace GUI {

    /**
     * @param l is the path
     **/
    project::project(boost::filesystem::path& l) : manager(project_manager::create(l)) {
    }
  }
}
