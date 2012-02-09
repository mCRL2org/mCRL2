// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file configpanel.h


#ifndef MCRL2_GUI_CONFIGPANEL_H_
#define MCRL2_GUI_CONFIGPANEL_H_

#include "initialization.h"
#include <wx/utils.h>
#include <mcrl2-process.h>
#include <wx/radiobox.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/filepicker.h>
#include <wx/statline.h>
#include <gui/outputconfigpanel.h>
#include <wx/scrolwin.h>
#include <wx/gbsizer.h>
#include <wx/event.h>
#include <wx/filename.h>

enum ToolStatus { STATUS_NONE, STATUS_RUNNING, STATUS_COMPLETE, STATUS_FAILED};


#define ID_RUN_TOOL 1000
#define ID_OUTPUT_FILE 1001
#define ID_OUTPUT_EXT 1002

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(wxEVT_UPDATE_PROJECT_TREE, 7777)
END_DECLARE_EVENT_TYPES()

DEFINE_EVENT_TYPE(wxEVT_UPDATE_PROJECT_TREE)

// it may also be convenient to define an event table macro for this event type
#define EVT_UPDATE_PROJECT_TREE(id, fn) \
  DECLARE_EVENT_TABLE_ENTRY( \
                             wxEVT_UPDATE_PROJECT_TREE, id, wxID_ANY, \
                             (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
                             (wxObject *) NULL \
                           ),

class ConfigPanel: public wxNotebookPage
{
  public:

    ConfigPanel(wxAuiNotebook* parent, wxWindowID id, Tool& tool,
                OutPutTextCtrl* listbox_output, FileIO& fileIO) :
      wxNotebookPage(parent, id)
    {
      m_tool = tool;
      m_parent = parent;
      m_listbox_output = listbox_output;
      m_fileIO = fileIO;
      m_pid = 0;
      m_switchOnCerrOutput = false;
      m_switchOnStdOutput = false;
      m_additional_input_files.clear();
      m_tool_std_output = NULL;

      int row = 0;

      // Top Panel
      m_configpanel = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                        wxAUI_NB_BOTTOM
                                       );

      m_tool_cerr_output = new OutputConfigPanel(m_configpanel, wxID_ANY, wxDefaultPosition, wxDefaultSize);

      m_tool_cerr_output->SetRunCognizance(this);

      m_wsw = new wxScrolledWindow(m_configpanel, wxID_ANY, wxDefaultPosition, wxDefaultSize);

      /* Define size large enough for top*/
      wxPanel* top = new wxPanel(m_wsw , wxID_ANY, wxDefaultPosition, wxSize(4096,4096));

      wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

      wxGridBagSizer* fgs = new wxGridBagSizer(5, 5);

      m_working_directory = wxFileName(wxString(m_fileIO.input_file.c_str(), wxConvUTF8)).GetPath();

      fgs->Add(new wxStaticText(top, wxID_ANY, wxT("Working directory:") ) , wxGBPosition(row,0));
      fgs->Add(new wxStaticText(top, wxID_ANY, m_working_directory.GetFullPath()), wxGBPosition(row,1),
          wxGBSpan(1,2));

      row++;
      for( vector< string >::iterator s = m_tool.m_input_types.begin() ; s != m_tool.m_input_types.end(); s++  )
      {
        if ( s == m_tool.m_input_types.begin() )
        {

          fgs->Add(new wxStaticText(top, wxID_ANY, wxT("input file:") ) , wxGBPosition(row,0));

          wxFileName inf = wxFileName( wxString(m_fileIO.input_file.c_str(), wxConvUTF8));
          inf.MakeRelativeTo(  m_working_directory.GetFullPath() );

          fgs->Add(new wxStaticText(top, wxID_ANY,inf.GetFullPath()), wxGBPosition(row,1),
              wxGBSpan(1,2));

        } else {

          string t = "input " + *s + " file:";

          fgs->Add(new wxStaticText(top, wxID_ANY, wxString(t.c_str() , wxConvUTF8) ) , wxGBPosition(row,0));

          m_additional_input_files.push_back( new wxFilePickerCtrl(top, wxID_ANY,
              wxT(""), wxT("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize,
              wxFLP_USE_TEXTCTRL | wxFLP_OPEN )
          );

          fgs->Add( m_additional_input_files.back() , wxGBPosition(row,1) );

        }
        ++row;
      }

      suggested_output_file = NULL;

      wxString filesuggestion =wxEmptyString;

      if (!m_tool.m_output_type.empty())
      {
        fgs->Add(new wxStaticText(top, wxID_ANY, wxT("output file:")),
                 wxGBPosition(row, 0));

        if (m_tool.m_output_type.compare("txt") != 0)
        {
          filesuggestion = GenerateOutputFileSuggestion();
        }

        suggested_output_file = new wxFilePickerCtrl(top, ID_OUTPUT_FILE,
            wxT(""), wxT("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize,
            wxFLP_USE_TEXTCTRL | wxFLP_SAVE | wxFLP_OVERWRITE_PROMPT);

        if (m_tool.m_output_type.compare("txt") == 0)
        {
          suggested_output_file->SetToolTip(wxT("Leave blank if the output should be written to screen."))  ;
        }

        wxFileName ouf = wxFileName( filesuggestion );
        ouf.MakeRelativeTo(  m_working_directory.GetFullPath() );

        suggested_output_file->SetPath( ouf.GetFullPath() );
        suggested_output_file->SetMinSize(wxSize(350,30));
        suggested_output_file->SetTextCtrlProportion(6);

        fgs->Add(suggested_output_file , wxGBPosition(row, 1), wxGBSpan(1,2));

        m_fileIO.output_file = filesuggestion.mb_str(wxConvUTF8);

        // Display different output formats if there are more than one
        if (tool.m_extentions.size() > 1)
        {
          ++row;
          int select = 0;
          wxArrayString sa;
          for (std::vector<std::string>::iterator it = tool.m_extentions.begin();
               it != tool.m_extentions.end();
               ++it)
          {
            sa.Add(wxString(it->c_str() , wxConvUTF8));

            if (it->compare(m_tool.m_output_type) == 0)
            {
              select = std::distance(tool.m_extentions.begin() , it);
            }

          }

          output_ext = new wxRadioBox(top, ID_OUTPUT_EXT, wxString(
                                        "format", wxConvUTF8), wxDefaultPosition, wxDefaultSize, sa
                                      ,0, wxRA_SPECIFY_COLS
                                     );

          output_ext->SetSelection(select);

          fgs->Add(output_ext , wxGBPosition(row, 1), wxGBSpan(1,2));
        }



      }

      //fgs->Add(new wxStaticLine(top,wxID_ANY, wxDefaultPosition, wxSize(800,1)), wxGBPosition(row,0), wxGBSpan(1,3));
      /* Display RUN & ABORT button */
      m_runbutton = new wxButton(top, ID_RUN_TOOL, wxT("Run"));
      m_abortbutton = new wxButton(top, wxID_ABORT, wxT("Abort"));

      ++row;
      fgs->Add(m_runbutton, wxGBPosition(row,0));
      fgs->Add(m_abortbutton, wxGBPosition(row,1));
      ++row;
      fgs->Add(new wxStaticLine(top,wxID_ANY, wxDefaultPosition, wxSize(800,1)), wxGBPosition(row,0), wxGBSpan(1,3));
      ++row;

      /* Parse and display options */
      vector<Tool_option> vto = tool.m_tool_options;

      wxRadioBox* rb;
      wxStaticText* ws, *ws1, *ws2;
      wxArrayString as;
      wxCheckBox* cb;
      wxTextCtrl* tc;
      wxFilePickerCtrl* fp;


      for (vector<Tool_option>::iterator i = vto.begin(); i != vto.end(); ++i)
      {
        //TODO: extend with Optional/mandatory checkbox

        switch ((*i).m_widget)
        {
          case none:
            break;
          case checkbox:

            cb = new wxCheckBox(top, wxID_ANY, wxString(
                                  (*i).m_flag.c_str(), wxConvUTF8));

            if ((*i).m_values[(*i).m_default_value].compare("true") == 0)
            {
              cb->SetValue(true);
            }

            m_checkbox_ptrs.push_back(cb);

            /* Display help text */
            ws = new wxStaticText(top, wxID_ANY, wxString(
                                    (*i).m_help.c_str(), wxConvUTF8));

            ws->Wrap(800);

            row++;
            fgs->Add(cb, wxGBPosition(row,0));
            fgs->Add(ws, wxGBPosition(row,1), wxGBSpan(1,2));

            break;
          case radiobox:

            /* Prepare radio values */
            as.Clear();
            for (vector<string>::iterator j = (*i).m_values.begin(); j
                 != (*i).m_values.end(); ++j)
            {
              as.Add(wxString((*j).c_str(), wxConvUTF8));
            }

            /* create radio box */
            rb = new wxRadioBox(top, wxID_ANY, wxString(
                                  (*i).m_flag.c_str(), wxConvUTF8), wxDefaultPosition, wxDefaultSize, as
                                ,0, wxRA_SPECIFY_ROWS
                               );

            m_radiobox_ptrs.push_back(rb);

            /* Set default value */
            rb->SetStringSelection(wxString((*i).m_values[(*i).m_default_value].c_str(),wxConvUTF8));

            /* Display help text */
            ws = new wxStaticText(top, wxID_ANY, wxString((*i).m_help.c_str(), wxConvUTF8));

            ws->Wrap(800);

            row++;
            fgs->Add(rb, wxGBPosition(row,0));
            fgs->Add(ws, wxGBPosition(row,1), wxGBSpan(1,2));

            break;
          case textctrl:
            /* display label */

            /* create text input box */
            tc = new wxTextCtrl(top, wxID_ANY, wxT(""));

            tc->SetName(wxString(
                          (*i).m_flag.c_str(), wxConvUTF8));

            tc->SetValue(wxT(""));

            m_textctrl_ptrs.push_back(tc);

            row++;
            fgs->Add(new wxStaticText(top, wxID_ANY, wxString(
                                        (*i).m_flag.c_str(), wxConvUTF8)),
                     wxGBPosition(row,0));
            fgs->Add(tc, wxGBPosition(row,1));
            fgs->Add(new wxStaticText(top, wxID_ANY, wxString(
                                        (*i).m_help.c_str(), wxConvUTF8)),
                     wxGBPosition(row,2));

            break;

          case filepicker:
            /* display label */
            ws1 = new wxStaticText(top, wxID_ANY, wxString(
                                     (*i).m_flag.c_str(), wxConvUTF8));

            /* create text input box */
            fp = new wxFilePickerCtrl(top, wxID_ANY,  wxT(""),
                                      wxT("Select a file"), wxString((*i).m_values[0].c_str(), wxConvUTF8), wxDefaultPosition, wxDefaultSize,
                                      wxFLP_USE_TEXTCTRL | wxFLP_OPEN);

            fp->SetLabel(wxString(
                           (*i).m_flag.c_str(), wxConvUTF8));

            m_filepicker_ptrs.push_back(fp);

            /* Remove trailing empty lines */
            string str = (*i).m_help;
            while (str.find_last_of("\n") == (str.size()-1))
            {
              str = str.substr(0, str.size()-1);
            }
            ws2 = new wxStaticText(top, wxID_ANY, wxString(
                                     str.c_str(), wxConvUTF8));

            row++;
            fgs->Add(ws1, wxGBPosition(row,0));
            fgs->Add(ws2,  wxGBPosition(row,1));

            row++;
            fgs->Add(fp,  wxGBPosition(row,1));
            fp->SetMinSize(wxSize(350,25));
            fp->SetTextCtrlProportion(6);

            break;
        }
      };


      hbox->Add(fgs, 1, wxALL, 15);
      top->SetSizer(hbox);
      top->Layout();

      /* Hide after layout to prevent widget overlap*/
      m_abortbutton->Show(false);

      int w, h;
      top->GetBestSize(&w,&h);
      m_wsw->SetScrollbars(20, 20, w/20 , h/20);

      m_configpanel->AddPage(m_wsw , wxT("Configuration"), true);
      m_configpanel->AddPage(m_tool_cerr_output  , wxT("Logging"), false);


      if (m_tool.m_output_type.compare("txt") == 0)
      {
        m_tool_std_output = new OutputConfigPanel(m_configpanel, wxID_ANY, wxDefaultPosition, wxDefaultSize);
        m_tool_std_output->SetRunCognizance(this);

        m_configpanel->AddPage( m_tool_std_output , wxT("Output"), false);
      }
    }
    ;

    void UpdateToolTipStatus(ToolStatus s)
    {

      //enum ToolStatus { STATUS_NONE, STATUS_RUNNING, STATUS_COMPLETE, STATUS_FAILED};

      /* Remove text after last last '[' */
      wxString toolTipText = m_parent->GetPageText(m_parent->GetPageIndex(this));

      if (!toolTipText.BeforeFirst(_T(' ')).empty())
      {
        toolTipText = toolTipText.BeforeFirst(_T(' '));
      }

      switch (s)
      {
        case STATUS_NONE:
          break;
        case STATUS_RUNNING:
          toolTipText = toolTipText.Append(wxT(" [Running]"));
          break;
        case STATUS_COMPLETE:
          toolTipText = toolTipText.Append(wxT(" [Done]"));
          break;
        case STATUS_FAILED:
          toolTipText = toolTipText.Append(wxT(" [Failed]"));
          break;
      }

      m_parent->SetPageText(m_parent->GetPageIndex(this), toolTipText);


    }

    void Run()
    {
      UpdateToolTipStatus(STATUS_RUNNING);

      m_tool_cerr_output->GetOutput()->Clear();
      if( m_tool_std_output )
      {
        m_tool_std_output->GetOutput()->Clear();
      }

      wxString cmd = wxString(m_tool.m_location.c_str(), wxConvUTF8);

      wxString run = cmd;
      for (vector<wxRadioBox*>::iterator i = m_radiobox_ptrs.begin(); i
           != m_radiobox_ptrs.end(); ++i)
      {
        run = run + wxT(" --") + (*i)->GetLabel() + wxT("=")
              + (*i)->GetStringSelection();
      }

      for (vector<wxCheckBox*>::iterator i = m_checkbox_ptrs.begin(); i
           != m_checkbox_ptrs.end(); ++i)
      {
        if ((*i)->GetValue())
        {
          run = run + wxT(" --") + (*i)->GetLabel();
        }
      }

      for (vector<wxTextCtrl*>::iterator i = m_textctrl_ptrs.begin(); i
           != m_textctrl_ptrs.end(); ++i)
      {
        if (!(*i)->GetValue().IsEmpty())
        {
          run = run + wxT(" --") + (*i)->GetName() + wxT("=") +(*i)->GetValue();
        }
      }


      std::vector<wxString> absolutefilepickerpaths;

      /* Conversion of relative filepaths to absolute filepaths */
      for (vector<wxFilePickerCtrl*>::iterator i = m_filepicker_ptrs.begin();
          i != m_filepicker_ptrs.end(); ++i)
      {
        if (!(*i)->GetPath().IsEmpty())
        {

          /*Normalize absolute filepath*/
          wxFileName fn;
          fn = wxFileName((*i)->GetPath());

          if(!fn.IsAbsolute())
          {
            /* Relative to Absolute Path */
            fn = wxFileName(m_working_directory.GetFullPath() + wxFileName::GetPathSeparator() + (*i)->GetPath());
          }

          /*Normalize absolute filepath*/
          if(!fn.MakeAbsolute())
          {
            wxMessageDialog *dial = new wxMessageDialog(NULL,
               wxString("Cannot normalize \"")+ (*i)->GetPath() + wxString("\" for argument \"")+ (*i)->GetLabel() + wxT("\".") , wxT("Error"), wxOK | wxICON_ERROR);
            dial->ShowModal();
            return;
          }

          absolutefilepickerpaths.push_back( StringSpaceEscape(fn.GetFullPath()) );

        }
      }


      for (vector<wxFilePickerCtrl*>::iterator i = m_filepicker_ptrs.begin();
          i != m_filepicker_ptrs.end(); ++i)
      {
        if (!(*i)->GetPath().IsEmpty())
        {

          run = run + wxT(" --") + (*i)->GetLabel() + wxT("=") + absolutefilepickerpaths.front();
          absolutefilepickerpaths.erase( absolutefilepickerpaths.begin() );

        }
      }

      // Append input file
      run.Append( wxT(" ") );
      run.Append( StringSpaceEscape( wxString(m_fileIO.input_file.c_str(), wxConvUTF8) ) );

      // Append additional input files (if any)
      if (!m_additional_input_files.empty())
      {
        for(vector<wxFilePickerCtrl*>::iterator i = m_additional_input_files.begin()
            ; i != m_additional_input_files.end()
            ; ++i ){
          run.Append(wxT(" "));

          wxFileName inf = wxFileName( (*i)->GetPath() );

          if(!inf.IsAbsolute())
          {
            inf = wxFileName( m_working_directory.GetFullPath() + wxFileName::GetPathSeparator() + inf.GetFullPath() );
          }

          if(inf.MakeAbsolute())
          {
            run.Append( StringSpaceEscape( inf.GetFullPath() ) );
          } else {
            wxMessageDialog *dial = new wxMessageDialog(NULL,
               wxString("Could not normalize input file \"")+ inf.GetFullPath() + wxT("\" .") , wxT("Error"), wxOK | wxICON_ERROR);
            dial->ShowModal();
            return;
          }
        }
      }

      // Append output file (if any)

      if( suggested_output_file )
      {
        wxString output_file = suggested_output_file->GetPath();
  
        if ( !output_file.IsEmpty() )
        {
          run.Append(wxT(" "));

          wxFileName ouf = wxFileName( output_file );

          if(!ouf.IsAbsolute())
          {
            ouf = wxFileName( m_working_directory.GetFullPath() + wxFileName::GetPathSeparator() + ouf.GetFullPath() );
          }

          if(ouf.MakeAbsolute())
          {
            run.Append( StringSpaceEscape( ouf.GetFullPath() ) );
          } else {
            wxMessageDialog *dial = new wxMessageDialog(NULL,
               wxString("Could not normalize output file \"")+ ouf.GetFullPath() + wxT("\" .") , wxT("Error"), wxOK | wxICON_ERROR);
            dial->ShowModal();
            return;
          }
        }
      }

      m_listbox_output->AppendText(run + wxTextFile::GetEOL());

      m_process = new MyPipedProcess(this);

      // Gui tools should be visible:
      // Tools redirect the standard input and/or output of the process being launched by calling Redirect.
      // For these child processes IO is redirected. For Windows these process windows are not shown by default.
      // To avoid that GUI tools are not shown a wxEXEC_NOHIDE flag is used to flag that the child process window
      // are shown normally.

      switch (m_tool.m_tool_type)
      {
        case shell:
          m_pid = wxExecute(run, wxEXEC_ASYNC, m_process);
          break;
        case gui:
          m_pid = wxExecute(run, wxEXEC_ASYNC | wxEXEC_NOHIDE , m_process);
          break;
        case ishell:
          m_pid = wxShell(run);
          break;
      }

      if ((m_tool.m_tool_type == shell) || (m_tool.m_tool_type == gui))
      {
        if (!m_pid)
        {
          wxLogError(wxT("Execution of '%s' failed."), run.c_str());
          m_pid = 0;
          delete m_process;
          UpdateToolTipStatus(STATUS_FAILED);
        }
        else
        {

          if( m_tool_std_output )
          {
            m_process->AddAsyncProcessWithOutput(m_tool_cerr_output->GetOutput(), m_tool_std_output->GetOutput());
          }
          else
          {
            m_process->AddAsyncProcessWithOutput(m_tool_cerr_output->GetOutput());
          }
          m_runbutton->Disable();
          m_abortbutton->Show(true);

          m_process->m_ext_pid = m_pid;

          m_switchOnCerrOutput = true;
          m_switchOnStdOutput = true;

        };
      }

    };

    wxString StringSpaceEscape( wxString s )
    {
#ifdef _WIN32
      s.Prepend(wxT("\""));
      s.Append(wxT("\""));
#else
      s.Replace(wxT(" "),wxT("\\ "));
#endif
      return s;
    }

    void SwitchToToolCerrOutputNotebook()
    {

      if (m_switchOnCerrOutput)
      {
        m_configpanel->SetSelection(1);
        m_switchOnCerrOutput = false;
      }
    }

    void SwitchToToolStdOutputNotebook()
    {

      if (m_switchOnStdOutput)
      {
        if( m_tool_std_output )
        {
          m_configpanel->SetSelection(2);
        }
        else
        {
          m_configpanel->SetSelection(1);
        }
        m_switchOnStdOutput = false;
        m_switchOnCerrOutput = false;
      }
    }

    void OnRunClick(wxCommandEvent& /*event*/)
    {
      m_tool_cerr_output->SetBackgroundColour( wxT("MEDIUM GOLDENROD") ) ;
      if( m_tool_std_output )
      {
        m_tool_std_output->SetBackgroundColour( wxT("MEDIUM GOLDENROD") ) ;
      }

      Run();
    }
    ;

    void OnAbortClick(wxCommandEvent& /*event*/)
    {
      if (wxProcess::Exists(m_pid) && (m_pid != 0))
      {
        wxProcess::Kill(m_pid, wxSIGKILL);
      }
      m_abortbutton->Show(false);
      m_runbutton->Enable();
      m_tool_cerr_output->ResetColor();
      if( m_tool_std_output )
      {
        m_tool_std_output->ResetColor();
      }
    };

    void OnOutputFileChange(wxFileDirPickerEvent& event)
    {
      m_fileIO.output_file = event.GetPath().mb_str(wxConvUTF8);
    };

    virtual bool Destroy()
    {

      /* Implement Veto */
      //Killing m_pid == 0 can terminate application
      if (wxProcess::Exists(m_pid) && (m_pid != 0))
      {
        wxProcess::Kill(m_pid, wxSIGKILL);
      }
      return true;
    }
    ;

    void OnResize(wxSizeEvent& /*event*/)
    {
      m_configpanel->SetSize(this->GetSize());
    }

    wxAuiNotebook* m_parent;
    wxAuiNotebook* m_configpanel;
    OutPutTextCtrlBase* m_listbox_output;
    OutputConfigPanel *m_tool_std_output;
    OutputConfigPanel* m_tool_cerr_output;
    wxScrolledWindow* m_wsw;
    wxString m_input_file;
    Tool m_tool;

    FileIO m_fileIO;

    wxFileName m_working_directory;

    vector<wxFilePickerCtrl*> m_additional_input_files;
    vector<wxRadioBox*> m_radiobox_ptrs;
    vector<wxCheckBox*> m_checkbox_ptrs;
    vector<wxTextCtrl*> m_textctrl_ptrs;
    vector<wxFilePickerCtrl*> m_filepicker_ptrs;

    wxButton* m_runbutton;
    wxButton* m_abortbutton;

    MyPipedProcess* m_process;

    wxFilePickerCtrl* suggested_output_file;

    wxRadioBox* output_ext;

    long m_pid;

  private:

    bool m_switchOnCerrOutput;
    bool m_switchOnStdOutput;

    wxString
    GenerateOutputFileSuggestion()
    {
      /* Suggest/Generate output file */

      size_t found;
      string file_suggestion = m_fileIO.input_file;
      found = file_suggestion.find_last_of(".");
      file_suggestion = file_suggestion.substr(0, found) + "." + m_tool.m_name;

      struct stat stFileInfo;
      int intStat = 0;
      int cntr = 0;

      char c[5];
      while (intStat == 0)
      {
        sprintf(c, "%02d", cntr);
        intStat = stat(
                    (file_suggestion + c + "." + m_tool.m_output_type).c_str(),
                    &stFileInfo);

        size_t n = m_parent->GetPageCount();
        if (intStat != 0)
        {
          for (size_t i = 0; i < n ; ++i)
          {
            if (((ConfigPanel*) m_parent->GetPage(i))->suggested_output_file != NULL)
            {
              string suggestion = file_suggestion + c + "." + m_tool.m_output_type;
              if ((((ConfigPanel*) m_parent->GetPage(i))->suggested_output_file->GetPath()) != wxEmptyString)
              {
                intStat = suggestion.compare(((ConfigPanel*) m_parent->GetPage(i))->suggested_output_file->GetPath().mb_str());
              }
            }
          }
        }
        ++cntr;
      }

      file_suggestion = file_suggestion + c + "." + m_tool.m_output_type;
      return wxString(file_suggestion.c_str(), wxConvUTF8);
    }

    void
    OnProcessEnd(wxCommandEvent& evt)
    {
      m_abortbutton->Show(false);
      m_runbutton->Enable();

      int status = evt.GetInt();

      if (status)
      {
        m_tool_cerr_output->SetBackgroundColour( wxT("SALMON") );
        if( m_tool_std_output )
        {
          m_tool_std_output->SetBackgroundColour( wxT("SALMON") );
        }
      } else {
        m_tool_cerr_output->SetBackgroundColour( wxT("PALE GREEN") ) ;
        if( m_tool_std_output )
        {
          m_tool_std_output->SetBackgroundColour( wxT("PALE GREEN") ) ;
        }
      }

      wxCommandEvent eventCustom(wxEVT_UPDATE_PROJECT_TREE);
      /* Notify parents to expand to the created file*/

      if (!m_fileIO.output_file.empty())
      {
        wxStringClientData* scd = new wxStringClientData(wxString(
              m_fileIO.output_file.c_str(), wxConvUTF8));
        eventCustom.SetClientData(scd);
      }
      else
      {
        eventCustom.SetClientData(NULL);
      }

      wxPostEvent(m_parent, eventCustom);

      UpdateToolTipStatus(STATUS_COMPLETE);
    }

    void OnToolHasCerrOutput(wxCommandEvent& /* evt */)
    {
      SwitchToToolCerrOutputNotebook();
    }

    void OnToolHasStdOutput(wxCommandEvent& /* evt */)
    {
      SwitchToToolStdOutputNotebook();
    }


    void OnChangeExtension(wxCommandEvent&)
    {

      // Change extension
      wxFileName file = wxFileName(suggested_output_file->GetTextCtrlValue());
      file.ClearExt();
      file.SetExt(output_ext->GetStringSelection());
      suggested_output_file->SetPath(file.GetFullPath());
    }

    DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(ConfigPanel, wxNotebookPage)
  EVT_BUTTON(ID_RUN_TOOL, ConfigPanel::OnRunClick)
  EVT_BUTTON(wxID_ABORT, ConfigPanel::OnAbortClick)
  EVT_FILEPICKER_CHANGED(ID_OUTPUT_FILE, ConfigPanel::OnOutputFileChange)
  EVT_SIZE(ConfigPanel::OnResize)
  EVT_MY_PROCESS_END(wxID_ANY, ConfigPanel::OnProcessEnd)
  EVT_MY_PROCESS_RUN(wxID_ANY, ConfigPanel::OnRunClick)
  EVT_MY_PROCESS_PRODUCES_CERR_OUTPUT(wxID_ANY, ConfigPanel::OnToolHasCerrOutput)
  EVT_MY_PROCESS_PRODUCES_STD_OUTPUT(wxID_ANY, ConfigPanel::OnToolHasStdOutput)
  EVT_RADIOBOX(ID_OUTPUT_EXT, ConfigPanel::OnChangeExtension)
END_EVENT_TABLE()

#endif /* MCRL2_GUI_CONFIGPANEL_H_ */
