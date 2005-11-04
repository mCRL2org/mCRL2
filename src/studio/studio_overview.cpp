#include <map>

#include "studio_overview.h"

#include <wx/splitter.h>
#include <wx/treectrl.h>
#include <wx/process.h>
#include <wx/textdlg.h>

#include "resources.h"
#include "new_specification.h"
#include "specification_properties.h"

IMPLEMENT_CLASS(StudioOverview, wxFrame)

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
#define ID_ANALYSIS_NEW             114
#define ID_ANALYSIS_REMOVE          115
#define ID_ANALYSIS_PERFORM         116
#define ID_MODEL                    117
#define ID_ANALYSIS                 118

#define ID_FRAME_MODEL              151
#define ID_FRAME_ANALYSIS           152

BEGIN_EVENT_TABLE(StudioOverview, wxFrame)
  EVT_MENU(wxID_NEW,                        StudioOverview::NewProject)
  EVT_MENU(wxID_OPEN,                       StudioOverview::LoadProject)
  EVT_MENU(wxID_CLOSE,                      StudioOverview::CloseProject)
  EVT_MENU(ID_PROJECT_LOAD,                 StudioOverview::LoadProject)
  EVT_MENU(ID_PROJECT_STORE,                StudioOverview::StoreProject)
  EVT_MENU(ID_SPECIFICATION_NEW,            StudioOverview::NewSpecification)
  EVT_MENU(ID_SPECIFICATION_EDIT,           StudioOverview::EditSpecification)
  EVT_MENU(ID_SPECIFICATION_REMOVE,         StudioOverview::RemoveSpecification)
  EVT_MENU(ID_SPECIFICATION_RENAME,         StudioOverview::ActivateRename)
  EVT_MENU(ID_SPECIFICATION_MARK_DIRTY,     StudioOverview::MarkDirty)
  EVT_MENU(ID_SPECIFICATION_PROPERTIES,     StudioOverview::EditSpecificationProperties)
  EVT_MENU(ID_ANALYSIS_NEW,                 StudioOverview::AddAnalysis)
  EVT_MENU(ID_ANALYSIS_REMOVE,              StudioOverview::RemoveAnalysis)
  EVT_MENU(ID_ANALYSIS_PERFORM,             StudioOverview::PerformAnalysis)
  EVT_TREE_ITEM_RIGHT_CLICK(ID_FRAME_MODEL, StudioOverview::SpawnContextMenu)
  EVT_TREE_END_LABEL_EDIT(ID_FRAME_MODEL,   StudioOverview::RenameSpecification)
  EVT_MENU(wxID_EXIT,                       StudioOverview::Quit)
END_EVENT_TABLE()

StudioOverview::StudioOverview(ToolManager& new_tool_manager, wxWindow* parent, wxWindowID id) :
  wxFrame(parent, id, wxT("Studio - No project"), wxDefaultPosition, wxDefaultSize), tool_manager(new_tool_manager), project_manager() {

  /* Resize and centre frame on display */
  Centre();
  SetSize(0,0,800,600);

  /* Create menubar & toolbar */
  GenerateMenuBar();

  GenerateContextMenus();

#if !defined(DISABLE_TOOLBAR)
  GenerateToolBar();
#endif

  /* Upper level window splitter */
  wxBoxSizer*       mainSizer    = new wxBoxSizer(wxVERTICAL);
  wxSplitterWindow* mainSplitter = new wxSplitterWindow(this);

  /* Load main icons */
  main_icon_list = LoadMainIcons();

  /* Load default icons for file formats */
  format_icon_list = LoadFormatIcons();

  /* Load default small icons for file formats */
  format_small_icon_list = LoadSmallFormatIcons();

  /* Generate model view */
  specifications = new wxTreeCtrl(mainSplitter, ID_FRAME_MODEL, wxDefaultPosition, wxDefaultSize, wxTR_EDIT_LABELS|wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT|wxTR_SINGLE|wxSUNKEN_BORDER);
  specifications->AssignImageList(format_icon_list);
  specifications->AddRoot(wxT("Leonard of Quirm!"));

  /* Generate progress view */
  progress = new wxPanel(mainSplitter, ID_FRAME_ANALYSIS, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER);

  wxStaticBoxSizer* asizer            = new wxStaticBoxSizer(wxVERTICAL, progress, wxT("Applying transformation"));
  wxStaticBoxSizer* bsizer            = new wxStaticBoxSizer(wxVERTICAL, progress, wxT("Instantiating specification"));
  wxBoxSizer*       csizer            = new wxBoxSizer(wxVERTICAL);
  wxGauge*          example_indicator = new wxGauge(progress, wxID_ANY, 400, wxDefaultPosition, wxDefaultSize, wxGA_SMOOTH|wxGA_HORIZONTAL);

  example_indicator->SetValue(133);
  asizer->Add(example_indicator, 0, wxALL|wxEXPAND, 5);

  csizer->Add(asizer, 0, wxALL|wxEXPAND, 5);
  csizer->Add(bsizer, 0, wxALL|wxEXPAND, 5);
  bsizer->Add(new wxStaticText(progress, wxID_ANY, wxT("Explored 1003827 unique states!")), 0, wxEXPAND|wxALIGN_CENTER|wxLEFT|wxTOP, 15);
  bsizer->Add(new wxStaticText(progress, wxID_ANY, wxT("Using 3702211 transitions!")), 0, wxEXPAND|wxALIGN_CENTER|wxLEFT|wxBOTTOM|wxTOP, 15);
  progress->SetSizer(csizer);

  mainSplitter->SetSplitMode(wxSPLIT_VERTICAL);
  mainSplitter->SplitVertically(specifications,progress,0);

  mainSizer->Add(mainSplitter, 1, wxALL|wxEXPAND, 2);

  SetSizer(mainSizer);

  /* Create status bar and set it to empty explicitly */
  CreateStatusBar();
  SetStatusText(wxT("ready"));
}

StudioOverview::~StudioOverview() {
  const std::map < std::pair < std::string, std::string >, wxMenu* >::const_iterator b = context_menus.end();
        std::map < std::pair < std::string, std::string >, wxMenu* >::const_iterator i = context_menus.begin();

  /* Cleanup context menus */
  while (i != b) {
    delete (*i).second;

    ++i;
  }

  delete tree_popup_menu;
}

void StudioOverview::SetToolManager(ToolManager& new_tool_manager) {
  tool_manager = new_tool_manager;

  context_menus.clear();
  tool_categories.clear();

  /* Regenerate context menus */
  GenerateContextMenus();
}

/* Convenience function to fill the menu */
inline void StudioOverview::GenerateMenuBar() {
  wxMenuBar* menu = new wxMenuBar();

  /* File menu */
  wxMenu* file_menu = new wxMenu();

  /* File->New menu */
  wxMenu* file_new_menu  = new wxMenu();

  file_new_menu->Append(wxID_NEW, wxT("&New Project"));
  file_new_menu->AppendSeparator();
  file_new_menu->Append(ID_SPECIFICATION_NEW, wxT("&New Specification\tCTRL-n"));
  file_new_menu->Append(ID_ANALYSIS_NEW, wxT("&New Analysis"));
  file_menu->Append(ID_NEW, wxT("&New\tSHIFT-ALT-n"), file_new_menu);

  file_menu->Append(wxID_OPEN, wxT("&Open...\tCTRL-o"));
  file_menu->Append(wxID_CLOSE, wxT("&Close\tCTRL-F4"));
  file_menu->AppendSeparator();
  file_menu->Append(ID_PROJECT_STORE, wxT("&Save\tCTRL-s"));
  file_menu->Append(ID_SAVEAS, wxT("Save &As"));
  file_menu->AppendSeparator();
  file_menu->Append(wxID_EXIT, wxT("E&xit"));
  menu->Append(file_menu, wxT("&File"));

  wxMenu* project_menu  = new wxMenu();

  project_menu->Append(wxID_NEW, wxT("&New"), wxT("Create new project"));
  project_menu->Append(ID_PROJECT_LOAD, wxT("&Open"), wxT("Open existing project"));
  project_menu->Append(ID_PROJECT_BUILD, wxT("&Build"), wxT("Generate all specification that are not up to date"));
  project_menu->AppendSeparator();

  wxMenu* project_specification_menu = new wxMenu();

  project_specification_menu->Append(ID_SPECIFICATION_NEW, wxT("&New\tCTRL-n"), wxT("Create new specification"));
  project_menu->Append(ID_MODEL, wxT("&Specification"), project_specification_menu);

  wxMenu* project_analysis_menu = new wxMenu();

  project_analysis_menu->Append(ID_ANALYSIS_NEW, wxT("&New"));
  project_analysis_menu->Append(ID_ANALYSIS_REMOVE, wxT("&Remove"));
  project_analysis_menu->Append(ID_ANALYSIS_PERFORM, wxT("&Perform\tCTRL-P"));
  project_menu->Append(ID_ANALYSIS, wxT("&Analysis"), project_analysis_menu);
  menu->Append(project_menu, wxT("&Project"));

  wxMenu* settings_menu  = new wxMenu();

  settings_menu->Append(wxID_PREFERENCES, wxT("&Defaults"));
  menu->Append(settings_menu, wxT("&Settings"));

  wxMenu* help_menu  = new wxMenu();

  help_menu->Append(wxID_HELP, wxT("&User Manual"));
  help_menu->AppendSeparator();
  help_menu->Append(wxID_ABOUT, wxT("&About"));
  menu->Append(help_menu, wxT("&Help"));

  SetMenuBar(menu);
}

/* Generate context menus for all tool-categories for all input types */
inline void StudioOverview::GenerateContextMenus() {
  const std::list < Tool* >                 tools               = tool_manager.GetTools();
  const std::list < Tool* >::const_iterator b                   = tools.end();
        std::list < Tool* >::const_iterator i                   = tools.begin();
        unsigned int                        tool_category_pairs = 0;

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
           
            context_menus[new_pair]->Append(wxID_HIGHEST, wxString((*i)->GetName().c_str(), wxConvLocal), wxString((*i)->GetDescription().c_str(), wxConvLocal));

            ++tool_category_pairs;
          }

          /* Do this for all selectable formats */
          while (l != e) {
            new_pair.first = (*l).first;
          
            if (context_menus.count(new_pair) == 0) {
              /* No menu created for this pair */
              context_menus[new_pair] = new wxMenu();
            }

            context_menus[new_pair]->Append(wxID_HIGHEST, wxString((*i)->GetName().c_str(), wxConvLocal), wxString((*i)->GetDescription().c_str(), wxConvLocal));

            ++tool_category_pairs;
          }
        }

        /* Register tool category */
        tool_categories.insert(category);

        ++k;
      }

      ++j;
    }

    ++i;
  }

  /* Connect event handler for pop-up menu entries */
  Connect(wxID_HIGHEST, wxID_HIGHEST + tool_category_pairs, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(StudioOverview::ToolSelected));
}

/* Convenience function to fill the menu */
inline void StudioOverview::GenerateToolBar() {
  /* Create toolbar */
  wxToolBar* toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL);

  /* Add tools */
  toolbar->AddTool(ID_SPECIFICATION_NEW, main_icon_list->GetBitmap(0), wxT("New specification"));
  toolbar->AddTool(ID_SPECIFICATION_REMOVE, main_icon_list->GetBitmap(2), wxT("Remove specification"));
  toolbar->Realize();

  SetToolBar(toolbar);
}

/*
 * A tool was selected to add a new specification
 *
 * Need :
 *  - the selected tool
 *  - the selected category (to determine compatible tool modes)
 *  - the selected specification (via specification->GetSelection())
 */
void StudioOverview::ToolSelected(wxCommandEvent &event) {
//  wxTreeItemId   selected      = specifications->GetSelection();
//  Specification* specification = ((SpecificationData*) specifications->GetItemData(selected))->specification;

  /* event.GetId() is an integer that identifies a tool and a category */
  std::cerr << "Selected tool : " << event.GetId() << std::endl;
/* TODO
  Specification     new_specification;
  wxTreeItemId      new_item;
  wxTextEntryDialog dialog(this, wxT("File name for specification"), wxT("Please enter a file name"));

  if (dialog.ShowModal() == wxID_OK) {
    std::string             name(dialog.GetValue().fn_str());
    SpecificationOutputType new_output = { "Mango", name, "MD5 hash" };

    new_specification.output_objects.push_back(new_output);
    new_specification.SetName(name);

    new_item = CreateSpecification(specifications->GetSelection(), new_specification);

    specifications->SelectItem(new_item);
    specifications->EditLabel(new_item);
  }
*/
  /* Specification is not yet available and not up-to-date */
//  new_specification->ToggleRegeneration();

  /* Set corresponding status of new object */
//  specifications->SetItemImage(selected, (specifications->GetItemImage(selected) >> 2) + not_up_to_date);
}

/* Handlers for operations of project level */
void StudioOverview::NewProject(wxCommandEvent &event) {
  wxDirDialog* directory_dialog  = new wxDirDialog(this, wxT("Select a project directory"));

  if (directory_dialog->ShowModal() == wxID_OK) {
    wxString project_directory;

    /* Clear specifications view */
    specifications->DeleteChildren(specifications->GetRootItem());

    /* Close current project */
    project_manager.Close();

    project_directory = directory_dialog->GetPath();

    /* Communicate project directory with project manager */
    project_manager.SetProjectDirectory(std::string(project_directory.fn_str()));
 
    /* Project name is derived from project directory name (TODO check whether portable)*/
    SetTitle(project_directory.AfterLast('/').Prepend(wxT("Studio - ")));
  }

  directory_dialog->~wxDirDialog();
}

void StudioOverview::CloseProject(wxCommandEvent &event) {
  /* Reset title bar */
  SetTitle(wxT("Studio - No project"));

  project_manager.Close();

  specifications->DeleteChildren(specifications->GetRootItem());
}

void StudioOverview::LoadProject(wxCommandEvent &event) {
  wxDirDialog* directory_dialog  = new wxDirDialog(this, wxT("Select a project directory"));

  if (directory_dialog->ShowModal() == wxID_OK) {
    const std::list < Specification >*               specifications = project_manager.GetSpecifications();
          std::map  < Specification*, wxTreeItemId > to_tree_id;

    wxString project_directory = directory_dialog->GetPath();
 
    /* Clean up an open project and clear specification view */
    CloseProject(event);

    /* Communicate project directory with project manager */
    project_manager.SetProjectDirectory(std::string(project_directory.fn_str()));
 
    /* Load specification into project manager */
    project_manager.Load();
 
    /* Set window title (TODO check whether portable)*/
    SetTitle(project_directory.AfterLast('/').Prepend(wxT("Studio - ")));

    std::list < Specification >::const_iterator b = specifications->end();
    std::list < Specification >::const_iterator i = specifications->begin();
 
    /* Connect a graphic representation to each specification */
    while (i != b) {
      const Specification*     specification      = &(*i);
            SpecificationData* specification_data = new SpecificationData(const_cast < Specification* > (specification));
      const wxTreeItemId       root               = this->specifications->GetRootItem();
            wxTreeItemId       new_item;
      const wxString           name(specification->GetName().c_str(), wxConvLocal);
 
      if (specification->GetNumberOfInputObjects() == 0) {
        /* Specification is provided (not generated) */
        new_item = this->specifications->AppendItem(root, name, 0, -1, specification_data);
      }
      else {
        const wxTreeItemId parent = to_tree_id[specification->GetInputObjects().front().derived_from.pointer];
 
        /* Specification is or must be generated; connect to inputs (currently only one input object is allowed) */
        new_item = this->specifications->AppendItem(parent, name, 0, -1, specification_data);
 
        if (!this->specifications->IsExpanded(parent)) {
          this->specifications->Expand(parent);
        }
      }
 
      /* Add to map */
      to_tree_id[specification_data->specification] = new_item;
 
      ++i;
    }
  }

  directory_dialog->~wxDirDialog();
}

void StudioOverview::StoreProject(wxCommandEvent &event) {
  project_manager.Store();
}

/*
 * Adds a new specification as a child of the selected specification
 *
 * TODO
 *  A new specification relies on a single input only; must be extended.
 */
inline wxTreeItemId StudioOverview::CreateSpecification(wxTreeItemId parent, Specification& specification) {
  wxTreeItemId   root_item  = specifications->GetRootItem();
  wxTreeItemId   new_item;

  /* Add dependencies */
  if (root_item != parent) {
    Specification*         dependency = ((SpecificationData*) specifications->GetItemData(parent))->specification;
    SpecificationInputType new_input;

    /* Dependens on the first output of <|dependency|> */
    new_input.derived_from.pointer = dependency;
    new_input.output_number        = 0;

    /* Only one output of the parent at this time */
    specification.GetModifiableInputObjects().push_back(new_input);
  }

  try {
    SpecificationData*      specification_data;
    wxString                name(specification.GetName().c_str(), wxConvLocal);
    SpecificationOutputType new_output = { "Banana", specification.GetName(), "MD5 hash" };

    /* TODO Determine outputs, using tool characteristics and selected tool mode */
    specification.GetModifiableOutputObjects().push_back(new_output);

    specification_data = new SpecificationData(project_manager.Add(specification));

    /* Add new item to (local) project manager and tree-control */
    new_item = specifications->AppendItem(parent, name, 0, -1, specification_data);

    if (!specifications->IsExpanded(parent) && root_item != parent) {
      specifications->Expand(parent);
    }
  }
  catch (int) {
    /* TODO */
  }

  return (new_item);
}

void StudioOverview::EditSpecification(wxCommandEvent &event) {
  wxProcess* editor = new wxProcess();

  /* TODO only takes the last output object */
  wxString filename = wxString(((SpecificationData*) specifications->GetItemData(specifications->GetSelection()))->specification->GetOutputObjects().back().file_name.c_str(), wxConvLocal);

  filename.Prepend(wxT("/")).Prepend(wxString(project_manager.GetProjectDirectory().c_str(), wxConvLocal));

  /* Test with gVim as editor */
  editor->Open(filename.Prepend(wxT("gvim ")));
}

/* Handlers for operations on specifications */
void StudioOverview::NewSpecification(wxCommandEvent &event) {
  NewSpecificationDialog* dialog = new NewSpecificationDialog(this, wxID_ANY);

  if (dialog->ShowModal() == wxID_OK) {
    wxString name = dialog->GetName();

    if (name != wxT("")) {
      wxString file_name = dialog->GetFileName();

      if (file_name != wxT("")) {
        /* Insert new specification into tree */
        std::string   std_name(name.fn_str());
        Specification new_specification;

        new_specification.SetName(std_name);

        specifications->SelectItem(CreateSpecification(specifications->GetRootItem(), new_specification));
      }
    }
  }

  dialog->~NewSpecificationDialog();
}

void StudioOverview::MarkDirty(wxCommandEvent &event) {
  const wxTreeItemId selected = specifications->GetSelection();

  /* Make that the specification will be regenerated */
  ((SpecificationData*) specifications->GetItemData(selected))->specification->ToggleRegeneration();

  /* Set Specification state */
  specifications->SetItemImage(selected, (specifications->GetItemImage(selected) >> 2) + not_up_to_date);
}

void StudioOverview::EditSpecificationProperties(wxCommandEvent &event) {
  Specification*                 specification = ((SpecificationData*) specifications->GetItemData(specifications->GetSelection()))->specification;
  wxString                       name          = wxString(specification->GetName().c_str(), wxConvLocal);
  wxString                       title         = wxString(wxT("Properties of `")).Append(name).Append(wxT("'"));
  SpecificationPropertiesDialog* dialog        = new SpecificationPropertiesDialog(this, wxID_ANY, title, specification);

  /* TODO set proper icon when a format can be resolved to an icon */
  dialog->SetIcon(main_icon_list->GetIcon(0));
  dialog->Show(true);
  dialog->UpdateSizes();

  if (dialog->ShowModal() == wxID_OK) {
  }

  dialog->~SpecificationPropertiesDialog();
}

void StudioOverview::RemoveSpecification(wxCommandEvent &event) {
  wxTreeItemId selected = specifications->GetSelection();

  if (selected != specifications->GetRootItem()) {
    /* Invisible root is not selected */
    specifications->DeleteChildren(selected);
    specifications->Delete(selected);

    /* TODO, remove specifications associated to node and node children */
  }
}

/* Handlers for operations on progress */
void StudioOverview::AddAnalysis(wxCommandEvent &event) {
}

void StudioOverview::RemoveAnalysis(wxCommandEvent &event) {
}

void StudioOverview::PerformAnalysis(wxCommandEvent &event) {
}

void StudioOverview::SpawnContextMenu(wxTreeEvent &event) {
  /* Set selected tree item, for communication with menu event handlers */
  Specification*              specification        = ((SpecificationData*) specifications->GetItemData(event.GetItem()))->specification;
  unsigned int                generated_categories = 0;
  std::vector < wxMenuItem* > attached_menus;

  /* Analyses, for instance, do not have output objects */
  tree_popup_menu->Enable(ID_SPECIFICATION_EDIT, 0 < specification->GetNumberOfOutputObjects());
  tree_popup_menu->Enable(ID_SPECIFICATION_MARK_DIRTY, specification->IsUpToDate());

  /* Add tool categories if there are tools in this category that operate on this type of specification */
  const std::set < std::string >::const_iterator b      = tool_categories.end();
        std::set < std::string >::const_iterator i      = tool_categories.begin();
        std::string                              format = specification->GetModifiableOutputObjects().front().format;

  /* TODO when tree visualisation changes... do something better here */
  while (i != b) {
    std::pair < std::string, std::string > new_pair(format, *i);

    if (context_menus.count(new_pair) != 0) {
      /* Recursively copy the menu to circumvent wxGTK bugs in wxMenu, and wxMenuItem */
      wxMenu*         new_menu = new wxMenu();

      wxMenuItemList::compatibility_iterator node = context_menus[new_pair]->GetMenuItems().GetFirst();

      /* For all menu items */
      while (node != 0) {
        wxMenuItem* item     = node->GetData();
        wxMenuItem* new_item = new wxMenuItem(new_menu, wxID_HIGHEST + generated_categories, item->GetLabel(), item->GetHelp());

        new_menu->Append(new_item);

        node = node->GetNext();
      }

      attached_menus.push_back(tree_popup_menu->Append(new wxMenuItem(tree_popup_menu, ID_MENU_TOOLS, wxString((*i).c_str(), wxConvLocal), wxT(""), wxITEM_NORMAL, new_menu)));

      ++generated_categories;
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

void StudioOverview::ActivateRename(wxCommandEvent &event) {
  specifications->EditLabel(specifications->GetSelection());
}

void StudioOverview::RenameSpecification(wxTreeEvent &event) {
  /* Communicate change of name with Project Manager */
  if (!event.IsEditCancelled()) {
    std::string    name(event.GetLabel().fn_str());
    Specification* specification = ((SpecificationData*) specifications->GetItemData(event.GetItem()))->specification;

    specification->SetName(name);

#ifndef NDEBUG
    std::cerr << "Renamed specification\n\n";

    specification->Print();
#endif
  }
}

void StudioOverview::Quit(wxCommandEvent &event) {
  Destroy();
}

