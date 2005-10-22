#ifndef STUDIO_OVERVIEW_H_
#define STUDIO_OVERVIEW_H_

#include <wx/wx.h>
#include <wx/treectrl.h>

#include "project_manager.h"

#define STUDIO_OVERVIEW_STYLE wxCAPTION | wxRESIZE_BORDER | wxCLOSE_BOX

class StudioOverview : public wxFrame {
  DECLARE_CLASS(StudioOverview)
  DECLARE_EVENT_TABLE()

  public:
    StudioOverview();
    ~StudioOverview();
    StudioOverview(wxWindow* parent, wxWindowID id);

  private:
    wxTreeCtrl*  specifications;
    wxImageList* main_icon_list;
    wxPanel*     progress;

    /* The project manager */
    ProjectManager project_manager;

    /* Convenience functions */
    inline void GenerateMenuBar();
    inline void GenerateToolBar();

    /* Adds a new specification tot the project manager and specification tree view */
    inline wxTreeItemId CreateSpecification(Specification& specification);

    /* Handlers for operations of project level */
    void ProjectNew(wxCommandEvent &event);
    void ProjectClose(wxCommandEvent &event);
    void ProjectLoad(wxCommandEvent &event);
    void ProjectStore(wxCommandEvent &event);

    void ModelsRefresh(wxCommandEvent &event);

    /* Handlers for operations on models */
    void NewSpecification(wxCommandEvent &event);
    void AddSpecification(wxCommandEvent &event);
    void RemoveSpecification(wxCommandEvent &event);
    void RenameSpecification(wxTreeEvent &event);

    /* Handlers for operations on progress */
    void AddAnalysis(wxCommandEvent &event);
    void RemoveAnalysis(wxCommandEvent &event);
    void PerformAnalysis(wxCommandEvent &event);

    /* Event handler for all context menus */
    void SpawnContextMenu(wxTreeEvent &event);

    /* Activate rename label from context menu */
    void ActivateRename(wxCommandEvent &event);
    void MarkDirty(wxCommandEvent &event);
    void ShowSpecificationProperties(wxCommandEvent &event);

    /* Generic GUI only operations */
    void Quit(wxCommandEvent &event);
};

/* To connect Specification objects to elements in the tree view */
class SpecificationData : public wxTreeItemData {
  public:
    Specification* specification; /* Pointer to a Specification */

    SpecificationData(Specification* specification) : specification(specification) {
    };
};

#endif /* STUDIO_OVERVIEW_H_ */
