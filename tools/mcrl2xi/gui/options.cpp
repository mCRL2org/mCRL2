// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file options.h

#include "options.h"
#include "mcrl2/process/parse.h"
#include "mainframe.h"
#include <wx/textfile.h>

#include "mcrl2/data/classic_enumerator.h"
#include "mcrl2/data/enumerator_factory.h"



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

Options::Options(wxWindow *parent, wxWindowID id, xEditor *editor, outputpanel *output, mcrl2::data::rewriter::strategy rewrite_strategy) :
    wxPanel( parent , id, wxDefaultPosition, wxSize(250,-1) )
  {
    p_output = output;
    p_editor = editor;

    m_rewrite_strategy = rewrite_strategy;

    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    wxGridBagSizer *fgs = new wxGridBagSizer(5, 5);

    tc = new typeCheckSpec(this, wxID_ANY);
    fgs->Add( tc, wxGBPosition(0,0) );

    ev = new evalDataExpr(this, wxID_ANY);
    fgs->Add( ev, wxGBPosition(1,0) );

    sd = new solveDataExpr(this, wxID_ANY);
    fgs->Add( sd, wxGBPosition(2,0) );
    sizer->Add(fgs, 1, wxALL, 10);

    SetSizer(sizer);
    this->Layout();

  };

  void Options::OnEval(wxCommandEvent& /*event*/) {
    p_output->Clear();
    wxStreamToTextRedirector redirect(p_output);
    try{
      std::cout << p_output->PrintTime() << "Evaluate: \"" << ev->getDataExprVal().mb_str() << "\""<< std::endl;
      std::cout << p_output->PrintTime() <<"Parsing and type checking specification" << std::endl;
      wxString wx_spec = p_editor->GetStringFromDataEditor();
      mcrl2::process::process_specification spec = mcrl2::process::parse_process_specification( std::string(wx_spec.mb_str() ));

      std::cout << p_output->PrintTime() <<"Parsing data expression:\"" << ev->getDataExprVal().mb_str() << "\""<< std::endl;
      mcrl2::data::data_expression term = mcrl2::data::parse_data_expression( std::string(ev->getDataExprVal().mb_str()) ,spec.data() );

      std::cout << p_output->PrintTime() <<"Rewriting data expression:\"" << ev->getDataExprVal().mb_str() << "\""<< std::endl;
      mcrl2::data::rewriter rewr(spec.data(),m_rewrite_strategy);
      atermpp::map < mcrl2::data::variable, mcrl2::data::data_expression > assignments;
      std::cout << p_output->PrintTime() <<"Result: " << pp(rewr(term,make_map_substitution_adapter(assignments))) << std::endl;

    } catch ( mcrl2::runtime_error e) {
      std::cout << p_output->PrintTime() << e.what() <<std::endl;
    }
  };

  void Options::OnTypeCheck(wxCommandEvent& /*event*/) {
    p_output->Clear();
    wxStreamToTextRedirector redirect(p_output);
    try{
      std::cout << p_output->PrintTime() <<"Parsing and type check current specification" << std::endl;
      wxString wx_spec = p_editor->GetStringFromDataEditor();
      mcrl2::process::process_specification spec = mcrl2::process::parse_process_specification( std::string(wx_spec.mb_str() ));
      std::cout << p_output->PrintTime() <<"Specification is valid" << std::endl;

    } catch ( mcrl2::runtime_error e) {
      std::cout << p_output->PrintTime() << e.what() <<std::endl;
    }
  };

  void Options::SolveExpr(wxCommandEvent& e) {
    p_output->Clear();

    wxString dataexpr = sd->getDataExprSolve();

    wxTextCtrl *p_solutions = sd->getSolutionWindow();
    wxStreamToTextRedirector redirect(p_solutions);
    p_solutions->Clear();
    try{
      p_output->AppendText(wxString(p_output->PrintTime().c_str(), wxConvUTF8) + wxT( "Solving: \"") + dataexpr + wxT("\"") + wxTextFile::GetEOL() );

      int dotpos = dataexpr.Find('.');
      if( dotpos  == -1){
        throw mcrl2::runtime_error( p_output->PrintTime() + "Expect a `.' in the input.");
      }

      atermpp::set <mcrl2::data::variable> vars;
      wxString wx_spec = p_editor->GetStringFromDataEditor();
      mcrl2::process::process_specification spec = mcrl2::process::parse_process_specification( std::string(wx_spec.mb_str() ));

      parse_variables(std::string(dataexpr.BeforeFirst('.').mb_str()) + ";",std::inserter(vars,vars.begin()),spec.data());

      mcrl2::data::data_expression term =
          mcrl2::data::parse_data_expression(
              std::string(dataexpr.AfterFirst('.').mb_str()),
              vars.begin(), vars.end(),
              spec.data()
              );
      if ( term.sort()!=mcrl2::data::sort_bool::bool_())
      { throw mcrl2::runtime_error(p_output->PrintTime()+"Expression is not of sort Bool.");
      }

      mcrl2::data::rewriter rewr(spec.data(),m_rewrite_strategy);
      term=rewr(term);

      mcrl2::data::enumerator_factory < mcrl2::data::classic_enumerator<> > e(spec.data(),rewr);
      for (mcrl2::data::classic_enumerator< > i =
           e.make(atermpp::convert < std::set <mcrl2::data::variable > >(vars),rewr,term);
                                                    i != mcrl2::data::classic_enumerator<>() ; ++i)
      {
        std::cout << "[";
        for ( atermpp::set< mcrl2::data::variable >::const_iterator v=vars.begin(); v!=vars.end() ; ++v )
        { std::cout << pp(*v) << " := " << pp((*i)(*v));
          if ( boost::next(v)!=vars.end() )
          { std::cout << ", ";
          }
        }
        std::cout << "] evaluates to "<< pp(rewr(term,*i)) << std::endl;
      }
      p_output->AppendText(wxString( std::string( p_output->PrintTime() + "Done solving.").c_str()  , wxConvUTF8) );

    } catch ( mcrl2::runtime_error e) {
      p_output->AppendText(wxString(  std::string(p_output->PrintTime() + e.what()).c_str(), wxConvUTF8 ));
    }

  };


  void Options::OnSize(wxSizeEvent& /*event*/){
    int w, h;
    this->GetSize(&w, &h);
    tc->SetSize( wxSize(w-20 , -1 ));
    ev->SetSize( wxSize(w-20 , -1 ));
    sd->SetSize( wxSize(w-20 , -1 ));
  };

