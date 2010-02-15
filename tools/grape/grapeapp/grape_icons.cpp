// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file grape_icons.cpp
//
// Maps all icons for the application.

#include "wx.hpp" // precompiled headers

#include "grape_icons.h"

#include "pics/archref.xpm"
#include "pics/attach.xpm"
#include "pics/blocked.xpm"
#include "pics/chancomm.xpm"
#include "pics/channel.xpm"
#include "pics/close.xpm"
#include "pics/comment.xpm"
#include "pics/copy.xpm"
#include "pics/cut.xpm"
#include "pics/del.xpm"
#include "pics/detach.xpm"
#include "pics/edit.xpm"
#include "pics/exit.xpm"
#include "pics/export.xpm"
#include "pics/grape.xpm"
#include "pics/helpabout.xpm"
#include "pics/helpcontents.xpm"
#include "pics/image.xpm"
#include "pics/initdes.xpm"
#include "pics/new.xpm"
#include "pics/newarch.xpm"
#include "pics/newproc.xpm"
#include "pics/open.xpm"
#include "pics/paste.xpm"
#include "pics/procref.xpm"
#include "pics/properties.xpm"
#include "pics/redo.xpm"
#include "pics/save.xpm"
#include "pics/save-as.xpm"
#include "pics/state.xpm"
#include "pics/text.xpm"
#include "pics/termtransition.xpm"
#include "pics/toolselect.xpm"
#include "pics/transition.xpm"
#include "pics/undo.xpm"
#include "pics/validate.xpm"
#include "pics/visible.xpm"

IconMap g_icons;

void init_icons()
{
  static bool initialized = false;
  if ( initialized )
  {
    return;
  }

  g_icons[_T("archref")] = wxBitmap( archref_xpm );
  g_icons[_T("attach")] = wxBitmap( attach_xpm );
  g_icons[_T("blocked")] = wxBitmap( blocked_xpm );
  g_icons[_T("chancomm")] = wxBitmap( chancomm_xpm );
  g_icons[_T("channel")] = wxBitmap( channel_xpm );
  g_icons[_T("close")] = wxBitmap( close_xpm );
  g_icons[_T("comment")] = wxBitmap( comment_xpm );
  g_icons[_T("copy")] = wxBitmap( copy_xpm );
  g_icons[_T("cut")] = wxBitmap( cut_xpm );
  g_icons[_T("del")] = wxBitmap( delete_xpm );
  g_icons[_T("detach")] = wxBitmap( detach_xpm );
  g_icons[_T("edit")] = wxBitmap( edit_xpm );
  g_icons[_T("export")] = wxBitmap( export_xpm );
  g_icons[_T("exit")] = wxBitmap( exit_xpm );
  g_icons[_T("grape")] = wxBitmap( grape_xpm );
  g_icons[_T("helpabout")] = wxBitmap( helpabout_xpm );
  g_icons[_T("helpcontents")] = wxBitmap( helpcontents_xpm );
  g_icons[_T("image")] = wxBitmap( image_xpm );
  g_icons[_T("initdes")] = wxBitmap( initdes_xpm );
  g_icons[_T("new")] = wxBitmap( new_xpm );
  g_icons[_T("newarch")] = wxBitmap( newarch_xpm );
  g_icons[_T("newproc")] = wxBitmap( newproc_xpm );
  g_icons[_T("open")] = wxBitmap( open_xpm );
  g_icons[_T("paste")] = wxBitmap( paste_xpm );
  g_icons[_T("procref")] = wxBitmap( procref_xpm );
  g_icons[_T("properties")] = wxBitmap( properties_xpm );
  g_icons[_T("redo")] = wxBitmap( redo_xpm );
  g_icons[_T("save")] = wxBitmap( save_xpm );
  g_icons[_T("saveas")] = wxBitmap( saveas_xpm );
  g_icons[_T("state")] = wxBitmap( state_xpm );
  g_icons[_T("termtransition")] = wxBitmap( termtransition_xpm );
  g_icons[_T("text")] = wxBitmap( text_xpm );
  g_icons[_T("toolselect")] = wxBitmap( toolselect_xpm );
  g_icons[_T("transition")] = wxBitmap( transition_xpm );
  g_icons[_T("undo")] = wxBitmap( undo_xpm );
  g_icons[_T("validate")] = wxBitmap( validate_xpm );
  g_icons[_T("visible")] = wxBitmap( visible_xpm );
}
