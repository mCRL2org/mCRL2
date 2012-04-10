#ifndef MCRL2XI_QT_PARSE_AND_TYPECHECK_H
#define MCRL2XI_QT_PARSE_AND_TYPECHECK_H

#include <QWidget>
#include "QTextEdit"
#include "mcrl2xi_qt_mainwindow.h"

namespace Ui {
class parse_and_typecheck;
}

class MainWindow;

class parse_and_typecheck : public QWidget
{
    Q_OBJECT
    
public:
    explicit parse_and_typecheck(QWidget *parent = 0);
    ~parse_and_typecheck();
    void setSelectedEditor(QTextEdit* edtr);
private slots:
    void onParseAndTypeCheck();

private:
    Ui::parse_and_typecheck *ui;
    QTextEdit* m_selectedEditor;
};

#endif // MCRL2XI_QT_PARSE_AND_TYPECHECK_H
