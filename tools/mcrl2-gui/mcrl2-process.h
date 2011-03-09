/*
 * process.h
 *
 *  Created on: Jun 6, 2010
 *      Author: fstapper
 */

#ifndef MCRL2_GUI_PROCESS_H_
#define MCRL2_GUI_PROCESS_H_

#include <wx/process.h>
#include <wx/txtstrm.h>
#include "wx/defs.h"
#include <wx/thread.h>
#include <wx/listbox.h>

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(wxEVT_MY_PROCESS_END, 7777)
DECLARE_EVENT_TYPE(wxEVT_MY_PROCESS_RUN, 7777)
DECLARE_EVENT_TYPE(wxEVT_MY_PROCESS_PRODUCES_CERR_OUTPUT, 7777)
DECLARE_EVENT_TYPE(wxEVT_MY_PROCESS_PRODUCES_STD_OUTPUT, 7777)
END_DECLARE_EVENT_TYPES()


DEFINE_EVENT_TYPE(wxEVT_MY_PROCESS_END)

// it may also be convenient to define an event table macro for this event type
#define EVT_MY_PROCESS_END(id, fn) \
  DECLARE_EVENT_TABLE_ENTRY( \
                             wxEVT_MY_PROCESS_END, id, wxID_ANY, \
                             (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
                             (wxObject *) NULL \
                           ),

DEFINE_EVENT_TYPE(wxEVT_MY_PROCESS_RUN)

#define EVT_MY_PROCESS_RUN(id, fn) \
  DECLARE_EVENT_TABLE_ENTRY( \
                             wxEVT_MY_PROCESS_RUN, id, wxID_ANY, \
                             (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
                             (wxObject *) NULL \
                           ),

DEFINE_EVENT_TYPE(wxEVT_MY_PROCESS_PRODUCES_CERR_OUTPUT)

#define EVT_MY_PROCESS_PRODUCES_CERR_OUTPUT(id, fn) \
  DECLARE_EVENT_TABLE_ENTRY( \
                             wxEVT_MY_PROCESS_PRODUCES_CERR_OUTPUT, id, wxID_ANY, \
                             (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
                             (wxObject *) NULL \
                           ),

DEFINE_EVENT_TYPE(wxEVT_MY_PROCESS_PRODUCES_STD_OUTPUT)

#define EVT_MY_PROCESS_PRODUCES_STD_OUTPUT(id, fn) \
  DECLARE_EVENT_TABLE_ENTRY( \
                             wxEVT_MY_PROCESS_PRODUCES_STD_OUTPUT, id, wxID_ANY, \
                             (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
                             (wxObject *) NULL \
                           ),



class MyPipedProcess;
WX_DEFINE_ARRAY_PTR(MyPipedProcess*, MyActiveProcessArray)
;
MyActiveProcessArray running_processes;

class MyProcess: public wxProcess
{
  public:
    MyProcess(wxProcess* parent) :
      wxProcess(parent)
    {
      //m_parent = parent;
    }

    virtual void OnTerminate(int /* pid */, int /* status */)
    {
      // Delete object
      delete this;
    }
    ;
};

// A Process for redirecting the output
class MyPipedProcess: public MyProcess
{
  public:

    long m_ext_pid;

    wxWindow* m_parent;

    MyPipedProcess(wxWindow* parent) :
      MyProcess(NULL)
    {
      Redirect();
      m_parent = parent;
    }

    void AddAsyncProcessWithOutput(wxTextCtrl* cerr, wxTextCtrl* cout = NULL)
    {
      // Set cerr- and cout-TextCtrl to which process needs to report
      m_cerr_output = cerr;

      if( cout == NULL )
      {
        m_cout_output = cerr;
      }
      else
      {
        m_cout_output = cout;
      }

      running_processes.Add(this);
    }

    virtual void OnTerminate(int pid, int status)
    {
      // show output remainder
      while (HasInput())
      {
        ;
      }

      running_processes.Remove(this);

      MyProcess::OnTerminate(pid, status);

      wxCommandEvent eventCustom(wxEVT_MY_PROCESS_END);
      eventCustom.SetInt( status );
      wxPostEvent(m_parent, eventCustom);


    }
    ;

    virtual bool HasInput()
    {
      bool hasInput = false;

      if (IsInputAvailable())
      {
        wxTextInputStream tis(*GetInputStream());
        wxString m_msg;
        // assumption output is line buffered
        // m_msg << m_cmd << wxT(" (stdout): ") << tis.ReadLine();
        m_msg << tis.ReadLine();
        if (m_cout_output != NULL)
        {
          int old = m_cout_output->GetNumberOfLines();

          m_cout_output->AppendText(m_msg + wxT("\n"));

          if (m_cout_output == wxWindow::FindFocus())
          {
            // AutoScroll
            //m_listbox_output->Select(m_listbox_output->GetCount() - 1);
            //m_listbox_output->SetSelection(wxNOT_FOUND);
          }

          if (old <  m_cout_output->GetNumberOfLines() && (!m_msg.empty()))
          {
            wxCommandEvent eventCustom(wxEVT_MY_PROCESS_PRODUCES_STD_OUTPUT);
            wxPostEvent(m_parent, eventCustom);
          }

        }
        m_msg.Clear();
        hasInput = true;
      }

      if (IsErrorAvailable())
      {
        wxTextInputStream tis(*GetErrorStream());
        wxString m_msg;
        // assumption output is line buffered
        m_msg << tis.ReadLine();
        if (m_cerr_output != NULL)
        {

          int old = m_cerr_output->GetNumberOfLines();

          m_cerr_output->AppendText(m_msg + wxT("\n"));
          if (m_cerr_output == wxWindow::FindFocus())
          {
            // AutoScroll
            //        m_listbox_output->Select(m_listbox_output->GetCount() - 1);
            //        m_listbox_output->SetSelection(wxNOT_FOUND);
          }

          if (old <  m_cerr_output->GetNumberOfLines() && (!m_msg.empty()))
          {
            wxCommandEvent eventCustom(wxEVT_MY_PROCESS_PRODUCES_CERR_OUTPUT);
            wxPostEvent(m_parent, eventCustom);
          }
        }

        m_msg.Clear();

        hasInput = true;
      }

      return hasInput;
    }
    ;
  protected:
    wxTextCtrl* m_cerr_output;
    wxTextCtrl* m_cout_output;

};


#endif /* MCRL2_GUI_PROCESS_H_ */
