#ifndef REWRITER_H
#define REWRITER_H

#include <QWidget>
#include <QThread>
#include <QTextEdit>
#include "mcrl2/data/rewrite_strategy.h"
#include "ui_rewriter.h"

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
    Ui::rewriter ui;
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

class Rewriter : public QObject
{
  Q_OBJECT
  public:
    Rewriter(const mcrl2::lps::specification &specification, mcrl2::data::rewrite_strategy strategy);

  public slots:
    void rewriteDataExpression(std::string expression);

  signals:
    void rewroteDataExpression(std::string original, std::string result);

  private:
    mcrl2::lps::specification m_specification;
    mcrl2::data::rewriter m_rewriter;
};

#endif // REWRITER_H
