// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./frame.cpp

#include "wx.hpp" // precompiled headers

#include "frame.h"
#include "figures.xpm"
#include "mcrl2/lts/lts_io.h"

using namespace std;

// For compatibility with older wxWidgets versions (pre 2.8)
#if (wxMINOR_VERSION < 8)
# define wxFD_OPEN wxOPEN
# define wxFD_SAVE wxSAVE
# define wxFD_OVERWRITE_PROMPT wxOVERWRITE_PROMPT
#endif

// -- constructors and destructor -----------------------------------


Frame::Frame(
  Mediator* m,
  Settings* s,
  wxString title)
  : wxFrame(
    NULL,
    wxID_ANY,
    wxString(title)),
  Colleague(m)
{
  SetAutoLayout(true);
  initFrame();
  dofMenu = false;
  clustMenu = NULL;
  frameNote = NULL;
  frameTextSize = NULL;
  currentShapeId = NON_EXISTING;

  /// TODO
  settingsDialog = new SettingsDialog(0, s);
}


Frame::~Frame()
{
#if wxCHECK_VERSION(2, 9, 0)
  this->PopEventHandler(true);
#endif
}


// -- set functions ---------------------------------------------


void Frame::setTitleText(const string& msg)
{
  string titleMsg = "";
  titleMsg.append(msg);
  titleMsg.append(" - DiaGraphica");
  SetTitle(wxString(titleMsg.c_str(), wxConvUTF8));
}


void Frame::setStatusText(const string& msg)
{
  SetStatusText(wxString(msg.c_str(), wxConvUTF8));
}


void Frame::setOutputText(const string& /*msg*/)
{
  //textCtrl->SetValue( wxString( msg.c_str(), wxConvUTF8 ) );
}


void Frame::appOutputText(const string& /*msg*/)
{
  //textCtrl->AppendText( wxString( msg.c_str(), wxConvUTF8 ) );
}

void Frame::enableEditMode(const bool& enable)
{
  modeMenu->Enable(ID_MENU_ITEM_MODE_EDIT, enable);
}


void Frame::setFileOptionsActive()
{
  fileMenu->Enable(wxID_SAVE, true);
  fileMenu->Enable(wxID_SAVEAS, true);
  fileMenu->Enable(ID_MENU_ITEM_LOAD_CONFIG, true);
  fileMenu->Enable(ID_MENU_ITEM_SAVE_CONFIG, true);
  fileMenu->Enable(ID_MENU_ITEM_LOAD_DIAGRAM, true);
  fileMenu->Enable(ID_MENU_ITEM_SAVE_DIAGRAM, true);
}


void Frame::setEditModeSelect()
{
  if (toolBarEdit != NULL)
  {
    toolBarEdit->ToggleTool(ID_TOOL_SELECT, true);
  }
}


void Frame::setEditModeNote()
{
  if (toolBarEdit != NULL)
  {
    toolBarEdit->ToggleTool(ID_TOOL_NOTE, true);
  }
}


void Frame::setEditModeDOF()
{
  if (toolBarEdit != NULL)
  {
    toolBarEdit->ToggleTool(ID_TOOL_DOF, true);
  }
}


void Frame::setDOFColorSelected()
{
  if (listCtrlDOF != NULL)
  {
    listCtrlDOF->SetItemState(5, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
  }
}


void Frame::setDOFOpacitySelected()
{
  if (listCtrlDOF != NULL)
  {
    listCtrlDOF->SetItemState(6, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
  }
}


void Frame::handleNote(const size_t& shapeId, const string& msg)
{
  currentShapeId = shapeId;
  currentShapeNote = msg;
  initFrameNote();
}


void Frame::handleTextSize(const size_t& shapeId, const size_t& textSize)
{
  currentShapeId = shapeId;
  currentTextSize = textSize;
  initFrameTextSize();
}


void Frame::displNumNodes(const size_t& val)
{
  string lbl = Utils::size_tToStr(val);
  lblNumNodes->SetLabel(wxString(lbl.c_str(), wxConvUTF8));
}


void Frame::displNumEdges(const size_t& val)
{
  string lbl = Utils::size_tToStr(val);
  lblNumEdges->SetLabel(wxString(lbl.c_str(), wxConvUTF8));
}


void Frame::displAttrInfo(
  const vector< size_t > &indices,
  const vector< string > &names,
  const vector< string > &types,
  const vector< size_t > &cards,
  const vector< string > &range)
{
  // clear attribute list
  listCtrlAttr->DeleteAllItems();

  // display new items
  if (((size_t) std::numeric_limits<long>::max()) < names.size())
  {
    wxMessageBox(wxT("Size of names exceeds max value of long"), wxT("Size of names exceeds max value of long"), wxOK|wxICON_INFORMATION, this);
  }

  long l_names_size = (long) names.size();

  for (long i = 0; i < l_names_size; ++i)
  {
    // add row
    listCtrlAttr->InsertItem(i, wxString(wxT("")));

    // associate index with item
    listCtrlAttr->SetItemData((long) indices[i], i);

    // column 0, do nothing
    // column 1
    listCtrlAttr->SetItem(
      i,
      1,
      wxString(Utils::size_tToStr(indices[i]).c_str(), wxConvUTF8));
    // column 2
    listCtrlAttr->SetItem(i, 2, wxString(names[i].c_str(), wxConvUTF8));
    // column 3
    listCtrlAttr->SetItem(i, 3, wxString(types[i].c_str(), wxConvUTF8));
    // column 4
    listCtrlAttr->SetItem(
      i,
      4,
      wxString(Utils::size_tToStr(cards[i]).c_str(), wxConvUTF8));
    // column 5
    listCtrlAttr->SetItem(
      i, 5, wxString(range[i].c_str(), wxConvUTF8));
  }

  attributeMenu->Enable(ID_MENU_ITEM_ATTR_DISTR_PLOT, false);
  attributeMenu->Enable(ID_MENU_ITEM_ATTR_CORRL_PLOT, false);
  attributeMenu->Enable(ID_MENU_ITEM_ATTR_COMBN_PLOT, false);
  attributeMenu->Enable(ID_MENU_ITEM_ATTR_DUPL, false);
  attributeMenu->Enable(ID_MENU_ITEM_ATTR_RENAME, false);
  attributeMenu->Enable(ID_MENU_ITEM_ATTR_DELETE, false);
  attributeMenu->Enable(ID_MENU_ITEM_ATTR_CLUST, false);
}


void Frame::displAttrInfo(
  const size_t& selectIdx,
  const vector< size_t > &indices,
  const vector< string > &names,
  const vector< string > &types,
  const vector< size_t > &cards,
  const vector< string > &range)
{
  displAttrInfo(
    indices,
    names,
    types,
    cards,
    range);

  // get attribute index
  listCtrlAttr->SetItemState(
    (long)selectIdx,
    wxLIST_STATE_SELECTED,
    wxLIST_STATE_SELECTED);
}


void Frame::displDomainInfo(
  const vector< size_t > &indices,
  const vector< string > &values,
  const vector< size_t > &number,
  const vector< double > &perc)
{
  // clear domain list
  listCtrlDomain->DeleteAllItems();

  if (((size_t) std::numeric_limits<long>::max()) < values.size())
  {
    wxMessageBox(wxT("Size of values exceeds max value of long"), wxT("Size of values exceeds max value of long"), wxOK|wxICON_INFORMATION, this);
  }
  long l_values_size = (long) values.size();

  // display new items
  for (long i = 0; i < l_values_size ; ++i)
  {
    // add row
    listCtrlDomain->InsertItem((long) indices[i], wxString(wxT("")));

    // associate index with item
    listCtrlDomain->SetItemData((long) indices[i], i);

    // column 0, do nothing
    // column 1
    listCtrlDomain->SetItem(
      i,
      1,
      wxString(Utils::intToStr((const int) indices[i]).c_str(), wxConvUTF8));
    // column 2
    listCtrlDomain->SetItem(i, 2, wxString(values[i].c_str(), wxConvUTF8));
    // column 3
    listCtrlDomain->SetItem(
      i,
      3,
      wxString(Utils::size_tToStr(number[i]).c_str(), wxConvUTF8));
    // column 4
    listCtrlDomain->SetItem(
      i,
      4,
      wxString(Utils::dblToStr(perc[i]).c_str(), wxConvUTF8));
  }

  domainMenu->Enable(ID_MENU_ITEM_DOM_GROUP, false);
  domainMenu->Enable(ID_MENU_ITEM_DOM_UNGROUP, false);
  domainMenu->Enable(ID_MENU_ITEM_DOM_RENAME, false);
}


void Frame::clearDomainInfo()
{
  listCtrlDomain->DeleteAllItems();

  domainMenu->Enable(ID_MENU_ITEM_DOM_GROUP, false);
  domainMenu->Enable(ID_MENU_ITEM_DOM_UNGROUP, false);
  domainMenu->Enable(ID_MENU_ITEM_DOM_RENAME, false);
}


void Frame::displDOFInfo(
  const vector< size_t > &degsOfFrdmIndcs,
  const vector< string > &degsOfFrdm,
  const vector< string > &attrNames,
  const size_t& selIdx)
{
  // make sure frame exists
  if (frameDOF == NULL)
  {
    initFrameDOF();
  }

  // clear all previous items
  listCtrlDOF->DeleteAllItems();

  if (((size_t) std::numeric_limits<long>::max()) < degsOfFrdmIndcs.size())
  {
    wxMessageBox(wxT("Size of degsOfFrdmIndcs exceeds max value of long"), wxT("Size of degsOfFrdmIndcs exceeds max value of long"), wxOK|wxICON_INFORMATION, this);
  }

  long l_degsOfFrdmIndcs_size = (long) degsOfFrdmIndcs.size();

  // display items
  for (long i = 0; i < l_degsOfFrdmIndcs_size; ++i)
  {
    // add row
    listCtrlDOF->InsertItem(i, wxString(wxT("")));

    // associate index with item
    listCtrlDOF->SetItemData(i, (long) degsOfFrdmIndcs[i]);

    // column 0, do nothing
    // column 1
    listCtrlDOF->SetItem(
      i,
      0,
      wxString(Utils::intToStr(i).c_str(), wxConvUTF8));
    // column 2
    listCtrlDOF->SetItem(i, 1, wxString(degsOfFrdm[i].c_str(), wxConvUTF8));
    // column 3
    listCtrlDOF->SetItem(i, 2, wxString(attrNames[i].c_str(), wxConvUTF8));
  }

  // select
  if (degsOfFrdmIndcs.size() > 0)
  {
    long item = listCtrlDOF->FindItem(
                  -1,
                  selIdx);
    if (item >= 0)
    {
      listCtrlDOF->SetItemState(
        item,
        wxLIST_STATE_SELECTED,
        wxLIST_STATE_SELECTED);

      /* int textStatus = mediator->handleGetDOFTextStatus( selIdx );
      if ( textStatus == DOF::ID_TEXT_NONE )
          radioBoxTextDOF->SetSelection( 0 );
      else if ( textStatus == DOF::ID_TEXT_ALL )
          radioBoxTextDOF->SetSelection( 1 );
      else if ( textStatus == DOF::ID_TEXT_ATTR )
          radioBoxTextDOF->SetSelection( 2 );
      else if ( textStatus == DOF::ID_TEXT_VAL )
          radioBoxTextDOF->SetSelection( 3 );
      */
    }
  }
}


void Frame::displShapeMenu(
  const bool& cut,
  const bool& copy,
  const bool& paste,
  const bool& clear,
  const bool& bringToFront,
  const bool& sendToBack,
  const bool& bringForward,
  const bool& sendBackward,
  const bool& editDOF,
  const int&  checkedItemId)
{
  wxMenu menu;
  dofMenu = false;

  addAttributeMenu = new wxMenu();
  int i;
  int id = wxID_LOWEST; // Event id's for Attributes

  // List All the Attributes in a Menu
  for (i = 0; i < listCtrlAttr->GetItemCount(); i++)
  {
    wxListItem rowInfo;
    wxString   celInfo;

    // set row
    rowInfo.m_itemId = i;
    // set column
    rowInfo.m_col    = 2;
    // set text mask
    rowInfo.m_mask   = wxLIST_MASK_TEXT;
    listCtrlAttr->GetItem(rowInfo);
    celInfo = rowInfo.m_text;
    wxString helpString;
    helpString << i; // Convert int to wxString
    wxMenuItem* item = new wxMenuItem(addAttributeMenu, id, celInfo, helpString, wxITEM_CHECK);
    addAttributeMenu->Append(item);
    if (id == checkedItemId)
    {
      addAttributeMenu->Check(id, true);
    }
    Connect(id, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Frame::onPopupMenu));
    id--;
  }

  menu.Append(ID_MENU_ITEM_SHOW_VARIABLES,
              wxT("Show Variable"),
              addAttributeMenu,
              wxT("Show variables values on this shape"));

  menu.Append(ID_MENU_ITEM_SHOW_NOTE,
              wxT("Edit Note"),
              wxT("Edit or add note to this shape"));
  menu.Append(ID_MENU_ITEM_TEXT_SIZE,
              wxT("Text Size"),
              wxT("Change the size of the text displayed on the shape"));

  menu.AppendSeparator();
  menu.Append(
    ID_MENU_ITEM_SHAPE_CUT,
    wxT("Cut"),
    wxT("Cut this shape"));
  menu.Append(
    ID_MENU_ITEM_SHAPE_COPY,
    wxT("Copy"),
    wxT("Copy this shape"));
  menu.Append(
    ID_MENU_ITEM_SHAPE_PASTE,
    wxT("Paste"),
    wxT("Paste shape"));

  menu.AppendSeparator();
  menu.Append(
    ID_MENU_ITEM_SHAPE_DELETE,
    wxT("Delete"),
    wxT("Delete this shape"));

  menu.AppendSeparator();
  menu.Append(
    ID_MENU_ITEM_SHAPE_BRING_TO_FRONT,
    wxT("Bring to front"),
    wxT("Bring this shape to front"));
  menu.Append(
    ID_MENU_ITEM_SHAPE_SEND_TO_BACK,
    wxT("Send to back"),
    wxT("Send this shape to back"));
  menu.Append(
    ID_MENU_ITEM_SHAPE_BRING_FORWARD,
    wxT("Bring forward"),
    wxT("Bring this shape forward"));
  menu.Append(
    ID_MENU_ITEM_SHAPE_SEND_BACKWARD,
    wxT("Send backward"),
    wxT("Send this shape backward"));
  menu.AppendSeparator();
  menu.Append(
    ID_MENU_ITEM_SHAPE_EDIT_DOF,
    wxT("Edit DOF"),
    wxT("Edit this shape's degrees of freedom"));

  if (cut != true)
  {
    menu.Enable(ID_MENU_ITEM_SHAPE_CUT, false);
  }
  if (copy != true)
  {
    menu.Enable(ID_MENU_ITEM_SHAPE_COPY, false);
  }
  if (paste != true)
  {
    menu.Enable(ID_MENU_ITEM_SHAPE_PASTE, false);
  }

  if (clear != true)
  {
    menu.Enable(ID_MENU_ITEM_SHAPE_DELETE, false);
  }
  if (bringToFront != true)
  {
    menu.Enable(ID_MENU_ITEM_SHAPE_BRING_TO_FRONT, false);
  }
  if (sendToBack != true)
  {
    menu.Enable(ID_MENU_ITEM_SHAPE_SEND_TO_BACK, false);
  }
  if (bringForward != true)
  {
    menu.Enable(ID_MENU_ITEM_SHAPE_BRING_FORWARD, false);
  }
  if (sendBackward != true)
  {
    menu.Enable(ID_MENU_ITEM_SHAPE_SEND_BACKWARD, false);
    menu.Enable(ID_MENU_ITEM_SHOW_NOTE, false);
    menu.Enable(ID_MENU_ITEM_TEXT_SIZE, false);
  }
  if (editDOF != true)
  {
    menu.Enable(ID_MENU_ITEM_SHAPE_EDIT_DOF, false);
    menu.Enable(ID_MENU_ITEM_SHOW_VARIABLES, false);
  }

  PopupMenu(&menu);

}


void Frame::displClusterMenu()
{
  if (clustMenu == NULL)
  {
    clustMenu = new wxMenu();

    int i;
    int id = wxID_LOWEST; // Event id's for Attributes
    // List All the Attributes in a Menu
    for (i = 0; i < listCtrlAttr->GetItemCount(); i++)
    {
      wxListItem rowInfo;
      wxString   celInfo;

      // set row
      rowInfo.m_itemId = i;
      // set column
      rowInfo.m_col    = 2;
      // set text mask
      rowInfo.m_mask   = wxLIST_MASK_TEXT;
      listCtrlAttr->GetItem(rowInfo);
      celInfo = rowInfo.m_text;
      wxString helpString;
      helpString << i; // Convert int to wxString
      wxMenuItem* item = new wxMenuItem(clustMenu, id, celInfo, helpString, wxITEM_CHECK);
      clustMenu->Append(item);
      Connect(id, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Frame::onClusterMenu));
      id--;
    }
    clustMenu->AppendSeparator();
    clustMenu->Append(
      ID_MENU_ITEM_CLUSTER,
      wxT("Cluster nodes"),
      wxT("Subcluster this cluster based on selected attributes"));
    clustMenu->Enable(ID_MENU_ITEM_CLUSTER, false);
  }
  PopupMenu(clustMenu);
}


void Frame::displDgrmMenu(
  const bool& sendSglToSiml,
  const bool& sendSglToTrace,
  const bool& sendSetToTrace,
  const bool& sendSglToExnr,
  const bool& sentSetToExnr)
{
  wxMenu menu;

  menu.Append(
    ID_MENU_ITEM_DGRM_SGL_TO_SIML,
    wxT("Send this to simulator"),
    wxT("Send this diagram to simulator"));

  menu.AppendSeparator();

  menu.Append(
    ID_MENU_ITEM_DGRM_SGL_TO_TRACE,
    wxT("Mark this in trace"),
    wxT("Mark this diagram in trace view"));
  menu.Append(
    ID_MENU_ITEM_DGRM_SET_TO_TRACE,
    wxT("Mark all in trace"),
    wxT("Mark set of diagrams in trace view"));

  menu.AppendSeparator();

  menu.Append(
    ID_MENU_ITEM_DGRM_SGL_TO_EXNR,
    wxT("Send this to examiner"),
    wxT("Send this diagram to examiner"));
  menu.Append(
    ID_MENU_ITEM_DGRM_SET_TO_EXNR,
    wxT("Send all to examiner"),
    wxT("Send set of diagrams to examiner"));

  if (sendSglToSiml != true)
  {
    menu.Enable(ID_MENU_ITEM_DGRM_SGL_TO_SIML, false);
  }
  if (sendSglToTrace != true)
  {
    menu.Enable(ID_MENU_ITEM_DGRM_SGL_TO_TRACE, false);
  }
  if (sendSetToTrace != true)
  {
    menu.Enable(ID_MENU_ITEM_DGRM_SET_TO_TRACE, false);
  }
  if (sendSglToExnr != true)
  {
    menu.Enable(ID_MENU_ITEM_DGRM_SGL_TO_EXNR, false);
  }
  if (sentSetToExnr != true)
  {
    menu.Enable(ID_MENU_ITEM_DGRM_SET_TO_EXNR, false);
  }

  PopupMenu(&menu);
}


void Frame::clearDOFInfo()
{
  if (frameDOF != NULL)
  {
    frameDOF->Close();
    frameDOF = NULL;
  }
}


void Frame::displClustMenu()
{
  wxMenu menu;

  // group & ungroup
  menu.Append(
    ID_MENU_ITEM_CLUST_DISTR_PLOT,
    wxT("Distribution plot"),
    wxT("Visualize the distribution of values"));
  menu.Append(
    ID_MENU_ITEM_CLUST_CORRL_PLOT,
    wxT("Correlation plot"),
    wxT("Visualize the correlation of values"));
  menu.Append(
    ID_MENU_ITEM_CLUST_COMBN_PLOT,
    wxT("Combination plot"),
    wxT("Visualize the combinations of values"));

  menu.AppendSeparator();
  menu.Append(
    ID_MENU_ITEM_CLUST_SUBCLUST,
    wxT("Cluster"),
    wxT("Subcluster this cluster based on selected attributes"));
  menu.Enable(ID_MENU_ITEM_CLUST_SUBCLUST, false);
  menu.Append(
    ID_MENU_ITEM_CLUST_UNCLUST,
    wxT("Uncluster"),
    wxT("Remove this cluster"));
  menu.Enable(ID_MENU_ITEM_CLUST_UNCLUST, false);

  PopupMenu(&menu);

}


void Frame::displAttrInfoClust(
  const vector< size_t > &indices,
  const vector< string > &names)
{
  if (frameClust != NULL)
  {
    // clear list
    listCtrlClust->DeleteAllItems();

    // display new items
    for (size_t i = 0; i < indices.size(); ++i)
    {
      // add row
      listCtrlClust->InsertItem((long) indices[i], wxString(wxT("")));

      // associate index with item
      listCtrlClust->SetItemData((long) indices[i], (long) i);

      // column 0, do nothing
      // column 1
      listCtrlClust->SetItem(
        (long) i,
        1,
        wxString(Utils::size_tToStr(indices[i]).c_str(), wxConvUTF8));
      // column 2
      listCtrlClust->SetItem((long) i, 2, wxString(names[i].c_str(), wxConvUTF8));
    }
  }
}


void Frame::displAttrInfoPart(
  string attrName,
  size_t minParts,
  size_t maxParts,
  size_t curParts)
{
}


void Frame::displSimClearDlg()
{
  wxString msg(wxT("Are you sure you want to clear the simulator?"));
  wxMessageDialog dialog(
    this,
    msg,
    wxString(wxT("Confirm simulator clear")),
    wxOK | wxCANCEL);

  // delete attribute
  if (dialog.ShowModal() == wxID_OK)
  {
    mediator->handleClearSim(this);
  }

  dialog.Destroy();
}


void Frame::displExnrClearDlg()
{
  wxString msg(wxT("Are you sure you want to clear the examiner history?"));
  wxMessageDialog dialog(
    this,
    msg,
    wxString(wxT("Confirm examiner clear")),
    wxOK | wxCANCEL);

  // delete attribute
  if (dialog.ShowModal() == wxID_OK)
  {
    mediator->handleClearExnr(this);
  }

  dialog.Destroy();
}


void Frame::displExnrFrameMenu(const bool& clear)
{
  wxMenu* menu = new wxMenu();

  menu->Append(
    ID_MENU_ITEM_EXNR_CLEAR,
    wxT("Delete"),
    wxT("Delete this diagram"));

  if (clear == false)
  {
    menu->Enable(ID_MENU_ITEM_EXNR_CLEAR, false);
  }

  PopupMenu(menu);

  delete menu;
  menu = NULL;
}


void Frame::selectAttribute(const size_t& idx)
{
  if (idx != NON_EXISTING && idx < ((size_t) listCtrlAttr->GetItemCount()))
  {
    // select idx'th item
    long item = -1;

    item = listCtrlAttr->FindItem(item, idx);

    if (item >= 0)
    {
      listCtrlAttr->SetItemState(
        item,
        wxLIST_STATE_SELECTED,
        wxLIST_STATE_SELECTED);
    }
  }
}


void Frame::selectDomainVal(const size_t& idx)
{
  if (idx != NON_EXISTING && idx < ((size_t) listCtrlDomain->GetItemCount()))
  {
    // select idx'th item
    long item = -1;

    item = listCtrlDomain->FindItem(item, idx);

    if (item >= 0)
      listCtrlDomain->SetItemState(
        item,
        wxLIST_STATE_SELECTED,
        wxLIST_STATE_SELECTED);
  }
}


void Frame::handleDragDrop(
  const int& srcId,
  const int& tgtId,
  const int& tgtX,
  const int& tgtY,
  const vector< int > &data)
{
  if (srcId == ID_LIST_CTRL_ATTR &&
      tgtId == ID_LIST_CTRL_ATTR)
  {
    int idxFr = -1;
    int idxTo = -1;
    int flag = wxLIST_HITTEST_ONITEM;

    idxFr = data[0];
    idxTo = listCtrlAttr->HitTest(wxPoint(tgtX, tgtY), flag);

    if (idxFr >= 0 && idxTo >= 0)
    {
      mediator->handleMoveAttr(idxFr, idxTo);
    }
  }
  else if (srcId == ID_LIST_CTRL_DOMAIN &&
           tgtId == ID_LIST_CTRL_DOMAIN)
  {
    int attrIdx  = -1;
    int domIdxFr = -1;
    int domIdxTo = -1;
    int flag = wxLIST_HITTEST_ONITEM;

    attrIdx  = data[0];
    domIdxFr = data[1];
    domIdxTo = listCtrlDomain->HitTest(wxPoint(tgtX, tgtY), flag);

    if (attrIdx >= 0 && domIdxFr >= 0 && domIdxTo >= 0)
    {
      mediator->handleMoveDomVal(attrIdx, domIdxFr, domIdxTo);
    }
  }
  else if (srcId == ID_LIST_CTRL_ATTR &&
           tgtId == ID_LIST_CTRL_DOF)
  {
    int idxAttr = -1;
    int idxDOF  = -1;
    size_t idDOF   = NON_EXISTING;
    int flag = wxLIST_HITTEST_ONITEM;

    idxAttr = data[0];

    idxDOF = listCtrlDOF->HitTest(wxPoint(tgtX, tgtY), flag);

    long item = -1;
    for (int i = 0; i < listCtrlDOF->GetItemCount(); ++i)
    {
      item = listCtrlDOF->GetNextItem(item);

      if (i == idxDOF)
      {
        break;
      }
    }

    idDOF = listCtrlDOF->GetItemData(item);
    mediator->handleLinkDOFAttr(idDOF, idxAttr);
  }
}


void Frame::closePopupFrames()
{
  if (frameDOF != NULL)
  {
    frameDOF->Close();
    frameDOF = NULL;
  }

  if (framePlot != NULL)
  {
    framePlot->Close();
    framePlot = NULL;
  }

  if (frameClust != NULL)
  {
    frameClust->Close();
    frameClust = NULL;
  }
}


void Frame::handleCloseFrame(PopupFrame* f)
{
  if (f == frameDOF)
  {
    // clean up ptr
    frameDOF     = NULL;
    canvasColDOF = NULL;
    canvasOpaDOF  = NULL;
    // deselect all shapes
    mediator->handleDOFFrameDestroy();
  }
  else if (f == framePlot)
  {
    // clean up ptrs
    framePlot  = NULL;
    canvasPlot = NULL;
    // clean up other vis ptrs to canvas
    mediator->handlePlotFrameDestroy();
  }
  else if (f == frameClust)
  {
    // clean up ptr
    frameClust = NULL;
  }
  else if (f == frameNote)
  {
    frameNote = NULL;
    toolBarEdit->EnableTool(ID_TOOL_NOTE, true);
  }
  else if (f == frameTextSize)
  {
    frameTextSize = NULL;
  }
}


// -- get functions -------------------------------------------------


GLCanvas* Frame::getCanvasArcD()
{
  return canvasOne;
}


GLCanvas* Frame::getCanvasSiml()
{
  return canvasTwo;
}


GLCanvas* Frame::getCanvasTrace()
{
  return canvasTwo;
}


GLCanvas* Frame::getCanvasExnr()
{
  return canvasThree;
}


GLCanvas* Frame::getCanvasEdit()
{
  return canvasOne;
}


GLCanvas* Frame::getCanvasDistr()
{
  return canvasPlot;
}


GLCanvas* Frame::getCanvasCorrl()
{
  return canvasPlot;
}


GLCanvas* Frame::getCanvasCombn()
{
  return canvasPlot;
}


GLCanvas* Frame::getCanvasColDOF()
{
  return canvasColDOF;
}


GLCanvas* Frame::getCanvasOpaDOF()
{
  return canvasOpaDOF;
}


// -- clear functions -----------------------------------------------


void Frame::clearOuput()
{
  lblNumNodes->SetLabel(wxT(""));
  lblNumEdges->SetLabel(wxT(""));

  //buttonClustAttr->Enable( false );
  //buttonTraceAttr->Enable( false );

  listCtrlAttr->DeleteAllItems();
  listCtrlDomain->DeleteAllItems();


  //textCtrl->Clear();

}


// -- overloaded operators ------------------------------------------


void Frame::operator<<(const string& msg)
{
  appOutputText(msg);
}


// -- GUI initialization --------------------------------------------


void Frame::initFrame()
{
  wxSize maximum_size = wxGetClientDisplayRect().GetSize();
  int hCur;

  // set min and max size
  SetSizeHints(
    800,
    600,
    maximum_size.GetWidth(),
    maximum_size.GetHeight());

  // init sizer for frame
  sizerFrame = new wxBoxSizer(wxVERTICAL);
  SetSizer(sizerFrame);
  sizerFrame->Fit(this);

  // init contents of frame
  initIcon();
  initMenuBar();
  initSplitterFrame();
  CreateStatusBar();

  // maximize and center
  Maximize();
  Center();

  // fit everything before positioning sash positions
  Layout();
  Fit();

  // set sash positions
  hCur = GetClientSize().GetHeight();
  splitterFrame->SetSashPosition(300);
  splitterLft->SetSashPosition(hCur-300);
  splitterRgt->SetSashPosition(hCur-300);
  splitterTopLft->SetSashPosition((int)(0.66*(hCur-300)));

  // DOF frame only shown on request
  frameDOF = NULL;
  // plot frame only shown on request
  framePlot = NULL;
  // cluster frame only shown on request
  frameClust = NULL;

  // fit everything after positioning sash positions
  Layout();
  Fit();
}


void Frame::initIcon()
{
  try
  {
    wxIcon icon(logo);
    SetIcon(icon);   // icon defined in 'figures.xpm'
  }
  catch (...)
  {
    // do nothing
  }
}


void Frame::initMenuBar()
{
  // menu bar
  menuBar = new wxMenuBar();

  // file menu
  fileMenu = new wxMenu();
  fileMenu->Append(
    wxID_OPEN,
    wxString(wxT("&Open...")),
    wxString(wxT("Opens an FSM file")));
  fileMenu->Append(
    wxID_SAVE,
    wxString(wxT("&Save")),
    wxString(wxT("Saves the FSM file")));
  fileMenu->Enable(wxID_SAVE, false);
  fileMenu->Append(
    wxID_SAVEAS,
    wxString(wxT("&Save as...")),
    wxString(wxT("Saves the FSM file with a new name")));
  fileMenu->Enable(wxID_SAVEAS, false);

  fileMenu->AppendSeparator();
  fileMenu->Append(
    ID_MENU_ITEM_LOAD_CONFIG,
    wxString(wxT("Load attribute config")),
    wxString(wxT("Loads user-defined attribute configuration")));
  fileMenu->Enable(ID_MENU_ITEM_LOAD_CONFIG, false);
  fileMenu->Append(
    ID_MENU_ITEM_SAVE_CONFIG,
    wxString(wxT("Save attribute config")),
    wxString(wxT("Saves user-defined attribute configuration")));
  fileMenu->Enable(ID_MENU_ITEM_SAVE_CONFIG, false);

  fileMenu->AppendSeparator();
  fileMenu->Append(
    ID_MENU_ITEM_LOAD_DIAGRAM,
    wxString(wxT("Load diagram")),
    wxString(wxT("&Loads user-defined diagram")));
  fileMenu->Enable(ID_MENU_ITEM_LOAD_DIAGRAM, false);
  fileMenu->Append(
    ID_MENU_ITEM_SAVE_DIAGRAM,
    wxString(wxT("Save &diagram")),
    wxString(wxT("Saves user-defined diagram")));
  fileMenu->Enable(ID_MENU_ITEM_SAVE_DIAGRAM, false);

  fileMenu->AppendSeparator();
  fileMenu->Append(
    wxID_CLOSE,
    wxString(wxT("E&xit")),
    wxString(wxT("Quits the program")));
  menuBar->Append(
    fileMenu,
    wxString(wxT("&File")));

  // mode menu
  modeMenu = new wxMenu();
  modeMenu->AppendRadioItem(
    ID_MENU_ITEM_MODE_ANALYSIS,
    wxString(wxT("&Analysis mode")),
    wxString(wxT("Changes to analysis mode")));
  modeMenu->Check(ID_MENU_ITEM_MODE_ANALYSIS, true);
  modeMenu->AppendRadioItem(
    ID_MENU_ITEM_MODE_EDIT,
    wxString(wxT("&Edit mode")),
    wxString(wxT("Changes to edit mode")));
  modeMenu->Check(ID_MENU_ITEM_MODE_EDIT, false);
  modeMenu->Enable(ID_MENU_ITEM_MODE_EDIT, false);
  menuBar->Append(
    modeMenu,
    wxString(wxT("&Mode")));

  // view menu
  viewMenu = new wxMenu();
  viewMenu->AppendRadioItem(
    ID_MENU_ITEM_VIEW_SIM,
    wxString(wxT("Simulation")),
    wxString(wxT("Shows simulation view")));
  viewMenu->Check(ID_MENU_ITEM_VIEW_SIM, true);
  viewMenu->AppendRadioItem(
    ID_MENU_ITEM_VIEW_TRACE,
    wxString(wxT("Trace")),
    wxString(wxT("Shows trace view")));
  viewMenu->Check(ID_MENU_ITEM_VIEW_TRACE, false);
  menuBar->Append(
    viewMenu,
    wxString(wxT("&View")));

  // attribute menu
  attributeMenu = new wxMenu();
  attributeMenu->Append(
    ID_MENU_ITEM_ATTR_CLUST,
    wxString(wxT("Cluster nodes")),
    wxString(wxT("Cluster nodes based on selected attributes")));
  attributeMenu->Enable(ID_MENU_ITEM_ATTR_CLUST, false);
  attributeMenu->Append(
    ID_MENU_ITEM_ATTR_TRACE,
    wxString(wxT("View trace")),
    wxString(wxT("View trace based on selected attributes")));
  attributeMenu->Enable(ID_MENU_ITEM_ATTR_TRACE, false);
  attributeMenu->AppendSeparator();
  attributeMenu->Append(
    ID_MENU_ITEM_ATTR_DISTR_PLOT,
    wxString(wxT("Distribution plot")),
    wxString(wxT("Visualize the distribution of values")));
  attributeMenu->Enable(ID_MENU_ITEM_ATTR_DISTR_PLOT, false);
  attributeMenu->Append(
    ID_MENU_ITEM_ATTR_CORRL_PLOT,
    wxString(wxT("Correlation plot")),
    wxString(wxT("Visualize the correlation of values")));
  attributeMenu->Enable(ID_MENU_ITEM_ATTR_CORRL_PLOT, false);
  attributeMenu->Append(
    ID_MENU_ITEM_ATTR_COMBN_PLOT,
    wxString(wxT("Combination plot")),
    wxString(wxT("Visualize the combinations of values")));
  attributeMenu->AppendSeparator();
  attributeMenu->Enable(ID_MENU_ITEM_ATTR_COMBN_PLOT, false);
  attributeMenu->Append(
    ID_MENU_ITEM_ATTR_DUPL,
    wxString(wxT("Duplicate")),
    wxString(wxT("Duplicate attribute")));
  attributeMenu->Enable(ID_MENU_ITEM_ATTR_DUPL, false);
  attributeMenu->Append(
    ID_MENU_ITEM_ATTR_RENAME,
    wxString(wxT("Rename")),
    wxString(wxT("Rename attribute")));
  attributeMenu->Enable(ID_MENU_ITEM_ATTR_RENAME, false);
  attributeMenu->Append(
    ID_MENU_ITEM_ATTR_DELETE,
    wxString(wxT("Delete")),
    wxString(wxT("Delete attribute")));
  attributeMenu->Enable(ID_MENU_ITEM_ATTR_DELETE, false);

  menuBar->Append(
    attributeMenu,
    wxString(wxT("&Attributes")));

  // domain menu
  domainMenu = new wxMenu();
  domainMenu->Append(
    ID_MENU_ITEM_DOM_GROUP,
    wxString(wxT("Group")),
    wxString(wxT("Group selected domain values")));
  domainMenu->Enable(ID_MENU_ITEM_DOM_GROUP, false);
  domainMenu->Append(
    ID_MENU_ITEM_DOM_UNGROUP,
    wxString(wxT("Ungroup")),
    wxString(wxT("Ungroup domain values")));
  domainMenu->Enable(ID_MENU_ITEM_DOM_UNGROUP, false);
  domainMenu->Append(
    ID_MENU_ITEM_DOM_RENAME,
    wxString(wxT("Rename")),
    wxString(wxT("Rename selected domain value")));
  domainMenu->Enable(ID_MENU_ITEM_DOM_RENAME, false);
  menuBar->Append(
    domainMenu,
    wxString(wxT("&Domain")));

  // settings menu
  settingsMenu = new wxMenu();
  settingsMenu->Append(
    ID_MENU_ITEM_SETTINGS_GENERAL,
    wxString(wxT("General")),
    wxString(wxT("Adjusts general settings.")));
  settingsMenu->Append(
    ID_MENU_ITEM_SETTINGS_CLUST_TREE,
    wxString(wxT("Clustering tree")),
    wxString(wxT("Adjusts clustering tree settings.")));
  settingsMenu->Append(
    ID_MENU_ITEM_SETTINGS_BAR_TREE,
    wxString(wxT("Bar tree")),
    wxString(wxT("Adjusts bar tree settings.")));
  settingsMenu->Append(
    ID_MENU_ITEM_SETTINGS_ARC_DIAGRAM,
    wxString(wxT("Arc diagram")),
    wxString(wxT("Adjusts arc diagram settings.")));
  settingsMenu->Append(
    ID_MENU_ITEM_SETTINGS_SIMULATOR,
    wxString(wxT("Simulation")),
    wxString(wxT("Adjusts simulation settings.")));
  settingsMenu->Append(
    ID_MENU_ITEM_SETTINGS_TRACE,
    wxString(wxT("Trace")),
    wxString(wxT("Adjusts trace settings")));
  settingsMenu->Append(
    ID_MENU_ITEM_SETTINGS_EDITOR,
    wxString(wxT("Diagram editor")),
    wxString(wxT("Adjusts diagram editor settings.")));
  menuBar->Append(
    settingsMenu,
    wxString(wxT("Se&ttings")));

  // help menu
  helpMenu = new wxMenu();
  helpMenu->Append(
    wxID_HELP,
    wxString(wxT("&Contents")),
    wxString(wxT("Show help contents")));
  helpMenu->AppendSeparator();
  helpMenu->Append(
    wxID_ABOUT,
    wxString(wxT("&About")));
  menuBar->Append(
    helpMenu,
    wxString(wxT("&Help")));

  // add menubar
  SetMenuBar(menuBar);
}


void Frame::initSplitterFrame()
{
  // init splitter window
  splitterFrame = new wxSplitterWindow(
    this,
    ID_SPLITTER_FRAME, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE);
  splitterFrame->SetSashGravity(0.0);
  splitterFrame->SetMinimumPaneSize(200);
  sizerFrame->Add(
    splitterFrame,
    1,
    wxEXPAND);

  // init children
  initSplitterLft();
  initSplitterRgt();

  // split window
  splitterFrame->SplitVertically(
    splitterLft,
    splitterRgt);
}


void Frame::initSplitterLft()
{
  // init splitter window
  splitterLft = new wxSplitterWindow(
    splitterFrame,
    ID_SPLITTER_LFT, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE);
  splitterLft->SetSashGravity(1.0);
  splitterLft->SetMinimumPaneSize(200);

  // init children
  initSplitterTopLft();
  initPanelBotLft();

  // split window
  splitterLft->SplitHorizontally(
    splitterTopLft,
    panelBotLft);
}


void Frame::initSplitterTopLft()
{
  // init splitter window
  splitterTopLft = new wxSplitterWindow(
    splitterLft,
    ID_SPLITTER_TOP_LFT, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE);
  splitterTopLft->SetSashGravity(1.0);
  splitterTopLft->SetMinimumPaneSize(100);

  // init children
  initPanelTopTopLft();
  initPanelBotTopLft();

  // split window
  splitterTopLft->SplitHorizontally(
    panelTopTopLft,
    panelBotTopLft);
}


void Frame::initPanelTopTopLft()
{
  // init panel
  sizerTopTopLft = new wxBoxSizer(wxVERTICAL);
  panelTopTopLft = new wxScrolledWindow(
    splitterTopLft,
    ID_PANEL_TOP_TOP_LFT,
    wxDefaultPosition,
    wxDefaultSize,
    wxHSCROLL |
    wxVSCROLL |
    wxRAISED_BORDER);
  panelTopTopLft->SetSizer(sizerTopTopLft);
  sizerTopTopLft->Fit(panelTopTopLft);
  panelTopTopLft->SetScrollRate(10, 10);

  // init children
  initLabelsGraphInfo();
  initListCtrlAttr();
  initButtonsAttr();
}


void Frame::initLabelsGraphInfo()
{
  // init static box
  wxStaticBoxSizer* box = new wxStaticBoxSizer(
    wxVERTICAL,
    panelTopTopLft,
    wxString(wxT("Graph")));
  sizerTopTopLft->Add(
    box,
    0,        // vert no stretch
    wxSHAPED  // hori stretch
    | wxALL,  // border around
    5);

  // init grid sizer
  wxFlexGridSizer* lblSizer = new wxFlexGridSizer(
    2,        // rows
    2,        // cols
    0,        // vgap
    0);       // hgap

  // nodes label
  wxStaticText* nodesLbl = new wxStaticText(
    panelTopTopLft,
    wxID_ANY,
    wxString(wxT("Number nodes:\t")));
  lblSizer->Add(
    nodesLbl,
    0,        // vert no stretch
    wxGROW |  // hori stretch
    wxLEFT,   // border left
    5);

  // num nodes label
  lblNumNodes = new wxStaticText(
    panelTopTopLft,
    wxID_ANY,
    wxString(wxT("                   ")));
  lblSizer->Add(
    lblNumNodes,
    0,        // vert no stretch
    wxGROW);  // hori stretch

  // edges label
  wxStaticText* edgesLbl = new wxStaticText(
    panelTopTopLft,
    wxID_ANY,
    wxString(wxT("Number edges:\t")));
  lblSizer->Add(
    edgesLbl,
    0,        // vert not stretch
    wxGROW |  // hori stretch
    wxTOP |   // border top
    wxLEFT,   // border left
    5);

  // num edges label
  lblNumEdges =new wxStaticText(
    panelTopTopLft,
    wxID_ANY,
    wxString(wxT("                   ")));
  lblSizer->Add(
    lblNumEdges,
    0,        // vert not stretchable
    wxGROW |  // hori stretch
    wxTOP,    // border top
    5);

  // add grid sizer
  box->Add(
    lblSizer,
    0,        // vert not stretchable
    wxGROW);  // hori stretchable

  // clear ptrs
  box      = NULL;
  lblSizer = NULL;
  nodesLbl = NULL;
  edgesLbl = NULL;
}


void Frame::initListCtrlAttr()
{
  // label
  wxStaticText* lbl = new wxStaticText(
    panelTopTopLft,
    wxID_ANY,
    wxString(wxT("Attributes")));
  sizerTopTopLft->Add(
    lbl,
    0,          // vert not stretch
    wxEXPAND |  // hori stretch
    wxLEFT,     // border left
    10);

  // attribute list
  listCtrlAttr = new wxListCtrl(
    panelTopTopLft,
    ID_LIST_CTRL_ATTR,
    wxDefaultPosition,
    wxDefaultSize,
    wxLC_REPORT |
    wxLC_HRULES |
    wxLC_VRULES);

  sizerTopTopLft->Add(
    listCtrlAttr,
    1,
    wxEXPAND | wxBOTTOM |wxLEFT,  // vert & hori stretch, border all around
    10);

  // add drop target
  listCtrlAttr->SetDropTarget(
    new DropTarget(
      listCtrlAttr,
      mediator));

  // columns
  wxListItem colItem;

  // dummy column
  colItem.SetText(wxT(""));
  listCtrlAttr->InsertColumn(0, colItem);
  listCtrlAttr->SetColumnWidth(0, 0);
  // column 1
  colItem.SetText(wxT(""));
  colItem.SetAlign(wxLIST_FORMAT_RIGHT);
  listCtrlAttr->InsertColumn(1, colItem);
  listCtrlAttr->SetColumnWidth(1, 30);
  // column 2
  colItem.SetText(wxT("Name"));
  colItem.SetAlign(wxLIST_FORMAT_LEFT);
  listCtrlAttr->InsertColumn(2, colItem);
  listCtrlAttr->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
  // column 3
  colItem.SetText(wxT("Type"));
  colItem.SetAlign(wxLIST_FORMAT_LEFT);
  listCtrlAttr->InsertColumn(3, colItem);
  listCtrlAttr->SetColumnWidth(3, wxLIST_AUTOSIZE_USEHEADER);
  // column 4
  colItem.SetText(wxT("Cardinality"));
  colItem.SetAlign(wxLIST_FORMAT_RIGHT);
  listCtrlAttr->InsertColumn(4, colItem);
  listCtrlAttr->SetColumnWidth(4, wxLIST_AUTOSIZE_USEHEADER);
  // column 5
  colItem.SetText(wxT("[ Range ]"));
  colItem.SetAlign(wxLIST_FORMAT_LEFT);
  listCtrlAttr->InsertColumn(5, colItem);
  listCtrlAttr->SetColumnWidth(5, wxLIST_AUTOSIZE_USEHEADER);

  // reset ptr
  lbl = NULL;
}


void Frame::initButtonsAttr()
{
  /*
  // init static box
  wxStaticBoxSizer* box = new wxStaticBoxSizer(
      wxHORIZONTAL,
      panelTopTopLft,
      wxString( wxT( "" ) ) );
  sizerTopTopLft->Add(
      box,
      0,        // vert no stretch
  wxALL,  // border around
      0 );

  // cluster button
  buttonClustAttr = new wxButton(
      panelTopTopLft,
      ID_BUTTON_CLUST_ATTR,
      wxString( wxT( "Cluster nodes >>") ),
      wxDefaultPosition,
      wxDefaultSize );
  buttonClustAttr->Enable( false );

  box->Add(
      buttonClustAttr,
      0,                           // vert stretch
      wxALIGN_RIGHT |              // hori stretch
      wxLEFT | wxRIGHT | wxBOTTOM, // border
      2 );

  // trace button
  buttonTraceAttr = new wxButton(
      panelTopTopLft,
      ID_BUTTON_TRACE_ATTR,
      wxString( wxT( "View trace >>") ),
      wxDefaultPosition,
      wxDefaultSize );
  buttonTraceAttr->Enable( false );

  box->Add(
      buttonTraceAttr,
      0,
      wxALIGN_RIGHT |
      wxLEFT | wxRIGHT | wxBOTTOM, // border
      2 );

  */
}


void Frame::initPanelBotTopLft()
{
  // init panel
  sizerBotTopLft = new wxBoxSizer(wxVERTICAL);
  panelBotTopLft = new wxScrolledWindow(
    splitterTopLft,
    ID_PANEL_BOT_TOP_LFT,
    wxDefaultPosition,
    wxDefaultSize,
    wxHSCROLL |
    wxVSCROLL |
    wxRAISED_BORDER);
  panelBotTopLft->SetSizer(sizerBotTopLft);
  sizerBotTopLft->Fit(panelBotTopLft);
  panelBotTopLft->SetScrollRate(10, 10);

  // children
  initListCtrlDomain();
}


void Frame::initListCtrlDomain()
{
  // label
  wxStaticText* lbl = new wxStaticText(
    panelBotTopLft,
    wxID_ANY,
    wxString(wxT("Domain")));
  sizerBotTopLft->Add(
    lbl,
    0,          // vert not stretch
    wxEXPAND |  // hori stretch
    wxLEFT,     // border left
    10);

  // list
  listCtrlDomain = new wxListCtrl(
    panelBotTopLft,
    ID_LIST_CTRL_DOMAIN,
    wxDefaultPosition,
    wxDefaultSize,
    wxLC_REPORT |
    wxLC_HRULES |
    wxLC_VRULES);
  sizerBotTopLft->Add(
    listCtrlDomain,
    1,         // vert stretch
    wxEXPAND | // hori stretch
    wxLEFT,     // border
    5);

  // add drop target
  listCtrlDomain->SetDropTarget(
    new DropTarget(
      listCtrlDomain,
      mediator));

  // columns
  wxListItem colItem;

  // dummy column
  colItem.SetText(wxT(""));
  listCtrlDomain->InsertColumn(0, colItem);
  listCtrlDomain->SetColumnWidth(0, 0);
  // column 1
  colItem.SetText(wxT(""));
  colItem.SetAlign(wxLIST_FORMAT_RIGHT);
  listCtrlDomain->InsertColumn(1, colItem);
  listCtrlDomain->SetColumnWidth(1, 30);
  // column 2
  colItem.SetText(wxT("Value"));
  colItem.SetAlign(wxLIST_FORMAT_LEFT);
  listCtrlDomain->InsertColumn(2, colItem);
  listCtrlDomain->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
  // column 3
  colItem.SetText(wxT("Num nodes"));
  colItem.SetAlign(wxLIST_FORMAT_RIGHT);
  listCtrlDomain->InsertColumn(3, colItem);
  listCtrlDomain->SetColumnWidth(3, wxLIST_AUTOSIZE_USEHEADER);
  // column 4
  colItem.SetText(wxT("Perc nodes"));
  colItem.SetAlign(wxLIST_FORMAT_RIGHT);
  listCtrlDomain->InsertColumn(4, colItem);
  listCtrlDomain->SetColumnWidth(4, wxLIST_AUTOSIZE_USEHEADER);
}


void Frame::initPanelBotLft()
{
  // init panel
  sizerBotLft = new wxBoxSizer(wxHORIZONTAL);
  panelBotLft = new wxScrolledWindow(
    splitterLft,
    ID_PANEL_BOT_LFT,
    wxDefaultPosition,
    wxDefaultSize,
    wxRAISED_BORDER);
  panelBotLft->SetSizer(sizerBotLft);
  sizerBotLft->Fit(panelBotLft);
  panelBotLft->SetScrollRate(10, 10);

  // children
  initCanvasThree();
}


void Frame::initCanvasThree()
{
  canvasThree = new GLCanvas(
    mediator,
    panelBotLft,
    ID_CANVAS_RGT);

  sizerBotLft->Add(
    canvasThree,
    1,
    wxEXPAND | wxALL, 10);
}


void Frame::initSplitterRgt()
{
  // init splitter window
  splitterRgt = new wxSplitterWindow(
    splitterFrame,
    ID_SPLITTER_RGT, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE);
  splitterRgt->SetSashGravity(1.0);
  splitterRgt->SetMinimumPaneSize(200);

  // init children
  initPanelTopRgt();
  initPanelBotRgt();

  // split window
  splitterRgt->SplitHorizontally(
    panelTopRgt,
    panelBotRgt);
}


void Frame::initPanelTopRgt()
{
  // init panel
  sizerTopRgt = new wxBoxSizer(wxHORIZONTAL);
  panelTopRgt = new wxScrolledWindow(
    splitterRgt,
    ID_PANEL_TOP_RGT,
    wxDefaultPosition,
    wxDefaultSize,
    wxHSCROLL |
    wxVSCROLL |
    wxRAISED_BORDER);
  panelTopRgt->SetSizer(sizerTopRgt);
  sizerTopRgt->Fit(panelTopRgt);
  panelTopRgt->SetScrollRate(10, 10);
  //panelTopRgt->SetWindowStyle( wxNO_BORDER );

  // children
  initCanvasOne();
  initToolbarEdit();

  toolBarEdit->Show(false);
}


void Frame::initCanvasOne()
{
  canvasOne = new GLCanvas(
    mediator,
    panelTopRgt,
    ID_CANVAS_MAIN);
  //canvasOne->SetMinSize( panelTopRgt->GetSize() );

  sizerTopRgt->Add(
    canvasOne,
    1,
    wxEXPAND | wxALL, 10);

}


void Frame::initToolbarEdit()
{
  // init toolbar
  toolBarEdit = new wxToolBar(
    panelTopRgt,
    ID_TOOL_BAR_EDIT,
    wxDefaultPosition,
    wxDefaultSize,
    wxTB_VERTICAL |
    wxTB_FLAT);

  // add tools, figures defined in 'figures.xpm'
  // selection
  wxBitmap selectBmp(select_icon);
  toolBarEdit->AddTool(ID_TOOL_SELECT, wxString(wxT("Selection")), selectBmp);
  // note
  wxBitmap noteBmp(note);
  toolBarEdit->AddTool(ID_TOOL_NOTE, wxString(wxT("Add Note")), noteBmp);
  // edit DOF
  wxBitmap dofBmp(dof);
  toolBarEdit->AddTool(ID_TOOL_DOF, wxString(wxT("Edit DOF")), dofBmp);
  // rectangle
  wxBitmap rectBmp(rectangle);
  toolBarEdit->AddTool(ID_TOOL_RECT, wxString(wxT("Rectangle")), rectBmp);
  // ellipse
  wxBitmap ellipseBmp(ellipse);
  toolBarEdit->AddTool(ID_TOOL_ELLIPSE, wxString(wxT("Ellipse")), ellipseBmp);
  // line
  wxBitmap lineBmp(line);
  toolBarEdit->AddTool(ID_TOOL_LINE, wxString(wxT("Line")), lineBmp);
  // arrow
  wxBitmap arrowBmp(arrow);
  toolBarEdit->AddTool(ID_TOOL_ARROW, wxString(wxT("Arrow")), arrowBmp);
  // double arrow
  wxBitmap darrowBmp(darrow);
  toolBarEdit->AddTool(ID_TOOL_DARROW, wxString(wxT("Double arrow")), darrowBmp);

  toolBarEdit->AddSeparator();
  // fill color
  wxBitmap fillColBmp(fillcol);
  toolBarEdit->AddTool(ID_TOOL_FILL_COL, wxString(wxT("Fill color")), fillColBmp);
  // line color
  wxBitmap lineColBmp(linecol);
  toolBarEdit->AddTool(ID_TOOL_LINE_COL, wxString(wxT("Line color")), lineColBmp);

  toolBarEdit->AddSeparator();
  // show grid
  wxBitmap showGridBmp(showgrid);
  toolBarEdit->AddCheckTool(ID_TOOL_SHOW_GRID, wxString(wxT("Show grid")), showGridBmp);
  toolBarEdit->ToggleTool(ID_TOOL_SHOW_GRID, true);
  // snap grid
  wxBitmap snapGridBmp(snapgrid);
  toolBarEdit->AddCheckTool(ID_TOOL_SNAP_GRID, wxString(wxT("Snap grid")), snapGridBmp);
  toolBarEdit->ToggleTool(ID_TOOL_SNAP_GRID, true);

  // finalize toolbar
  toolBarEdit->Realize();
  sizerTopRgt->Add(
    toolBarEdit,
    0,
    wxEXPAND |
    wxALL,
    5);
}


void Frame::initPanelBotRgt()
{
  // init panel
  sizerBotRgt = new wxBoxSizer(wxHORIZONTAL);
  panelBotRgt = new wxScrolledWindow(
    splitterRgt,
    ID_PANEL_BOT_RGT,
    wxDefaultPosition,
    wxDefaultSize,
    wxHSCROLL |
    wxVSCROLL |
    wxRAISED_BORDER);
  panelBotRgt->SetSizer(sizerBotRgt);
  sizerBotRgt->Fit(panelBotRgt);
  panelBotRgt->SetScrollRate(10, 10);

  // children
  initCanvasTwo();
}


void Frame::initCanvasTwo()
{
  canvasTwo = new GLCanvas(
    mediator,
    panelBotRgt,
    ID_CANVAS_LFT);
  canvasTwo->SetMinSize(panelBotRgt->GetSize());

  sizerBotRgt->Add(
    canvasTwo,
    1,         // vert stretch
    wxEXPAND | // hori stretch
    wxALL,     // border
    10);
}

void Frame::initTextCtrl()
{
  textCtrl = new wxTextCtrl(
    panelRgtBotRgt,
    ID_TEXTCTRL,
    wxString(wxT("")),
    wxDefaultPosition,
    wxDefaultSize,
    wxTE_MULTILINE | // multiline
    wxTE_RICH |      // allow more text
    wxTE_READONLY);  // read-only

  sizerRgtBotRgt->Add(
    textCtrl,
    0,         // vert stretch
    wxEXPAND | // hori stretch
    wxALL,     // border
    5);
}

void Frame::initFrameDOF()
{
  // init frame
  sizerFrameDOF = new wxBoxSizer(wxHORIZONTAL);
  frameDOF = new PopupFrame(
    mediator,
    this,
    ID_FRAME_DOF,
    wxString(wxT("Degrees of freedom")),
    wxPoint(350,250),
    wxSize(
      (int)(0.27*this->GetSize().GetWidth()),
      (int)(0.50*this->GetSize().GetHeight())));
  frameDOF->SetSizer(sizerFrameDOF);

  // init children
  initPanelDOF();

  frameDOF->Layout();
  frameDOF->Fit();

  // show
  frameDOF->Show();
}


void Frame::initFrameNote()
{
  if (frameNote == NULL)
  {
    // init frame
    sizerFrameNote = new wxBoxSizer(wxVERTICAL);
    frameNote = new PopupFrame(
      mediator,
      this,
      ID_FRAME_NOTE,
      wxString(wxT("Add Note")),
      wxPoint(350,300),
      wxSize(
        (int)(0.27*this->GetSize().GetWidth()),
        (int)(0.50*this->GetSize().GetHeight())));

    frameNote->SetSizer(sizerFrameNote);

    // create children

    // create textbox
    noteText = new wxTextCtrl(frameNote, -1, wxT(""), wxPoint(-1, -1), wxSize(150, 100), wxTE_MULTILINE);
    const char* msg = currentShapeNote.c_str();
    wxString textValue;
    textValue = textValue.FromAscii(msg);
    noteText->SetValue(textValue);   // Initialize the text field with the current note of the shape
    sizerFrameNote->Add(noteText, 0, wxEXPAND | wxALL, 5);

    initPanelNote();

    frameNote->Layout();
    frameNote->Fit();

    frameNote->SetMinSize(frameNote->GetSize());
    frameNote->SetMaxSize(frameNote->GetSize());
    toolBarEdit->EnableTool(ID_TOOL_NOTE, true);

    // show
    frameNote->Show();
    frameNote->Raise();
    noteText->SetFocus(); // Set focus to the textbox
  }
}


void Frame::initFrameTextSize()
{
  if (frameTextSize == NULL)
  {
    // init frame
    sizerTextSize = new wxBoxSizer(wxVERTICAL);
    frameTextSize = new PopupFrame(
      mediator,
      this,
      ID_FRAME_TEXT_SIZE,
      wxString(wxT("Change Text Size")),
      wxPoint(350,250),
      wxSize(
        (int)(0.27*this->GetSize().GetWidth()),
        (int)(0.50*this->GetSize().GetHeight())));

    frameTextSize->SetSizer(sizerTextSize);

    // create children


    wxString initials[7];
    for (int i = 0; i < 7; i++)
    {
      initials[i] = wxString::Format(_T("%d"), (8 + 2*i));
    }
    // create textbox
    wxString currentSize = wxString::Format(_T("%lu"), currentTextSize);
    textSizeBox = new wxComboBox(frameTextSize, -1, currentSize, wxPoint(-1, -1), wxSize(40, 40), 7, initials);
    sizerTextSize->Add(textSizeBox, 0, wxEXPAND | wxALL, 5);
    wxButton* okButton = new wxButton(frameTextSize, ID_OK_BUTTON_TEXT_SIZE, wxT("Ok"));
    sizerTextSize->Add(okButton, 0, wxEXPAND | wxALL, 5);

    // handle event for buttons
    Connect(ID_OK_BUTTON_TEXT_SIZE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Frame::onButton));

    frameTextSize->Layout();
    frameTextSize->Fit();

    frameTextSize->SetMinSize(frameTextSize->GetSize());
    frameTextSize->SetMaxSize(frameTextSize->GetSize());

    // show
    frameTextSize->Show();
    frameTextSize->Raise();
    textSizeBox->SetFocus(); // Set focus to the textbox
  }
}


void Frame::initPanelDOF()
{
  // init panel
  sizerDOF = new wxBoxSizer(wxVERTICAL);
  panelDOF = new wxScrolledWindow(
    frameDOF,
    ID_PANEL_DOF,
    wxDefaultPosition,
    wxDefaultSize,
    wxHSCROLL |
    wxVSCROLL |
    wxRAISED_BORDER);
  panelDOF->SetSizer(sizerDOF);
  panelDOF->SetScrollRate(10, 10);
  sizerFrameDOF->Add(
    panelDOF,
    1,
    wxEXPAND);

  // init children
  initListCtrlDOF();
  //initTextOptionsDOF();
  initCanvasColDOF();
  initCanvasOpaDOF();

}


void Frame::initPanelNote()
{
  // init panel
  sizerNote = new wxBoxSizer(wxHORIZONTAL);
  panelNote = new wxScrolledWindow(
    frameNote,
    wxID_ANY,
    wxDefaultPosition,
    wxDefaultSize,
    wxHSCROLL |
    wxVSCROLL |
    wxRAISED_BORDER);
  panelNote->SetSizer(sizerNote);
  panelNote->SetScrollRate(10, 10);
  sizerFrameNote->Add(panelNote, 0, wxEXPAND);

  // create buttons
  wxButton* addButton = new wxButton(panelNote, ID_ADD_BUTTON_NOTE, wxT("Add"));
  wxButton* clearButton = new wxButton(panelNote, ID_CLEAR_BUTTON_NOTE, wxT("Clear"));
  sizerNote->Add(addButton, 0, wxALL, 5);
  sizerNote->Add(clearButton, 0, wxALL, 5);
  sizerNote->Fit(panelNote);

  // handle event for buttons
  Connect(ID_ADD_BUTTON_NOTE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Frame::onButton));
  Connect(ID_CLEAR_BUTTON_NOTE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Frame::onButton));
}


void Frame::initListCtrlDOF()
{
  wxSize* size = new wxSize(180,200);
  // init list
  listCtrlDOF = new wxListCtrl(
    panelDOF,
    ID_LIST_CTRL_DOF,
    wxDefaultPosition,
    *size,
    wxLC_REPORT |
    wxLC_HRULES |
    wxLC_VRULES |
    wxLC_SINGLE_SEL);

  sizerDOF->Add(
    listCtrlDOF,
    0,
    wxEXPAND |
    wxALL,
    5);

  // init as drop target
  listCtrlDOF->SetDropTarget(
    new DropTarget(
      listCtrlDOF,
      mediator));

  // columns
  wxListItem colItem;

  // dummy column
  /*colItem.SetText( wxT( "" ) );
  listCtrlDOF->InsertColumn( 0, colItem );
  listCtrlDOF->SetColumnWidth( 0, 0 );*/
  // column 1
  colItem.SetText(wxT(""));
  colItem.SetAlign(wxLIST_FORMAT_RIGHT);
  listCtrlDOF->InsertColumn(0, colItem);
  listCtrlDOF->SetColumnWidth(0, 30);
  // column 2
  colItem.SetText(wxT("Degree of freedom"));
  colItem.SetAlign(wxLIST_FORMAT_LEFT);
  listCtrlDOF->InsertColumn(1, colItem);
  listCtrlDOF->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
  // column 3
  colItem.SetText(wxT("Associated attribute"));
  colItem.SetAlign(wxLIST_FORMAT_LEFT);
  listCtrlDOF->InsertColumn(2, colItem);
  listCtrlDOF->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);

}


void Frame::initCanvasColDOF()
{
  // label
  wxStaticText* lbl = new wxStaticText(
    panelDOF,
    wxID_ANY,
    wxString(wxT("Color")));
  sizerDOF->Add(
    lbl,
    0,          // vert not stretch
    wxEXPAND |  // hori stretch
    wxLEFT,     // border left
    10);

  // init canvas
  canvasColDOF = new GLCanvas(
    mediator,
    panelDOF,
    ID_CANVAS_COL_DOF);
  canvasColDOF->SetSizeHints(
    150,  // minimum width
    75);  // minimum height

  sizerDOF->Add(
    canvasColDOF,
    1,
    wxEXPAND |
    wxALL,
    5);

  // reset ptr
  lbl = NULL;
}


void Frame::initCanvasOpaDOF()
{
  // label
  wxStaticText* lbl = new wxStaticText(
    panelDOF,
    wxID_ANY,
    wxString(wxT("Opacity")));
  sizerDOF->Add(
    lbl,
    0,          // vert not stretch
    wxEXPAND |  // hori stretch
    wxLEFT,     // border left
    10);

  // init canvas
  canvasOpaDOF = new GLCanvas(
    mediator,
    panelDOF,
    ID_CANVAS_OP_DOF);
  canvasOpaDOF->SetSizeHints(
    150,  // minimum width
    75);  // minimum height

  sizerDOF->Add(
    canvasOpaDOF,
    1,
    wxEXPAND |
    wxALL,
    5);

  // reset ptr
  lbl = NULL;
}


void Frame::initFramePlot()
{
  // init frame
  sizerFramePlot = new wxBoxSizer(wxVERTICAL);
  framePlot = new PopupFrame(
    mediator,
    this,
    ID_FRAME_PLOT,
    wxString(wxT("Attribute plot")),
    wxDefaultPosition,
    wxSize(
      (int)(0.33*this->GetSize().GetWidth()),
      (int)(0.33*this->GetSize().GetHeight())));
  framePlot->SetMinSize(wxSize(200,200));
  framePlot->SetSizer(sizerFramePlot);

  // init children
  initPanelPlot();

  // show
  framePlot->Show();
}


void Frame::initPanelPlot()
{
  // init panel
  sizerPlot = new wxBoxSizer(wxVERTICAL);
  panelPlot = new wxScrolledWindow(
    framePlot,
    ID_PANEL_PLOT,
    wxDefaultPosition,
    wxDefaultSize,
    wxHSCROLL |
    wxVSCROLL |
    wxRAISED_BORDER);
  panelPlot->SetSizer(sizerPlot);
  panelPlot->SetScrollRate(10, 10);
  sizerFramePlot->Add(
    panelPlot,
    1,
    wxEXPAND);

  // init children
  initCanvasPlot();
}


void Frame::initCanvasPlot()
{
  canvasPlot = new GLCanvas(
    mediator,
    panelPlot,
    ID_CANVAS_PLOT);

  sizerPlot->Add(
    canvasPlot,
    1,
    wxEXPAND);
}


void Frame::initFrameClust()
{
  // init frame
  sizerFrameClust = new wxBoxSizer(wxVERTICAL);
  frameClust = new PopupFrame(
    mediator,
    this,
    ID_FRAME_CLUST,
    wxString(wxT("Select attributes")),
    wxDefaultPosition,
    wxSize(
      (int)(0.25*this->GetSize().GetWidth()),
      (int)(0.25*this->GetSize().GetHeight())));
  frameClust->SetSizer(sizerFrameClust);

  // init children
  initPanelClust();

  // show
  frameClust->Center();
  frameClust->Show();
}


void Frame::initPanelClust()
{
  // init panel
  sizerClust = new wxBoxSizer(wxVERTICAL);
  panelClust = new wxScrolledWindow(
    frameClust,
    ID_PANEL_CLUST,
    wxDefaultPosition,
    wxDefaultSize,
    wxHSCROLL |
    wxVSCROLL |
    wxRAISED_BORDER);
  panelClust->SetSizer(sizerClust);
  panelClust->SetScrollRate(10, 10);
  sizerFrameClust->Add(
    panelClust,
    1,
    wxEXPAND);

  // init children
  initListCtrlClust();
  initButtonsClust();
}


void Frame::initListCtrlClust()
{
  // init list
  listCtrlClust = new wxListCtrl(
    panelClust,
    ID_LIST_CTRL_CLUST,
    wxDefaultPosition,
    wxDefaultSize,
    wxLC_REPORT |
    wxLC_HRULES |
    wxLC_VRULES);
  sizerClust->Add(
    listCtrlClust,
    1,
    wxEXPAND |
    wxALL,
    5);

  // columns
  wxListItem colItem;

  // dummy column
  colItem.SetText(wxT(""));
  listCtrlClust->InsertColumn(0, colItem);
  listCtrlClust->SetColumnWidth(0, 0);
  // column 1
  colItem.SetText(wxT(""));
  colItem.SetAlign(wxLIST_FORMAT_RIGHT);
  listCtrlClust->InsertColumn(1, colItem);
  listCtrlClust->SetColumnWidth(1, 30);
  // column 2
  colItem.SetText(wxT("Attribute"));
  colItem.SetAlign(wxLIST_FORMAT_LEFT);
  listCtrlClust->InsertColumn(2, colItem);
  listCtrlClust->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
}


void Frame::initButtonsClust()
{
  wxButton* buttonOKClust = new wxButton(
    panelClust,
    ID_BUTTON_OK_CLUST,
    wxString(wxT("OK")));
  sizerClust->Add(
    buttonOKClust,
    0,
    wxALIGN_RIGHT |
    wxALL,
    5);
}


// -- event handlers --------------------------------------------


void Frame::onMenuBar(wxCommandEvent& e)
{
  closePopupFrames();

  if (e.GetId() == wxID_OPEN)
  {
    // open file dialog
    wxString filePath = wxT("");
    wxString caption  = wxString(wxT("Choose a file"));
    wxString wildcard = wxString(("All supported files (" +
                                  mcrl2::lts::detail::lts_extensions_as_string() +
                                  ")|" +
                                  mcrl2::lts::detail::lts_extensions_as_string(";") +
                                  "|All files (*.*)|*.*").c_str(),
                                 wxConvLocal);

    wxFileDialog* fileDialog = new wxFileDialog(
      this,
      caption,
      wxString(wxT("")),     // default directory
      wxString(wxT("")),     // default filename
      wildcard,  // extensions
      wxFD_OPEN);

    if (fileDialog->ShowModal() == wxID_OK)
    {
      filePath = fileDialog->GetPath();
      mediator->openFile(
        string(filePath.mb_str(wxConvUTF8)));
    }

    delete fileDialog;
    fileDialog = NULL;
  }
  else if (e.GetId() == wxID_SAVE)
  {
    // open file dialog
    wxString filePath = wxT("");
    wxString caption  = wxString(wxT("Save the FSM file"));
    wxString wildcard = wxString(wxT("FSM file (*.fsm)|*.fsm"));

    wxFileDialog* fileDialog = new wxFileDialog(
      this,
      caption,
      wxString(wxT("")),     // default directory
      wxString(wxT("")),     // default filename
      wildcard,  // extensions
      wxFD_SAVE |
      wxFD_OVERWRITE_PROMPT);

    if (fileDialog->ShowModal() == wxID_OK)
    {
      filePath = fileDialog->GetPath();
      string s = string(filePath.mb_str(wxConvUTF8));
      size_t found = s.find(".fsm");
      if (found == string::npos)
      {
        s.append(".fsm");
      }
      mediator->saveFile(s);
    }

    delete fileDialog;
    fileDialog = NULL;
  }
  else if (e.GetId() == wxID_SAVEAS)
  {
    // open file dialog
    wxString filePath = wxT("");
    wxString caption  = wxString(wxT("Save the FSM file with a new name"));
    wxString wildcard = wxString(wxT("FSM file (*.fsm)|*.fsm"));

    wxFileDialog* fileDialog = new wxFileDialog(
      this,
      caption,
      wxString(wxT("")),   // default directory
      wxString(wxT("")),   // default filename
      wildcard,  // extensions
      wxFD_SAVE |
      wxFD_OVERWRITE_PROMPT);

    if (fileDialog->ShowModal() == wxID_OK)
    {
      filePath = fileDialog->GetPath();
      string s = string(filePath.mb_str(wxConvUTF8));
      size_t found = s.find(".fsm");
      if (found == string::npos)
      {
        s.append(".fsm");
      }
      mediator->saveFile(s);
    }

    delete fileDialog;
    fileDialog = NULL;
  }
  else if (e.GetId() == ID_MENU_ITEM_LOAD_CONFIG)
  {
    // open file dialog
    wxString filePath = wxT("");
    wxString caption  = wxString(wxT("Load attribute configuration"));
    wxString wildcard = wxString(wxT("DGD file (*.dgc)|*.dgc"));

    wxFileDialog* fileDialog = new wxFileDialog(
      this,
      caption,
      wxString(wxT("")),     // default directory
      wxString(wxT("")),     // default filename
      wildcard,  // extensions
      wxFD_OPEN);

    if (fileDialog->ShowModal() == wxID_OK)
    {
      filePath = fileDialog->GetPath();
      mediator->handleLoadAttrConfig(
        string(string(filePath.mb_str(wxConvUTF8))));
    }

    delete fileDialog;
    fileDialog = NULL;

  }
  else if (e.GetId() == ID_MENU_ITEM_SAVE_CONFIG)
  {
    // open file dialog
    wxString filePath = wxT("");
    wxString caption  = wxString(wxT("Save attribute configuration"));
    wxString wildcard = wxString(wxT("DGC file (*.dgc)|*.dgc"));

    wxFileDialog* fileDialog = new wxFileDialog(
      this,
      caption,
      wxString(wxT("")),     // default directory
      wxString(wxT("")),     // default filename
      wildcard,  // extensions
      wxFD_SAVE |
      wxFD_OVERWRITE_PROMPT);

    if (fileDialog->ShowModal() == wxID_OK)
    {
      filePath = fileDialog->GetPath();
      string s = string(filePath.mb_str(wxConvUTF8));
      size_t found = s.find(".dgc");
      if (found == string::npos)
      {
        s.append(".dgc");
      }
      mediator->handleSaveAttrConfig(s);
    }

    delete fileDialog;
    fileDialog = NULL;
  }
  else if (e.GetId() == ID_MENU_ITEM_LOAD_DIAGRAM)
  {
    // open file dialog
    wxString filePath = wxT("");
    wxString caption  = wxString(wxT("Load diagram"));
    wxString wildcard = wxString(wxT("DGD file (*.dgd)|*.dgd"));

    wxFileDialog* fileDialog = new wxFileDialog(
      this,
      caption,
      wxString(wxT("")),     // default directory
      wxString(wxT("")),     // default filename
      wildcard,  // extensions
      wxFD_OPEN);

    if (fileDialog->ShowModal() == wxID_OK)
    {
      filePath = fileDialog->GetPath();
      mediator->handleLoadDiagram(
        string(filePath.mb_str(wxConvUTF8)));
    }

    delete fileDialog;
    fileDialog = NULL;
  }
  else if (e.GetId() == ID_MENU_ITEM_SAVE_DIAGRAM)
  {
    // open file dialog
    wxString filePath = wxT("");
    wxString caption  = wxString(wxT("Save diagram"));
    wxString wildcard = wxString(wxT("DGD file (*.dgd)|*.dgd"));

    wxFileDialog* fileDialog = new wxFileDialog(
      this,
      caption,
      wxString(wxT("")),     // default directory
      wxString(wxT("")),     // default filename
      wildcard,  // extensions
      wxFD_SAVE |
      wxFD_OVERWRITE_PROMPT);

    if (fileDialog->ShowModal() == wxID_OK)
    {
      filePath = fileDialog->GetPath();
      string s = string(filePath.mb_str(wxConvUTF8));
      size_t found = s.find(".dgd");
      if (found == string::npos)
      {
        s.append(".dgd");
      }
      mediator->handleSaveDiagram(
        string(s));
    }

    delete fileDialog;
    fileDialog = NULL;
  }
  else if (e.GetId() == ID_MENU_ITEM_MODE_ANALYSIS)
  {
    mediator->handleSetModeAnalysis();

    // hide view menu
    //menuBar->Insert( 2, viewMenu, wxString( wxT( "View" ) ) );
    menuBar->EnableTop(2, true);

    // enable cluster & trace buttons
    if (listCtrlAttr->GetSelectedItemCount() > 0)
    {
      /*
      buttonClustAttr->Enable( true );
      if ( mediator->getView() == Mediator::VIEW_TRACE )
          buttonTraceAttr->Enable( true );
      */
    }

    // hide toolbar
    toolBarEdit->Show(false);

    // update menubar
    if (listCtrlAttr->GetSelectedItemCount() == 0)
    {
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_DISTR_PLOT, false);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_CORRL_PLOT, false);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_COMBN_PLOT, false);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_DUPL, false);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_RENAME, false);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_DELETE, false);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_CLUST, false);
    }
    else if (listCtrlAttr->GetSelectedItemCount() == 1)
    {
      // get selected item
      long item = -1;
      item = listCtrlAttr->GetNextItem(
               item,
               wxLIST_NEXT_ALL,
               wxLIST_STATE_SELECTED);

      attributeMenu->Enable(ID_MENU_ITEM_ATTR_DISTR_PLOT, true);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_CORRL_PLOT, false);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_COMBN_PLOT, true);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_DUPL, true);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_RENAME, true);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_DELETE, true);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_CLUST, true);
    }
    else if (listCtrlAttr->GetSelectedItemCount() == 2)
    {
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_DISTR_PLOT, false);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_CORRL_PLOT, true);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_COMBN_PLOT, true);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_DUPL, false);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_RENAME, false);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_DELETE, false);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_CLUST, true);
    }
    else
    {
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_DISTR_PLOT, false);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_CORRL_PLOT, false);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_COMBN_PLOT, true);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_DUPL, false);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_RENAME, false);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_DELETE, false);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_CLUST, true);
    }

    // show 2nd panel (panelBotRgt)
    int w, h;
    splitterRgt->GetSize(&w, &h);
    splitterRgt->SplitHorizontally(
      panelTopRgt,
      panelBotRgt);
    splitterRgt->SetSashPosition((int)(sashRatioRgt*h));

    // show panelBotLft panel
    splitterLft->GetSize(&w, &h);
    splitterLft->SplitHorizontally(
      splitterTopLft,
      panelBotLft);
    splitterLft->SetSashPosition((int)(sashRatioLft*h));

  }
  else if (e.GetId() == ID_MENU_ITEM_MODE_EDIT)
  {
    mediator->handleSetModeEdit();

    // show view menu
//        menuBar->Remove( 2 );
    menuBar->EnableTop(2, false);



    // disable cluster & trace buttons
    //buttonClustAttr->Enable( false );
    //buttonTraceAttr->Enable( false );

    // show toolbar
    toolBarEdit->Show(true);

    // update menubar
    attributeMenu->Enable(ID_MENU_ITEM_ATTR_DISTR_PLOT, false);
    attributeMenu->Enable(ID_MENU_ITEM_ATTR_CORRL_PLOT, false);
    attributeMenu->Enable(ID_MENU_ITEM_ATTR_COMBN_PLOT, false);
    attributeMenu->Enable(ID_MENU_ITEM_ATTR_DUPL, false);
    attributeMenu->Enable(ID_MENU_ITEM_ATTR_RENAME, false);
    attributeMenu->Enable(ID_MENU_ITEM_ATTR_DELETE, false);
    attributeMenu->Enable(ID_MENU_ITEM_ATTR_CLUST, false);

    // hide 2nd panel
    int w, h;
    splitterRgt->GetSize(&w, &h);
    sashRatioRgt = splitterRgt->GetSashPosition()/(double)h;
    splitterRgt->Unsplit(panelBotRgt);

    // hide panelBotLft panel
    splitterLft->GetSize(&w, &h);
    sashRatioLft = splitterLft->GetSashPosition()/(double)h;
    splitterLft->Unsplit(panelBotLft);

  }
  else if (e.GetId() == ID_MENU_ITEM_VIEW_SIM)
  {
    attributeMenu->Enable(ID_MENU_ITEM_ATTR_TRACE, false);
    //buttonTraceAttr->Enable( false );

    // update visualizers
    mediator->handleSetViewSim();
    canvasTwo->Refresh();
  }
  else if (e.GetId() == ID_MENU_ITEM_VIEW_TRACE)
  {
    // update GUI
    if (listCtrlAttr->GetSelectedItemCount() > 0)
    {
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_TRACE, true);
      //buttonTraceAttr->Enable( true );
    }

    // update visualizers
    mediator->handleSetViewTrace();
    canvasTwo->Refresh();
  }
  else if (e.GetId() == ID_MENU_ITEM_SETTINGS_GENERAL ||
           e.GetId() == ID_MENU_ITEM_SETTINGS_CLUST_TREE ||
           e.GetId() == ID_MENU_ITEM_SETTINGS_BAR_TREE ||
           e.GetId() == ID_MENU_ITEM_SETTINGS_ARC_DIAGRAM ||
           e.GetId() == ID_MENU_ITEM_SETTINGS_SIMULATOR ||
           e.GetId() == ID_MENU_ITEM_SETTINGS_TRACE ||
           e.GetId() == ID_MENU_ITEM_SETTINGS_EDITOR)
  {
    settingsDialog->show();
  }
  else if (e.GetId() == wxID_CLOSE)
  {
    // shut down app
    Close();
  }

  // Following lines cause stange resize behaviour when opening a file in Windows
#ifndef WIN32
  // fit everything after handling the request
  Fit();
  Layout();
#endif
}


void Frame::onListCtrlSelect(wxListEvent& e)
{
  if (e.GetId() == ID_LIST_CTRL_ATTR)
  {
    if (mediator->getMode() == Mediator::MODE_EDIT)
    {
      // get idx of last selected item
      int attrIdx = e.GetIndex();
      // handle selection
      mediator->handleAttributeSel(attrIdx);

      // deselect all other items
      long item = -1;
      item = listCtrlAttr->GetNextItem(
               item,
               wxLIST_NEXT_ALL,
               wxLIST_STATE_SELECTED);
      while (item != -1)
      {
        if ((int) listCtrlAttr->GetItemData(item) != e.GetIndex())
        {
          listCtrlAttr->SetItemState(
            item,
            0,
            wxLIST_STATE_SELECTED |
            wxLIST_STATE_FOCUSED);
        }

        item = listCtrlAttr->GetNextItem(
                 item,
                 wxLIST_NEXT_ALL,
                 wxLIST_STATE_SELECTED);
      }

      // update menubar
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_DISTR_PLOT, false);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_CORRL_PLOT, false);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_COMBN_PLOT, false);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_DUPL, false);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_RENAME, false);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_DELETE, false);
      attributeMenu->Enable(ID_MENU_ITEM_ATTR_CLUST, false);
    }
    else if (mediator->getMode() == Mediator::MODE_ANALYSIS)
    {
      /*
      buttonClustAttr->Enable( true );
      if ( mediator->getView() == Mediator::VIEW_TRACE )
          buttonTraceAttr->Enable( true );
      */

      if (listCtrlAttr->GetSelectedItemCount() == 0)
      {
        clearDomainInfo();

        // update menubar
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_DISTR_PLOT, false);
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_CORRL_PLOT, false);
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_COMBN_PLOT, false);
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_DUPL, false);
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_RENAME, false);
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_DELETE, false);
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_CLUST, false);
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_TRACE, false);
      }
      else if (listCtrlAttr->GetSelectedItemCount() == 1)
      {
        // get idx of last selected item
        int attrIdx = e.GetIndex();
        // handle selection
        mediator->handleAttributeSel(attrIdx);

        // update menubar
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_DISTR_PLOT, true);
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_CORRL_PLOT, false);
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_COMBN_PLOT, true);
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_DUPL, true);
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_RENAME, true);
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_DELETE, true);
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_CLUST, true);
        if (mediator->getView() == Mediator::VIEW_TRACE)
        {
          attributeMenu->Enable(ID_MENU_ITEM_ATTR_TRACE, true);
        }
        else
        {
          attributeMenu->Enable(ID_MENU_ITEM_ATTR_TRACE, false);
        }
      }
      else if (listCtrlAttr->GetSelectedItemCount() == 2)
      {
        clearDomainInfo();

        // update menubar
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_DISTR_PLOT, false);
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_CORRL_PLOT, true);
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_COMBN_PLOT, true);
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_DUPL, false);
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_RENAME, false);
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_DELETE, false);
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_CLUST, true);
        if (mediator->getView() == Mediator::VIEW_TRACE)
        {
          attributeMenu->Enable(ID_MENU_ITEM_ATTR_TRACE, true);
        }
        else
        {
          attributeMenu->Enable(ID_MENU_ITEM_ATTR_TRACE, false);
        }
      }
      else
      {
        clearDomainInfo();

        // update menubar
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_DISTR_PLOT, false);
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_CORRL_PLOT, false);
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_COMBN_PLOT, true);
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_DUPL, false);
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_RENAME, false);
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_DELETE, false);
        attributeMenu->Enable(ID_MENU_ITEM_ATTR_CLUST, true);
        if (mediator->getView() == Mediator::VIEW_TRACE)
        {
          attributeMenu->Enable(ID_MENU_ITEM_ATTR_TRACE, true);
        }
        else
        {
          attributeMenu->Enable(ID_MENU_ITEM_ATTR_TRACE, false);
        }
      }
    }
  }
  else if (e.GetId() == ID_LIST_CTRL_DOMAIN)
  {
    if (listCtrlDomain->GetSelectedItemCount() == 1)
    {
      domainMenu->Enable(ID_MENU_ITEM_DOM_GROUP, false);
      domainMenu->Enable(ID_MENU_ITEM_DOM_UNGROUP, true);
      domainMenu->Enable(ID_MENU_ITEM_DOM_RENAME, true);
    }
    else
    {
      domainMenu->Enable(ID_MENU_ITEM_DOM_GROUP, true);
      domainMenu->Enable(ID_MENU_ITEM_DOM_UNGROUP, false);
      domainMenu->Enable(ID_MENU_ITEM_DOM_RENAME, false);
    }
  }
  else if (e.GetId() == ID_LIST_CTRL_DOF)
  {
    // get idx of selected item
    int id = e.GetIndex();
    // handle selection
    mediator->handleDOFSel(id);
    /*
    int textStatus = mediator->handleGetDOFTextStatus( id );
    if ( textStatus == DOF::ID_TEXT_NONE )
        radioBoxTextDOF->SetSelection( 0 );
    else if ( textStatus == DOF::ID_TEXT_ALL )
        radioBoxTextDOF->SetSelection( 1 );
    else if ( textStatus == DOF::ID_TEXT_ATTR )
        radioBoxTextDOF->SetSelection( 2 );
    else if ( textStatus == DOF::ID_TEXT_VAL )
        radioBoxTextDOF->SetSelection( 3 );
    */
  }
  else if (e.GetId() == ID_LIST_CTRL_CLUST)
  {
    if (mediator->getClustMode() == Mediator::CLUST_DISTR_PLOT)
    {
      // deselect all other items
      long item = -1;
      item = listCtrlClust->GetNextItem(
               item,
               wxLIST_NEXT_ALL,
               wxLIST_STATE_SELECTED);
      while (item != -1)
      {
        if ((int) listCtrlClust->GetItemData(item) != e.GetIndex())
        {
          listCtrlClust->SetItemState(
            item,
            0,
            wxLIST_STATE_SELECTED |
            wxLIST_STATE_FOCUSED);
        }

        item = listCtrlClust->GetNextItem(
                 item,
                 wxLIST_NEXT_ALL,
                 wxLIST_STATE_SELECTED);
      }
    }
    else if (mediator->getClustMode() == Mediator::CLUST_CORRL_PLOT)
    {
      // get idx of last selected item
      size_t attrIdx2 = NON_EXISTING;

      // deselect all but second last item & get its idx
      int  selCount = listCtrlClust->GetSelectedItemCount();
      long item = -1;
      item = listCtrlClust->GetNextItem(
               item,
               wxLIST_NEXT_ALL,
               wxLIST_STATE_SELECTED);
      while (item != -1)
      {
        if ((int) listCtrlClust->GetItemData(item) != e.GetIndex())
        {
          if (selCount > 2)
          {
            listCtrlClust->SetItemState(
              item,
              0,
              wxLIST_STATE_SELECTED |
              wxLIST_STATE_FOCUSED);
            --selCount;
          }
          else if (selCount == 2)
          {
            attrIdx2 = listCtrlClust->GetItemData(item);
          }
        }

        item = listCtrlClust->GetNextItem(
                 item,
                 wxLIST_NEXT_ALL,
                 wxLIST_STATE_SELECTED);
      }
    }
  }
}


void Frame::onListCtrlBeginDrag(wxListEvent& e)
{
  if (e.GetId() == ID_LIST_CTRL_ATTR)
  {
    int    srcId  = -1;
    int    attrIdx = -1;
    string msg    = "";

    // prepare data
    srcId  = ID_LIST_CTRL_ATTR;
    attrIdx = e.GetIndex();

    // init data
    wxTextDataObject data(wxString::Format(wxT("%d %d"), srcId, attrIdx));

    // drag start
    wxDropSource source(listCtrlAttr);
    source.SetData(data);
    source.DoDragDrop(wxDrag_DefaultMove);
  }
  else if (e.GetId() == ID_LIST_CTRL_DOMAIN)
  {
    int    srcId   = -1;
    int    attrIdx = -1;
    int    domIdx  = -1;
    string msg     = "";

    // prepare data
    srcId  = ID_LIST_CTRL_DOMAIN;
    attrIdx = listCtrlAttr->GetNextItem(
                attrIdx,
                wxLIST_NEXT_ALL,
                wxLIST_STATE_SELECTED);;
    domIdx = e.GetIndex();

    msg.append(Utils::intToStr(srcId));
    msg.append(" ");
    msg.append(Utils::intToStr(attrIdx));
    msg.append(" ");
    msg.append(Utils::intToStr(domIdx));

    // init data
    wxTextDataObject data(wxString(msg.c_str(), wxConvUTF8));

    // drag start
    wxDropSource source(listCtrlDomain);
    source.SetData(data);
    source.DoDragDrop(wxDrag_DefaultMove);
  }
}


void Frame::onListCtrlRgtClick(wxListEvent& e)
{
  if (e.GetId() == ID_LIST_CTRL_ATTR)
  {
    if (mediator->getMode() == Mediator::MODE_ANALYSIS)
    {
      wxMenu menu;

      // group & ungroup
      menu.Append(
        ID_MENU_ITEM_ATTR_CLUST,
        wxT("Cluster nodes"),
        wxT("Cluster nodes based on selected attributes"));
      menu.Append(
        ID_MENU_ITEM_ATTR_TRACE,
        wxT("View trace"),
        wxT("View trace based on selected attributes"));
      menu.AppendSeparator();
      menu.Append(
        ID_MENU_ITEM_ATTR_DISTR_PLOT,
        wxT("Distribution plot"),
        wxT("Visualize the distribution of values"));
      menu.Append(
        ID_MENU_ITEM_ATTR_CORRL_PLOT,
        wxT("Correlation plot"),
        wxT("Visualize the correlation of values"));
      menu.Append(
        ID_MENU_ITEM_ATTR_COMBN_PLOT,
        wxT("Combination plot"),
        wxT("Visualize the combinations of values"));
      menu.AppendSeparator();
      menu.Append(
        ID_MENU_ITEM_ATTR_DUPL,
        wxT("Duplicate"),
        wxT("Duplicate attribute"));
      menu.Append(
        ID_MENU_ITEM_ATTR_RENAME,
        wxT("Rename"),
        wxT("Rename attribute"));
      menu.Append(
        ID_MENU_ITEM_ATTR_DELETE,
        wxT("Delete"),
        wxT("Delete attribute"));

      int selCnt = listCtrlAttr->GetSelectedItemCount();
      if (selCnt == 0)
      {
        menu.Enable(ID_MENU_ITEM_ATTR_DISTR_PLOT, false);
        menu.Enable(ID_MENU_ITEM_ATTR_CORRL_PLOT, false);
        menu.Enable(ID_MENU_ITEM_ATTR_COMBN_PLOT, false);
        menu.Enable(ID_MENU_ITEM_ATTR_DUPL, false);
        menu.Enable(ID_MENU_ITEM_ATTR_RENAME, false);
        menu.Enable(ID_MENU_ITEM_ATTR_DELETE, false);
        menu.Enable(ID_MENU_ITEM_ATTR_CLUST, false);
        menu.Enable(ID_MENU_ITEM_ATTR_TRACE, false);
      }
      else if (selCnt == 1)
      {
        // get selected item
        long item = -1;
        item = listCtrlAttr->GetNextItem(
                 item,
                 wxLIST_NEXT_ALL,
                 wxLIST_STATE_SELECTED);

        menu.Enable(ID_MENU_ITEM_ATTR_DISTR_PLOT, true);
        menu.Enable(ID_MENU_ITEM_ATTR_CORRL_PLOT, false);
        menu.Enable(ID_MENU_ITEM_ATTR_COMBN_PLOT, true);
        menu.Enable(ID_MENU_ITEM_ATTR_DUPL, true);
        menu.Enable(ID_MENU_ITEM_ATTR_RENAME, true);
        menu.Enable(ID_MENU_ITEM_ATTR_DELETE, true);
        menu.Enable(ID_MENU_ITEM_ATTR_CLUST, true);
        if (mediator->getView() == Mediator::VIEW_TRACE)
        {
          menu.Enable(ID_MENU_ITEM_ATTR_TRACE, true);
        }
        else
        {
          menu.Enable(ID_MENU_ITEM_ATTR_TRACE, false);
        }
      }
      else if (selCnt == 2)
      {
        menu.Enable(ID_MENU_ITEM_ATTR_DISTR_PLOT, false);
        menu.Enable(ID_MENU_ITEM_ATTR_CORRL_PLOT, true);
        menu.Enable(ID_MENU_ITEM_ATTR_COMBN_PLOT, true);
        menu.Enable(ID_MENU_ITEM_ATTR_DUPL, false);
        menu.Enable(ID_MENU_ITEM_ATTR_RENAME, false);
        menu.Enable(ID_MENU_ITEM_ATTR_DELETE, false);
        menu.Enable(ID_MENU_ITEM_ATTR_CLUST, true);
        if (mediator->getView() == Mediator::VIEW_TRACE)
        {
          menu.Enable(ID_MENU_ITEM_ATTR_TRACE, true);
        }
        else
        {
          menu.Enable(ID_MENU_ITEM_ATTR_TRACE, false);
        }
      }
      else
      {
        menu.Enable(ID_MENU_ITEM_ATTR_DISTR_PLOT, false);
        menu.Enable(ID_MENU_ITEM_ATTR_CORRL_PLOT, false);
        menu.Enable(ID_MENU_ITEM_ATTR_COMBN_PLOT, true);
        menu.Enable(ID_MENU_ITEM_ATTR_DUPL, false);
        menu.Enable(ID_MENU_ITEM_ATTR_RENAME, false);
        menu.Enable(ID_MENU_ITEM_ATTR_DELETE, false);
        menu.Enable(ID_MENU_ITEM_ATTR_CLUST, true);
        if (mediator->getView() == Mediator::VIEW_TRACE)
        {
          menu.Enable(ID_MENU_ITEM_ATTR_TRACE, true);
        }
        else
        {
          menu.Enable(ID_MENU_ITEM_ATTR_TRACE, false);
        }
      }

      PopupMenu(&menu);

    }
  }
  else if (e.GetId() == ID_LIST_CTRL_DOMAIN)
  {
    wxMenu menu;

    // group & ungroup
    menu.Append(
      ID_MENU_ITEM_DOM_GROUP,
      wxT("Group"),
      wxT("Group selected domain values"));
    menu.Append(
      ID_MENU_ITEM_DOM_UNGROUP,
      wxT("Ungroup"),
      wxT("Ungroup domain values"));
    // rename
    menu.Append(
      ID_MENU_ITEM_DOM_RENAME,
      wxT("Rename"),
      wxT("Rename selected domain value"));

    int selCnt = listCtrlDomain->GetSelectedItemCount();
    if (selCnt == 0)
    {
      menu.Enable(ID_MENU_ITEM_DOM_GROUP, false);
      menu.Enable(ID_MENU_ITEM_DOM_RENAME, false);
    }
    else if (selCnt == 1)
    {
      menu.Enable(ID_MENU_ITEM_DOM_GROUP, false);
    }
    else
    {
      menu.Enable(ID_MENU_ITEM_DOM_RENAME, false);
    }

    PopupMenu(&menu);

  }
  else if (e.GetId() == ID_LIST_CTRL_DOF)
  {
    int  flag   = wxLIST_HITTEST_ONITEM;
    int  idxDOF = -1;
    idxDOF = listCtrlDOF->HitTest(e.GetPoint(), flag);
    dofMenu = true;

    if (0 <= idxDOF && idxDOF < listCtrlDOF->GetItemCount())
    {
      wxMenu menu;

      menu.Append(
        ID_MENU_ITEM_DOF_UNLINK,
        wxT("Remove attribute"),
        wxT("Remove attribute"));

      addAttributeMenu = new wxMenu();
      int i;
      int id = wxID_LOWEST; // Event id's for Attributes

      // List All the Attributes in a Menu
      for (i = 0; i < listCtrlAttr->GetItemCount(); i++)
      {
        wxListItem rowInfo;
        wxString   celInfo;

        // set row
        rowInfo.m_itemId = i;
        // set column
        rowInfo.m_col    = 2;
        // set text mask
        rowInfo.m_mask   = wxLIST_MASK_TEXT;
        listCtrlAttr->GetItem(rowInfo);
        celInfo = rowInfo.m_text;
        wxString helpString;
        helpString << i; // Convert int to wxString
        wxMenuItem* item = new wxMenuItem(addAttributeMenu, id, celInfo, helpString, wxITEM_NORMAL);
        addAttributeMenu->Append(
          item);
        Connect(id, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Frame::onPopupMenu));
        id--;
      }

      menu.Append(ID_MENU_ITEM_DOF_ATTRIBUTE_LIST,
                  wxT("Select attribute"),
                  addAttributeMenu,
                  wxT("Select attribute"));

      // determine of an attribute has been linked
      // code thanks to 'www.wxwidgets.org/wiki'
      wxListItem rowInfo;
      wxString   celInfo;

      // set row
      rowInfo.m_itemId = idxDOF;
      // set column
      rowInfo.m_col    = 2;
      // set text mask
      rowInfo.m_mask   = wxLIST_MASK_TEXT;

      listCtrlDOF->GetItem(rowInfo);
      celInfo = rowInfo.m_text;

      // if no attribute, no option
      if (celInfo == wxT(""))
      {
        menu.Enable(ID_MENU_ITEM_DOF_UNLINK, false);
      }

      PopupMenu(&menu);

    }
  }
}


void Frame::onPopupMenu(wxCommandEvent& e)
{
  if (e.GetId() == ID_MENU_ITEM_ATTR_DISTR_PLOT)
  {
    // make sure frame plot exists
    if (framePlot != NULL)
    {
      framePlot->Close();
    }
    initFramePlot();

    // get idx of last selected item
    size_t attrIdx = NON_EXISTING;
    long item   = -1;
    item = listCtrlAttr->GetNextItem(item,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);
    attrIdx = listCtrlAttr->GetItemData(item);
    // handle selection
    mediator->handleAttributePlot(attrIdx);
  }
  else if (e.GetId() == ID_MENU_ITEM_ATTR_CORRL_PLOT)
  {
    // make sure frame plot exists
    if (framePlot != NULL)
    {
      framePlot->Close();
    }
    initFramePlot();

    // get idx of 2 selected items
    size_t attrIdx1 = NON_EXISTING;
    size_t attrIdx2 = NON_EXISTING;

    long item = -1;
    item = listCtrlAttr->GetNextItem(item,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);
    attrIdx1 = listCtrlAttr->GetItemData(item);

    item = listCtrlAttr->GetNextItem(item,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);
    attrIdx2 = listCtrlAttr->GetItemData(item);

    // handle selections
    mediator->handleAttributePlot(attrIdx1, attrIdx2);
  }
  else if (e.GetId() == ID_MENU_ITEM_ATTR_COMBN_PLOT)
  {
    // make sure frame plot exists
    if (framePlot != NULL)
    {
      framePlot->Close();
    }
    initFramePlot();

    // get idcs of selected (multiple) items
    vector< size_t > indcs;
    long item = -1;
    item = listCtrlAttr->GetNextItem(item,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);
    while (item != -1)
    {
      indcs.push_back(listCtrlAttr->GetItemData(item));

      item = listCtrlAttr->GetNextItem(
               item,
               wxLIST_NEXT_ALL,
               wxLIST_STATE_SELECTED);
    }

    // handle selections
    mediator->handleAttributePlot(indcs);
  }
  else if (e.GetId() == ID_MENU_ITEM_ATTR_DUPL)
  {
    // get idcs of selected (multiple) items
    vector< size_t > indcs;
    long item = -1;
    item = listCtrlAttr->GetNextItem(item,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);
    while (item != -1)
    {
      indcs.push_back(listCtrlAttr->GetItemData(item));

      item = listCtrlAttr->GetNextItem(
               item,
               wxLIST_NEXT_ALL,
               wxLIST_STATE_SELECTED);
    }

    // handle selections
    mediator->handleAttributeDuplicate(indcs);
  }
  else if (e.GetId() == ID_MENU_ITEM_ATTR_RENAME)
  {
    long   item    = -1;
    int    attrIdx = -1;
    string name    = "";

    // get attribute index
    item = listCtrlAttr->GetNextItem(
             item,
             wxLIST_NEXT_ALL,
             wxLIST_STATE_SELECTED);
    attrIdx = item;

    if (attrIdx >= 0)
    {
      // get new value
      wxTextEntryDialog* dlg = new wxTextEntryDialog(
        this,
        wxT("Enter new name"),
        wxT("Rename attribute"));
      while (dlg->ShowModal() == wxID_OK && name.size() <= 0)
      {
        name = string(dlg->GetValue().mb_str(wxConvUTF8));

        if (name.size() > 0)
        {
          mediator->handleAttributeRename(
            attrIdx,
            name);
          break;
        }
      }

      delete dlg;
      dlg = NULL;
    }
  }
  else if (e.GetId() == ID_MENU_ITEM_ATTR_DELETE)
  {
    // get idcs of selected (multiple) items
    vector< size_t > indcs;
    long item = -1;
    item = listCtrlAttr->GetNextItem(item,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);
    while (item != -1)
    {
      indcs.push_back(listCtrlAttr->GetItemData(item));

      item = listCtrlAttr->GetNextItem(
               item,
               wxLIST_NEXT_ALL,
               wxLIST_STATE_SELECTED);
    }

    // confirm action
    vector< wxString > names;
    mediator->getAttributeNames(indcs, names);

    wxString msg(wxT("Are you sure you want to delete \'"));
    for (size_t i = 0; i < names.size(); ++i)
    {
      msg.Append(names[i]);
      if (i < names.size()-1)
      {
        msg.Append(wxT(", "));
      }
    }
    msg.Append(wxString(wxT("\' ?")));
    wxMessageDialog dialog(
      this,
      msg,
      wxString(wxT("Confirm attribute delete")),
      wxOK | wxCANCEL);

    // delete attribute
    if (dialog.ShowModal() == wxID_OK)
    {
      mediator->handleAttributeDelete(indcs[0]);
    }

    dialog.Destroy();
  }
  else if (e.GetId() == ID_MENU_ITEM_ATTR_CLUST)
  {
    // get idcs of selected (multiple) items
    vector< size_t > indcs;
    long item = -1;
    item = listCtrlAttr->GetNextItem(item,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);
    while (item != -1)
    {
      indcs.push_back(listCtrlAttr->GetItemData(item));

      item = listCtrlAttr->GetNextItem(
               item,
               wxLIST_NEXT_ALL,
               wxLIST_STATE_SELECTED);
    }

    // handle selections
    mediator->handleAttributeCluster(indcs);
  }
  else if (e.GetId() == ID_MENU_ITEM_ATTR_TRACE)
  {
    // get idcs of selected (multiple) items
    vector< size_t > indcs;
    long item = -1;
    item = listCtrlAttr->GetNextItem(item,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);
    while (item != -1)
    {
      indcs.push_back(listCtrlAttr->GetItemData(item));

      item = listCtrlAttr->GetNextItem(
               item,
               wxLIST_NEXT_ALL,
               wxLIST_STATE_SELECTED);
    }

    // handle selections
    mediator->initTimeSeries(indcs);
  }
  else if (e.GetId() == ID_MENU_ITEM_DOM_GROUP)
  {
    long   item    = -1;
    int    attrIdx = -1;
    string name    = "";
    vector< int > domIdcs;

    // get attribute index
    item = listCtrlAttr->GetNextItem(
             item,
             wxLIST_NEXT_ALL,
             wxLIST_STATE_SELECTED);
    attrIdx = item;

    if (attrIdx >= 0)
    {
      // get indices of selected values
      item = -1;
      item = listCtrlDomain->GetNextItem(
               item,
               wxLIST_NEXT_ALL,
               wxLIST_STATE_SELECTED);
      while (item != -1)
      {
        domIdcs.push_back(item);
        item = listCtrlDomain->GetNextItem(
                 item,
                 wxLIST_NEXT_ALL,
                 wxLIST_STATE_SELECTED);
      }

      // get new value
      wxTextEntryDialog* dlg = new wxTextEntryDialog(
        this,
        wxT("Enter group name"),
        wxT("Group domain values"));
      while (dlg->ShowModal() == wxID_OK && name.size() <= 0)
      {
        name = string(dlg->GetValue().mb_str(wxConvUTF8));

        if (name.size() > 0)
        {
          mediator->handleDomainGroup(
            attrIdx,
            domIdcs,
            name);
          break;
        }
      }

      delete dlg;
      dlg = NULL;
    }
  }
  else if (e.GetId() == ID_MENU_ITEM_DOM_UNGROUP)
  {
    long item    = -1;
    int  attrIdx = -1;

    // get attribute index
    item = listCtrlAttr->GetNextItem(
             item,
             wxLIST_NEXT_ALL,
             wxLIST_STATE_SELECTED);
    attrIdx = item;

    if (attrIdx >= 0)
    {
      mediator->handleDomainUngroup(attrIdx);
    }
  }
  else if (e.GetId() == ID_MENU_ITEM_DOM_RENAME)
  {
    long   item    = -1;
    int    attrIdx = -1;
    string name    = "";
    vector< int > domIdcs;

    // get attribute index
    item = listCtrlAttr->GetNextItem(
             item,
             wxLIST_NEXT_ALL,
             wxLIST_STATE_SELECTED);
    attrIdx = item;

    // get domain index
    item = -1;
    item = listCtrlDomain->GetNextItem(
             item,
             wxLIST_NEXT_ALL,
             wxLIST_STATE_SELECTED);
    domIdcs.push_back(item);

    if (attrIdx >= 0)
    {
      // get new value
      wxTextEntryDialog* dlg = new wxTextEntryDialog(
        this,
        wxT("Enter new name"),
        wxT("Rename domain value"));
      while (dlg->ShowModal() == wxID_OK && name.size() <= 0)
      {
        name = string(dlg->GetValue().mb_str(wxConvUTF8));

        if (name.size() > 0)
        {
          mediator->handleDomainGroup(
            attrIdx,
            domIdcs,
            name);
          break;
        }
      }

      delete dlg;
      dlg = NULL;
    }
  }
  else if (e.GetId() == ID_MENU_ITEM_DOF_UNLINK)
  {
    long item   = -1;
    size_t  idDOF  = NON_EXISTING;

    item = listCtrlDOF->GetNextItem(
             item,
             wxLIST_NEXT_ALL,
             wxLIST_STATE_SELECTED);

    if (item >= 0)
    {
      idDOF = listCtrlDOF->GetItemData(item);
      mediator->handleUnlinkDOFAttr(idDOF);
      mediator->handleCheckedVariable(idDOF, -1);   // Uncheck the variable in the right menu which is displayed when right clicked on a shape
    }
  }
  else if (e.GetId() == ID_MENU_ITEM_CLUST_DISTR_PLOT)
  {
    mediator->setClustMode(Mediator::CLUST_DISTR_PLOT);
    // make sure frame exists
    if (frameClust == NULL)
    {
      initFrameClust();
    }
    mediator->handleClustFrameDisplay();
  }
  else if (e.GetId() == ID_MENU_ITEM_CLUST_CORRL_PLOT)
  {
    mediator->setClustMode(Mediator::CLUST_CORRL_PLOT);
    // make sure frame exists
    if (frameClust == NULL)
    {
      initFrameClust();
    }
    mediator->handleClustFrameDisplay();
  }
  else if (e.GetId() == ID_MENU_ITEM_CLUST_COMBN_PLOT)
  {
    mediator->setClustMode(Mediator::CLUST_COMBN_PLOT);
    // make sure frame exists
    if (frameClust == NULL)
    {
      initFrameClust();
    }
    mediator->handleClustFrameDisplay();
  }
  else if (e.GetId() == ID_MENU_ITEM_CLUST_SUBCLUST)
  {
    *this << "subcluster.\n";

    // make sure frame exists
    if (frameClust == NULL)
    {
      initFrameClust();
    }
    mediator->handleClustFrameDisplay();
  }
  else if (e.GetId() == ID_MENU_ITEM_CLUST_UNCLUST)
  {
    *this << "uncluster.\n";

    // make sure frame exists
    if (frameClust == NULL)
    {
      initFrameClust();
    }
    mediator->handleClustFrameDisplay();
  }
  else if (e.GetId() == ID_MENU_ITEM_SHAPE_CUT)
  {
    mediator->handleCutShape();
  }
  else if (e.GetId() == ID_MENU_ITEM_SHAPE_COPY)
  {
    mediator->handleCopyShape();
  }
  else if (e.GetId() == ID_MENU_ITEM_SHAPE_PASTE)
  {
    mediator->handlePasteShape();
  }
  else if (e.GetId() == ID_MENU_ITEM_SHAPE_DELETE)
  {
    mediator->handleDeleteShape();
  }
  else if (e.GetId() == ID_MENU_ITEM_SHAPE_BRING_TO_FRONT)
  {
    mediator->handleBringToFrontShape();
  }
  else if (e.GetId() == ID_MENU_ITEM_SHAPE_SEND_TO_BACK)
  {
    mediator->handleSendToBackShape();
  }
  else if (e.GetId() == ID_MENU_ITEM_SHAPE_BRING_FORWARD)
  {
    mediator->handleBringForwardShape();
  }
  else if (e.GetId() == ID_MENU_ITEM_SHAPE_SEND_BACKWARD)
  {
    mediator->handleSendBackwardShape();
  }
  else if (e.GetId() == ID_MENU_ITEM_SHAPE_EDIT_DOF)
  {
    mediator->handleEditDOFShape();
  }
  else if (e.GetId() == ID_MENU_ITEM_DGRM_SGL_TO_SIML)
  {
    mediator->handleSendDgrmSglToSiml();
  }
  else if (e.GetId() == ID_MENU_ITEM_DGRM_SGL_TO_TRACE)
  {
    mediator->handleSendDgrmSglToTrace();
  }
  else if (e.GetId() == ID_MENU_ITEM_DGRM_SET_TO_TRACE)
  {
    mediator->handleSendDgrmSetToTrace();
  }
  else if (e.GetId() == ID_MENU_ITEM_DGRM_SGL_TO_EXNR)
  {
    mediator->handleSendDgrmSglToExnr();
  }
  else if (e.GetId() == ID_MENU_ITEM_DGRM_SET_TO_EXNR)
  {
    mediator->handleSendDgrmSetToExnr();
  }
  else if (e.GetId() == ID_MENU_ITEM_EXNR_CLEAR)
  {
    mediator->handleClearExnrCur(this);
  }
  else if (e.GetId() == ID_MENU_ITEM_SHOW_NOTE)
  {
    size_t shapeId;
    string msg;
    mediator->handleAddText(msg, shapeId);
    if (shapeId != NON_EXISTING)
    {
      handleNote(shapeId, msg);
    }
  }
  else if (e.GetId() == ID_MENU_ITEM_TEXT_SIZE)
  {
    size_t shapeId;
    size_t textSize;
    mediator->handleTextSize(textSize, shapeId);
    if (shapeId != NON_EXISTING)
    {
      handleTextSize(shapeId, textSize);
    }
  }
  else
  {
    long item   = -1;
    size_t  idDOF  = NON_EXISTING;
    int idxAttr = -1;

    if (addAttributeMenu != NULL)
    {
      wxMenuItem* x = addAttributeMenu->FindItem(e.GetId());
      if (x != NULL)
      {
        wxString name = x->GetHelp(); // Get the index of the selected attribute
        name.ToLong((long*)&idxAttr);  // Converting String to int

        name = x->GetLabelFromText(x->GetText());
        int checkedItemId = -1;

        if (dofMenu == true)
        {
          item = listCtrlDOF->GetNextItem(
                   item,
                   wxLIST_NEXT_ALL,
                   wxLIST_STATE_SELECTED);
          if (item >= 0)
          {
            checkedItemId = e.GetId();
            idDOF = listCtrlDOF->GetItemData(item);
            item = listCtrlAttr->FindItem(-1, name);
            mediator->handleLinkDOFAttr(idDOF, idxAttr);
            mediator->handleCheckedVariable(idDOF, checkedItemId);
          }
        }
        else
        {
          string name = "";
          if (!x->IsChecked())
          {
            x->Check(false);
            mediator->handleSetDOF(NON_EXISTING);   //UnLink the variable from DOFText, -1 indicates Unlink Operation
          }
          else
          {
            checkedItemId = e.GetId();
            Graph* g = (Graph*)(mediator->getGraph());  // Obtain graph from the mediator for retrieving attribute properties
            Attribute* attribute = g->getAttribute(idxAttr); // Get selected attribute
            name = attribute->getName(); // Get the name of the selected attribute
            if (attribute->getSizeCurValues() != 0)
            {
              string value = attribute->getCurValue(0)->getValue(); // Get current value of the selected attribute
              name.append(": ");
              name.append(value); // Generate the text will be displayed on the shape
            }
            mediator->handleSetDOF(idxAttr);   //Link the variable to the DOFText
          }
          mediator->handleShowVariable(name, checkedItemId);
        }
      }
      x = NULL;
    }
    addAttributeMenu = NULL;
  }
}


void Frame::onClusterMenu(wxCommandEvent& e)
{
  if (clustMenu != NULL)
  {
    size_t i;
    size_t itemCount = clustMenu->GetMenuItemCount();
    if (e.GetId() == ID_MENU_ITEM_CLUSTER)
    {
      // get idcs of selected (multiple) items
      vector< size_t > indcs;
      for (i = 0; i < itemCount; i++)
      {
        wxMenuItem* item = clustMenu->FindItemByPosition(i);
        if (item->IsChecked())
        {
          indcs.push_back(i);
        }
      }
      // handle selections
      mediator->handleAttributeCluster(indcs);
      clustMenu = NULL;
    }
    else
    {
      if (!e.IsChecked())
      {
        bool checked = false;
        for (i = 0; i < itemCount && checked == false; i++)
        {
          wxMenuItem* item = clustMenu->FindItemByPosition(i);
          if (item->IsChecked())
          {
            checked = true;
          }
        }
        if (!checked)
        {
          clustMenu->Enable(ID_MENU_ITEM_CLUSTER, false);
        }
      }
      else
      {
        clustMenu->Enable(ID_MENU_ITEM_CLUSTER, true);
      }
    }
  }
}


void Frame::onTool(wxCommandEvent& e)
{
  if (e.GetId() != ID_TOOL_DOF &&
      e.GetId() != ID_TOOL_FILL_COL &&
      e.GetId() != ID_TOOL_LINE_COL)
  {
    if (frameDOF != NULL)
    {
      frameDOF->Close();
      frameDOF = NULL;
    }
  }

  if (e.GetId() == ID_TOOL_SELECT)
  {
    mediator->handleEditModeSelect();
  }
  else if (e.GetId() == ID_TOOL_NOTE)
  {
    mediator->handleEditModeNote();
  }
  else if (e.GetId() == ID_TOOL_DOF)
  {
    mediator->handleEditModeDOF(this);
  }
  else if (e.GetId() == ID_TOOL_RECT)
  {
    mediator->handleEditModeRect();
  }
  else if (e.GetId() == ID_TOOL_ELLIPSE)
  {
    mediator->handleEditModeEllipse();
  }
  else if (e.GetId() == ID_TOOL_LINE)
  {
    mediator->handleEditModeLine();
  }
  else if (e.GetId() == ID_TOOL_ARROW)
  {
    mediator->handleEditModeArrow();
  }
  else if (e.GetId() == ID_TOOL_DARROW)
  {
    mediator->handleEditModeDArrow();
  }
  else if (e.GetId() == ID_TOOL_FILL_COL)
  {
    mediator->handleEditModeFillCol();
  }
  else if (e.GetId() == ID_TOOL_LINE_COL)
  {
    mediator->handleEditModeLineCol();
  }
  else if (e.GetId() == ID_TOOL_SHOW_GRID)
  {
    mediator->handleEditShowGrid(toolBarEdit->GetToolState(ID_TOOL_SHOW_GRID));
  }
  else if (e.GetId() == ID_TOOL_SNAP_GRID)
  {
    mediator->handleEditSnapGrid(toolBarEdit->GetToolState(ID_TOOL_SNAP_GRID));
  }


}


void Frame::onButton(wxCommandEvent& e)
{
  if (e.GetId() == ID_BUTTON_CLUST_ATTR)
  {
    // get idcs of selected (multiple) items
    vector< size_t > indcs;
    long item = -1;
    item = listCtrlAttr->GetNextItem(item,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);
    while (item != -1)
    {
      indcs.push_back(listCtrlAttr->GetItemData(item));

      item = listCtrlAttr->GetNextItem(
               item,
               wxLIST_NEXT_ALL,
               wxLIST_STATE_SELECTED);
    }

    // handle selections
    mediator->handleAttributeCluster(indcs);
  }
  if (e.GetId() == ID_BUTTON_TRACE_ATTR)
  {
    // get idcs of selected (multiple) items
    vector< size_t > indcs;
    long item = -1;
    item = listCtrlAttr->GetNextItem(item,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);
    while (item != -1)
    {
      indcs.push_back(listCtrlAttr->GetItemData(item));

      item = listCtrlAttr->GetNextItem(
               item,
               wxLIST_NEXT_ALL,
               wxLIST_STATE_SELECTED);
    }

    // handle selections
    mediator->initTimeSeries(indcs);
  }
  else if (e.GetId() == ID_BUTTON_OK_CLUST)
  {
    if (mediator->getClustMode() == Mediator::CLUST_DISTR_PLOT)
    {
      long item = -1;
      int  attrIdx;

      // get attribute index
      item = listCtrlClust->GetNextItem(
               item,
               wxLIST_NEXT_ALL,
               wxLIST_STATE_SELECTED);
      attrIdx = item;

      if (attrIdx >= 0)
      {
        // make sure frame plot exists
        if (framePlot != NULL)
        {
          framePlot->Close();
        }
        initFramePlot();

        mediator->handleClustPlotFrameDisplay(attrIdx);
      }

      if (frameClust != NULL)
      {
        frameClust->Destroy();
        frameClust = NULL;
      }
    }
    else if (mediator->getClustMode() == Mediator::CLUST_CORRL_PLOT)
    {
      long item = -1;
      int attrIdx1;
      int attrIdx2;

      // get attribute indices
      item = listCtrlClust->GetNextItem(
               item,
               wxLIST_NEXT_ALL,
               wxLIST_STATE_SELECTED);
      attrIdx1 = item;
      item = listCtrlClust->GetNextItem(
               item,
               wxLIST_NEXT_ALL,
               wxLIST_STATE_SELECTED);
      attrIdx2 = item;

      if (attrIdx1 >= 0 && attrIdx2 >= 0)
      {
        // make sure frame plot exists
        if (framePlot != NULL)
        {
          framePlot->Close();
        }
        initFramePlot();

        mediator->handleClustPlotFrameDisplay(attrIdx1, attrIdx2);
      }

      if (frameClust != NULL)
      {
        frameClust->Destroy();
        frameClust = NULL;
      }
    }
    else if (mediator->getClustMode() == Mediator::CLUST_COMBN_PLOT)
    {
      // make sure frame plot exists
      if (framePlot != NULL)
      {
        framePlot->Close();
      }
      initFramePlot();

      // get idcs of selected (multiple) items
      vector< size_t > indcs;
      long item = -1;
      item = listCtrlClust->GetNextItem(item,
                                        wxLIST_NEXT_ALL,
                                        wxLIST_STATE_SELECTED);
      while (item != -1)
      {
        indcs.push_back(listCtrlClust->GetItemData(item));

        item = listCtrlClust->GetNextItem(
                 item,
                 wxLIST_NEXT_ALL,
                 wxLIST_STATE_SELECTED);
      }

      // handle selections
      mediator->handleClustPlotFrameDisplay(indcs);

      if (frameClust != NULL)
      {
        frameClust->Destroy();
        frameClust = NULL;
      }
    }
  }
  else if (e.GetId() == ID_BUTTON_ABOUT)
  {
    frameAbout->Destroy();
  }
  else if (e.GetId() == ID_ADD_BUTTON_NOTE)
  {
    if (frameNote->Destroy())
    {
      string noteString = (const char*)(noteText->GetValue()).mb_str(wxConvUTF8);
      frameNote = NULL;
      mediator->handleShowNote(noteString, currentShapeId);
    }
  }
  else if (e.GetId() == ID_CLEAR_BUTTON_NOTE)
  {
    noteText->Clear();
  }
  else if (e.GetId() == ID_OK_BUTTON_TEXT_SIZE)
  {
    if (textSizeBox->GetValue().IsNumber())  // Check whether user entered a number or not
    {
      double size;
      textSizeBox->GetValue().ToDouble(&size);
      size_t textSize = (size_t) size;
      if (textSize < 8)  // Font size can be between 8 and 40
      {
        wxMessageBox(wxT("Entered value should be an integer bigger than or equal to 8"),wxT("Font size is too small"), wxOK | wxICON_INFORMATION, this);
      }
      else if (textSize > 40)
      {
        wxMessageBox(wxT("Entered value should be an integer less than or equal to 40"),wxT("Font size is too big"), wxOK | wxICON_INFORMATION, this);
      }
      else if (frameTextSize->Destroy())
      {
        frameTextSize = NULL;
        mediator->handleSetTextSize(textSize, currentShapeId);
      }
    }
    else
    {
      wxMessageBox(wxT("Please enter an integer between 8 and 40"),wxT("Wrong Input"), wxOK | wxICON_INFORMATION, this);
    }
  }
}

/*
void Frame::onRadioBox( wxCommandEvent &e )
{
    if ( e.GetId() == ID_RADIO_BOX_TEXT_DOF )
    {
        // get selected item
        int  idx  = -1;
        long item = -1;
        item = listCtrlDOF->GetNextItem(
            item,
            wxLIST_NEXT_ALL,
            wxLIST_STATE_SELECTED );

        if ( item != -1 )
        {
            idx = listCtrlDOF->GetItemData( item );
            int sel = radioBoxTextDOF->GetSelection();

            if ( 0 <= idx && idx < listCtrlDOF->GetItemCount() )
            {
                if ( sel == 0 )
                    mediator->handleSetDOFTextStatus( idx, DOF::ID_TEXT_NONE );
                else if ( sel == 1 )
                    mediator->handleSetDOFTextStatus( idx, DOF::ID_TEXT_ALL );
                else if ( sel == 2 )
                    mediator->handleSetDOFTextStatus( idx, DOF::ID_TEXT_ATTR );
                else if ( sel == 3 )
                    mediator->handleSetDOFTextStatus( idx, DOF::ID_TEXT_VAL );
            }
        }
    }
}
*/


void Frame::onSplitterDoubleClick(wxSplitterEvent& e)
{
  if (e.GetId() == ID_SPLITTER_FRAME)
  {
    // minimize the splitterLeft pane
    splitterFrame->SetSashPosition(splitterFrame->GetMinimumPaneSize(), true);
  }
  else if (e.GetId() == ID_SPLITTER_LFT)
  {
    // minimize the panelBotLft pane
    splitterLft->SetSashPosition((0 - splitterLft->GetMinimumPaneSize()), true);
  }
  else if (e.GetId() == ID_SPLITTER_TOP_LFT)
  {
    // minimize the panelBotTopLft pane
    splitterTopLft->SetSashPosition((0 - splitterTopLft->GetMinimumPaneSize()), true);
  }
  else if (e.GetId() == ID_SPLITTER_RGT)
  {
    // minimize the panelBotRgt pane
    splitterRgt->SetSashPosition((0 - splitterRgt->GetMinimumPaneSize()), true);
  }

  // fit everything after minimizing panes
  Layout();
  Fit();
}


// -- implement event table -----------------------------------------


BEGIN_EVENT_TABLE(Frame, wxFrame)
  // menu bar
  EVT_MENU(wxID_OPEN, Frame::onMenuBar)
  EVT_MENU(wxID_SAVE, Frame::onMenuBar)
  EVT_MENU(wxID_SAVEAS, Frame::onMenuBar)
  EVT_MENU(ID_MENU_ITEM_LOAD_CONFIG, Frame::onMenuBar)
  EVT_MENU(ID_MENU_ITEM_SAVE_CONFIG, Frame::onMenuBar)
  EVT_MENU(ID_MENU_ITEM_LOAD_DIAGRAM, Frame::onMenuBar)
  EVT_MENU(ID_MENU_ITEM_SAVE_DIAGRAM, Frame::onMenuBar)
  EVT_MENU(ID_MENU_ITEM_MODE_ANALYSIS, Frame::onMenuBar)
  EVT_MENU(ID_MENU_ITEM_MODE_EDIT, Frame::onMenuBar)
  EVT_MENU(ID_MENU_ITEM_VIEW_SIM, Frame::onMenuBar)
  EVT_MENU(ID_MENU_ITEM_VIEW_TRACE, Frame::onMenuBar)
  EVT_MENU(ID_MENU_ITEM_SETTINGS_GENERAL, Frame::onMenuBar)
  EVT_MENU(ID_MENU_ITEM_SETTINGS_CLUST_TREE, Frame::onMenuBar)
  EVT_MENU(ID_MENU_ITEM_SETTINGS_BAR_TREE, Frame::onMenuBar)
  EVT_MENU(ID_MENU_ITEM_SETTINGS_ARC_DIAGRAM, Frame::onMenuBar)
  EVT_MENU(ID_MENU_ITEM_SETTINGS_SIMULATOR, Frame::onMenuBar)
  EVT_MENU(ID_MENU_ITEM_SETTINGS_TRACE, Frame::onMenuBar)
  EVT_MENU(ID_MENU_ITEM_SETTINGS_EDITOR, Frame::onMenuBar)
  EVT_MENU(wxID_CLOSE, Frame::onMenuBar)
  // splitter windows
  EVT_SPLITTER_DCLICK(ID_SPLITTER_FRAME, Frame::onSplitterDoubleClick)
  EVT_SPLITTER_DCLICK(ID_SPLITTER_LFT, Frame::onSplitterDoubleClick)
  EVT_SPLITTER_DCLICK(ID_SPLITTER_TOP_LFT, Frame::onSplitterDoubleClick)
  EVT_SPLITTER_DCLICK(ID_SPLITTER_RGT, Frame::onSplitterDoubleClick)
  // attributes & domain
  EVT_LIST_ITEM_SELECTED(ID_LIST_CTRL_ATTR, Frame::onListCtrlSelect)
  EVT_LIST_ITEM_DESELECTED(ID_LIST_CTRL_ATTR, Frame::onListCtrlSelect)
  EVT_LIST_BEGIN_DRAG(ID_LIST_CTRL_ATTR, Frame::onListCtrlBeginDrag)
  EVT_LIST_ITEM_RIGHT_CLICK(ID_LIST_CTRL_ATTR, Frame::onListCtrlRgtClick)
  EVT_BUTTON(ID_BUTTON_CLUST_ATTR, Frame::onButton)
  EVT_BUTTON(ID_BUTTON_TRACE_ATTR, Frame::onButton)
  EVT_MENU(ID_MENU_ITEM_ATTR_DISTR_PLOT, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_ATTR_CORRL_PLOT, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_ATTR_COMBN_PLOT, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_ATTR_DUPL, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_ATTR_RENAME, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_ATTR_DELETE, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_ATTR_CLUST, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_ATTR_TRACE, Frame::onPopupMenu)
  EVT_LIST_ITEM_SELECTED(ID_LIST_CTRL_DOMAIN, Frame::onListCtrlSelect)
  EVT_LIST_BEGIN_DRAG(ID_LIST_CTRL_DOMAIN, Frame::onListCtrlBeginDrag)
  EVT_LIST_ITEM_RIGHT_CLICK(ID_LIST_CTRL_DOMAIN, Frame::onListCtrlRgtClick)
  EVT_MENU(ID_MENU_ITEM_DOM_GROUP, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_DOM_UNGROUP, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_DOM_RENAME, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_CLUSTER, Frame::onClusterMenu)
  // diagram editor
  EVT_TOOL(ID_TOOL_SELECT, Frame::onTool)
  EVT_TOOL(ID_TOOL_NOTE, Frame::onTool)
  EVT_TOOL(ID_TOOL_DOF, Frame::onTool)
  EVT_TOOL(ID_TOOL_RECT, Frame::onTool)
  EVT_TOOL(ID_TOOL_ELLIPSE, Frame::onTool)
  EVT_TOOL(ID_TOOL_LINE, Frame::onTool)
  EVT_TOOL(ID_TOOL_ARROW, Frame::onTool)
  EVT_TOOL(ID_TOOL_DARROW, Frame::onTool)
  EVT_TOOL(ID_TOOL_FILL_COL, Frame::onTool)
  EVT_TOOL(ID_TOOL_LINE_COL, Frame::onTool)
  EVT_TOOL(ID_TOOL_SHOW_GRID, Frame::onTool)
  EVT_TOOL(ID_TOOL_SNAP_GRID, Frame::onTool)
  // edit shapes
  EVT_MENU(ID_MENU_ITEM_SHOW_VARIABLES, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_SHOW_NOTE, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_TEXT_SIZE, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_SHAPE_CUT, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_SHAPE_COPY, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_SHAPE_PASTE, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_SHAPE_DELETE, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_SHAPE_BRING_TO_FRONT, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_SHAPE_SEND_TO_BACK, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_SHAPE_BRING_FORWARD, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_SHAPE_SEND_BACKWARD, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_SHAPE_EDIT_DOF, Frame::onPopupMenu)
  // dof frame
  EVT_LIST_ITEM_SELECTED(ID_LIST_CTRL_DOF, Frame::onListCtrlSelect)
  EVT_LIST_ITEM_RIGHT_CLICK(ID_LIST_CTRL_DOF, Frame::onListCtrlRgtClick)
  //EVT_RADIOBOX( ID_RADIO_BOX_TEXT_DOF, Frame::onRadioBox )
  EVT_MENU(ID_MENU_ITEM_DOF_UNLINK, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_DOF_ATTRIBUTE_LIST, Frame::onPopupMenu)
  // interaction with clusters
  EVT_MENU(ID_MENU_ITEM_CLUST_DISTR_PLOT, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_CLUST_CORRL_PLOT, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_CLUST_COMBN_PLOT, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_CLUST_SUBCLUST, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_CLUST_UNCLUST, Frame::onPopupMenu)
  EVT_LIST_ITEM_SELECTED(ID_LIST_CTRL_CLUST, Frame::onListCtrlSelect)
  EVT_BUTTON(ID_BUTTON_OK_CLUST, Frame::onButton)
  // sending diagrams
  EVT_MENU(ID_MENU_ITEM_DGRM_SGL_TO_SIML, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_DGRM_SGL_TO_TRACE, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_DGRM_SET_TO_TRACE, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_DGRM_SGL_TO_EXNR, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_DGRM_SET_TO_EXNR, Frame::onPopupMenu)
  EVT_MENU(ID_MENU_ITEM_EXNR_CLEAR, Frame::onPopupMenu)
  // about frame
  EVT_BUTTON(ID_BUTTON_ABOUT, Frame::onButton)
END_EVENT_TABLE()


// -- end -----------------------------------------------------------
