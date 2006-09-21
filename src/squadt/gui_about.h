#include <wx/wx.h>
#include <wx/msgdlg.h>
#include <mcrl2_revision.h>

#define STRINGIFY(x) EXPAND(x)
#define EXPAND(x) #x

namespace squadt {
  namespace GUI {
    class about : public wxMessageDialog {

      public:

        about(wxWindow*);
    };

    /**
     * \param[in] p the parent window
     * \param[in] p the message to print
     **/
    about::about(wxWindow* p) : wxMessageDialog(p,
       wxT("deskSQuADT (revision " STRINGIFY(REVISION) ")\n\n")
       wxT("A controlled environment that provides a graphical user interface that helps users to "
           "interact with all kinds of connected tools. The goal is to simplify the use of these "
           "tools especially in combination with each other.\n\n")
       wxT("Developed by Jeroen van der Wulp\n\n")
       wxT("Distributed as part of the mCRL2 toolset\n")
       wxT("For information: http://www.mcrl2.org\n")
       wxT("For complaints and bug reports: bug@mcrl2.org\n"),
       wxT("About deskSQuADT"), wxOK|wxICON_INFORMATION) {
    }
  }
}
