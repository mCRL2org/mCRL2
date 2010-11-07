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
  EVT_STC_UPDATEUI(wxID_ANY, xStcEditor::OnxStcUIUpdate)
  EVT_STC_CHARADDED(wxID_ANY, xStcEditor::AutoComplete)
END_EVENT_TABLE ()

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

          AutoCompSetIgnoreCase(true);
          AutoCompSetAutoHide(true);
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

};

