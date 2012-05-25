#include "mcrl2xi_qt_solver.h"
#include "ui_mcrl2xi_qt_solver.h"
#include "iostream"

#include "mcrl2/process/parse.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/classic_enumerator.h"

solver::solver(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::solver)
{
    ui->setupUi(this);

    //Connect Buttons
    connect( ui->SolveButton, SIGNAL(clicked()), this, SLOT(onSolve()));
    connect( ui->CancelButton, SIGNAL(clicked()), this, SLOT(onCancel()));

    //Setup solver thread
    m_thread = NULL;

}

solver::~solver()
{
    if (m_thread != NULL)
    {
        static_cast<SolverThread*>(m_thread)->stop();
        m_thread->wait();
    }
    delete ui;
}

void solver::setSelectedEditor(QTextEdit* edtr)
{
    m_selectedEditor = edtr;
}

void solver::onSolve(){

    if (m_thread == NULL)
    {
        /* Clear output */
        ui->solveOutput->clear();

        m_thread = new SolverThread();

        /* Should eventually be moved into constructor */
        m_thread->setDataExpression( ui->evalExpression->toPlainText().toStdString() );
        m_thread->setSpecification(m_selectedEditor->toPlainText().toStdString() );
        m_thread->setRewriter(mcrl2::data::jitty);

        m_thread->connect(m_thread, SIGNAL(started()), this, SLOT(onStarted()));
        m_thread->connect(m_thread, SIGNAL(finished()), this, SLOT(onStopped()));
        m_thread->connect(m_thread, SIGNAL(emitToLocalOutput(QString)), this, SLOT(onOutputText(QString)) );

        m_thread->setPriority(QThread::IdlePriority);

        m_thread->start();

    }
}

void solver::onCancel(){
    if(m_thread != NULL){
        m_thread->stop();
        m_thread = NULL;
    }
}

void solver::onOutputText(QString s){
    ui->solveOutput->append(s);
}

void solver::onStarted(){
    ui->SolveButton->setEnabled(false);
}

void solver::onStopped(){
    ui->SolveButton->setEnabled(true);
}

void SolverThread::setDataExpression(std::string s)
{
    m_dataExpression = s;
}

void SolverThread::setSpecification(std::string s)
{
    m_specification = s;
}

void SolverThread::setRewriter( mcrl2::data::rewrite_strategy rw)
{
    m_rewrite_strategy = rw;
}

void SolverThread::stop()
{
    m_abort=true;
}

void SolverThread::run()
{
//    MoveToThread uitvoeren:
//    http://www.developer.nokia.com/Community/Wiki/How_to_move_a_QObject_to_a_thread
//
//    Ipv:
//    aterm::ATerm stack;
//    aterm::ATinit(&stack);

    m_abort = false;
    try
    {

      mCRL2log(info) << "Solving: \"" << m_dataExpression << "\"" << std::endl;

      int dotpos =  m_dataExpression.find('.');
      if (dotpos  == -1)
      {
        throw mcrl2::runtime_error("Expect a `.' in the input.");
      }

      mcrl2::data::data_specification data_spec;
      std::set <mcrl2::data::variable > vars;

      if(!mcrl2xi_qt::parse_mcrl2_specification_with_variables(m_specification, data_spec, vars))
      {
        return;
      }

      parse_variables(std::string(m_dataExpression.substr(0, dotpos)
                          ) + ";",std::inserter(vars,vars.begin()),data_spec);

      mcrl2::data::data_expression term =
        mcrl2::data::parse_data_expression(
          m_dataExpression.substr(dotpos+1, m_dataExpression.length()-1),
          vars.begin(), vars.end(),
          data_spec
        );
      if (term.sort()!=mcrl2::data::sort_bool::bool_())
      {
        throw mcrl2::runtime_error("Expression is not of sort Bool.");
      }

      mcrl2::data::rewriter rewr(data_spec,m_rewrite_strategy);
      term=rewr(term);

      typedef mcrl2::data::classic_enumerator< mcrl2::data::rewriter > enumerator_type;

      enumerator_type enumerator(data_spec,rewr);

      for (enumerator_type::iterator i = enumerator.begin(vars,term,10000); // Stop when more than 10000 internal variables are required.
                   i != enumerator.end() && !m_abort; ++i)
      {
        mCRL2log(info) << "Solution found" << std::endl;

        QString s('[');

        for (std::set< mcrl2::data::variable >::const_iterator v=vars.begin(); v!=vars.end() ; ++v)
        {
          if( v != vars.begin() )
          {
            s.append(", ");
          }
          s.append(mcrl2::data::pp(*v).c_str());
          s.append(" := ");
          s.append(mcrl2::data::pp((*i)(*v)).c_str());
        }
        s.append("] evaluates to ");
        s.append(mcrl2::data::pp(rewr(term,*i)).c_str());

        emit emitToLocalOutput(s);
      }

      if (m_abort)
          mCRL2log(info) << "Abort by user." << std::endl;
      else
          mCRL2log(info) << "Done solving." << std::endl;

    }
    catch (mcrl2::runtime_error e)
    {
      mCRL2log(error) << e.what() << std::endl;
    }

}
