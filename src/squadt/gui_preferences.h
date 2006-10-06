#ifndef SQUADT_PREFERENCES_H_
#define SQUADT_PREFERENCES_H_

#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/notebook.h>

namespace squadt {
  namespace GUI {

    class preferences : public wxDialog {

      private:

        wxButton* okay_button;

      public:

        /** \brief Constructor */
        preferences(wxWindow*);
    };

    /**
     * \param[in] p the parent window
     **/
    preferences::preferences(wxWindow* p) : wxDialog(p, wxID_ANY, wxT("Preferences")) {
      wxNotebook* tab_manager = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);

      wxPanel* execution_tab = new wxPanel(tab_manager, wxID_ANY);

      wxSizer* current_sizer = new wxBoxSizer(wxVERTICAL);
      current_sizer->Add(new wxStaticText(execution_tab, wxID_ANY, wxT("bla execution")));

      execution_tab->SetSizer(current_sizer);

      tab_manager->AddPage(execution_tab, wxT("Execution"));

      wxPanel* editing_tab = new wxPanel(tab_manager, wxID_ANY);

      current_sizer = new wxBoxSizer(wxVERTICAL);
      current_sizer->Add(new wxStaticText(editing_tab, wxID_ANY, wxT("bla editting")));

      execution_tab->SetSizer(current_sizer);

      tab_manager->AddPage(editing_tab, wxT("Editing"));

      wxPanel* debug_tab = new wxPanel(tab_manager, wxID_ANY);

      current_sizer = new wxBoxSizer(wxVERTICAL);
      current_sizer->Add(new wxStaticText(debug_tab, wxID_ANY, wxT("bla debug")));

      execution_tab->SetSizer(current_sizer);

      tab_manager->AddPage(debug_tab, wxT("Debug"));

      okay_button = new wxButton(this, wxID_OK);

      current_sizer = new wxBoxSizer(wxVERTICAL);
      current_sizer->Add(tab_manager, 1, wxEXPAND|wxALL, 3);
      current_sizer->Add(okay_button, 0, wxALIGN_RIGHT);

      SetSizer(current_sizer);
    }
  }
}

#endif
