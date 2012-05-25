#ifndef MCRL2XI_QT_REWRITER_H
#define MCRL2XI_QT_REWRITER_H

#include <QWidget>
#include <QThread>
#include <QTextEdit>
#include "mcrl2/data/rewrite_strategy.h"

namespace Ui {
class rewriter;
}

class RewriteThread;

class rewriter : public QWidget
{
    Q_OBJECT
    
public:
    explicit rewriter(QWidget *parent = 0);
    ~rewriter();
    void setSelectedEditor( QTextEdit* edt );
private slots:
    void onRewrite();
    void onCancel();
    void onOutputText(QString s);
private:
    Ui::rewriter *ui;
    RewriteThread *m_thread ;
    QTextEdit  *m_selectedEditor;

};

// Rewrite Thread
class RewriteThread : public QThread
{
    Q_OBJECT
public:
    void setDataExpression( std::string s );
    void setSpecification( std::string s);
    void setRewriter( mcrl2::data::rewrite_strategy rw);
signals:
    void emitToLocalOutput(QString s);
private:
    void run();
    std::string m_dataExpression;
    std::string m_specification;
    mcrl2::data::rewrite_strategy m_rewrite_strategy;

};

#endif // MCRL2XI_QT_REWRITER_H
