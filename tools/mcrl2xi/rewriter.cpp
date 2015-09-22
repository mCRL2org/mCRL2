// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/data/rewriter.h"
#include "rewriter.h"
#include "parsing.h"

#include "mcrl2/gui/atermthread.h"

const std::string Rewriter::className = "Rewriter";

Rewriter::Rewriter()
{
  moveToThread(mcrl2::gui::qt::get_aterm_thread());
  thread()->setPriority(QThread::IdlePriority);
  m_parsed = false;
}

void Rewriter::setRewriter(QString rewriter)
{
  m_rewrite_strategy = mcrl2::data::parse_rewrite_strategy(rewriter.toStdString())  ;
}

void Rewriter::rewrite(QString specification, QString dataExpression)
{
  if (m_specification != specification || !m_parsed)
  {
    m_parsed = false;
    m_specification = specification;
    try
    {
      mcrl2xi_qt::parseMcrl2Specification(m_specification.toStdString(), m_data_spec, m_vars);
      m_parsed = true;
    }
    catch (mcrl2::runtime_error e)
    {
      m_parseError = QString::fromStdString(e.what());
    }
  }

  if (m_parsed)
  {
    try
    {

      std::string stdDataExpression = dataExpression.toStdString();

      mCRL2log(info) << "Evaluate: \"" << stdDataExpression << "\"" << std::endl;
      mCRL2log(info) << "Parsing data expression: \"" << stdDataExpression << "\"" << std::endl;

      mcrl2::data::data_expression term = mcrl2::data::parse_data_expression(stdDataExpression,
                                                                             m_vars.begin(), m_vars.end(), m_data_spec);

      mCRL2log(info) << "Rewriting data expression: \"" << stdDataExpression << "\"" << std::endl;

      std::set<mcrl2::data::sort_expression> all_sorts=find_sort_expressions(term);
      m_data_spec.add_context_sorts(all_sorts);
      mcrl2::data::rewriter rewr(m_data_spec, m_rewrite_strategy);
      mcrl2::data::mutable_map_substitution < std::map < mcrl2::data::variable, mcrl2::data::data_expression > > assignments;

      std::string result = mcrl2::data::pp(rewr(term,assignments));

      mCRL2log(info) << "Result: \"" << result << "\"" << std::endl;

      emit rewritten(QString::fromStdString(result));

    }
    catch (mcrl2::runtime_error e)
    {
      QString err = QString::fromStdString(e.what());
      emit(exprError(err));
    }
  }
  else
  {
    emit(parseError(m_parseError));
  }

  emit(finished());
}

