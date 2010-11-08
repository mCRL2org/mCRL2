// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file xstceditor.cpp

#include "settings.h"
#include "xstceditor.h"
#include "wx/tokenzr.h"

BEGIN_EVENT_TABLE(xStcEditor, wxStyledTextCtrl)
  EVT_STC_UPDATEUI(wxID_ANY,  xStcEditor::OnxStcUIUpdate)
  EVT_STC_CHARADDED(wxID_ANY, xStcEditor::AutoComplete)
  EVT_MENU (wxID_COPY,        xStcEditor::OnCopy)
  EVT_MENU (wxID_SELECTALL,   xStcEditor::OnSelectAll)
  EVT_MENU (wxID_PASTE,       xStcEditor::OnPaste)
  EVT_MENU (wxID_UNDO,        xStcEditor::OnUndo)
  EVT_MENU (wxID_REDO,        xStcEditor::OnRedo)
  EVT_MENU (wxID_CUT,         xStcEditor::OnCut)
  EVT_MENU (wxID_CLEAR,       xStcEditor::OnDelete)

  EVT_STC_PAINTED(wxID_ANY, xStcEditor::Focus )
//  EVT_SET_FOCUS(wxStyledTextCtrl::OnGainFocus)
//  EVT_KILL_FOCUS(xStcEditor::OnLoseFocus)
END_EVENT_TABLE ()

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_UPDATE_EDITOR_FOCUS, 7777)
    DECLARE_EVENT_TYPE(wxEVT_SETSTATUSTEXT, 7777)
END_DECLARE_EVENT_TYPES()


  xStcEditor::xStcEditor(wxWindow *parent, wxWindowID id):
    wxStyledTextCtrl(parent,id)
      {
          int MARGIN_LINE_NUMBERS=0;
          StyleClearAll();

          StyleSetForeground (wxSTC_STYLE_LINENUMBER, wxColour (wxT("DARK GREY")));
          StyleSetBackground (wxSTC_STYLE_LINENUMBER, *wxWHITE);
          StyleSetForeground(wxSTC_STYLE_INDENTGUIDE, wxColour (wxT("DARK GREY")));
          SetMarginWidth (MARGIN_LINE_NUMBERS, 25);
          SetMarginType (MARGIN_LINE_NUMBERS, wxSTC_MARGIN_NUMBER);

          wxFont font (10, wxMODERN, wxNORMAL, wxNORMAL);
          // default fonts for all styles!
          int Nr;
          for (Nr = 0; Nr < wxSTC_STYLE_LASTPREDEFINED; Nr++) {
              StyleSetFont (Nr, font);
          }

          /* Abusing Lexer for LUA */
          SetLexer(wxSTC_LEX_LUA);
          SetKeyWords(0, mcrl2_spec_keywords + mcrl2_proc_keywords );
          StyleSetForeground (wxSTC_LUA_WORD, wxColour (wxT("FOREST GREEN")));
          StyleSetBold(wxSTC_LUA_WORD,true);

          SetKeyWords(1, mcrl2_sort_keywords );
          StyleSetForeground (wxSTC_LUA_WORD2, wxColour (wxT("BLUE")));
          StyleSetBold(wxSTC_LUA_WORD2,true);

          SetKeyWords(2, mcrl2_data_keywords );
          StyleSetForeground (wxSTC_LUA_WORD3, wxColour (wxT("ORCHID")));
          StyleSetBold(wxSTC_LUA_WORD3,true);

          SetKeyWords(3, mcrl2_operator_keywords );
          StyleSetForeground (wxSTC_LUA_WORD4, wxColour (wxT("ORANGE")));
          StyleSetBold(wxSTC_LUA_WORD4,true);

          StyleSetForeground (wxSTC_LUA_COMMENT, wxColour (wxT("GRAY")));


          // setup the colors and bold font
          StyleSetBackground(wxSTC_STYLE_BRACELIGHT, wxColour(0xff, 0xcc, 0x00));        // brace highlight
          StyleSetBackground(wxSTC_STYLE_BRACEBAD, wxColour(0xff, 0x33, 0x33));        // brace bad highlight
          StyleSetBold(wxSTC_STYLE_BRACELIGHT, TRUE);
          StyleSetBold(wxSTC_STYLE_BRACEBAD, TRUE);

          SetWrapMode(true);

          // set visibility
          SetVisiblePolicy (wxSTC_VISIBLE_STRICT|wxSTC_VISIBLE_SLOP, 1);
          SetXCaretPolicy (wxSTC_CARET_EVEN|wxSTC_VISIBLE_STRICT|wxSTC_CARET_SLOP, 1);
          SetYCaretPolicy (wxSTC_CARET_EVEN|wxSTC_VISIBLE_STRICT|wxSTC_CARET_SLOP, 1);

          SetCaretLineVisible(true);
          EnsureCaretVisible();
          SetCaretForeground(wxT("BLACK"));
          SetCaretLineBackground(wxColour(235,235,235));

          SetCaretWidth(1);
          SetCaretPeriod(500);

  };

void xStcEditor::AutoComplete(wxStyledTextEvent &/*event*/){
  int pos = GetCurrentPos();
  int start = WordStartPosition(pos, true);
  if (pos - start > 2 && !AutoCompActive())    // require 3 characters to show auto-complete
  {
      // GTK version crashes if nothing matches, so this check must be made for GTK
      // For MSW, it doesn't crash but it flashes on the screen (also not very nice)
      if (HasWord(GetTextRange(start, pos), mcrl2_spec_keywords))
          AutoCompShow(pos-start, mcrl2_spec_keywords);
  }
}

bool xStcEditor::HasWord(wxString word, wxString &wordlist)
{
    word.MakeUpper();   // entire wordlist is uppercase

    wxStringTokenizer tkz(wordlist, wxT(" "));
    while (tkz.HasMoreTokens())
    {
        wxString token = tkz.GetNextToken();
        if (token.StartsWith(word))
            return true;
    }
    return false;
}

void xStcEditor::OnxStcUIUpdate(wxStyledTextEvent &/*event*/){

  /*
   * Matchs brackets
   */
  int p = GetCurrentPos();
  int c1 = GetCharAt(p);
  int c2 = (p > 1 ? GetCharAt(p-1) : 0);

  if (c2=='(' || c2==')' || c1=='(' || c1==')')
  {
      int sp = (c2=='(' || c2==')') ? p-1 : p;

      int q = BraceMatch(sp);
      if (q == wxSTC_INVALID_POSITION)
          BraceBadLight(sp);
      else
          BraceHighlight(sp, q);
  }
  else
      BraceBadLight(wxSTC_INVALID_POSITION);    // remove light

  wxCommandEvent eventCustom(wxEVT_SETSTATUSTEXT);

  if( GetCurrentPos() > 0 ){

  wxString *s = new wxString(
         wxT("pos: ")
      +  wxString::Format (wxT("%d"), GetCurrentLine())
      + wxT(",")
      + wxString::Format( wxT("%d"), GetColumn( GetCurrentPos()))
     );
  eventCustom.SetClientData(s);
  wxPostEvent(this->GetParent(), eventCustom);
  }
};


void xStcEditor::OnCopy (wxCommandEvent &/*event*/) {
   if (GetSelectionEnd()-GetSelectionStart() <= 0) return;
   Copy();
};

void xStcEditor::OnSelectAll (wxCommandEvent &/*event*/) {
      SetSelection (0, GetTextLength ());
};

void xStcEditor::OnPaste (wxCommandEvent &/*event*/){
  if (!CanPaste()) return;
  Paste();
};

void xStcEditor::OnUndo (wxCommandEvent &/*event*/){
  Undo();
};

void xStcEditor::OnRedo (wxCommandEvent &/*event*/){
  Redo();
};

void xStcEditor::OnCut (wxCommandEvent &/*event*/){
  if ( (GetSelectionEnd()-GetSelectionStart() <= 0)) return;

  if (GetReadOnly()){
    Copy();
  } else{
    Cut();
  }
  ;

};

void xStcEditor::OnDelete (wxCommandEvent &/*event*/){
  if (GetReadOnly()) return;
  Clear();
};

void  xStcEditor::Focus(wxStyledTextEvent &/* event*/){
//Sets Focus to current editor.... Ugly workaround
if ( GetSTCFocus() ){
    wxCommandEvent eventCustom(wxEVT_UPDATE_EDITOR_FOCUS);
    /* Send pointer of focused window */
    eventCustom.SetClientData(this);
    wxPostEvent(this->GetParent(), eventCustom);
} else {
    wxCommandEvent eventCustom(wxEVT_UPDATE_EDITOR_FOCUS);
    /* Send NO Focus */
    eventCustom.SetClientData(NULL);
    wxPostEvent(this->GetParent(), eventCustom);
}};

//void wxStyledTextCtrl::OnGainFocus( wxFocusEvent &/*event*/ ){
//  wxCommandEvent eventCustom(wxEVT_UPDATE_EDITOR_FOCUS);
//  /* Send pointer of focused window */
//  eventCustom.SetClientData(this);
//  wxPostEvent(this->GetParent(), eventCustom);
//};
//
//void wxStyledTextCtrl::OnLoseFocus( wxFocusEvent &/*event*/){
//  wxCommandEvent eventCustom(wxEVT_UPDATE_EDITOR_FOCUS);
//  /* Send NO Focus */
//  eventCustom.SetClientData(NULL);
//  wxPostEvent(this->GetParent(), eventCustom);
//};


