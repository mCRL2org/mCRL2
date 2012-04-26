// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "rewriter.h"
#include "parsing.h"

#include "mcrl2/utilities/atermthread.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/data/classic_enumerator.h"

Rewriter::Rewriter()
{
  moveToThread(mcrl2::utilities::qt::get_aterm_thread());
}

void Rewriter::setRewriter(QString rewriter)
{
  m_rewrite_strategy = mcrl2::data::parse_rewrite_strategy(rewriter.toStdString())  ;
}

void Rewriter::setSpecification(QString specification)
{
  m_specification = specification.toStdString();
}

void Rewriter::rewrite(QString dataExpression)
{
  try
  {
    std::string stdDataExpression = dataExpression.toStdString();

    mCRL2log(info) << "Evaluate: \"" << stdDataExpression << "\"" << std::endl;
    mCRL2log(info) << "Parsing and type checking specification" << std::endl;

    mcrl2::data::data_specification data_spec;
    atermpp::set <mcrl2::data::variable > vars;

    if(!mcrl2xi_qt::parse_mcrl2_specification_with_variables(m_specification, data_spec, vars))
    {
      return;
    }

    mCRL2log(info) << "Parsing data expression: \"" << stdDataExpression << "\"" << std::endl;

    mcrl2::data::data_expression term = mcrl2::data::parse_data_expression(stdDataExpression,
        vars.begin(), vars.end(), data_spec);

    mCRL2log(info) << "Rewriting data expression: \"" << stdDataExpression << "\"" << std::endl;

    mcrl2::data::rewriter rewr(data_spec, m_rewrite_strategy);
    mcrl2::data::mutable_map_substitution < atermpp::map < mcrl2::data::variable, mcrl2::data::data_expression > > assignments;

    mCRL2log(info) << "Result: \"" << mcrl2::data::pp(rewr(term,assignments)) << "\"" << std::endl;

    emit rewritten(mcrl2::data::pp(rewr(term,assignments)).c_str());

  }
  catch (mcrl2::runtime_error e)
  {
    mCRL2log(error) << e.what() << std::endl;
  }
}

