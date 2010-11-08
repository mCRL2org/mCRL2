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

BEGIN_EVENT_TABLE(Options, wxPanel)
EVT_BUTTON(OPTION_EVAL, Options::OnEval)
EVT_SIZE(Options::OnSize)
//EVT_UPDATE_EDITOR_FOCUS(wxID_ANY, Options::UpdateFocus)
END_EVENT_TABLE()

Options::Options(wxWindow *parent, wxWindowID id, xEditor *editor, wxTextCtrl *output, mcrl2::data::rewriter::strategy rewrite_strategy) :
    wxPanel( parent , id, wxDefaultPosition, wxSize(250,-1) )
  {
    p_output = output;
    p_editor = editor;

    m_rewrite_strategy = rewrite_strategy;

    int row = 0;

    wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);

    wxGridBagSizer *fgs = new wxGridBagSizer(5, 5);

    fgs->Add( new wxStaticText(this, wxID_ANY, wxT("Data expression:")) , wxGBPosition(row,0));
    row++;
    EvalExpr = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, -1));
    fgs->Add( EvalExpr , wxGBPosition(row,0));
    row++;
    fgs->Add( new wxButton(this, OPTION_EVAL, wxT("Evaluate")) , wxGBPosition(row,0));

    hbox->Add(fgs, 1, wxALL, 15);
    this->SetSizer(hbox);
    this->Layout();

    int w, h;
    this->GetSize(&w, &h);
    EvalExpr->SetSize( wxSize(w -50, -1 ));

  };

  void Options::OnEval(wxCommandEvent& /*event*/) {
    wxStreamToTextRedirector redirect(p_output);
    try{
    std::cout << "Evaluate: \"" << EvalExpr->GetValue().mb_str() << "\""<< std::endl;
    std::cout << "Parsing and type checking specification" << std::endl;
    wxString wx_spec = p_editor->GetStringFromDataEditor();
    mcrl2::process::process_specification spec = mcrl2::process::parse_process_specification( std::string(wx_spec.mb_str() ));

    std::cout << "Parsing data expression:\"" << EvalExpr->GetValue().mb_str() << "\""<< std::endl;
    mcrl2::data::data_expression term = mcrl2::data::parse_data_expression( std::string(EvalExpr->GetValue().mb_str()) ,spec.data() );

    std::cout << "Rewriting data expression:\"" << EvalExpr->GetValue().mb_str() << "\""<< std::endl;
    mcrl2::data::rewriter rewr(spec.data(),m_rewrite_strategy);
    atermpp::map < mcrl2::data::variable, mcrl2::data::data_expression > assignments;
    std::cout << "Result: " << pp(rewr(term,make_map_substitution_adapter(assignments))) << std::endl;

    } catch ( mcrl2::runtime_error e) {
      std::cout << e.what() <<std::endl;
    }
  };

  void Options::OnSize(wxSizeEvent& /*event*/){
    int w, h;
    this->GetSize(&w, &h);
    EvalExpr->SetSize( wxSize(w -50, -1 ));
  };

  void Options::UpdateFocus( wxCommandEvent& event ){

    std::cout << "blaat" << std::endl;
  }

