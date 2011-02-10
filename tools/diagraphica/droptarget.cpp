// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./droptarget.cpp

#include "wx.hpp" // precompiled headers

#include "droptarget.h"

using namespace std;

// -- constructors and destructor -----------------------------------


// --------------------
DropTarget::DropTarget(
  wxWindow* ownr,
  Mediator* m)
  : Colleague(m)
// --------------------
{
  owner = ownr;
}


// ----------------------
DropTarget::~DropTarget()
// ----------------------
{
  owner = NULL;
}


// -- overridden functions from wxTextDropTarget --------------------


// -------------------------
bool DropTarget::OnDropText(
  wxCoord x,
  wxCoord y,
  const wxString& text)
// -------------------------
{
  vector< wxString > tokens;
  int    srcId;
  vector< int > data;

  wxStringTokenizer tkz(
    text,         // string
    wxString(wxT(" ")));    // delimiters
  while (tkz.HasMoreTokens())
  {
    tokens.push_back(tkz.GetNextToken());
  }

  // get src id
  if (tokens.size() > 0)
  {
    srcId = Utils::strToInt(string(tokens[0].mb_str(wxConvUTF8)));
  }
  // get remaining data
  for (size_t i = 1; i < tokens.size(); ++i)
  {
    data.push_back(Utils::strToInt(string(tokens[i].mb_str(wxConvUTF8))));
  }

  mediator->handleDragDrop(
    srcId,
    owner->GetId(),
    x,
    y,
    data);
  return true;
}


// -- end -----------------------------------------------------------
