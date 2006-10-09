#ifndef SAVEPICDIALOG_H
#define SAVEPICDIALOG_H

#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <wx/slider.h>
#include <wx/statline.h>
#include <wx/filename.h>
#include <vector>

using namespace std;

const int myID_F_BUTTON = wxID_HIGHEST + 1;
const int myID_R_SLIDER = myID_F_BUTTON + 1;
const int myID_FT_CHOICE = myID_R_SLIDER + 1;

class SavePicDialog : public wxDialog {
  public:
    SavePicDialog(wxWindow* parent,int w,int h,int w_max,int h_max,
      wxString filename,wxString dir);
    ~SavePicDialog();
    int  getImageWidth();
    int  getImageHeight();
    wxString getFileName();
    long getFileType();
    void onSlider(wxScrollEvent& event);
    void onChangeFile(wxCommandEvent& event);
    void onChoice(wxCommandEvent& event);
  private:
    float	  ar;
    wxSlider*     r_slider;
    wxStaticText* r_text;
    wxChoice*     ft_choice;
    wxStaticText* f_text;
    wxButton*	  f_button;
    wxFileName	  f_name;
    wxArrayString f_exts;
    vector<long>  f_types;

    DECLARE_EVENT_TABLE()
};
#endif
