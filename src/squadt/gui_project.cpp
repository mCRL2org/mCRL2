#include "gui_main.h"
#include "gui_project.h"
#include "gui_resources.h"
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
      processor_view       = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS|wxTR_SINGLE|wxSUNKEN_BORDER);
      process_display_view = new wxPanel(this, wxID_ANY);

      processor_view->SetImageList(format_icon_list);
      processor_view->AddRoot(wxT("Store"));

      SplitVertically(processor_view, process_display_view);

      /* Connect event handlers */
      processor_view->Connect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(project::on_tree_item_activate));
    }

    /**
     * @param e a reference to a tree event object
     **/
    void project::on_tree_item_activate(wxTreeEvent& e) {
    }

    wxString project::get_name() const {
      return (wxString(manager->get_name().c_str(), wxConvLocal));
    }
  }
}
