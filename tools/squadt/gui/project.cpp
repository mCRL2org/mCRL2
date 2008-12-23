// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file gui/project.cpp

#include "wx.hpp" // precompiled headers

#include <map>
#include <set>
#include <stack>
#include <utility>

#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/ref.hpp>

#include "gui/main.hpp"
#include "gui/project.hpp"
#include "gui/resources.hpp"
#include "gui/tool_display.hpp"
#include "gui/dialog/base.hpp"
#include "gui/dialog/project_settings.hpp"
#include "gui/dialog/processor_settings.hpp"
#include "project_manager.hpp"
#include "tool_manager.hpp"
#include "type_registry.hpp"
#include "build_system.hpp"

#include <wx/choicdlg.h>
#include <wx/filedlg.h>
#include <wx/textdlg.h>

#define cmID_EDIT        (wxID_HIGHEST + 1)
#define cmID_REMOVE      (wxID_HIGHEST + 2)
#define cmID_REFRESH     (wxID_HIGHEST + 3)
#define cmID_RENAME      (wxID_HIGHEST + 4)
#define cmID_CLEAN       (wxID_HIGHEST + 5)
#define cmID_CONFIGURE   (wxID_HIGHEST + 6)
#define cmID_DETAILS     (wxID_HIGHEST + 7)
#define cmID_TOOLS       (wxID_HIGHEST + 8)

#define wxEVT_DISPATCH   (-1)

namespace squadt {
  namespace GUI {

    class project::dispatch_event : public wxCommandEvent {
      private:

        boost::function< void () > m_handler;

      public:

        inline dispatch_event(boost::function< void () > const& h) :
                                 wxCommandEvent(wxEVT_DISPATCH), m_handler(h) {
          assert(!m_handler.empty());
        }

        inline void invoke() {
          m_handler();
        }

        inline wxEvent* Clone() const {
          return new dispatch_event(m_handler);
        }
    };

    class project::tool_data : public wxTreeItemData {
      friend class project;

      private:

        /** \brief The associated project */
        project&                                         parent;

        /** \brief The associated output object */
        boost::weak_ptr < processor::object_descriptor > target;

      public:

        /**
         * \brief Constructor
         *
         * @param[in,out] p a shared pointer to the processor for which process is monitored and reported
         * @param[in] t the processor that is to be associated
         **/
        inline tool_data(project& p, boost::shared_ptr < processor::object_descriptor > const& t) : parent(p), target(t) {
        }

        /** \brief Gets the processor that the target object descriptor is a part of */
        inline boost::shared_ptr< processor > get_processor() {
          boost::shared_ptr< processor::object_descriptor > t(target.lock());
          boost::shared_ptr< processor >                    r;

          if (t.get() != 0) {
            r = t->get_generator();
          }

          return r;
        }

        /** \brief Gets a pointer to the target object */
        inline boost::shared_ptr< processor::object_descriptor > get_object() {
          return target.lock();
        }
    };

    /**
     * \param p the parent window
     * \param l is the path
     * \param d is a description for this project
     * \param b whether or not the project should be created anew
     *
     * Creates a project_manager object that is either:
     *  - read from l, if l is a project store
     *  - the default project_manager, and l is the new project store
     **/
    project::project(main* p, const boost::filesystem::path& l, const std::string& d, bool b) :
                                wxSplitterWindow(p, wxID_ANY), manager(project_manager::create(l, b)) {

      if (!d.empty()) {
        manager->set_description(d);
        manager->store();
      }

      build();
    }

    project::~project() {
      manager->store();

      object_view = 0;

      /* Close tool displays */
      process_display_view->GetSizer()->Clear(true);

      manager.reset();
    }

    void project::schedule_update(boost::function< void () > const& action) {
      dispatch_event event(action); // as local variable instead of temporary for wx 2.8 compatibility

      AddPendingEvent(event);
    }

    void project::synchronised_update(wxCommandEvent& e) {
      static_cast< dispatch_event& >(e).invoke();
    }

    void project::store() {
      manager->store();
    }

    void project::update_object_status(boost::weak_ptr< processor > const& w, const wxTreeItemId s) {
      boost::shared_ptr< processor > g = w.lock();

      if (g.get() != 0 && s.IsOk()) {
        wxTreeItemIdValue cookie; // For wxTreeCtrl traversal

        for (wxTreeItemId j = object_view->GetFirstChild(s, cookie); j.IsOk(); j = object_view->GetNextChild(s, cookie)) {
          project::tool_data* instance_data = static_cast < project::tool_data* > (object_view->GetItemData(j));

          if (instance_data->get_processor().get() == g.get()) {
            object_view->SetItemImage(j, instance_data->get_object()->get_status());
          }
        }

        object_view->Refresh();
        object_view->Update();
      }
    }

    /* Updates the status of files in the object view */
    void project::set_object_status(boost::weak_ptr< processor > const& w, const wxTreeItemId s) {
      boost::shared_ptr< processor > g = w.lock();

      if (g.get() != 0) {
        schedule_update(boost::bind(&project::update_object_status, this, w, s));
      }
    }

    void project::load_views() {
      wxTreeItemId root_item_id = object_view->GetRootItem();

      object_view->DeleteChildren(root_item_id);

      project_manager::processor_sequence processor_range(manager->get_processor_sequence());

      /* Update view: First add objects that are not generated from other objects in the project */
      BOOST_FOREACH(boost::shared_ptr < processor > const& p, processor_range) {
        if (p->number_of_inputs() == 0) {
          boost::iterator_range< processor::output_object_iterator > output_range(p->get_output_iterators());

          BOOST_FOREACH(boost::shared_ptr< processor::object_descriptor > const& object, output_range) {
            wxTreeItemId item = object_view->AppendItem(root_item_id,
                        wxString(boost::filesystem::path(object->get_location()).leaf().c_str(), wxConvLocal), object->get_status());

            object_view->SetItemData(item, new tool_data(*this, object));

            object_view->EnsureVisible(item);
          }
        }
      }

      std::stack < wxTreeItemId > id_stack;

      id_stack.push(root_item_id);

      while (!id_stack.empty()) {
        wxTreeItemIdValue cookie;             // For wxTreeCtrl traversal
        wxTreeItemId      c = id_stack.top(); // The current node

        id_stack.pop();

        for (wxTreeItemId j = object_view->GetFirstChild(c, cookie); j.IsOk(); j = object_view->GetNextChild(c, cookie)) {

          boost::shared_ptr< processor > t = static_cast < tool_data* > (object_view->GetItemData(j))->get_processor();

          project_manager::processor_sequence processor_range(manager->get_processor_sequence());

          BOOST_FOREACH(boost::shared_ptr < processor > const& p, processor_range) {
            boost::iterator_range< processor::input_object_iterator > input_range(p->get_input_iterators());

            // for non tree visualisation use: BOOST_FOREACH(boost::shared_ptr< processor::object_descriptor > const& iobject, input_range) {
            if (!input_range.empty()) {
              boost::shared_ptr < processor::object_descriptor > const& iobject(*input_range.begin());

              if (iobject.get() != 0 && iobject->get_generator() == t) {
                boost::iterator_range< processor::output_object_iterator > output_range(p->get_output_iterators());

                BOOST_FOREACH(boost::shared_ptr< processor::object_descriptor > const& oobject, output_range) {

                  wxTreeItemId item = object_view->AppendItem(j,
                     wxString(boost::filesystem::path(oobject->get_location()).leaf().c_str(), wxConvLocal), oobject->get_status());

                  object_view->SetItemData(item, new tool_data(*this, oobject));

                  object_view->EnsureVisible(item);
                }
              }
            }

          }

          t->check_status(false);

          processor::monitor& m = *(t->get_monitor());

          /* Register handler for status changes */
          m.set_status_handler(boost::bind(&project::set_object_status, this, t, c));

          if (object_view->ItemHasChildren(j)) {
            id_stack.push(j);
          }
        }
      }

      /* Workaround for lockups on wxGTK */
      object_view->Enable(true);
    }

    void project::build() {
      process_display_view = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL|wxTAB_TRAVERSAL);
      object_view          = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                        (wxTR_LINES_AT_ROOT|wxTR_HIDE_ROOT|wxTR_HAS_BUTTONS|wxTR_SINGLE|wxSUNKEN_BORDER)&(~wxTR_EDIT_LABELS));

      SetMinimumPaneSize(1);
      SplitVertically(object_view, process_display_view);
      SetSashPosition(250);

      process_display_view->SetSizer(new wxBoxSizer(wxVERTICAL));
      process_display_view->SetScrollRate(10, 10);

      object_view->SetImageList(format_icon_list);
      object_view->AddRoot(wxT("Store"));

      /* Connect event handlers */
      object_view->Connect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(project::on_tree_item_activate), 0, this);
      object_view->Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(project::on_tree_item_activate), 0, this);
      object_view->Connect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(project::on_object_name_edited), 0, this);

      /* Disable dragging */
      object_view->Connect(wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler(project::on_object_drag), 0, this);
      object_view->Connect(wxEVT_COMMAND_TREE_BEGIN_RDRAG, wxTreeEventHandler(project::on_object_drag), 0, this);
      object_view->Connect(wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler(project::on_object_drag), 0, this);

      Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(project::on_context_menu_select));
      Connect(wxEVT_DISPATCH, wxCommandEventHandler(project::synchronised_update));
    }

    /**
     * \param[in] p the processor::monitor that is connected to the associated tool process
     * \param[in] t the title for the tool display
     **/
    GUI::tool_display* project::install_tool_display(boost::shared_ptr < processor::monitor > p, std::string const& t) {
      wxSizer* s = process_display_view->GetSizer();

      GUI::tool_display* display = new GUI::tool_display(process_display_view, this, p);

      s->Insert(0, display, 0, wxEXPAND|wxALL, 2);
      s->Layout();

      display->set_title(wxString(t.c_str(), wxConvLocal));

      return display;
    }

    /**
     * \param e a reference to a tree event object
     **/
    void project::on_tree_item_activate(wxTreeEvent& e) {
      if (object_view->GetRootItem() != e.GetItem()) {
        // Workaround for problem with wxMSW, the problem there is that the selection is changed only visually
        object_view->SelectItem(e.GetItem());

        spawn_context_menu(*(static_cast < tool_data* > (object_view->GetItemData(e.GetItem()))));
      }
      else {
        project::add_existing_file();
      }
    }

    /**
     * \param e a reference to a tree event object
     **/
    void project::on_object_name_edited(wxTreeEvent& e) {
      if (!e.GetLabel().IsEmpty()) {
        wxTreeItemId s = e.GetItem();

        boost::shared_ptr< processor >                    p = reinterpret_cast < tool_data* > (object_view->GetItemData(s))->get_processor();
        boost::shared_ptr< processor::object_descriptor > t = reinterpret_cast < tool_data* > (object_view->GetItemData(s))->get_object();

        try {
          std::string new_name(e.GetLabel().fn_str());

          if (t->get_location() != new_name) {
            p->relocate_output(*t, new_name);
          }
        }
        catch (std::exception& ex) {
          wxMessageDialog(0, wxT("Renaming failed: \n\n ") + wxString(ex.what(), wxConvLocal), wxT("Error"), wxOK|wxICON_ERROR).ShowModal();

          e.Veto();
        }
      }
      else {
        e.Veto();
      }
    }

    /**
     * \param e a reference to a tree event object
     **/
    void project::on_object_drag(wxTreeEvent& e) {
      e.Veto();
    }

    void project::add_existing_file() {
      dialog::add_to_project dialog(this, manager->get_project_store().string());

      if (dialog.ShowModal()) {
        try {
          /* File does not exist in project directory */
          processor& p(*manager->import_file(
                                boost::filesystem::path(dialog.get_source()),
                                boost::filesystem::path(dialog.get_destination()).leaf()));

          /* Add to the new project */
          wxTreeItemId i = object_view->AppendItem(object_view->GetRootItem(),
                  wxString(dialog.get_name().c_str(), wxConvLocal), processor::object_descriptor::original);

          object_view->SetItemData(i, new tool_data(*this, *p.get_output_iterators().begin()));
          object_view->EnsureVisible(i);

          manager->store();
        }
        catch (std::exception& e) {
          wxMessageDialog(0, wxT("Failed to add `") + wxString(dialog.get_name().c_str(), wxConvLocal) +
                wxT("' to project, please try again.\n\n Details: ") +
                wxString(e.what(), wxConvLocal) + wxT("."), wxT("Error")).ShowModal();
        }
      }
    }

    void project::add_new_file() {
      wxTextEntryDialog file_name_dialog(0, wxT("Please enter a name for the new file (everything before '/' or '\\' is ignored)"), wxT("Enter file name"));

      while (file_name_dialog.ShowModal() == wxID_OK) {
        boost::filesystem::path  name(file_name_dialog.GetValue().fn_str());

        try {
          boost::shared_ptr < processor > new_processor(manager->construct());

          new_processor->register_output("authentic",
                global_build_system.get_type_registry().mime_type_from_name(name),
                name.leaf(), processor::object_descriptor::original);

          // Try creating the file
          std::ofstream f((boost::filesystem::path(manager->get_project_store()) / name).string().c_str(), std::ios::out);

          f.close();

          manager->commit(new_processor);

          manager->store();

          // Add to the new project
          wxTreeItemId i = object_view->AppendItem(object_view->GetRootItem(), wxEmptyString, processor::object_descriptor::original);

          object_view->SetItemText(i, wxString(name.string().c_str(), wxConvLocal));
          object_view->SetItemData(i, new tool_data(*this, *new_processor->get_output_iterators().begin()));
          object_view->EnsureVisible(i);
          object_view->Refresh();
          object_view->Update();

          break;
        }
        catch (std::exception& e) {
          wxMessageDialog(0, wxT("Failed to add `") + wxString(name.string().c_str(), wxConvLocal) +
                wxT("' to project, please try again.\n\n Details: ") +
                wxString(e.what(), wxConvLocal) + wxT("."), wxT("Error")).ShowModal();
        }
      }
    }

    /* Helper function */
    void project::prepare_tool_display(processor* p) {
      install_tool_display(p->get_monitor(), p->get_tool()->get_name() + " : " +
                boost::filesystem::path((*p->get_input_iterators().begin())->get_location()).leaf());
    }

    void project::update() {
      manager->update(boost::bind(&project::prepare_tool_display, this, _1));
    }

    /* Helper class for associating a tool input combination with a menu item */
    class cmMenuItem : public wxMenuItem {

      public:

        const boost::shared_ptr < const tool >                    the_tool;

        const boost::weak_ptr < const tool::input_configuration > input_configuration;

      public:

        cmMenuItem(wxMenu* m, int id, const wxString& t, const boost::shared_ptr< const tool >& tp, const boost::shared_ptr < const tool::input_configuration >& ic) :
                                        wxMenuItem(m, id, t), the_tool(tp), input_configuration(ic) {
        }
    };

    /**
     * \param n an tool_data object used to establish which tools to add to the menu
     **/
    void project::spawn_context_menu(tool_data& n) {
      using namespace boost;

      assert(n.get_processor().get() != 0);

      type_registry& registry(global_build_system.get_type_registry());

      bool   generated              = (0 < n.get_processor()->number_of_inputs());
      bool   show_update_operations = !n.get_processor()->is_active();

      wxMenu  context_menu;

      bool   editable               = registry.has_registered_command(n.get_object()->get_format(), true);
      size_t separator_position     = 3;

      context_menu.Append(cmID_EDIT, wxT("edit"))->Enable(show_update_operations && editable);
      context_menu.Append(cmID_RENAME, wxT("rename"))->Enable(show_update_operations);
      context_menu.Append(cmID_REMOVE, wxT("remove"))->Enable(show_update_operations);

      if (generated) {
        context_menu.Append(cmID_REFRESH, wxT("refresh"))->Enable(show_update_operations);
        context_menu.Append(cmID_CONFIGURE, wxT("configure"))->Enable(show_update_operations);
        context_menu.Append(cmID_CLEAN, wxT("clean"))->Enable(show_update_operations);

        separator_position += 3;
      }

      type_registry::tool_sequence range = registry.tools_by_mime_type(n.get_object()->get_format());

      if (!range.empty()) {
        bool        enabled     = boost::filesystem::exists(n.get_object()->get_location());
        int         identifier  = cmID_TOOLS; // wxWidgets identifier for menu items
        std::string last_seen_category;
        wxMenu*     target_menu = 0;

        BOOST_FOREACH(type_registry::tool_sequence::value_type i, range) {

          if (last_seen_category != i.first.get_name()) {
            target_menu = new wxMenu;

            last_seen_category = i.first.get_name();

            context_menu.Append(identifier++, wxString(i.first.get_name().c_str(), wxConvLocal), target_menu);
          }

          cmMenuItem* new_menu_item = new cmMenuItem(target_menu, identifier++,
                                    wxString(i.second->get_name().c_str(), wxConvLocal),
                                    i.second, i.second->find_input_configuration(i.first, n.get_object()->get_format()));

          target_menu->Append(new_menu_item)->Enable(show_update_operations && enabled);
        }
      }

      context_menu.AppendSeparator();

      if (!context_menu.FindItemByPosition(separator_position)->IsSeparator()) {
        context_menu.InsertSeparator(separator_position);
      }

      context_menu.Append(cmID_DETAILS, wxT("details"));

      object_view->PopupMenu(&context_menu);
    }

    /**
     * \param e a reference to a menu event object
     **/
    void project::on_context_menu_select(wxCommandEvent& e) {
      wxTreeItemId                                      selection = object_view->GetSelection();
      tool_data*                                        node_data = reinterpret_cast < tool_data* > (object_view->GetItemData(selection));

      boost::shared_ptr< processor >                    p = node_data->get_processor();
      boost::shared_ptr< processor::object_descriptor > object = node_data->get_object();

      try {
        type_registry& registry(global_build_system.get_type_registry());

        switch (e.GetId()) {
          case cmID_EDIT:
            p->edit(registry.get_registered_command(object->get_format(), object->get_location().leaf()).get());
            break;
          case cmID_REMOVE:
            if (wxMessageDialog(this, wxT("This operation will remove files from the project store do you wish to continue?"),
                             wxT("Warning: irreversible operation"), wxYES_NO|wxNO_DEFAULT).ShowModal() == wxID_YES) {

              manager->remove(p);

              object_view->Delete(selection);
            }
            break;
          case cmID_RENAME:
            object_view->EditLabel(selection);
            break;
          case cmID_REFRESH:
            p->flush_outputs();

            /* Register handler to on update the object view after process termination */
            p->get_monitor()->once_on_completion(boost::bind(&project::update_after_configuration, this, object_view->GetItemParent(selection), p, false));

            /* Attach tool display */
            manager->update(p, boost::bind(&project::prepare_tool_display, this, _1));
            break;
          case cmID_CLEAN:
            p->flush_outputs();
            break;
          case cmID_DETAILS: {
              dialog::processor_details dialog(this, p, object);

              if (object_view->GetItemParent(selection) == object_view->GetRootItem()) {
                dialog.show_tool_selector(false);
                dialog.show_input_objects(false);
              }
              else {
                boost::shared_ptr< const tool > selected_tool = p->get_tool();

                if (p->has_input_configuration()) {
                  /* Add the main input (must exist) */
                  dialog.populate_tool_list(registry.tools_by_mime_type(p->get_input_configuration()->get_primary_object_descriptor().second.sub_type()));

                  if (selected_tool) {
                    dialog.select_tool(p->get_input_configuration().get(), p->get_tool()->get_name());
                  }

                  dialog.allow_tool_selection(false);
                }
                else {
                  if (selected_tool) {
                    wxMessageDialog(this, wxString(boost::str(
                             boost::format("Tool %s is improperly initialised!") %
                               selected_tool->get_name()).c_str(), wxConvLocal),
                             wxT("Warning: tool problem"), wxOK).ShowModal();
                  }
                  else {
                    wxMessageDialog(this, wxT("Tool is unknown!"),
                             wxT("Warning: tool problem"), wxOK).ShowModal();
                  }
                }
              }

              if (dialog.ShowModal()) {
              }
            }
            break;
          case cmID_CONFIGURE:
              /* Attach tool display */
              install_tool_display(p->get_monitor(), p->get_tool()->get_name() + " : " + boost::filesystem::path(object->get_location()).leaf());

              /* Register handler to on update the object view after process termination */
              p->get_monitor()->once_on_completion(boost::bind(&project::update_after_configuration, this, object_view->GetItemParent(selection), p, false));

              /* Start tool configuration phase */
              p->reconfigure();
            break;
          default: {
              /* Assume that a tool was selected */
              wxMenu*     menu      = reinterpret_cast < wxMenu* > (e.GetEventObject());
              cmMenuItem* menu_item = reinterpret_cast < cmMenuItem* > (menu->FindItem(e.GetId()));

              boost::shared_ptr< const tool::input_configuration > icon(menu_item->input_configuration.lock());

              if (icon.get()) {
                /* Create a temporary processor */
                boost::shared_ptr< processor > tp(manager->construct(menu_item->the_tool, icon));

                /* Attach the new processor by relating it to t */
                tp->register_input(icon->get_primary_object_descriptor().first, object);

                /* Attach tool display */
                install_tool_display(tp->get_monitor(), tp->get_tool()->get_name() + " : " + boost::filesystem::path(object->get_location()).leaf());

                /* Register handler to on update the object view after process termination */
                tp->get_monitor()->once_on_completion(boost::bind(&project::update_after_configuration, this, selection, tp, true));

                /* Start tool configuration phase */
                tp->configure(icon, boost::filesystem::path(object->get_location()));
              }
              else {
                wxMessageDialog(0, wxT("Selected tool configuration is not available.\n\nTool (") +
                                    wxString(menu_item->the_tool->get_name().c_str(), wxConvLocal) +
                                    wxT(") initialisation problem?"), wxT("Error"), wxOK).ShowModal();
              }
            }
            break;
        }
      }
      catch (std::exception& e) {
        wxMessageDialog(0, wxT("Error in command execution: \n") +
                                    wxString(e.what(), wxConvLocal), wxT("Error"), wxOK).ShowModal();
      }
    }

    /**
     * \param[in] s the id of the tree item to which the objects should be attached
     * \param[in] tp the processor of which to read objects
     * \param[in] r whether or not to run the tool
     **/
    void project::update_after_configuration(wxTreeItemId s, boost::shared_ptr< processor > tp, bool r) {

      if (0 < tp->number_of_outputs()) {
        /* Add the processor to the project */
        if (synchronise_outputs_with_objects(s, tp)) {
          processor::monitor& m = *(tp->get_monitor());

          /* Schedule GUI update */
          m.set_status_handler(boost::bind(&project::set_object_status, this, tp, s));

          manager->commit(tp);
        }
        else {
          return;
        }
      }

      if (r) {
        tp->get_monitor()->once_on_completion(boost::bind(&project::update_after_configuration, this, s, tp, false));

        tp->run(true);
      }
    }

    /**
     * \param[in] s the id of the tree item to which the objects should be attached
     * \param[in] tp the processor of which to read objects
     *
     * \return whether or no conflicts were found
     **/
    bool project::synchronise_outputs_with_objects(wxTreeItemId s, boost::shared_ptr< processor > tp) {
      if (object_view != 0) {
        std::set < std::string > existing;

        /* Gather existing objects */
        wxTreeItemIdValue cookie;   // For wxTreeCtrl traversal

        for (wxTreeItemId j = object_view->GetFirstChild(s, cookie); j.IsOk(); j = object_view->GetNextChild(s, cookie)) {
          boost::shared_ptr< processor::object_descriptor > object = static_cast < tool_data* > (object_view->GetItemData(j))->get_object();

          if (object.get() != 0) {
            existing.insert(std::string(object_view->GetItemText(j).fn_str()));
          }
          else {
            remove_from_object_view(j);
          }
        }

        std::auto_ptr < project_manager::conflict_list > conflicts(manager->get_conflict_list(tp));

        if (tp->number_of_outputs() == 0 || 0 < conflicts->size()) {
          manager->remove(tp);
        }

        if (0 < conflicts->size()) {
          for (project_manager::conflict_list::iterator j = conflicts->begin(); j != conflicts->end(); ++j) {
            report_conflict(boost::str(boost::format("The file %s was already part of the project but has now also been produced by %s."
                  "The original file will be restored.") % (*j)->get_location() % tp->get_tool()->get_name()));

            (*j)->self_check();

            boost::shared_ptr < processor > g((*j)->get_generator());

            if (g.get() != 0) {
              manager->update_status(g);
            }
          }
        }
        else {
          boost::iterator_range< processor::output_object_iterator > output_range(tp->get_output_iterators());

          /// Add new outputs as objects to view
          BOOST_FOREACH(boost::shared_ptr< processor::object_descriptor > const& object, output_range) {
            std::set< std::string >::iterator existing_object(existing.find(boost::filesystem::path(object->get_location()).leaf()));

            if (existing_object == existing.end()) {
              add_to_object_view(s, object);
            }
            else {
              existing.erase(existing_object);
            }
          }

          /// Remove obsolete objects from view
          for (wxTreeItemId j = object_view->GetFirstChild(s, cookie); j.IsOk(); j = object_view->GetNextChild(s, cookie)) {
            boost::shared_ptr< processor::object_descriptor > object = static_cast < tool_data* > (object_view->GetItemData(j))->get_object();

            if (object.get() != 0 && object->get_generator() == tp &&
                existing.find(std::string(object_view->GetItemText(j).fn_str())) != existing.end()) {

              /* Remove from view */
              remove_from_object_view(j);
            }
          }
        }

        return conflicts->size() == 0;
      }

      return true;
    }

    void project::report_conflict(std::string const& s) {
      struct trampoline {
        static void show_message(wxWindow* parent, wxString s) {
          wxMessageDialog(parent, s, wxT("Warning: file overwritten"), wxOK).ShowModal();
        }
      };

      schedule_update(boost::bind(&trampoline::show_message, this, wxString(s.c_str(), wxConvLocal)));
    }

    void project::remove_from_object_view(wxTreeItemId& s) {
      struct trampoline {
        static void remove_from_view(wxTreeCtrl* view, wxTreeItemId id) {
          view->Delete(id);
        }
      };

      schedule_update(boost::bind(&trampoline::remove_from_view, object_view, s));
    }

    /**
     * \param[in] id the tree item to connect to
     * \param[in] target the object to associate the new item with
     **/
    void project::add_to_object_view(wxTreeItemId& id, boost::shared_ptr< processor::object_descriptor > target) {
      struct trampoline {
        static void add_to_view(wxTreeCtrl* view, wxTreeItemId id, boost::shared_ptr< processor::object_descriptor > target) {
          boost::shared_ptr< processor > owner(target->get_generator());

          if (owner.get() != 0) {
            wxTreeItemIdValue cookie;

            for (wxTreeItemId j = view->GetFirstChild(id, cookie); j.IsOk(); j = view->GetNextChild(id, cookie)) {
              boost::shared_ptr< processor::object_descriptor > object = static_cast < tool_data* > (view->GetItemData(j))->get_object();

              if (object == target) {
                return; // object is already present in view
              }
            }

            wxTreeItemId item = view->AppendItem(id,
                   wxString(boost::filesystem::path(target->get_location()).leaf().c_str(), wxConvLocal), target->get_status());

            view->SetItemData(item, new tool_data(*static_cast< squadt::GUI::project* > (view->GetParent()), target));

            if (!view->IsExpanded(id)) {
              view->Expand(id);
            }
          }
        }
      };

      schedule_update(boost::bind(&trampoline::add_to_view, object_view, id, target));
    }

    /**
     * \param[in] t path to a file relative to the project store
     **/
    bool project::exists(boost::filesystem::path const& t) const {
      return manager->exists(t);
    }

    wxString project::get_name() const {
      return (wxString(manager->get_name().c_str(), wxConvLocal));
    }
  }
}
