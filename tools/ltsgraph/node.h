// Author(s): Didier Le Lann, Carst Tankink, Muck van Weerdenburg and Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./node.h

#ifndef node_h
#define node_h

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/wxprec.h"
#include "wx/dataobj.h"
#include <wx/dcps.h>
#include <string>

#define POS_NODE_LBL_X 3.0
#define POS_NODE_LBL_Y 5.0
#define FONT_SIZE 9
#define CIRCLE_RADIUS 10


using namespace std;

class Node
{
public:
  Node(unsigned int _num, double _posX, double _posY, wxString const& _lbl, bool _initState);
  void OnPaint(wxDC * ptrDC);

  inline unsigned int Get_num() const {
    return num;
  }

  inline double GetX() const {
    return posX;
  }

  inline double GetY() const {
    return posY;
  }

  inline void SetX(double _posX) {
    posX = _posX;
  }

  inline void SetY(double _posY) {
    posY = _posY;
  }

  inline wxColour GetNodeColour() const {
    return node_colour;
  };

  inline void SetNodeColour(wxColour const& c) {
    node_colour = c;
  }

  inline wxColour GetBorderColour() const {
    return border_colour;
  }

  inline void SetBorderColour(wxColour const& c) {
    border_colour = c;
  }

  inline int GetRadius() const {
    return radius;
  }

  inline void SetRadius(int r) {
    radius = r;
  }

  std::string GetLabel() const {
    return std::string(lbl.fn_str());
  }

  inline void SetXY(double _x, double _y) {
    if (!locked) {
      posX = _x;
      posY = _y;
    }
  }

  inline void ForceSetXY(double _x, double _y) { //set the x and y even if the node is locked
    posX = _x;
    posY = _y;
  }

  inline bool IsLocked() {
    return locked;
  }

  inline void Lock() {
    locked = true;
  }

  inline void Unlock() {
    locked = false;
  }

  inline bool LabelVisible() {
    return labelsVisible;
  }

  inline void ShowLabels() {
    labelsVisible = true;
  }

  inline void HideLabels() {
    labelsVisible = false;
  }

  inline bool IsInitState() {
    return initState;
  }

  wxColour get_node_colour();
  wxColour get_border_colour();
  void set_node_colour(wxColour colour);
  void set_border_colour(wxColour colour);
  void reset_border_colour();
  //TODO (CT): Maybe make a reset for node colour?

private:

  double posX;
  double posY;
  int radius;
  bool locked;
  bool initState;
  bool labelsVisible;
  wxString lbl;
  unsigned int num;
  wxColour node_colour;
  wxColour border_colour;
};



#endif //node_h
