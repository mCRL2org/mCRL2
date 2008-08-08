#ifndef IDS_H
#define IDS_H
#include <wx/wx.h>

namespace IDS
{
  enum { 
    myID_MENU_EXPORT = wxID_HIGHEST + 1,
    myID_DISPLAY_STATE_LBL,
    myID_DISPLAY_TRANS_LBL,
    myID_DLG_INFO,
    myID_DLG_ALGO,
    
    myID_NS_SLIDER,
    myID_ES_SLIDER,
    myID_NL_SLIDER,
    myID_GF_SLIDER,
    myID_TP_SLIDER,
    myID_START_OPTI,
    myID_STOP_OPTI
  };

  enum PickState {TRANSITION, SELF_LOOP, STATE};
}

#endif //IDS_H
