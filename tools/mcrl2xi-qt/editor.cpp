#include "editor.h"

editor::editor()
{
    setupEditor();
}

void editor::setupEditor()
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);

    this->setFont(font);
    this->clear();

    highlighter = new Highlighter(this->document());
}
