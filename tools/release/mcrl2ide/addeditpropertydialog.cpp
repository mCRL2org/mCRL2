// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "addeditpropertydialog.h"
#include "ui_addeditpropertydialog.h"
#include "utilities.h"

#include <QStandardItemModel>

AddEditPropertyDialog::AddEditPropertyDialog(bool add,
                                             ProcessSystem* processSystem,
                                             FileSystem* fileSystem,
                                             QWidget* parent)
    : QDialog(parent), ui(new Ui::AddEditPropertyDialog),
      processSystem(processSystem), fileSystem(fileSystem),
      oldProperty(Property()), propertyParsingProcessid(-1),
      lastParsingPropertyIsMucalculus(true)
{
  ui->setupUi(this);

  propertyNameValidator = new QRegExpValidator(QRegExp("[A-Za-z0-9_\\s]*"));
  ui->propertyNameField->setValidator(propertyNameValidator);

  /* change the ui depending on whether this should be an add or edit property
   *   window */
  if (add)
  {
    windowTitle = "Add Property";
  }
  else
  {
    windowTitle = "Edit Property";
  }

  setWindowTitle(windowTitle);
  setWindowFlags(Qt::Window);

  ui->formulaTextField->setHighlightingRules(false);
  ui->initTextField->setHighlightingRules(true);

  connect(ui->parseButton, SIGNAL(clicked()), this, SLOT(parseProperty()));
  connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(addEditProperty()));
  connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
  connect(processSystem, SIGNAL(processFinished(int)), this,
          SLOT(parseResults(int)));
  connect(this, SIGNAL(rejected()), this, SLOT(onRejected()));
}

AddEditPropertyDialog::~AddEditPropertyDialog()
{
  delete ui;
  propertyNameValidator->deleteLater();
}

void AddEditPropertyDialog::resetFocus()
{
  ui->saveButton->setFocus();
  ui->propertyNameField->setFocus();
}

void AddEditPropertyDialog::clearFields()
{
  ui->propertyNameField->clear();
  ui->formulaTextField->clear();
  ui->initTextField->clear();
  ui->equivalenceComboBox->setCurrentIndex(0);
}

void AddEditPropertyDialog::setProperty(const Property& property)
{
  ui->propertyNameField->setText(property.name);
  if (property.mucalculus)
  {
    ui->formulaTextField->setPlainText(property.text);
    ui->tabWidget->setCurrentIndex(0);
  }
  else
  {
    ui->equivalenceComboBox->setSelectedEquivalence(property.equivalence);
    ui->initTextField->setPlainText(property.text);
    ui->tabWidget->setCurrentIndex(1);
  }
}

Property AddEditPropertyDialog::getProperty()
{
  if (ui->tabWidget->currentIndex() == 0) // mucalculus tab
  {
    return Property(ui->propertyNameField->text(),
                    ui->formulaTextField->toPlainText());
  }
  else // equivalence tab
  {
    return Property(ui->propertyNameField->text(),
                    ui->initTextField->toPlainText(), false,
                    ui->equivalenceComboBox->getSelectedEquivalence());
  }
}

void AddEditPropertyDialog::setOldProperty(const Property& oldProperty)
{
  this->oldProperty = oldProperty;
}

bool AddEditPropertyDialog::checkInput()
{
  QString propertyName = ui->propertyNameField->text();

  /* both input fields may not be empty and the property name may not exist
   *   already */
  if (propertyName.count() == 0)
  {
    executeInformationBox(this, windowTitle,
                          "The property name may not be empty");
  }
  else if (oldProperty.name != propertyName &&
           fileSystem->propertyNameExists(propertyName))
  {
    executeInformationBox(this, windowTitle,
                          "A property with this name already exists");
  }
  else if (ui->tabWidget->currentIndex() == 1 &&
           ui->equivalenceComboBox->getSelectedEquivalence() ==
               mcrl2::lts::lts_eq_none)
  {
    executeInformationBox(this, windowTitle,
                          "No equivalence has been selected");
  }
  else
  {
    return true;
  }

  return false;
}

void AddEditPropertyDialog::abortPropertyParsing()
{
  if (propertyParsingProcessid >= 0)
  {
    /* we first change propertyParsingProcessid so that parsingResult doesn't
     *   get triggered */
    int parsingid = propertyParsingProcessid;
    propertyParsingProcessid = -1;
    processSystem->abortProcess(parsingid);
  }
}

void AddEditPropertyDialog::parseProperty()
{
  /* if a parsing process is running, abort it */
  if (propertyParsingProcessid >= 0)
  {
    abortPropertyParsing();
    ui->parseButton->setText("Parse");
  }
  /* else parse the current property */
  else
  {
    if (checkInput())
    {
      /* save the property, start a parsing process and wait for a reply */
      Property property = getProperty();
      fileSystem->saveProperty(property, true);
      lastParsingPropertyIsMucalculus = property.mucalculus;
      propertyParsingProcessid = processSystem->parseProperty(property);
      ui->parseButton->setText("Abort Parsing");
    }
  }
}

void AddEditPropertyDialog::parseResults(int processid)
{
  /* check if the process that has finished is the parsing process of this
   *   dialog */
  if (processid == propertyParsingProcessid)
  {
    /* if valid accept, else show message */
    QString message = "";
    QString result = processSystem->getResult(processid);
    QString inputType = lastParsingPropertyIsMucalculus
                            ? "mu-calculus formula"
                            : "alternate initial process";

    if (result == "valid")
    {
      message = "The entered " + inputType + " is valid.";
    }
    else if (result == "invalid")
    {
      message = "The entered " + inputType +
                " is not valid. See the parsing console for more information";
    }
    else
    {
      message = "Could not parse the entered " + inputType +
                ". See the parsing console for more information";
    }

    executeInformationBox(this, windowTitle, message);
    ui->parseButton->setText("Parse");
    propertyParsingProcessid = -1;
  }
}

void AddEditPropertyDialog::addEditProperty()
{
  /* if the input is correct, remove the original property file if possible and
   *   save the current one */
  if (checkInput())
  {
    if (!oldProperty.name.isEmpty())
    {
      fileSystem->deletePropertyFile(oldProperty);
    }
    fileSystem->saveProperty(getProperty());
    accept();
  }
}

void AddEditPropertyDialog::onRejected()
{
  /* abort the parsing process */
  abortPropertyParsing();
}
