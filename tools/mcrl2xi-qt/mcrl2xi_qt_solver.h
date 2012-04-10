#ifndef MCRL2XI_QT_SOLVER_H
#define MCRL2XI_QT_SOLVER_H

#include <QWidget>
#include <QThread>
#include <QTextEdit>

#include "mcrl2xi_qt_parsing.h"
#include "mcrl2/data/rewrite_strategy.h"

namespace Ui {
class solver;
}

class SolverThread;

class solver : public QWidget
{
    Q_OBJECT
    
public:
    explicit solver(QWidget *parent = 0);
    ~solver();
    void setSelectedEditor( QTextEdit* edt );

private slots:
    void onSolve();
    void onCancel();
    void onStarted();
    void onStopped();
    void onOutputText(QString s);
private:
    Ui::solver *ui;
    SolverThread *m_thread;
    QTextEdit  *m_selectedEditor;
};

// Solver Thread
class SolverThread : public QThread
{
    Q_OBJECT
public:
    void setDataExpression( std::string s );
    void setSpecification( std::string s);
    void setRewriter( mcrl2::data::rewrite_strategy rw);
    void stop();
signals:
    void emitToLocalOutput(QString s);
private:
    void run();
    void DoWork();
    bool m_abort;
    std::string m_dataExpression;
    std::string m_specification;
    mcrl2::data::rewrite_strategy m_rewrite_strategy;
};

#endif // MCRL2XI_QT_SOLVER_H
