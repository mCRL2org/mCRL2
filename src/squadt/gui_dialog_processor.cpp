#include "gui_dialog_processor.h"

namespace squadt {
  namespace GUI {
    namespace dialog {

      /**
       * @param p a pointer to the parent window
       * @param t the title for the dialog window 
       **/
      processor_details::processor_details(wxWindow* p, wxString t, wxString s) :
                                                dialog::processor(p, t), project_store(s) {
        build();
      }

      void processor_details::build() {
        wxBoxSizer*       s = new wxBoxSizer(wxHORIZONTAL);
        wxBoxSizer*       t = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer*       u = new wxBoxSizer(wxHORIZONTAL);

        name = new wxTextCtrl(main_panel, wxID_ANY, wxT(""));

        u->Add(new wxStaticText(main_panel, wxID_ANY, wxT("Name :")));
        u->AddSpacer(5);
        u->Add(name, 1, wxEXPAND);

        wxNotebook* notebook = new wxNotebook(main_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);

        input_objects = new wxListCtrl(notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                        wxLC_REPORT|wxLC_SMALL_ICON|wxLC_ALIGN_LEFT|wxLC_SINGLE_SEL|wxLC_VRULES);
        input_objects->InsertColumn(0, wxT("Name"));
        input_objects->InsertColumn(1, wxT("Size"));
        input_objects->InsertColumn(2, wxT("Date"));
        output_objects = new wxListCtrl(notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                        wxLC_REPORT|wxLC_SMALL_ICON|wxLC_ALIGN_LEFT|wxLC_SINGLE_SEL|wxLC_VRULES);
        output_objects->InsertColumn(0, wxT("Name"));
        output_objects->InsertColumn(1, wxT("Size"));
        output_objects->InsertColumn(2, wxT("Date"));

        notebook->AddPage(input_objects, wxT("Input"));
        notebook->AddPage(output_objects, wxT("Output"));

        t->Add(u, 0, wxEXPAND);
        t->AddSpacer(5);
        t->Add(notebook, 1, wxEXPAND);

        tool_selector = new wxTreeCtrl(main_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                        wxTR_HIDE_ROOT|wxTR_HAS_BUTTONS|wxTR_SINGLE|wxSUNKEN_BORDER);

        s->AddSpacer(20);
        s->Add(tool_selector, 3, wxEXPAND|wxTOP|wxBOTTOM, 10);
        s->AddSpacer(10);
        s->Add(t, 5, wxEXPAND|wxTOP|wxBOTTOM, 10);
        s->AddSpacer(20);

        main_panel->SetSizer(s);

        Layout();

        /* Trigger event to set the buttons right */
      }
    }
  }
}
