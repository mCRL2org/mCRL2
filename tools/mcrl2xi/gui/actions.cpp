// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file actions.cpp

#include "actions.h"
#include "mcrl2/process/parse.h"
#include "mainframe.h"
#include <wx/textfile.h>

#include "mcrl2/data/classic_enumerator.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cctype>

#include "boost/utility.hpp"
#include "boost/lexical_cast.hpp"

BEGIN_EVENT_TABLE(Options, wxPanel)
  EVT_BUTTON(OPTION_EVAL, Options::OnEval)
  EVT_SIZE(Options::OnSize)
END_EVENT_TABLE()

Options::Options(wxWindow* parent, wxWindowID id, xEditor* editor, outputpanel* output, mcrl2::data::rewriter::strategy rewrite_strategy) :
  wxPanel(parent , id, wxDefaultPosition, wxSize(250,-1))
{
  p_output = output;
  p_editor = editor;

  m_rewrite_strategy = rewrite_strategy;

  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  wxGridBagSizer* fgs = new wxGridBagSizer(5, 5);

  tc = new typeCheckSpec(this, wxID_ANY);
  fgs->Add(tc, wxGBPosition(0,0));

  ev = new evalDataExpr(this, wxID_ANY);
  fgs->Add(ev, wxGBPosition(1,0));

  sd = new solveDataExpr(this, wxID_ANY);
  fgs->Add(sd, wxGBPosition(2,0));
  sizer->Add(fgs, 1, wxALL, 10);

  SetSizer(sizer);
  this->Layout();

};

void Options::OnEval(wxCommandEvent& /*event*/)
{
  p_output->Clear();

  try
  {
    p_output->AppendText(wxString(p_output->PrintTime().c_str(), wxConvUTF8)
                         + wxT("Evaluate: \"")
                         + ev->getDataExprVal()
                         + wxT("\"")
                         + wxTextFile::GetEOL());

    p_output->AppendText(wxString(p_output->PrintTime().c_str(), wxConvUTF8)
                         + wxT("Parsing and type checking specification")
                         + wxTextFile::GetEOL());

    wxString wx_spec = p_editor->GetStringFromDataEditor();

    mcrl2::process::process_specification spec;
    try
    {
      spec = mcrl2::process::parse_process_specification(std::string(wx_spec.Append(wxT("init delta;")).mb_str()));
    }
    catch(mcrl2::runtime_error& e)
    {
      try
      {
         spec = mcrl2::process::parse_process_specification(std::string(wx_spec.mb_str()));
      }
      catch(mcrl2::runtime_error& e)
      {
        p_output->AppendText( wxString(e.what() , wxConvUTF8 ) );
        return;
      }
    }

    p_output->AppendText(wxString(p_output->PrintTime().c_str(), wxConvUTF8)
                         + wxT("Parsing data expression: \"")
                         + ev->getDataExprVal()
                         + wxT("\"")
                         + wxTextFile::GetEOL());

    atermpp::set <mcrl2::data::variable> vars = spec.global_variables();

    mcrl2::data::data_expression term = mcrl2::data::parse_data_expression(std::string(ev->getDataExprVal().mb_str()),
        vars.begin(), vars.end(), spec.data());

    p_output->AppendText(wxString(p_output->PrintTime().c_str(), wxConvUTF8)
                         + wxT("Rewriting data expression: \"")
                         + ev->getDataExprVal()
                         + wxT("\"")
                         + wxTextFile::GetEOL());

    mcrl2::data::rewriter rewr(spec.data(),m_rewrite_strategy);
    atermpp::map < mcrl2::data::variable, mcrl2::data::data_expression > assignments;

    p_output->AppendText(wxString(p_output->PrintTime().c_str(), wxConvUTF8)
                         + wxT("Result: \"")
                         + wxString(pp(rewr(term,make_map_substitution(assignments))).c_str(), wxConvUTF8)
                         + wxT("\"")
                         + wxTextFile::GetEOL());

  }
  catch (mcrl2::runtime_error e)
  {
    p_output->AppendText(wxString(p_output->PrintTime().c_str(), wxConvUTF8)
                         + wxString(e.what(), wxConvUTF8)
                         + wxTextFile::GetEOL());
  }
};

void Options::OnTypeCheck(wxCommandEvent& /*event*/)
{
  p_output->Clear();
  try
  {
    p_output->AppendText(wxString(p_output->PrintTime().c_str(), wxConvUTF8)
                         + wxT("Parsing and type checking specification")
                         + wxTextFile::GetEOL());

    wxString wx_spec = p_editor->GetStringFromDataEditor();
    mcrl2::process::process_specification spec = mcrl2::process::parse_process_specification(std::string(wx_spec.mb_str()));
    p_output->AppendText(wxString(p_output->PrintTime().c_str(), wxConvUTF8)
                         + wxT("Specification is valid")
                         + wxTextFile::GetEOL());

  }
  catch (mcrl2::runtime_error e)
  {
    p_output->AppendText(wxString(p_output->PrintTime().c_str(), wxConvUTF8)
                         + wxString(e.what(), wxConvUTF8)
                         + wxTextFile::GetEOL());
  }
};

void Options::SolveExpr(wxCommandEvent& /*e*/)
{
  using namespace mcrl2::data;
  p_output->Clear();

  wxString dataexpr = sd->getDataExprSolve();

  wxTextCtrl* p_solutions = sd->getSolutionWindow();
  p_solutions->Clear();
  try
  {
    p_output->AppendText(wxString(p_output->PrintTime().c_str(), wxConvUTF8) + wxT("Solving: \"") + dataexpr + wxT("\"") + wxTextFile::GetEOL());

    int dotpos = dataexpr.Find('.');
    if (dotpos  == -1)
    {
      throw mcrl2::runtime_error(p_output->PrintTime() + "Expect a `.' in the input.");
    }


    wxString wx_spec = p_editor->GetStringFromDataEditor();

    mcrl2::process::process_specification spec;
    try
    {
      spec = mcrl2::process::parse_process_specification(std::string(wx_spec.Append(wxT("init delta;")).mb_str()));
    }
    catch(mcrl2::runtime_error& e)
    {
      try
      {
         spec = mcrl2::process::parse_process_specification(std::string(wx_spec.mb_str()));
      }
      catch(mcrl2::runtime_error& e)
      {
        p_output->AppendText( wxString(e.what() , wxConvUTF8 ) );
        return;
      }
    }

    atermpp::set <mcrl2::data::variable> vars = spec.global_variables();
    parse_variables(std::string(dataexpr.BeforeFirst('.').mb_str()) + ";",std::inserter(vars,vars.begin()),spec.data());

    mcrl2::data::data_expression term =
      mcrl2::data::parse_data_expression(
        std::string(dataexpr.AfterFirst('.').mb_str()),
        vars.begin(), vars.end(),
        spec.data()
      );
    if (term.sort()!=mcrl2::data::sort_bool::bool_())
    {
      throw mcrl2::runtime_error(p_output->PrintTime()+"Expression is not of sort Bool.");
    }

    rewriter rewr(spec.data(),m_rewrite_strategy);
    term=rewr(term);

    typedef classic_enumerator< rewriter > enumerator_type;

    enumerator_type enumerator(spec.data(),rewr);

    for (enumerator_type::iterator i = enumerator.begin(vars,term,10000); // Stop when more than 10000 internal variables are required.
                 i != enumerator.end() ; ++i)
    {
      p_solutions->AppendText(wxT("["));
      for (atermpp::set< mcrl2::data::variable >::const_iterator v=vars.begin(); v!=vars.end() ; ++v)
      {
        p_solutions->AppendText(wxString(std::string(pp(*v)).c_str(), wxConvUTF8)
                                +  wxT(" := ")
                                +  wxString(std::string(pp((*i)(*v))).c_str(), wxConvUTF8));
        if (boost::next(v)!=vars.end())
        {
          p_solutions->AppendText(wxT(", "));
        }
      }
      p_solutions->AppendText(wxT("] evaluates to "));
      p_solutions->AppendText(wxString(std::string(pp(rewr(term,*i))).c_str(),wxConvUTF8));
      p_solutions->AppendText(wxTextFile::GetEOL());

      /* Repaint */
      wxPaintEvent evt;
      wxEvtHandler* eh = this->GetEventHandler();
      if (eh)
      {
        eh->ProcessEvent(evt);
      }

      if (sd->getStopSolving())
      {
        p_output->AppendText(wxString(std::string(p_output->PrintTime() + "Abort by user.").c_str()  , wxConvUTF8) +  wxTextFile::GetEOL());
        break;
      }
      wxYield();
    }
    p_output->AppendText(wxString(std::string(p_output->PrintTime() + "Done solving.").c_str()  , wxConvUTF8));

  }
  catch (mcrl2::runtime_error e)
  {
    p_output->AppendText(wxString(p_output->PrintTime().c_str(), wxConvUTF8)
                         + wxString(e.what(), wxConvUTF8)
                         + wxTextFile::GetEOL());
  }

};


void Options::OnSize(wxSizeEvent& /*event*/)
{
  int w, h;
  this->GetSize(&w, &h);
  tc->SetSize(wxSize(w-20 , -1));
  ev->SetSize(wxSize(w-20 , -1));
  sd->SetSize(wxSize(w-20 , -1));
};

