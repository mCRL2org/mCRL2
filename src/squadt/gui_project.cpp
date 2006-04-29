#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/filesystem/path.hpp>

#include "gui_main.h"
#include "gui_project.h"
#include "gui_resources.h"
#include "project_manager.h"
#include "gui_dialog_base.h"
#include "gui_tool_display.h"
#include "gui_dialog_project.h"
#include "gui_dialog_processor.h"

#include "processor.tcc"

#define cmID_REMOVE     (wxID_HIGHEST + 1)
#define cmID_REBUILD    (wxID_HIGHEST + 2)
#define cmID_CLEAN      (wxID_HIGHEST + 3)
#define cmID_DETAILS    (wxID_HIGHEST + 4)
#define cmID_TOOLS      (wxID_HIGHEST + 5)

namespace squadt {
  namespace GUI {

    project::builder::builder() {
      Connect(wxEVT_IDLE, wxIdleEventHandler(builder::process));
    }
    
    void project::builder::process(wxIdleEvent&) {
      while (0 < tool_displays.size()) {
        tool_display* t = tool_displays.front();

        tool_displays.pop_front();

        t->instantiate();

        t->Show(true);
        t->GetSizer()->Layout();
      }
    }
    
    void project::builder::schedule_update(tool_display* t) {
      tool_displays.push_back(t);
    }
        
    project::~project() {
      /* Remove the event handler for the builder */
      PopEventHandler();

      manager->write();
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

      /* Update view */
      project_manager::processor_iterator i = manager->get_processor_iterator();

      while (i.valid()) {
        processor::output_object_iterator j = (*i)->get_outputs_iterator();

        while (j.valid()) {
          wxTreeItemId item = processor_view->AppendItem(processor_view->GetRootItem(),
                                                wxString(boost::filesystem::path((*j)->location).leaf().c_str(), wxConvLocal), 3);

          processor_view->SetItemData(item, new node_data(*this, *j));

          /* This is disabled to avoid bug in wxGTK (but would be desirable to do) */
//          processor_view->EnsureVisible(item);

          ++j;
        }

        ++i;
      }

      /* Make sure the root is expanded, here because EnsureVisible() could not be used */
      processor_view->Expand(processor_view->GetRootItem());
    }

    void project::build() {
      process_display_view = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL|wxTAB_TRAVERSAL);
      processor_view       = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                                        wxTR_HAS_BUTTONS|wxTR_SINGLE|wxSUNKEN_BORDER);

      SetMinimumPaneSize(GetParent()->GetSize().GetWidth() / 2);

      SplitVertically(processor_view, process_display_view);

      process_display_view->SetSizer(new wxBoxSizer(wxVERTICAL));
      process_display_view->SetScrollRate(10, 10);

      processor_view->SetImageList(format_icon_list);
      processor_view->AddRoot(wxT("Store"));

      /* Connect event handlers */
      processor_view->Connect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(project::on_tree_item_activate), 0, this);
      processor_view->Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(project::on_tree_item_activate), 0, this);

      Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(project::on_context_menu_select));

      /** Make sure gui_builder receives idle events */
      PushEventHandler(&gui_builder);
    }

    GUI::tool_display* project::add_tool_display() {
      wxSizer* s = process_display_view->GetSizer();

      GUI::tool_display* display = new GUI::tool_display(process_display_view, this);

      s->Insert(0, display, 0, wxEXPAND|wxALL, 2);

      s->Layout();

      return (display);
    }
            
    /**
     * @param e a reference to a tree event object
     **/
    void project::on_tree_item_activate(wxTreeEvent& e) {
      if (processor_view->GetRootItem() != e.GetItem()) {
        spawn_context_menu(reinterpret_cast < node_data* > (processor_view->GetItemData(e.GetItem()))->target->format);
      }
      else {
        dialog::add_to_project dialog(this, wxString(manager->get_project_directory().c_str(), wxConvLocal));

        if (dialog.ShowModal()) {
          /* Add to the new project */
          wxTreeItemId i = processor_view->AppendItem(e.GetItem(), wxString(dialog.get_name().c_str(), wxConvLocal), 3);

          /* File does not exist in project directory */
          processor* p = manager->import_file(
                                boost::filesystem::path(dialog.get_source()), 
                                boost::filesystem::path(dialog.get_destination()).leaf()).get();

          processor_view->SetItemData(i, new node_data(*this, *(p->get_outputs_iterator())));
          processor_view->EnsureVisible(i);
        }
      }
    }

    /**
     * @param f a storage format for which to add tools to the menu
     **/
    void project::spawn_context_menu(storage_format& f) {
      using namespace boost;

      wxMenu  context_menu;

      context_menu.Append(cmID_REMOVE, wxT("Remove"));
      context_menu.Append(cmID_REBUILD, wxT("Rebuild"));
      context_menu.Append(cmID_CLEAN, wxT("Clean"));
      context_menu.AppendSeparator();

      /* wxWidgets identifier for menu items */
      int identifier = cmID_TOOLS;

      main::tool_registry->by_format(f, bind(&project::add_to_context_menu, this, cref(f), _1, &context_menu, &identifier));

      context_menu.AppendSeparator();
      context_menu.Append(cmID_DETAILS, wxT("Details"));

      PopupMenu(&context_menu);
    }

    /* Helper class for associating a tool input combination with a menu item */
    class cmMenuItem : public wxMenuItem {

      public:

        const tool::ptr                the_tool;

        const tool::input_combination* input_combination;

        cmMenuItem(wxMenu* m, int id, const wxString& t, const tool::ptr& tp, const tool::input_combination* ic) :
                                        wxMenuItem(m, wxID_ANY, t), the_tool(tp), input_combination(ic) {
        }
    };

    /**
     * @param[in] f 
     * @param[in] p the main tool_selection_helper object that indexes the global tool manager
     * @param[in] c a reference to the context menu to which to add
     * @param[in,out] id a reference to the next free identifier
     **/
    void project::add_to_context_menu(const storage_format& f, const miscellaneous::tool_selection_helper::tools_by_category::value_type& p, wxMenu* c, int* id) {
      wxString    category_name = wxString(p.first.c_str(), wxConvLocal);
      int         item_id       = c->FindItem(category_name); 
      wxMenu*     target_menu;

      if (item_id == wxNOT_FOUND) {
        target_menu = new wxMenu();

        c->Append(*id++, category_name, target_menu);
      }
      else {
        target_menu = c->FindItem(item_id)->GetMenu();
      }

      cmMenuItem* new_menu_item = new cmMenuItem(target_menu, *id++, 
                                wxString(p.second->get_name().c_str(), wxConvLocal),
                                p.second,
                                p.second->find_input_combination(f, p.first));

      target_menu->Append(new_menu_item);
    }

    /**
     * @param e a reference to a menu event object
     **/
    void project::on_context_menu_select(wxCommandEvent& e) {
      wxTreeItemId                  s = processor_view->GetSelection();
      processor::object_descriptor* t = reinterpret_cast < node_data* > (processor_view->GetItemData(s))->target;
      processor*                    p = t->generator;

      switch (e.GetId()) {
        case cmID_REMOVE:
          manager->remove(p);

          processor_view->Delete(s);
          break;
        case cmID_REBUILD:
          p->flush_outputs();

          p->process();
          break;
        case cmID_CLEAN:
          p->flush_outputs();
          break;
        case cmID_DETAILS: {
            dialog::processor_details dialog(this, wxString(manager->get_project_directory().c_str(), wxConvLocal), p);

            if (processor_view->GetItemParent(s) == processor_view->GetRootItem()) {
              dialog.set_name(processor_view->GetItemText(s));
              dialog.show_tool_selector(false);
              dialog.show_input_objects(false);

              /* Add the main input (must exist) */
              dialog.populate_tool_list(t->format);
            }

            if (dialog.ShowModal()) {
              /* TODO Process changes */
            }
          }
          break;
        default: {
            /* Assume that a tool was selected */
            wxMenu*     menu      = reinterpret_cast < wxMenu* > (e.GetEventObject());
            cmMenuItem* menu_item = reinterpret_cast < cmMenuItem* > (menu->FindItem(e.GetId()));

            /* Create a temporary processor */
            processor::ptr tp = processor::ptr(new processor(menu_item->the_tool));

            temporary_processors.push_back(tp);

            global_tool_manager->find(std::string(menu_item->GetLabel().fn_str()));

            /* Attach tool display */
            GUI::tool_display* display = add_tool_display();

            tp->get_monitor()->set_layout_handler(boost::bind(&GUI::tool_display::set_layout, display, _1));

            tp->configure(menu_item->input_combination, boost::filesystem::path(t->location));
          }
          break;
      }
    }

    wxString project::get_name() const {
      return (wxString(manager->get_name().c_str(), wxConvLocal));
    }
  }
}
