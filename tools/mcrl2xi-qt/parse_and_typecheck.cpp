#include "parse_and_typecheck.h"
#include "ui_parse_and_typecheck.h"

#include "mcrl2/process/parse.h"
#include "mcrl2/data/parse.h"

#include <iostream>
#include <sstream>
#include <string>

#include "mainwindow.h"
#include "parsing.h"

#include "QTextEdit"

parse_and_typecheck::parse_and_typecheck(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);

    connect(ui.ParseAndTypeCheck, SIGNAL(clicked()), this, SLOT(onParseAndTypeCheck()) );
}

void parse_and_typecheck::onParseAndTypeCheck()
{
    std::string input = m_selectedEditor->toPlainText().toStdString();
    mcrl2xi_qt::parse_mcrl2_specification(input);
}

void parse_and_typecheck::setSelectedEditor(QTextEdit* edtr)
{
    m_selectedEditor = edtr;
}
