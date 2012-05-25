#ifndef MCRL2XI_QT_EDITOR_H
#define MCRL2XI_QT_EDITOR_H

#include <QTextEdit>
#include "mcrl2xi_qt_highlighter.h"

class editor : public QTextEdit
{
public:
    explicit editor();
    void setupEditor();
private:
    Highlighter *highlighter;
    
signals:
    
public slots:


    
};

#endif // MCRL2XI_QT_EDITOR_H
