#include <boost/bind.hpp>
#include <boost/filesystem/convenience.hpp>

#include "gui_main.h"
#include "gui_project.h"
#include "gui_resources.h"
#include "project_manager.h"
#include "gui_dialog_base.h"
#include "gui_dialog_project.h"
#include "gui_dialog_processor.h"

#include "processor.tcc"

#define cmID_REMOVE     (wxID_HIGHEST + 1)
#define cmID_REBUILD    (wxID_HIGHEST + 2)
#define cmID_CLEAN      (wxID_HIGHEST + 3)
#define cmID_DERIVE     (wxID_HIGHEST + 4)
#define cmID_DETAILS    (wxID_HIGHEST + 5)

namespace squadt {
  namespace GUI {

    /**
     * @param[in,out] p a shared pointer to the processor for which process is monitored and reported
     * @param[in] t the tool used by the processor
     **/
    project::node_data::node_data(project& p) : parent(p),
                    target(new processor(boost::bind(&node_data::update_state, this, _1))) {
    }

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

      SetMinimumPaneSize(150);

      SplitVertically(processor_view, process_display_view);

      /* Connect event handlers */
      processor_view->Connect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(project::on_tree_item_activate), 0, this);
      processor_view->Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(project::on_tree_item_activate), 0, this);

      Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(project::on_context_menu_select));
    }

    /**
     * @param e a reference to a tree event object
     **/
    void project::on_tree_item_activate(wxTreeEvent& e) {
      if (processor_view->GetRootItem() != e.GetItem()) {
        spawn_context_menu(reinterpret_cast < node_data* > (processor_view->GetItemData(e.GetItem()))->get_processor().get());
      }
      else {
        dialog::add_to_project dialog(this, wxString(manager->get_project_directory().c_str(), wxConvLocal));

        if (dialog.ShowModal()) {
          using namespace boost::filesystem;

          /* Add to the new project */
          wxTreeItemId i = processor_view->AppendItem(e.GetItem(), wxString(dialog.get_name().c_str(), wxConvLocal), 3);

          /* File does not exist in project directory */
          path destination_path = path(dialog.get_destination());

          copy_file(path(dialog.get_source()), destination_path);

          node_data* monitor = new node_data(*this);

          /* Add the processor to the project */
          manager->add(monitor->get_processor());

          processor_view->SetItemData(i, reinterpret_cast < wxTreeItemData* > (monitor));

          storage_format f = storage_format_unknown;

          /* TODO more intelligent file format check */
          if (!extension(destination_path).empty()) {
            f = extension(destination_path);

            f.erase(f.begin());
          }

          /* Add the input to the processor, as main input */
          monitor->get_processor()->append_output(f, destination_path.string());

          processor_view->EnsureVisible(i);
        }
      }
    }

    /**
     * @param p a pointer to a processor object
     **/
    void project::spawn_context_menu(processor* p) {
      wxMenu  context_menu;

      context_menu.Append(cmID_REMOVE, wxT("Remove"));
      context_menu.Append(cmID_REBUILD, wxT("Rebuild"));
      context_menu.Append(cmID_CLEAN, wxT("Clean"));
      context_menu.AppendSeparator();
      context_menu.Append(cmID_DERIVE, wxT("Derive ..."));
      context_menu.AppendSeparator();
      context_menu.Append(cmID_DETAILS, wxT("Details"));

      PopupMenu(&context_menu);
    }

    /**
     * @param e a reference to a menu event object
     **/
    void project::on_context_menu_select(wxCommandEvent& e) {
      wxTreeItemId    s = processor_view->GetSelection();
      processor::ptr& p = reinterpret_cast < node_data* > (processor_view->GetItemData(s))->get_processor();

      switch (e.GetId()) {
        case cmID_REMOVE:
          manager->remove(p.get());

          processor_view->Delete(s);
          break;
        case cmID_REBUILD:
          p->flush_outputs();

          p->process();
          break;
        case cmID_CLEAN:
          p->flush_outputs();
          break;
        case cmID_DERIVE: {
          }
          break;
        case cmID_DETAILS: {
            dialog::processor_details dialog(this, wxString(manager->get_project_directory().c_str(), wxConvLocal), processor_view->GetItemText(s));

            if (processor_view->GetItemParent(s) == processor_view->GetRootItem()) {
              /* Add the main input (must exist) */
              dialog.populate_tool_list(p->get_outputs().front()->format);
            }

            if (dialog.ShowModal()) {
              /* Process changes */
            }
          }
          break;
      }
    }

    wxString project::get_name() const {
      return (wxString(manager->get_name().c_str(), wxConvLocal));
    }
  }
}
