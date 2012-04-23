#ifndef EDITOR_H
#define EDITOR_H

#include <QTextEdit>
#include "highlighter.h"

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

#endif // EDITOR_H
