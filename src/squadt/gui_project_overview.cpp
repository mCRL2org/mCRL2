#include <set>
#include <map>
#include <stack>

/* Non portable header file for chdir() ? Windows equivalent ? */
#include <unistd.h>

#include "gui_project_overview.h"
#include "tool.h"
#include "tool_executor.h"
#include "settings_manager.tcc"
#include "logger.h"
#include "ui_core.h"

#include <wx/textctrl.h>
#include <wx/process.h>
#include <wx/textdlg.h>
#include <wx/filedlg.h>
#include <wx/event.h>
#include <wx/filename.h>
#include <wx/dialog.h>
#include <wx/statbmp.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/image.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#include "gui_resources.h"
#include "gui_new_specification.h"
#include "gui_specification_properties.h"

namespace squadt {
  namespace GUI {

    IMPLEMENT_CLASS(ProjectOverview, wxFrame)

    /* IDs for menu items */
    #define ID_NEW                      81
    #define ID_OPEN                     82
    #define ID_CLOSE                    83
    #define ID_SAVE                     84
    #define ID_SAVEAS                   85
    #define ID_UNDO                     87
    #define ID_REDO                     88
    #define ID_CUT                      89
    #define ID_COPY                     90
    #define ID_PASTE                    91
    #define ID_ABOUT                    92
    #define ID_ADD                      93
    #define ID_REMOVE                   94
    #define ID_HELP                     95

    #define ID_MENU_TOOLS               100
    #define ID_SETTINGS                 101
    #define ID_PROJECT_NEW              102
    #define ID_PROJECT_CLOSE            103
    #define ID_PROJECT_LOAD             104
    #define ID_PROJECT_STORE            105
    #define ID_PROJECT_BUILD            106
    #define ID_SPECIFICATION_NEW        107
    #define ID_SPECIFICATION_EDIT       108
    #define ID_SPECIFICATION_REMOVE     109
    #define ID_SPECIFICATION_RENAME     110
    #define ID_SPECIFICATION_MARK_DIRTY 112
    #define ID_SPECIFICATION_PROPERTIES 113

    #define ID_FRAME_MODEL              151
    #define ID_FRAME_ANALYSIS           152

    BEGIN_EVENT_TABLE(ProjectOverview, wxFrame)
      EVT_MENU(wxID_NEW,                        ProjectOverview::NewProject)
      EVT_MENU(wxID_OPEN,                       ProjectOverview::LoadProject)
      EVT_MENU(wxID_CLOSE,                      ProjectOverview::CloseProject)
      EVT_MENU(wxID_ABOUT,                      ProjectOverview::ShowAboutWindow)
      EVT_MENU(ID_PROJECT_LOAD,                 ProjectOverview::LoadProject)
      EVT_MENU(ID_PROJECT_STORE,                ProjectOverview::StoreProject)
      EVT_MENU(ID_PROJECT_BUILD,                ProjectOverview::BuildProject)
      EVT_MENU(ID_SPECIFICATION_NEW,            ProjectOverview::NewSpecification)
      EVT_MENU(ID_SPECIFICATION_EDIT,           ProjectOverview::EditSpecification)
      EVT_MENU(ID_SPECIFICATION_REMOVE,         ProjectOverview::RemoveSpecification)
      EVT_MENU(ID_SPECIFICATION_RENAME,         ProjectOverview::ActivateRename)
      EVT_MENU(ID_SPECIFICATION_MARK_DIRTY,     ProjectOverview::MarkDirty)
      EVT_MENU(ID_SPECIFICATION_PROPERTIES,     ProjectOverview::EditSpecificationProperties)
      EVT_TREE_ITEM_RIGHT_CLICK(ID_FRAME_MODEL, ProjectOverview::SpawnContextMenu)
      EVT_TREE_END_LABEL_EDIT(ID_FRAME_MODEL,   ProjectOverview::RenameSpecification)
      EVT_MENU(wxID_EXIT,                       ProjectOverview::MenuQuit)
      EVT_CLOSE(ProjectOverview::Quit)
    END_EVENT_TABLE()

    /* Mapping: (format,category) -> wxMenu* (shared for all project overviews) */
    std::map < std::pair < std::string, std::string >, wxMenu* > ProjectOverview::context_menus;

    ProjectOverview::ProjectOverview(wxWindow* parent, wxWindowID id) :
      wxFrame(parent, id, wxT("Squadt - No project"), wxDefaultPosition, wxDefaultSize), project_manager() {

      /* Resize and centre frame on display */
      Centre();
      SetSize(0,0,800,600);

      /* Create menubar & toolbar */
      GenerateMenuBar();

    #if defined(ENABLE_TOOLBAR)
      GenerateToolBar();
    #endif

      /* Upper level window splitter */
      wxBoxSizer*       mainSizer    = new wxBoxSizer(wxVERTICAL);

      top_splitter = new wxSplitterWindow(this);

      /* Load main icons */
      main_icon_list = LoadMainIcons();

      SetIcon(*gui_icon);

      /* Load default icons for file formats */
      format_icon_list = LoadFormatIcons();

      /* Load default small icons for file formats */
      format_small_icon_list = LoadSmallFormatIcons();

      /* Generate model view */
      specifications = new wxTreeCtrl(top_splitter, ID_FRAME_MODEL, wxDefaultPosition, wxDefaultSize, wxTR_EDIT_LABELS|wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT|wxTR_SINGLE|wxSUNKEN_BORDER);
      specifications->AssignImageList(format_icon_list);
      specifications->AddRoot(wxT("Leonard of Quirm!"));

      /* Generate progress view */
      progress = new wxPanel(top_splitter, ID_FRAME_ANALYSIS, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER);

      wxBoxSizer*       csizer = new wxBoxSizer(wxVERTICAL);
      wxStaticBoxSizer* asizer = new wxStaticBoxSizer(wxVERTICAL, progress, wxT("Log"));
      wxStaticBoxSizer* bsizer = new wxStaticBoxSizer(wxVERTICAL, progress, wxT("[finished] Instantiating `CVSS-bremen'"));
      wxStaticBoxSizer* dsizer = new wxStaticBoxSizer(wxVERTICAL, progress, wxT("[running] Conversion to FSM `CVSS-bremen'"));
      wxGauge*          gauge  = new wxGauge(progress, wxID_ANY, 100);


      log_display = new wxTextCtrl(progress, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxHSCROLL);

      gauge->SetValue(38);
      log_display->AppendText(wxT("[01:42:50] mcrl2lpe(`CVSS-bremen') finished\n"));
      log_display->AppendText(wxT("[01:42:51] lpeparelm(`CVSS-bremen') finished\n"));
      log_display->AppendText(wxT("[01:42:52] lpeconstelm(`CVSS-bremen') finished\n"));
      log_display->AppendText(wxT("[01:44:03] lpe2lts(`CVSS-bremen') finished\n"));

      bsizer->Add(new wxStaticText(progress, wxID_ANY, wxT("Number of seen states:              83516")), 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
      bsizer->Add(new wxStaticText(progress, wxID_ANY, wxT("Number of explored transitions: 212452")), 0, wxBottom|wxLEFT|wxRIGHT|wxEXPAND, 5);
      dsizer->Add(new wxStaticText(progress, wxID_ANY, wxT("Progress : ")), 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
      dsizer->Add(gauge, 0, wxBottom|wxLEFT|wxRIGHT|wxEXPAND, 5);
      asizer->Add(log_display, 3, wxALL|wxEXPAND, 1);

      csizer->Add(bsizer, 0, wxALL|wxEXPAND, 5);
      csizer->Add(dsizer, 0, wxALL|wxEXPAND, 5);
      csizer->Add(asizer, 3, wxALL|wxEXPAND, 5);
      progress->SetSizer(csizer);

      top_splitter->SetSplitMode(wxSPLIT_VERTICAL);
      top_splitter->SplitVertically(specifications,progress,0);

      mainSizer->Add(top_splitter, 1, wxALL|wxEXPAND, 2);

      SetSizer(mainSizer);

      /* Create status bar and set it to empty explicitly */
      CreateStatusBar();
      SetStatusText(wxT("ready"));

      /* Force calculation of sizes */
      Show(true);

      GetSizer()->Detach(top_splitter);
      GetSizer()->Add(new wxPanel(this, wxID_ANY), 1, wxALL|wxEXPAND, 2);
      GetSizer()->RecalcSizes();
      top_splitter->Show(false);
      Update();
    }

    ProjectOverview::~ProjectOverview() {
      const std::map < std::pair < std::string, std::string >, wxMenu* >::const_iterator b = context_menus.end();
            std::map < std::pair < std::string, std::string >, wxMenu* >::const_iterator i = context_menus.begin();

      /* Cleanup context menus */
      while (i != b) {
        delete (*i).second;

        ++i;
      }

      delete tree_popup_menu;
    }

    /* Convenience function to fill the menu */
    inline void ProjectOverview::GenerateMenuBar() {
      wxMenuBar* menu = new wxMenuBar();

      /* File menu */
      wxMenu* file_menu = new wxMenu();

      /* File->New menu */
      wxMenu* file_new_menu  = new wxMenu();

      file_new_menu->Append(wxID_NEW, wxT("&Project"));
      file_new_menu->AppendSeparator();
      file_new_menu->Append(ID_SPECIFICATION_NEW, wxT("&Specification\tCTRL-n"));
      file_menu->Append(ID_NEW, wxT("&New\tSHIFT-ALT-n"), file_new_menu);

      file_menu->Append(wxID_OPEN, wxT("&Open...\tCTRL-o"));
      file_menu->Append(wxID_CLOSE, wxT("&Close\tCTRL-F4"));
      file_menu->AppendSeparator();
      file_menu->Append(ID_PROJECT_STORE, wxT("&Save\tCTRL-s"));
      file_menu->AppendSeparator();
      file_menu->Append(wxID_EXIT, wxT("E&xit"));
      menu->Append(file_menu, wxT("&File"));

      wxMenu* project_menu  = new wxMenu();

      project_menu->Append(ID_PROJECT_BUILD, wxT("&Build"), wxT("Generate all specifications that are not up to date"));

      menu->Append(project_menu, wxT("&Project"));

      wxMenu* settings_menu  = new wxMenu();

      settings_menu->Append(wxID_PREFERENCES, wxT("&Defaults"));
      menu->Append(settings_menu, wxT("&Settings"));

      wxMenu* help_menu  = new wxMenu();

      help_menu->Append(wxID_HELP, wxT("&User Manual"));
      help_menu->AppendSeparator();
      help_menu->Append(wxID_ABOUT, wxT("&About"));
      menu->Append(help_menu, wxT("&Help"));

      /* Disable storage, when no project is active */
      menu->Enable(ID_PROJECT_STORE, false);
      menu->Enable(ID_SPECIFICATION_NEW, false);
      menu->Enable(wxID_CLOSE, false);

      SetMenuBar(menu);
    }

    /* Generate context menus for all tool-categories for all input types */
    void ProjectOverview::GenerateToolContextMenus() {
      const std::list < Tool* >                 tools                 = tool_manager.GetTools();
      const std::list < Tool* >::const_iterator b                     = tools.end();
            std::list < Tool* >::const_iterator i                     = tools.begin();
            unsigned int                        tool_category_pairs   = 0;
            unsigned int                        tool_number           = 0; /* uniquely identifies the tool */
      const unsigned int                        number_of_tools       = tools.size();

      /* Build popup menu */
      tree_popup_menu = new wxMenu();

      /* Add standard menu entries */
      tree_popup_menu->Append(ID_SPECIFICATION_EDIT, wxT("&Edit"));
      tree_popup_menu->Append(ID_SPECIFICATION_RENAME, wxT("&Rename"));
      tree_popup_menu->Append(ID_SPECIFICATION_REMOVE, wxT("&Delete"));
      tree_popup_menu->Append(ID_SPECIFICATION_MARK_DIRTY, wxT("&Regenerate"));
      tree_popup_menu->AppendSeparator();

      /* For all unique input formats add a menu-item to a context menu */
      while (i != b) {
        const std::vector < ToolMode* >                 modes = (*i)->GetModes();
        const std::vector < ToolMode* >::const_iterator c     = modes.end();
              std::vector < ToolMode* >::const_iterator j     = modes.begin();

        /* Do this for all tool modes */
        while (j != c) {
          const std::list < ToolObject* >                 objects  = (*j)->GetObjects();
          const std::list < ToolObject* >::const_iterator d        = objects.end();
                std::list < ToolObject* >::const_iterator k        = objects.begin();
          const std::string                               category = (*j)->GetCategory();

          /* Search and if unsuccesful, register tool category */
          unsigned int m = 0;

          while (m != tool_categories.size() && (tool_categories[m] != category)) {
            m++;
          }

          if (m == tool_categories.size()) {
            tool_categories.push_back(category);
          }

          /* Do this for all input objects */
          while (k != d) {
            if ((*k)->GetType() == input) {
              const std::map < std::string, std::string >                 formats = (*k)->GetFormatSelectors();
              const std::map < std::string, std::string >::const_iterator l       = formats.end();
                    std::map < std::string, std::string >::const_iterator e       = formats.begin();
                    std::pair < std::string, std::string >                new_pair("", category);

              if ((*k)->HasDefaultFormat()) {
                new_pair.first = (*k)->GetDefaultFormat();

                if (context_menus.count(new_pair) == 0) {
                  /* Create menu for default format */
                  context_menus[new_pair] = new wxMenu();
                }

                context_menus[new_pair]->Append(wxID_HIGHEST + m * number_of_tools + tool_number, wxString((*i)->GetName().c_str(), wxConvLocal), wxString((*i)->GetDescription().c_str(), wxConvLocal));

                ++tool_category_pairs;
              }

              /* Do this for all selectable formats */
              while (l != e) {
                new_pair.first = (*l).first;

                if (context_menus.count(new_pair) == 0) {
                  /* No menu created for this pair */
                  context_menus[new_pair] = new wxMenu();
                }

                context_menus[new_pair]->Append(wxID_HIGHEST + m * number_of_tools + tool_number, wxString((*i)->GetName().c_str(), wxConvLocal), wxString((*i)->GetDescription().c_str(), wxConvLocal));

                ++tool_category_pairs;
              }
            }

            ++k;
          }

          ++j;
        }

        ++i;

        ++tool_number;
      }

      /* Connect event handler for pop-up menu entries */
      Connect(wxID_HIGHEST, wxID_HIGHEST + tool_categories.size() * number_of_tools, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ProjectOverview::AddSpecifications));
    }

    /* Convenience function to fill the menu */
    inline void ProjectOverview::GenerateToolBar() {
      /* Create toolbar */
      wxToolBar* toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL);

      /* Add tools */
      toolbar->AddTool(ID_SPECIFICATION_NEW, main_icon_list->GetBitmap(0), wxT("New specification"));
      toolbar->AddTool(ID_SPECIFICATION_REMOVE, main_icon_list->GetBitmap(2), wxT("Remove specification"));
      toolbar->Realize();

      SetToolBar(toolbar);
    }

    void ProjectOverview::ShowAboutWindow(wxCommandEvent& /* event */) {
      wxDialog*   about = new wxDialog(NULL, wxID_ABOUT, wxT("About Squadt"), wxDefaultPosition, wxDefaultSize);
      wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
      wxBoxSizer* ssizer = new wxBoxSizer(wxHORIZONTAL);
      wxBitmap    bitmap;

      bitmap.LoadFile(wxString(_settings_manager->path_to_images("logo.jpg").c_str(), wxConvLocal), wxBITMAP_TYPE_JPEG);

      about->SetSizer(sizer);

      sizer->Add(new wxStaticBitmap(about, wxID_ANY, bitmap), 0, wxEXPAND|wxLEFT|wxRIGHT, 10);
      sizer->Add(new wxStaticText(about, wxID_ANY, wxT("Copyright 2006, Eindhoven University of Technology")), 0, wxALL|wxEXPAND, 5);
      sizer->Add(new wxStaticLine(about, wxID_ANY), 0, wxALL|wxEXPAND, 1);
      sizer->Add(ssizer, 0, wxEXPAND, 0);
      ssizer->Add(new wxStaticText(about, wxID_ANY, wxT("Version 0.1.0 (Experimental)")), 0, wxLEFT|wxRIGHT|wxEXPAND, 5);
      ssizer->Add(new wxButton(about, wxID_OK), 0, wxALL|wxALIGN_RIGHT, 1);

      about->ShowModal();

      delete (about);
    }

    /* Handlers for operations of project level */
    void ProjectOverview::NewProject(wxCommandEvent& /* event */) {
      wxDirDialog directory_dialog(this, wxT("Select a project directory"));

      if (directory_dialog.ShowModal() == wxID_OK) {
        wxString project_directory;

        /* Clear specifications view */
        specifications->DeleteChildren(specifications->GetRootItem());

        /* Close current project */
        project_manager.Close();

        project_directory = directory_dialog.GetPath();

        /* Communicate project directory with project manager */
        project_manager.SetProjectDirectory(std::string(project_directory.fn_str()));

        /* Project name is derived from project directory name (TODO check whether portable)*/
        SetTitle(project_directory.AfterLast('/').Prepend(wxT("Squadt - ")));

        chdir(project_directory.fn_str());

        /* Store default configuration */
        project_manager.Store();

        GetSizer()->Remove(0);
        GetSizer()->Add(top_splitter, 1, wxALL|wxEXPAND, 2);
        GetSizer()->RecalcSizes();
        top_splitter->Show(true);
        Update();

        GetMenuBar()->Enable(ID_PROJECT_STORE, true);
        GetMenuBar()->Enable(ID_SPECIFICATION_NEW, true);
        GetMenuBar()->Enable(wxID_CLOSE, true);
      }
    }

    void ProjectOverview::CloseProject(wxCommandEvent& /* event */) {
      /* Reset title bar */
      SetTitle(wxT("Squadt - No project"));

      /* Clear log window */
      log_display->Clear();

      GetSizer()->Detach(top_splitter);
      GetSizer()->Add(new wxPanel(this, wxID_ANY), 1, wxALL|wxEXPAND, 2);
      GetSizer()->RecalcSizes();
      top_splitter->Show(false);
      Update();

      project_manager.Store();
      project_manager.Close();

      specifications->DeleteChildren(specifications->GetRootItem());

      GetMenuBar()->Enable(ID_PROJECT_STORE, false);
      GetMenuBar()->Enable(ID_SPECIFICATION_NEW, false);
      GetMenuBar()->Enable(wxID_CLOSE, false);
    }

    void ProjectOverview::LoadProject(wxCommandEvent& /* event */) {
      wxDirDialog directory_dialog(this, wxT("Select a project directory"));

      if (directory_dialog.ShowModal() == wxID_OK) {
        wxString project_directory = directory_dialog.GetPath();

        /* Clean up an open project and clear specification view */
        project_manager.Store();
        project_manager.Close();

        specifications->DeleteChildren(specifications->GetRootItem());

        /* Communicate project directory with project manager */
        project_manager.SetProjectDirectory(std::string(project_directory.fn_str()));

        chdir(project_directory.fn_str());

        /* Load specification into project manager */
        project_manager.Load();

        /* Set window title (TODO check whether portable)*/
        SetTitle(project_directory.AfterLast('/').Prepend(wxT("Squadt - ")));

        std::list < Specification >::const_iterator b = project_manager.GetSpecifications()->end();
        std::list < Specification >::const_iterator i = project_manager.GetSpecifications()->begin();
        std::map  < Specification*, wxTreeItemId > to_tree_id;

        /* Connect a graphic representation to each specification */
        while (i != b) {
                Specification&     specification      = const_cast < Specification& > (*i);
                SpecificationData* specification_data = new SpecificationData(*specifications, specification);
          const wxTreeItemId       root               = specifications->GetRootItem();
                wxTreeItemId       new_item;
          const wxString           name(specification.GetName().c_str(), wxConvLocal);

          if (specification.GetNumberOfInputObjects() == 0) {
            /* Specification is provided (not generated) */
            new_item = specifications->AppendItem(root, name, specification.GetStatus(), -1, specification_data);
          }
          else {
            const wxTreeItemId parent = to_tree_id[specification.GetInputObjects().front().derived_from.pointer];

            /* Specification is or must be generated; connect to inputs (currently only one input object is allowed) */
            new_item = specifications->AppendItem(parent, name, specification.GetStatus(), -1, specification_data);

            if (!specifications->IsExpanded(parent)) {
              specifications->Expand(parent);
            }
          }

          /* Add to map */
          to_tree_id[&specification_data->specification] = new_item;

          ++i;
        }

        GetSizer()->Remove(0);
        GetSizer()->Add(top_splitter, 1, wxALL|wxEXPAND, 2);
        GetSizer()->RecalcSizes();
        top_splitter->Show(true);
        Update();

        GetMenuBar()->Enable(ID_PROJECT_STORE, true);
        GetMenuBar()->Enable(ID_SPECIFICATION_NEW, true);
        GetMenuBar()->Enable(wxID_CLOSE, true);
      }
    }

    void ProjectOverview::StoreProject(wxCommandEvent& /* event */) {
      project_manager.Store();
    }

    /* Build all specifications that are not up to date */
    void ProjectOverview::BuildProject(wxCommandEvent& /* event */) {
      /* Recursively traverse the tree for specifications to be build */
      wxTreeItemIdValue cookie = 0;
      wxTreeItemId      an_id  = specifications->GetFirstChild(specifications->GetRootItem(), cookie);

      std::vector < wxTreeItemId > stack;

      /* Start with the children of the root item */
      while (an_id.IsOk()) {
        stack.push_back(an_id);

        an_id = specifications->GetNextChild(specifications->GetRootItem(), cookie);
      }

      while (0 < stack.size()) {
        /* Set state to regeneration */
        wxTreeItemId&  descendant    = stack.back();
        Specification& specification = ((SpecificationData*) specifications->GetItemData(descendant))->specification;

        if (!specification.IsUpToDate()) {
          specification.Generate();
        }

        /* Proceed recursively */
        stack.pop_back();

        if (specifications->ItemHasChildren(descendant)) {
          an_id = specifications->GetFirstChild(descendant, cookie);

          while (an_id.IsOk()) {
            stack.push_back(an_id);

            an_id = specifications->GetNextSibling(an_id);
          }
        }
      }
    }

    /* Adds a new specification */
    void ProjectOverview::NewSpecification(wxCommandEvent& /* event */) {
      NewSpecificationDialog* dialog = new NewSpecificationDialog(this, wxID_ANY);

      if (dialog->ShowModal() == wxID_OK) {
        wxString name = dialog->GetName();

        if (name != wxT("")) {
          boost::filesystem::path file_name(std::string(dialog->GetFilePath().fn_str()));

          if (file_name.string() != "") {
            boost::filesystem::path target_name(project_manager.GetProjectDirectory());
            bool                    valid = true;

            target_name /= std::string(file_name.leaf());

            if (boost::filesystem::exists(target_name)) {
              /* A file with this name already exists in the project directory, consult the user (TODO add MD5 comparison) */
              wxMessageDialog message_dialog(this, wxT("A file with this name is already in the project, do you want to add it under a different name?"), wxT("Warning ..."), wxYES_NO|wxICON_QUESTION);

              if (message_dialog.ShowModal() == wxID_YES) {
                wxFileDialog new_file_name_dialog(this, wxT("Select a new file name"), wxString(project_manager.GetProjectDirectory().c_str(), wxConvLocal), wxString(file_name.leaf().c_str(), wxConvLocal), wxT("*.*"), wxSAVE|wxOVERWRITE_PROMPT);

                if (new_file_name_dialog.ShowModal() == wxID_OK) {

                  if (new_file_name_dialog.GetPath() == wxT("")) {
                    valid = false;
                  }
                  else if (std::string(new_file_name_dialog.GetPath().fn_str()) == target_name.string()) {
                    valid = false;
                  }
                  else {
                    target_name = std::string(project_manager.GetProjectDirectory());

                    try {
                      target_name /= boost::filesystem::path(std::string(new_file_name_dialog.GetPath().fn_str()));
                    }
                    catch (...) {
                      valid = false;
                    }
                  }

                  if (!valid) {
                    wxMessageDialog error_dialog(this, wxT("Invalid response, cancelling addition..."), wxT("Error ..."), wxOK|wxICON_ERROR);

                    error_dialog.ShowModal();
                  }
                  else {
                    target_name = boost::filesystem::path(std::string(project_manager.GetProjectDirectory())) / std::string(target_name.leaf());

                    if (file_name != target_name) {
                      boost::filesystem::remove(target_name);
                      boost::filesystem::copy_file(boost::filesystem::path(file_name.string()), target_name);
                    }
                    else {
                      /* Do something to avoid circular dependencies on a file to be added */
                    }
                  }
                }
                else {
                  valid = false;
                }
              }
              else {
                valid = false;
              }
            }
            else {
              /* Copy file to project directory */
              boost::filesystem::copy_file(boost::filesystem::path(file_name.string()), target_name);
            }

            if (valid) {
              /* Insert new specification into tree */
              Specification new_specification  = primary_specification;
              std::string   base_name          = target_name.leaf();
              wxTreeItemId  new_item;

              new_specification.SetName(base_name);

              /* TODO add mime type/file format database to determine the type of an input file */
              try {
                wxTreeItemId            root_item  = specifications->GetRootItem();
                std::string             format     = boost::filesystem::extension(base_name);
                SpecificationOutputType new_output = { format.erase(0,1), target_name.leaf(), md5::zero_digest, 0 };

                /* TODO Determine outputs, using tool characteristics and selected tool mode */
                new_specification.GetOutputObjects().push_back(new_output);

                /* Add new item to (local) project manager and tree-control */
                new_item = specifications->AppendItem(root_item, wxString(base_name.c_str(), wxConvLocal), up_to_date, -1);

                if (!specifications->IsExpanded(root_item)) {
                  specifications->Expand(root_item);
                }
              }
              catch (int) {
                /* TODO */
              }

              SpecificationData* specification_data = new SpecificationData(*specifications, project_manager.Add(new_specification));

              specifications->SetItemData(new_item, specification_data);
            }
          }
        }
      }

      dialog->~NewSpecificationDialog();
    }

    /*
     * A tool was selected, that should produce new specifications
     *
     * event.GetId() is an integer that identifies a tool and a category
     */
    void ProjectOverview::AddSpecifications(wxCommandEvent& event) {
            wxTreeItemId   selected          = specifications->GetSelection();    /* The selected specification */
            Specification& specification     = ((SpecificationData*) specifications->GetItemData(selected))->specification;
            wxTreeItemId   new_item;
            Specification  new_specification(false);
            unsigned int   mode_number       = event.GetId() - wxID_HIGHEST;      /* Will hold mode number, is used to hold a tool number first */
            std::string    category          = tool_categories[mode_number / tool_manager.GetNumberOfTools()];
      const Tool*          tool              = tool_manager.GetTool(mode_number % tool_manager.GetNumberOfTools());

      new_specification.SetToolIdentifier(mode_number % tool_manager.GetNumberOfTools());

      /* Find first compatible mode */
      std::vector < ToolMode* >::const_iterator i = tool->GetModes().begin(); 

      /* A suitable category must exist */
      while ((*i)->GetCategory() != category) {
        ++i;
      }

      mode_number = i - tool->GetModes().begin();

      new_specification.SetToolMode(mode_number);

      /* Find a name for the new specification (should not exist in project directory) */
      wxFileName file_helper(wxString(specification.GetOutputObjects().front().location.c_str(), wxConvLocal));

      file_helper.ClearExt();

      /* A new basename */
      std::string name = std::string(file_helper.GetFullName().Append(wxString::Format(wxT("-%u"), (unsigned int) specifications->GetChildrenCount(selected, false))).fn_str());

      /* Connect input object to compatible position (TODO generalise to multiple inputs and outputs) */
      std::vector < SpecificationInputType >& input_objects = new_specification.GetInputObjects();
      SpecificationInputType                  new_input;

      new_input.derived_from.pointer = &specification;
      new_input.output_number        = 0;

      input_objects.push_back(new_input);

      const ToolMode& tool_mode = tool->GetMode(mode_number);

      /* The format of the new specification */
      if (tool_mode.HasOutputObjects()) {
        std::vector < SpecificationOutputType >& output_objects = new_specification.GetOutputObjects();
        SpecificationOutputType                  new_output;

        /* Connect output object to compatible position (TODO generalise to multiple inputs and outputs) */
        new_output.format    = tool_mode.GetOutputObject(0).GetSomeFormat();
        new_output.location  = tool->GetMode(mode_number).ChooseName(0, name);
        new_output.checksum  = md5::zero_digest;
        new_output.timestamp = 0;

        /* Append an extension to the basename */
        name.append(".").append(tool_mode.GetOutputObject(0).GetSomeFormat());

        output_objects.push_back(new_output);
      }
      else {
        /* Tool performs an analysis of some kind and does not produce outputs */
        name.append(".report");
      }

      /* Set the name of the specification to the name of the main output file */
      new_specification.SetName(name);

      /* Add new item to (local) project manager and tree-control */
      Specification& added_specification = project_manager.Add(new_specification);

      new_item = specifications->PrependItem(selected, wxString(name.c_str(), wxConvLocal), 0, -1, new SpecificationData(*specifications, added_specification));

      if (!specifications->IsExpanded(selected)) {
        specifications->Expand(selected);
      }

      specifications->SelectItem(new_item);

      /* Specification is not yet available and not up-to-date */
      added_specification.ForceRegeneration();
    }

    void ProjectOverview::EditSpecification(wxCommandEvent& /* event */) {
      wxProcess* editor = new wxProcess();

      /* TODO only takes the last output object */
      wxString filename = wxString(((SpecificationData*) specifications->GetItemData(specifications->GetSelection()))->specification.GetOutputObjects().back().location.c_str(), wxConvLocal);

      filename.Prepend(wxT("/")).Prepend(wxString(project_manager.GetProjectDirectory().c_str(), wxConvLocal));

      /* Test with gVim as editor */
      editor->Open(filename.Prepend(wxT("gvim ")));
    }

    void ProjectOverview::MarkDirty(wxCommandEvent& /* event */) {
      wxTreeItemIdValue cookie = 0;

      std::vector < wxTreeItemId > stack;

      stack.push_back(specifications->GetSelection());

      while (0 < stack.size()) {
        /* Set state to regeneration */
        wxTreeItemId&  descendant    = stack.back();
        Specification& specification = ((SpecificationData*) specifications->GetItemData(descendant))->specification;

        /* Mark that the specification must be regenerated */
        specification.ForceRegeneration();

        /* Proceed recursively */
        stack.pop_back();

        if (specifications->ItemHasChildren(descendant)) {
          wxTreeItemId an_id = specifications->GetFirstChild(descendant, cookie);

          while (an_id.IsOk()) {
            stack.push_back(an_id);

            an_id = specifications->GetNextSibling(an_id);
          }
        }
      }
    }

    void ProjectOverview::EditSpecificationProperties(wxCommandEvent& /* event */) {
      Specification&                 specification = ((SpecificationData*) specifications->GetItemData(specifications->GetSelection()))->specification;
      wxString                       name          = wxString(specification.GetName().c_str(), wxConvLocal);
      wxString                       title         = wxString(wxT("Properties of `")).Append(name).Append(wxT("'"));
      SpecificationPropertiesDialog* dialog        = new SpecificationPropertiesDialog(this, wxID_ANY, title, specification, project_manager.GetProjectDirectory());

      /* TODO set proper icon when a format can be resolved to an icon */
      dialog->SetIcon(main_icon_list->GetIcon(0));
      dialog->Show(true);
      dialog->UpdateSizes();

      if (dialog->ShowModal() == wxID_OK) {
        dialog->StoreChanges();
      }

      dialog->Destroy();
    }

    void ProjectOverview::RemoveSpecification(wxCommandEvent& /* event */) {
      wxTreeItemId selected = specifications->GetSelection();

      if (selected != specifications->GetRootItem()) {
        /* Invisible root is not selected */
        std::stack  < wxTreeItemId >   stack;
        std::set    < Specification* > sdepricated;
        std::vector < Specification* > vdepricated;

        wxTreeItemIdValue cookie = 0;

        stack.push(selected);

        /* Find all specifications that must be deleted */
        while (0 < stack.size()) {
          wxTreeItemId&  descendant = stack.top();

          vdepricated.push_back(&((SpecificationData*) specifications->GetItemData(descendant))->specification);
          sdepricated.insert(vdepricated.back());

          /* Proceed recursively */
          stack.pop();

          if (specifications->ItemHasChildren(descendant)) {
            wxTreeItemId an_id = specifications->GetFirstChild(descendant, cookie);

            while (an_id.IsOk()) {
              stack.push(an_id);

              an_id = specifications->GetNextChild(descendant, cookie);
            }
          }
        }

        sdepricated.clear();

        /* Delete from project manager */
        project_manager.Remove(vdepricated);

        specifications->DeleteChildren(selected);
        specifications->Delete(selected);
      }
    }

    void ProjectOverview::SpawnContextMenu(wxTreeEvent& event) {
      /* Set selected tree item, for communication with menu event handlers */
      Specification&              specification        = ((SpecificationData*) specifications->GetItemData(event.GetItem()))->specification;
      unsigned int                generated_categories = 0;
      std::vector < wxMenuItem* > attached_menus;

      /* Analyses, for instance, do not have output objects */
      tree_popup_menu->Enable(ID_SPECIFICATION_EDIT, 0 < specification.GetNumberOfOutputObjects());
      tree_popup_menu->Enable(ID_SPECIFICATION_MARK_DIRTY, specification.IsUpToDate() && specification.GetNumberOfInputObjects() != 0);

      /* Add tool categories if there are tools in this category that operate on this type of specification */
      const std::vector < std::string >::const_iterator             b = tool_categories.end();
            std::vector < std::string >::const_iterator             i = tool_categories.begin();
      const std::vector < SpecificationOutputType >::const_iterator g = specification.GetOutputObjects().end();

      /* Merge menus for all output objects per category and add them to the popup menu */
      while (i != b) {
        /* For all output formats */
        std::set    < std::string >                             s; /* For which format a menu was added */
        std::vector < SpecificationOutputType >::const_iterator f = specification.GetOutputObjects().begin(); /* output formats */

        while (f != g) {
          std::string                            format = (*f).format;
          std::pair < std::string, std::string > new_pair(format, *i); /* (format, category) */

          if (s.find(format) == s.end() && context_menus.count(new_pair) != 0) {
            /* Recursively copy the menu to circumvent wxGTK bugs in wxMenu, and wxMenuItem */
            wxMenu* new_menu = new wxMenu();

            wxMenuItemList::compatibility_iterator node = context_menus[new_pair]->GetMenuItems().GetFirst();

            /* For all menu items */
            while (node != 0) {
              wxMenuItem* item     = node->GetData();
              wxMenuItem* new_item = new wxMenuItem(new_menu, item->GetId(), item->GetLabel(), item->GetHelp());

              new_menu->Append(new_item);

              node = node->GetNext();
            }

            attached_menus.push_back(tree_popup_menu->Append(new wxMenuItem(tree_popup_menu, ID_MENU_TOOLS, wxString((*i).c_str(), wxConvLocal), wxT(""), wxITEM_NORMAL, new_menu)));

            /* Count category */
            ++generated_categories;

            /* Add format to s */
            s.insert(format);
          }

          ++f;
        }

        ++i;
      }

      if (generated_categories) {
        attached_menus.push_back(tree_popup_menu->AppendSeparator());
      }

      tree_popup_menu->Append(ID_SPECIFICATION_PROPERTIES, wxT("&Properties"));

      PopupMenu(tree_popup_menu);

      /* Remove separator */
      if (generated_categories) {
        tree_popup_menu->Remove(attached_menus.back());

        attached_menus.pop_back();
      }

      /* Remove properties item */
      tree_popup_menu->Remove(ID_SPECIFICATION_PROPERTIES);

      /* Remove attached context_menus for categories to prevent deletion */
      while (0 < attached_menus.size()) {
        /* Member functions Remove and Delete are not functional under wxGTK */
        tree_popup_menu->Destroy(attached_menus.back());

        attached_menus.pop_back();
      }
    }

    void ProjectOverview::ActivateRename(wxCommandEvent& /* event */) {
      specifications->EditLabel(specifications->GetSelection());
    }

    void ProjectOverview::RenameSpecification(wxTreeEvent& event) {
      /* Communicate change of name with Project Manager */
      if (!event.IsEditCancelled()) {
        std::string    name(event.GetLabel().fn_str());
        Specification& specification = ((SpecificationData*) specifications->GetItemData(event.GetItem()))->specification;

        specification.SetName(name);

    #ifndef NDEBUG
        std::cerr << "Renamed specification\n\n";

        specification.Print();
    #endif
      }
    }

    void ProjectOverview::Quit(wxCloseEvent& /* event */) {
      CleanUp();

      Destroy();
    }

    void ProjectOverview::MenuQuit(wxCommandEvent& /* event */) {
      Close();
    }

    void ProjectOverview::CleanUp() {
      tool_manager.TerminateAll();

      logger->~Logger();
    }
  }
}

