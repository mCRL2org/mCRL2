#ifndef PARSE_AND_TYPECHECK_H
#define PARSE_AND_TYPECHECK_H

#include <QWidget>
#include "QTextEdit"
#include "mainwindow.h"
#include "ui_parse_and_typecheck.h"

namespace Ui {
class parse_and_typecheck;
}

class MainWindow;

class parse_and_typecheck : public QWidget
{
    Q_OBJECT
    
public:
    explicit parse_and_typecheck(QWidget *parent = 0);
    void setSelectedEditor(QTextEdit* edtr);
private slots:
    void onParseAndTypeCheck();

private:
    Ui::parse_and_typecheck ui;
    QTextEdit* m_selectedEditor;
};

#endif // PARSE_AND_TYPECHECK_H
