#ifndef PROJECT_OVERVIEW_H
#define PROJECT_OVERVIEW_H

#include <vector>
#include <map>

#include <wx/wx.h>
#include <wx/menu.h>
#include <wx/textctrl.h>
#include <wx/treectrl.h>
#include <wx/splitter.h>

#include "project_manager.h"
#include "tool_manager.h"

class wxSplitterWindow;
class wxTreeCtrl;
class wxMenu;
class wxPanel;

class ProjectOverview : public wxFrame {
  DECLARE_CLASS(ProjectOverview)
  DECLARE_EVENT_TABLE()

  private:
    wxSplitterWindow* top_splitter;
    wxTreeCtrl*       specifications;
    wxPanel*          progress;
    wxMenu*           tree_popup_menu;
    wxTextCtrl*       log_display;

    /* The tool categories in the system, sorted lexicographically */
    std::vector < std::string > tool_categories;

    /* Pre-created context menus per format and per category for all specification formats */
    static std::map < std::pair < std::string, std::string >, wxMenu* > context_menus;

    /* The project manager */
    ProjectManager project_manager;

    /* Convenience functions */
    inline void GenerateMenuBar();
    inline void GenerateToolBar();

    /*******************************
     * Handlers for tool selection *
     *******************************/
    void AddSpecifications(wxCommandEvent &event);

    /* Handlers for operations of project level */
    void NewProject(wxCommandEvent &event);
    void CloseProject(wxCommandEvent &event);
    void LoadProject(wxCommandEvent &event);
    void StoreProject(wxCommandEvent &event);
    void BuildProject(wxCommandEvent &event);

    /* Handlers for operations on models */
    void NewSpecification(wxCommandEvent &event);
    void EditSpecification(wxCommandEvent &event);
    void AddSpecification(wxCommandEvent &event);
    void EditSpecificationProperties(wxCommandEvent &event);
    void RemoveSpecification(wxCommandEvent &event);
    void RenameSpecification(wxTreeEvent &event);

    /* Event handler for all context menus */
    void SpawnContextMenu(wxTreeEvent &event);

    /* Activate rename label from context menu */
    void ActivateRename(wxCommandEvent &event);
    void MarkDirty(wxCommandEvent &event);

    /* Generic GUI only operations */
    void MenuQuit(wxCommandEvent &event);
    void Quit(wxCloseEvent &event);

    void CleanUp();

    /* To connect Specification objects to elements in the tree view */
    class SpecificationData : public wxTreeItemData, public SpecificationVisualiser {
      private:
        wxTreeCtrl& tree_view;

      public:
        Specification& specification;
     
        /* Visually updates the status of the specification */
        void VisualiseStatusChange(SpecificationStatus status) {
          wxTreeItemId item = GetId();

          tree_view.SetItemImage(item, (tree_view.GetItemImage(item) >> 2) + status);
        }
     
        SpecificationData(wxTreeCtrl& atree_view, Specification& aspecification) : tree_view(atree_view), specification(aspecification) {
          aspecification.SetVisualiser(this);
        }
    };

  public:

    ProjectOverview();
    ~ProjectOverview();
    ProjectOverview(wxWindow* parent, wxWindowID id);

    inline wxTextCtrl* GetLogDisplay() const {
      return (log_display);
    }

    /* Used to generate appropriate context menus using the tools in <|tool_manager|> */
    void GenerateToolContextMenus();
};

#endif /* PROJECT_OVERVIEW_H_ */
