#include "gui_main.h"
#include "gui_project.h"
#include "project_manager.h"

namespace squadt {
  namespace GUI {

    /**
     * @param p the parent window
     * @param l is the path
     *
     * Creates a project_manager object that is either:
     *  - read from l, if l is a project store
     *  - the default project_manager, and l is the new project store
     **/
    project::project(wxWindow* p, const boost::filesystem::path& l) : wxSplitterWindow(p, wxID_ANY), manager(project_manager::create(l)) {
      build();
    }

    /**
     * @param p the parent window
     * @param l is the path
     * @param d is a description for this project
     *
     * Creates a project_manager object that is either:
     *  - read from l, if l is a project store
     *  - the default project_manager, and l is the new project store
     **/
    project::project(wxWindow* p, const boost::filesystem::path& l, const std::string& d) : wxSplitterWindow(p, wxID_ANY), manager(project_manager::create(l)) {
      if (!d.empty()) {
        manager->set_description(d);
        manager->write();
      }

      build();
    }

    void project::build() {
      
    }

    wxString project::get_name() const {
      return (wxString(manager->get_name().c_str(), wxConvLocal));
    }
  }
}
