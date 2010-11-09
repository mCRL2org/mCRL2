// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file editor.cpp

#include "editor.h"
#include <fstream>
#include "wx/filename.h"
#include <wx/textfile.h>
#include <wx/aui/auibook.h>

#include "mcrl2/process/parse.h"
#include "mcrl2/lps/parse.h"

xEditor::xEditor(wxWindow *parent, wxWindowID id, wxTextCtrl *output ) :
  wxAuiNotebook( parent , id )
  {
    p_output = output;
  };

  void xEditor::AddEmptyPage(){
    p_data_editor = new xStcEditor( this, wxID_ANY );
    this->AddPage(p_data_editor, wxT("new"));
    p_data_editor->SetFocus();
    p_data_editor->SetFileInUse( wxEmptyString );
  }

  bool xEditor::LoadFile( const wxString &filename ){
    p_data_editor = new xStcEditor( this, wxID_ANY );
    this->AddPage(p_data_editor, wxFileName(filename).GetFullName());
    p_data_editor->SetFocus();

    wxStreamToTextRedirector redirect(p_output);

    try{
      wxFileName fn = wxFileName(filename);

      // Load datafile from LPS
      if(fn.GetExt() == wxT("lps")){
        mcrl2::lps::specification spec;
        spec.load(std::string(filename.mb_str(wxConvUTF8)));
        p_data_editor->Clear();
        p_data_editor->AppendText( wxString( mcrl2::lps::pp(spec).c_str(), wxConvUTF8 ) );
        p_data_editor->SetFileInUse( filename );
        return true;
      };

      // Load text for unknown extensions
      {
        p_data_editor->Clear();

        wxTextFile file(filename);

        file.Open();
        wxString i;
        for ( i = file.GetFirstLine(); !file.Eof();
            i = file.GetNextLine() )
        {
          p_data_editor->AppendText( i + wxTextFile::GetEOL() );
        }
        file.Close();
        p_data_editor->SetFileInUse( filename );
        return true;
      };

      return false;
    }
    catch (mcrl2::runtime_error &e) {
    {
      wxStreamToTextRedirector redirect(p_output);
      std::cout << e.what() <<std::endl;
    }

    }
    return false;
  };

  wxString xEditor::GetStringFromDataEditor(){
    if (( xStcEditor *) GetPage(GetSelection())){
      return (( xStcEditor *) GetPage(GetSelection())) -> GetText();
    }
    return wxEmptyString;
  };

  wxString xEditor::GetFileInUse(){
    if (( xStcEditor *) GetPage(GetSelection())){
      return (( xStcEditor *) GetPage(GetSelection())) -> GetFileInUse();
    }
    return wxEmptyString;
  };

  bool xEditor::SaveFile( const wxString &filename ){
    wxDateTime now = wxDateTime::Now();
    p_output->Clear();
    wxStreamToTextRedirector redirect(p_output);
    try{

      if (filename == wxEmptyString){
        std::cout << "Saving to empty file" << std::endl;
        return false;
      }

      std::cout << now.FormatTime().mb_str() <<" ** Saving: \"" << filename.mb_str() << "\""<< std::endl;

      /* Action for saving to lps */
      if(wxFileName(filename).GetExt() == wxT("lps")){
        std::cout << now.FormatTime().mb_str() <<" ** Parsing and type checking specification" << std::endl;
        std::string str_spec = std::string(GetStringFromDataEditor().mb_str());
        mcrl2::lps::specification spec = mcrl2::lps::parse_linear_process_specification( str_spec );
        spec.save(std::string(filename.mb_str()));
        std::cout << now.FormatTime().mb_str() <<" ** Successfully saved to: "<< filename.mb_str() << std::endl;
        /* Reassign filename in use */
        p_data_editor->SetFileInUse( filename );
        this->SetPageText( GetSelection(), wxFileName(filename).GetFullName() ) ;
        return true;
      }

      /* Action for saving to text for any other extension*/
      {
        wxString spec = GetStringFromDataEditor();

        wxFile f( filename, wxFile::write);
        if(f.Write( spec, wxConvUTF8 )){
          std::cout << now.FormatTime().mb_str() <<" ** Successfully saved to: "<< filename.mb_str() << std::endl;
          /* Reassign filename in use */
          p_data_editor->SetFileInUse( filename );
          this->SetPageText( GetSelection(), wxFileName(filename).GetFullName() ) ;
        } else {
          std::cout << now.FormatTime().mb_str() <<" ** Failed saving to: "<< filename.mb_str() << std::endl;
        }
        return true;
      }

    } catch ( mcrl2::runtime_error e) {
      std::cout << now.FormatTime().mb_str() <<" ** " << e.what() <<std::endl;
    }


    return true;
  };


