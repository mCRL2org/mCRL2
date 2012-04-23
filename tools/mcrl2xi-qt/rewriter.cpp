#include "rewriter.h"
#include "ui_rewriter.h"

#include "iostream"
#include "parsing.h"

#include "mcrl2/process/parse.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/classic_enumerator.h"

rewriter::rewriter(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);

    //Connect Buttons
    connect( ui.RewriteButton, SIGNAL(clicked()), this, SLOT(onRewrite()));
    connect( ui.CancelButton, SIGNAL(clicked()), this, SLOT(onCancel()));

    //Setup rewriter thread
    m_thread = new RewriteThread();

    connect( m_thread, SIGNAL(emitToLocalOutput(QString)), this, SLOT(onOutputText(QString)) );

}

void rewriter::setSelectedEditor(QTextEdit* edtr)
{
    m_selectedEditor = edtr;
}

void rewriter::onRewrite(){
    m_thread->setDataExpression( ui.dataExpression->toPlainText().toStdString() );
    m_thread->setSpecification(m_selectedEditor->toPlainText().toStdString() );
    m_thread->setRewriter(mcrl2::data::jitty);

    ui.rewriteOutput->clear();
    m_thread->start();
}

void rewriter::onCancel(){
    m_thread->terminate();
    m_thread->wait();

    delete m_thread;
    m_thread = new RewriteThread();
}

void rewriter::onOutputText(QString s){
    ui.rewriteOutput->append(s);
}

void RewriteThread::setDataExpression(std::string s)
{
    m_dataExpression = s;
}

void RewriteThread::setSpecification(std::string s)
{
    m_specification = s;
}

void RewriteThread::setRewriter( mcrl2::data::rewrite_strategy rw)
{
    m_rewrite_strategy = rw;
}

void RewriteThread::run()
{
    //    MoveToThread uitvoeren:
    //    http://www.developer.nokia.com/Community/Wiki/How_to_move_a_QObject_to_a_thread
    //
    //    Ipv:
    //    aterm::ATerm stack;
    //    aterm::ATinit(&stack);

    try
    {
      mCRL2log(info) << "Evaluate: \"" << m_dataExpression << "\"" << std::endl;
      mCRL2log(info) << "Parsing and type checking specification" << std::endl;

      mcrl2::data::data_specification data_spec;
      atermpp::set <mcrl2::data::variable > vars;

      if(!mcrl2xi_qt::parse_mcrl2_specification_with_variables(m_specification, data_spec, vars))
      {
        return;
      }

      mCRL2log(info) << "Parsing data expression: \"" << m_dataExpression << "\"" << std::endl;

      mcrl2::data::data_expression term = mcrl2::data::parse_data_expression(m_dataExpression,
          vars.begin(), vars.end(), data_spec);

      mCRL2log(info) << "Rewriting data expression: \"" << m_dataExpression << "\"" << std::endl;

      mcrl2::data::rewriter rewr(data_spec, m_rewrite_strategy);
      mcrl2::data::mutable_map_substitution < atermpp::map < mcrl2::data::variable, mcrl2::data::data_expression > > assignments;

      mCRL2log(info) << "Result: \"" << mcrl2::data::pp(rewr(term,assignments)) << "\"" << std::endl;

      emit emitToLocalOutput(mcrl2::data::pp(rewr(term,assignments)).c_str());

    }
    catch (mcrl2::runtime_error e)
    {
      mCRL2log(error) << e.what() << std::endl;
    }

}


