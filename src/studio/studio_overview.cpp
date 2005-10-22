#include <map>
#include <wx/splitter.h>
#include <wx/treectrl.h>

#include "studio_overview.h"
#include "new_model_dialog.h"

#include "pixmaps/stock_new.xpm"
#include "pixmaps/stock_delete.xpm"
#include "pixmaps/stock_open.xpm"
#include "pixmaps/stock_save.xpm"
#include "pixmaps/stock_refresh.xpm"

IMPLEMENT_CLASS(StudioOverview, wxFrame)

/* IDs for menu items */
#define ID_NEW                  81
#define ID_OPEN                 82
#define ID_CLOSE                83
#define ID_SAVE                 84
#define ID_SAVEAS               85
#define ID_UNDO                 87
#define ID_REDO                 88
#define ID_CUT                  89
#define ID_COPY                 90
#define ID_PASTE                91
#define ID_ABOUT                92
#define ID_ADD                  93
#define ID_REMOVE               94
#define ID_HELP                 95

#define ID_SETTINGS                 100
#define ID_PROJECT_NEW              101
#define ID_PROJECT_CLOSE            102
#define ID_PROJECT_LOAD             103
#define ID_PROJECT_STORE            104
#define ID_PROJECT_BUILD            105
#define ID_SPECIFICATION_NEW        106
#define ID_SPECIFICATION_REMOVE     107
#define ID_SPECIFICATION_RENAME     108
#define ID_SPECIFICATION_LOAD       109
#define ID_SPECIFICATION_MARK_DIRTY 110
#define ID_ANALYSIS_NEW             111
#define ID_ANALYSIS_REMOVE          112
#define ID_ANALYSIS_PERFORM         113
#define ID_MODEL                    114
#define ID_ANALYSIS                 115

#define ID_FRAME_MODEL              151
#define ID_FRAME_ANALYSIS           152

BEGIN_EVENT_TABLE(StudioOverview, wxFrame)
  EVT_MENU(wxID_NEW,                        StudioOverview::ProjectNew)
  EVT_MENU(wxID_OPEN,                       StudioOverview::ProjectLoad)
  EVT_MENU(wxID_CLOSE,                      StudioOverview::ProjectClose)
  EVT_MENU(ID_PROJECT_LOAD,                 StudioOverview::ProjectLoad)
  EVT_MENU(ID_PROJECT_STORE,                StudioOverview::ProjectStore)
  EVT_MENU(ID_SPECIFICATION_NEW,            StudioOverview::NewSpecification)
  EVT_MENU(ID_SPECIFICATION_REMOVE,         StudioOverview::RemoveSpecification)
  EVT_MENU(ID_SPECIFICATION_RENAME,         StudioOverview::ActivateRename)
  EVT_MENU(ID_SPECIFICATION_LOAD,           StudioOverview::AddSpecification)
  EVT_MENU(ID_SPECIFICATION_MARK_DIRTY,     StudioOverview::MarkDirty)
  EVT_MENU(ID_ANALYSIS_NEW,                 StudioOverview::AddAnalysis)
  EVT_MENU(ID_ANALYSIS_REMOVE,              StudioOverview::RemoveAnalysis)
  EVT_MENU(ID_ANALYSIS_PERFORM,             StudioOverview::PerformAnalysis)
  EVT_TREE_ITEM_RIGHT_CLICK(ID_FRAME_MODEL, StudioOverview::SpawnContextMenu)
  EVT_TREE_END_LABEL_EDIT(ID_FRAME_MODEL,   StudioOverview::RenameSpecification)
  EVT_MENU(wxID_EXIT,                       StudioOverview::Quit)
END_EVENT_TABLE()

StudioOverview::StudioOverview(wxWindow* parent, wxWindowID id) :
  wxFrame(parent, id, wxT("Studio - Automated Parking Garage"), wxDefaultPosition, wxDefaultSize, STUDIO_OVERVIEW_STYLE), project_manager() {

  /* Resize and centre frame on display */
  Centre();
  SetSize(0,0,800,600);

  /* Create menubar & toolbar */
  GenerateMenuBar();

#if !defined(DISABLE_TOOLBAR)
  GenerateToolBar();
#endif

  /* Upper level window splitter */
  wxBoxSizer*       mainSizer    = new wxBoxSizer(wxVERTICAL);
  wxSplitterWindow* mainSplitter = new wxSplitterWindow(this);

  /* Icons for model specifications */
  modeliconlist = new wxImageList(24,24, false, 3);
  modeliconlist->Add(wxIcon(stock_new_xpm));
  modeliconlist->Add(wxIcon(stock_open_xpm));
  modeliconlist->Add(wxIcon(stock_save_xpm));
  modeliconlist->Add(wxIcon(stock_refresh_xpm));

  /* Generate model view */
  specifications = new wxTreeCtrl(mainSplitter, ID_FRAME_MODEL, wxDefaultPosition, wxDefaultSize, wxTR_EDIT_LABELS|wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT|wxTR_SINGLE|wxSUNKEN_BORDER);
  specifications->AssignImageList(modeliconlist);
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
};

StudioOverview::~StudioOverview() {
};

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
  project_specification_menu->Append(ID_SPECIFICATION_LOAD, wxT("&Add"), wxT("Add existing specification to project"));
  project_menu->Append(ID_MODEL, wxT("&Specification"), project_specification_menu);

  wxMenu* project_analysis_menu = new wxMenu();

  project_analysis_menu->Append(ID_ANALYSIS_NEW, wxT("&New"));
  project_analysis_menu->Append(ID_ANALYSIS_REMOVE, wxT("&Remove"));
  project_analysis_menu->Append(ID_ANALYSIS_PERFORM, wxT("&Perform\tCTRL-P"));
  project_menu->Append(ID_ANALYSIS, wxT("&Analysis"), project_analysis_menu);
  menu->Append(project_menu, wxT("&Project"));

  wxMenu* settings_menu  = new wxMenu();

  settings_menu->Append(ID_SETTINGS, wxT("&Defaults"));
  menu->Append(settings_menu, wxT("&Settings"));

  wxMenu* help_menu  = new wxMenu();

  help_menu->Append(ID_HELP, wxT("&User Manual"));
  help_menu->AppendSeparator();
  help_menu->Append(ID_ABOUT, wxT("&About"));
  menu->Append(help_menu, wxT("&Help"));

  SetMenuBar(menu);
}

/* Convenience function to fill the menu */
inline void StudioOverview::GenerateToolBar() {
  /* Create toolbar */
  wxToolBar* toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL);

  /* Add 'new model' tool */
  toolbar->AddTool(ID_SPECIFICATION_NEW, wxBitmap(stock_new_xpm), wxT("New specification"));
  toolbar->AddTool(ID_SPECIFICATION_LOAD, wxBitmap(stock_open_xpm), wxT("Add specification"));
  toolbar->AddTool(ID_SPECIFICATION_REMOVE, wxBitmap(stock_delete_xpm), wxT("Remove specification"));
  toolbar->Realize();

  SetToolBar(toolbar);
}

/* Handlers for operations of project level */
void StudioOverview::ProjectNew(wxCommandEvent &event) {
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

void StudioOverview::ProjectClose(wxCommandEvent &event) {
  /* Reset title bar */
  SetTitle(wxT("Studio - No project"));

  project_manager.Close();

  specifications->DeleteChildren(specifications->GetRootItem());
}

void StudioOverview::ProjectLoad(wxCommandEvent &event) {
  wxDirDialog* directory_dialog  = new wxDirDialog(this, wxT("Select a project directory"));

  if (directory_dialog->ShowModal() == wxID_OK) {
    const std::list < Specification >*               specifications = project_manager.GetSpecifications();
          std::map  < Specification*, wxTreeItemId > to_tree_id;

    wxString project_directory = directory_dialog->GetPath();
 
    /* Clean up an open project and clear specification view */
    ProjectClose(event);

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
      const wxString           name(specification->name.c_str(), wxConvLocal);
 
      if (specification->input_objects.size() == 0) {
        /* Specification is provided (not generated) */
        new_item = this->specifications->AppendItem(root, name, 0, -1, specification_data);
      }
      else {
        const wxTreeItemId parent = to_tree_id[specification->input_objects.back().first];
 
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
};

void StudioOverview::ProjectStore(wxCommandEvent &event) {
  project_manager.Store();
};

void StudioOverview::ModelsRefresh(wxCommandEvent &event) {
};

/*
 * Adds a new specification as a child of the selected specification
 *
 * TODO
 *  A new specification relies on a single input only; must be extended.
 */
inline wxTreeItemId StudioOverview::CreateSpecification(Specification& specification) {
  wxTreeItemId   selected   = specifications->GetSelection();
  wxTreeItemId   root_item  = specifications->GetRootItem();
  wxTreeItemId   new_item;

  /* Add dependencies */
  if (root_item != selected) {
    Specification* dependency = ((SpecificationData*) specifications->GetItemData(selected))->specification;
    ObjectPair     new_pair(dependency, dependency->output_objects.back());

    /* Only one output of the parent at this time */
    specification.input_objects.push_back(new_pair);
  }

  try {
    SpecificationData* specification_data;
    wxString           name(specification.name.c_str(), wxConvLocal);

    /* Determine outputs, using tool characteristics TODO */
    specification.output_objects.push_back(specification.name.append(""));

    specification_data = new SpecificationData(project_manager.Add(specification));

    /* Add new item to (local) project manager and tree-control */
    new_item = specifications->AppendItem(selected, name, 0, -1, specification_data);

    if (!specifications->IsExpanded(selected) && root_item != selected) {
      specifications->Expand(selected);
    }
  }
  catch (int) {
  }

  return (new_item);
}

void StudioOverview::NewSpecification(wxCommandEvent &event) {
  Specification new_specification;
  wxTreeItemId  new_item;

  new_specification.name = "New specification"; 

  new_item = CreateSpecification(new_specification);

  specifications->SelectItem(new_item);
  specifications->EditLabel(new_item);
};

/* Handlers for operations on specifications */
void StudioOverview::AddSpecification(wxCommandEvent &event) {
  NewModelDialog* dialog = new NewModelDialog(this, wxID_ANY);

  if (dialog->ShowModal() == wxID_OK) {
    wxString name = dialog->GetModelName();

    if (name != wxT("")) {
      wxString file_name = dialog->GetModelFileName();

      if (file_name != wxT("")) {
        /* Insert new specification into tree */
        Specification  new_specification;

        new_specification.name     = name.fn_str();

        specifications->SelectItem(CreateSpecification(new_specification));
      }
    }
  }

  dialog->~NewModelDialog();
};

void StudioOverview::MarkDirty(wxCommandEvent &event) {
  Specification* specification = ((SpecificationData*) specifications->GetItemData(specifications->GetSelection()))->specification;

  specification->SetNotUpToDate();
}

void StudioOverview::RemoveSpecification(wxCommandEvent &event) {
  wxTreeItemId selected = specifications->GetSelection();

  if (selected != specifications->GetRootItem()) {
    /* Invisible root is not selected */
    specifications->DeleteChildren(selected);
    specifications->Delete(selected);
  }
};

/* Handlers for operations on progress */
void StudioOverview::AddAnalysis(wxCommandEvent &event) {
};

void StudioOverview::RemoveAnalysis(wxCommandEvent &event) {
};

void StudioOverview::PerformAnalysis(wxCommandEvent &event) {
};

void StudioOverview::SpawnContextMenu(wxTreeEvent &event) {
  /* Set selected tree item, for communication with menu event handlers */
  Specification* specification = ((SpecificationData*) specifications->GetItemData(event.GetItem()))->specification;

  /* Build popup menu */
  wxMenu* context_menu = new wxMenu();

  context_menu->Append(ID_SETTINGS, wxT("&Edit"));
  context_menu->Append(ID_SPECIFICATION_RENAME, wxT("&Rename"));
  context_menu->Append(ID_SPECIFICATION_REMOVE, wxT("&Delete"));

  if (specification->IsUpToDate()) {
    context_menu->Append(ID_SPECIFICATION_MARK_DIRTY, wxT("&Mark dirty"));
  }

  context_menu->AppendSeparator();

  wxMenu* visualise_menu  = new wxMenu();

  context_menu->Append(ID_SETTINGS, wxT("&Visualise..."), visualise_menu);

  wxMenu* convert_menu  = new wxMenu();

  context_menu->Append(ID_SETTINGS, wxT("&Convert..."), convert_menu);

  wxMenu* transform_menu  = new wxMenu();

  context_menu->Append(ID_SETTINGS, wxT("&Transform..."), transform_menu);

  wxMenu* analysis_menu  = new wxMenu();

  context_menu->Append(ID_SETTINGS, wxT("&Analyse..."), analysis_menu);
  context_menu->AppendSeparator();
  context_menu->Append(ID_SETTINGS, wxT("&Properties"));

  PopupMenu(context_menu);

  /* Clean up */
  context_menu->~wxMenu();
}

void StudioOverview::ActivateRename(wxCommandEvent &event) {
  specifications->EditLabel(specifications->GetSelection());
}

void StudioOverview::RenameSpecification(wxTreeEvent &event) {
  /* Communicate change of name with Project Manager */
  if (!event.IsEditCancelled()) {
    Specification* specification = ((SpecificationData*) specifications->GetItemData(event.GetItem()))->specification;

    specification->name = event.GetLabel().fn_str();

#ifndef NDEBUG
    std::cerr << "Renamed specification\n\n";

    specification->Print();
#endif
  }
}

void StudioOverview::Quit(wxCommandEvent &event) {
  Destroy();
};

