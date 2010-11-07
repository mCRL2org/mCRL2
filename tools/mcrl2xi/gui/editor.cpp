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
    p_data_editor = new xStcEditor( this, wxID_ANY );
    this->AddPage(p_data_editor, wxT("Data editor"));

    p_process_editor = new xStcEditor( this, wxID_ANY );
    p_process_editor->SetReadOnly(true);
    this->AddPage(p_process_editor, wxT("Process viewer [Read Only]"));
  };

  bool xEditor::LoadFile( const wxString &filename ){

    wxStreamToTextRedirector redirect(p_output);

    try{
      wxFileName fn = wxFileName(filename);

      // Load datafile from mcrl2 specification
      if(fn.GetExt() == wxT("mcrl2")){

        std::ifstream instream(filename.mb_str(), std::ifstream::in|std::ifstream::binary);
        if (!instream.is_open()) {
          throw mcrl2::runtime_error("cannot open input file: " + std::string(filename.mb_str()));
        }
        mcrl2::process::process_specification spec = mcrl2::process::parse_process_specification(instream, false);
        mcrl2::process::complete_data_specification (spec);

        p_data_editor->Clear();
        /* TODO: Segfault when a "mcrl2" spec opens  */
        std::string *p_data_string;
        *p_data_string = mcrl2::data::pp(mcrl2::data::detail::data_specification_to_aterm_data_spec(spec.data()));
        p_data_editor->AppendText( wxString( p_data_string->c_str(), wxConvUTF8 ) );

        return true;
      };

      // Load datafile from LPS
      if(fn.GetExt() == wxT("lps")){
        mcrl2::lps::specification spec;
        spec.load(std::string(filename.mb_str(wxConvUTF8)));
        p_data_editor->Clear();
        p_process_editor->Clear();

        p_data_editor->AppendText( wxString( mcrl2::data::pp(mcrl2::data::detail::data_specification_to_aterm_data_spec(spec.data())).c_str(), wxConvUTF8 ) );

        /* Text can only be appended if ReadOnly is disabled*/
        p_process_editor->SetReadOnly(false);

        /* Print action declarations */
        p_process_editor->AppendText(
            wxString(
                mcrl2::core::pp(
                    mcrl2::core::detail::gsMakeActSpec(spec.action_labels())).c_str()
                , wxConvUTF8) + wxTextFile::GetEOL());

        /* Print global variables */
        atermpp::set<mcrl2::data::variable> gvs = spec.global_variables();
        mcrl2::data::variable_list gvl = atermpp::convert<mcrl2::data::variable_list, atermpp::set<mcrl2::data::variable> >(  gvs);

        /* Print global variables */
        p_process_editor->AppendText(
              wxString(
                  mcrl2::core::pp(
                      mcrl2::core::detail::gsMakeGlobVarSpec( gvl
                          )
                    ).c_str()
                  , wxConvUTF8) + wxTextFile::GetEOL() );

        p_process_editor->AppendText( wxString( mcrl2::lps::pp( spec.process() ).c_str() , wxConvUTF8 ) + wxTextFile::GetEOL()) ;
        p_process_editor->AppendText( wxString( mcrl2::core::pp( spec.initial_process() ).c_str() , wxConvUTF8 ) + wxTextFile::GetEOL() ) ;
        p_process_editor->SetReadOnly(true);

        FileInUse = filename;
        return true;
      };

      return false;
      //return wxRichTextCtrl::LoadFile( filename ) ;
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
    return p_data_editor -> GetText();
  };

  wxString xEditor::GetStringFromProcessEditor(){
    return p_process_editor -> GetText();
  };

  wxString xEditor::GetFileInUse(){
    return FileInUse;
  };

  bool xEditor::SaveFile( const wxString &filename ){
    wxStreamToTextRedirector redirect(p_output);
    try{
    std::cout << "Saving: \"" << filename.mb_str() << "\""<< std::endl;

    /* Action for saving to lps */
    if(wxFileName(filename).GetExt() == wxT("lps")){
      std::cout << "Parsing and type checking specification" << std::endl;
      std::string str_spec = std::string(wxString(GetStringFromDataEditor() +  GetStringFromProcessEditor()).mb_str());
      mcrl2::lps::specification spec = mcrl2::lps::parse_linear_process_specification( str_spec );
      spec.save(std::string(filename.mb_str()));
      std::cout << "Successfully saved to: "<< filename.mb_str() << std::endl;
      return true;
    }

    /* Action for saving to mcrl2 or txt */
    if( wxFileName(filename).GetExt() == wxT("mcrl2") ||
        wxFileName(filename).GetExt() == wxT("txt") ){
      wxString spec = GetStringFromDataEditor() +  GetStringFromProcessEditor();

      wxFile f( filename, wxFile::write);
      if(f.Write( spec, wxConvUTF8 )){
        std::cout << "Successfully saved to: "<< filename.mb_str() << std::endl;
      } else {
        std::cout << "Failed saving to: "<< filename.mb_str() << std::endl;
      }
      return true;
    }


    } catch ( mcrl2::runtime_error e) {
      std::cout << e.what() <<std::endl;
    }


    return true;
  };


