#include "mcrl2xi_qt_parse_and_typecheck.h"
#include "ui_mcrl2xi_qt_parse_and_typecheck.h"

#include "mcrl2/process/parse.h"
#include "mcrl2/data/parse.h"

#include <iostream>
#include <sstream>
#include <string>
#include "mcrl2/utilities/logger.h"

#include "mcrl2xi_qt_mainwindow.h"
#include "mcrl2xi_qt_parsing.h"

#include "QTextEdit"

using namespace mcrl2::log;

parse_and_typecheck::parse_and_typecheck(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::parse_and_typecheck)
{
    ui->setupUi(this);

    connect(ui->ParseAndTypeCheck, SIGNAL(clicked()), this, SLOT(onParseAndTypeCheck()) );
}

parse_and_typecheck::~parse_and_typecheck()
{
    delete ui;
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
