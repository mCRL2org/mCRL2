#ifndef IDS_H
#define IDS_H
#include <wx/wx.h>

namespace IDs
{
  // define ID's for the toolbar buttons
  const int myID_SELECT = wxID_HIGHEST + 1;
  const int myID_ZOOM   = myID_SELECT + 1;
  const int myID_PAN    = myID_ZOOM + 1;
  const int myID_ROTATE = myID_PAN + 1;
  const int myID_RANK_STYLE = myID_ROTATE + 1;
  const int myID_COLOR_BUTTON = myID_RANK_STYLE + 1;
  const int myID_SETTINGS_CONTROL = myID_COLOR_BUTTON + 1;
  const int myID_MARK_RADIOBUTTON = myID_SETTINGS_CONTROL + 1;
  const int myID_MARK_ANYALL = myID_MARK_RADIOBUTTON + 1;
  const int myID_MARK_RULES = myID_MARK_ANYALL + 1;
  const int myID_ADD_RULE = myID_MARK_RULES + 1;
  const int myID_REMOVE_RULE = myID_ADD_RULE + 1;
  const int myID_MARK_TRANSITIONS = myID_REMOVE_RULE + 1;
  const int myID_PARAMETER_CHOICE = myID_MARK_TRANSITIONS + 1;
  const int myID_DISPLAY_STATES = myID_PARAMETER_CHOICE + 1;
  const int myID_DISPLAY_WIREFRAME = myID_DISPLAY_STATES + 1;
}
#endif
