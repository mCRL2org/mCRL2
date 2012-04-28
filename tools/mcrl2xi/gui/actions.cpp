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
#include "mcrl2/data/parse.h"
#include "mainframe.h"
#include <wx/textfile.h>

#include "mcrl2/data/classic_enumerator.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cctype>

#include "boost/utility.hpp"
#include "boost/lexical_cast.hpp"

using namespace mcrl2::log;

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

static
bool parse_data_specification_with_variables(const std::string s, mcrl2::data::data_specification& data_spec, std::set<mcrl2::data::variable>& vars)
{
  log_level_t old_level = mcrl2_logger::get_reporting_level();
  std::streambuf *old = std::cerr.rdbuf();
  try
  {
    mcrl2_logger::set_reporting_level(quiet);

    // Dirty hack: redirect cerr such that is becomes silent and parse errors are ignored.

    std::stringstream ss;
    std::cerr.rdbuf (ss.rdbuf());
    data_spec = mcrl2::data::parse_data_specification(s);

    //Restore cerr such that parse errors become visible.
    std::cerr.rdbuf (old);
    mcrl2_logger::set_reporting_level(old_level);
  }
  catch(mcrl2::runtime_error& )
  {
    std::cerr.rdbuf (old);
    mcrl2_logger::set_reporting_level(old_level);
    mcrl2::process::process_specification spec;
    try
    {
       spec = mcrl2::process::parse_process_specification(s);
       data_spec = spec.data();
       vars = spec.global_variables();
    }
    catch(mcrl2::runtime_error& e)
    {
      mCRL2log(error) << e.what() << std::endl;
      return false;
    }
  }
  return true;
}

void Options::OnEval(wxCommandEvent& /*event*/)
{
  p_output->Clear();

  try
  {
    mCRL2log(info) << "Evaluate: \"" << ev->getDataExprVal().mb_str() << "\"" << std::endl;
    mCRL2log(info) << "Parsing and type checking specification" << std::endl;

    wxString wx_spec = p_editor->GetStringFromDataEditor();

    mcrl2::data::data_specification data_spec;
    std::set <mcrl2::data::variable > vars;

    if(!parse_data_specification_with_variables(std::string(wx_spec.mb_str()), data_spec, vars))
    {
      return;
    }

    mCRL2log(info) << "Parsing data expression: \"" << ev->getDataExprVal().mb_str() << "\"" << std::endl;

    mcrl2::data::data_expression term = mcrl2::data::parse_data_expression(std::string(ev->getDataExprVal().mb_str()),
        vars.begin(), vars.end(), data_spec);

    mCRL2log(info) << "Rewriting data expression: \"" << ev->getDataExprVal().mb_str() << "\"" << std::endl;

    mcrl2::data::rewriter rewr(data_spec,m_rewrite_strategy);
    mcrl2::data::mutable_map_substitution < atermpp::map < mcrl2::data::variable, mcrl2::data::data_expression > > assignments;

    mCRL2log(info) << "Result: \"" << pp(rewr(term,assignments)) << "\"" << std::endl;

  }
  catch (mcrl2::runtime_error e)
  {
    mCRL2log(error) << e.what() << std::endl;
  }
};

void Options::OnTypeCheck(wxCommandEvent& /*event*/)
{
  p_output->Clear();
  try
  {
    mCRL2log(info) << "Parsing and type checking data specification" << std::endl;
    wxString wx_spec = p_editor->GetStringFromDataEditor();
    mcrl2::data::data_specification spec = mcrl2::data::parse_data_specification(std::string(wx_spec.mb_str()));
    mCRL2log(info) << "Specification is a valid data specification" << std::endl;

  }
  catch (mcrl2::runtime_error e)
  {
    p_output->Clear();
    try
    {
      mCRL2log(info) << "Parsing and type checking mCRL2 specification" << std::endl;

      wxString wx_spec = p_editor->GetStringFromDataEditor();
      mcrl2::process::process_specification spec = mcrl2::process::parse_process_specification(std::string(wx_spec.mb_str()));
      mCRL2log(info) << "Specification is a valid mCRL2 specification" << std::endl;

    }
    catch (mcrl2::runtime_error e)
    {
      mCRL2log(error) << "Specification contains no valid data or mCRL2 specification." << std::endl;
      mCRL2log(error) << e.what() << std::endl;
    }
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
    mCRL2log(info) << "Solving: \"" << dataexpr.mb_str() << "\"" << std::endl;

    int dotpos = dataexpr.Find('.');
    if (dotpos  == -1)
    {
      throw mcrl2::runtime_error("Expect a `.' in the input.");
    }

    wxString wx_spec = p_editor->GetStringFromDataEditor();

    mcrl2::data::data_specification data_spec;
    std::set <mcrl2::data::variable > vars;

    if(!parse_data_specification_with_variables(std::string(wx_spec.mb_str()), data_spec, vars))
    {
      return;
    }

    parse_variables(std::string(dataexpr.BeforeFirst('.').mb_str()) + ";",std::inserter(vars,vars.begin()),data_spec);

    mcrl2::data::data_expression term =
      mcrl2::data::parse_data_expression(
        std::string(dataexpr.AfterFirst('.').mb_str()),
        vars.begin(), vars.end(),
        data_spec
      );
    if (term.sort()!=mcrl2::data::sort_bool::bool_())
    {
      throw mcrl2::runtime_error("Expression is not of sort Bool.");
    }

    rewriter rewr(data_spec,m_rewrite_strategy);
    term=rewr(term);

    typedef classic_enumerator< rewriter > enumerator_type;

    enumerator_type enumerator(data_spec,rewr);

    for (enumerator_type::iterator i = enumerator.begin(vars,term,10000); // Stop when more than 10000 internal variables are required.
                 i != enumerator.end() ; ++i)
    {
      p_solutions->AppendText(wxT("["));
      for (std::set< mcrl2::data::variable >::const_iterator v=vars.begin(); v!=vars.end() ; ++v)
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
        mCRL2log(info) << "Abort by user." << std::endl;
        break;
      }
      wxYield();
    }

    mCRL2log(info) << "Done solving." << std::endl;

  }
  catch (mcrl2::runtime_error e)
  {
    mCRL2log(error) << e.what() << std::endl;
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

