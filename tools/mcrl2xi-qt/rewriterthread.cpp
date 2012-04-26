// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "rewriterthread.h"
#include "parsing.h"

#include "mcrl2/utilities/atermthread.h"

RewriterThread::RewriterThread()
{
  moveToThread(mcrl2::utilities::qt::get_aterm_thread());
  m_parsed = false;
}

void RewriterThread::setRewriter(QString rewriter)
{
  m_rewrite_strategy = mcrl2::data::parse_rewrite_strategy(rewriter.toStdString())  ;
}

void RewriterThread::rewrite(QString specification, QString dataExpression)
{
  try
  {
    if (m_specification != specification)
    {
      m_specification = specification;
      mCRL2log(info) << "Parsing and type checking specification" << std::endl;
      m_parsed = mcrl2xi_qt::parse_mcrl2_specification_with_variables(specification.toStdString(), m_data_spec, m_vars);
      if (!m_parsed) {
        emit rewritten(QString());
        return;
      }
    }
    if (!m_parsed) {
      mCRL2log(error) << "Specification contains no valid data or mCRL2 specification." << std::endl;
      emit rewritten(QString());
      return;
    }

    std::string stdDataExpression = dataExpression.toStdString();

    mCRL2log(info) << "Evaluate: \"" << stdDataExpression << "\"" << std::endl;
    mCRL2log(info) << "Parsing data expression: \"" << stdDataExpression << "\"" << std::endl;

    mcrl2::data::data_expression term = mcrl2::data::parse_data_expression(stdDataExpression,
        m_vars.begin(), m_vars.end(), m_data_spec);

    mCRL2log(info) << "Rewriting data expression: \"" << stdDataExpression << "\"" << std::endl;

    mcrl2::data::rewriter rewr(m_data_spec, m_rewrite_strategy);
    mcrl2::data::mutable_map_substitution < atermpp::map < mcrl2::data::variable, mcrl2::data::data_expression > > assignments;

    mCRL2log(info) << "Result: \"" << mcrl2::data::pp(rewr(term,assignments)) << "\"" << std::endl;

    emit rewritten(mcrl2::data::pp(rewr(term,assignments)).c_str());

  }
  catch (mcrl2::runtime_error e)
  {
    mCRL2log(error) << e.what() << std::endl;
    emit rewritten(QString());
  }
}

