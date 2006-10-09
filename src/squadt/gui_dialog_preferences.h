#ifndef SQUADT_PREFERENCES_H_
#define SQUADT_PREFERENCES_H_

#include "gui_main.h"

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/notebook.h>

namespace squadt {
  namespace GUI {

    /**
     * \brief Displays a dialog where users can adjust settings
     **/
    class preferences : public wxDialog {

      private:

        /** \brief The control that selects between pages, represented using tabs */
        wxNotebook* tab_manager;

      private:

        /** \brief Event handler for tab activation */
        void change_tab(wxNotebookEvent&);

      public:

        /** \brief Constructor */
        preferences(main*);
    };
  }
}

#endif
