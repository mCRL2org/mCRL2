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

  propertyNameValidator =
      new QRegularExpressionValidator(QRegularExpression("[A-Za-z0-9_\\s]*"));
  ui->propertyNameField->setValidator(propertyNameValidator);

  /* change the UI depending on whether this should be an add or edit property
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
  setWindowFlags(Qt::Dialog);

  ui->formulaTextField->setPurpose(false);
  ui->initTextField1->setPurpose(true);
  ui->initTextField2->setPurpose(true);

  /* connections for buttons */
  connect(ui->saveAndParseButton, SIGNAL(clicked()), this,
          SLOT(actionSaveAndParse()));
  connect(ui->saveAndCloseButton, SIGNAL(clicked()), this,
          SLOT(actionSaveAndClose()));
  connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(reject()));
  connect(processSystem, SIGNAL(processFinished(int)), this,
          SLOT(parseResults(int)));

  /* connections for modification state */
  connect(ui->formulaTextField, SIGNAL(modificationChanged(bool)),
          ui->tabWidget->widget(0), SLOT(setWindowModified(bool)));
  connect(ui->initTextField1, SIGNAL(textChanged()), this,
          SLOT(setEquivalenceTabToModified()));
  connect(ui->equivalenceComboBox, SIGNAL(currentIndexChanged(int)), this,
          SLOT(setEquivalenceTabToModified()));
  connect(ui->initTextField2, SIGNAL(textChanged()), this,
          SLOT(setEquivalenceTabToModified()));
}

AddEditPropertyDialog::~AddEditPropertyDialog()
{
  delete ui;
  propertyNameValidator->deleteLater();
}

void AddEditPropertyDialog::activateDialog(const Property& property)
{
  /* fill in a property if applicable */
  if (!property.name.isEmpty())
  {
    ui->propertyNameField->setText(property.name);
    if (property.mucalculus)
    {
      ui->formulaTextField->setPlainText(property.text);
      ui->tabWidget->setCurrentIndex(0);
    }
    else
    {
      ui->initTextField1->setPlainText(property.text);
      ui->equivalenceComboBox->setSelectedEquivalence(property.equivalence);
      ui->initTextField2->setPlainText(property.text2);
      ui->tabWidget->setCurrentIndex(1);
    }

    oldProperty = property;
  }

  /* reset focus and modification state */
  ui->saveAndParseButton->setFocus();
  ui->propertyNameField->setFocus();
  resetModificationState();

  /* make the dialog active and visible */
  if (isVisible())
  {
    activateWindow();
    setFocus();
    raise(); // for MacOS
  }
  else
  {
    show();
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
                    ui->initTextField1->toPlainText(), false,
                    ui->equivalenceComboBox->getSelectedEquivalence(),
                    ui->initTextField2->toPlainText());
  }
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

bool AddEditPropertyDialog::saveProperty()
{
  bool ok = checkInput();

  if (ok)
  {
    Property property = getProperty();
    if (oldProperty.name.isEmpty())
    {
      fileSystem->newProperty(property);
    }
    else
    {
      fileSystem->editProperty(oldProperty, property);
    }
    oldProperty = property;

    resetModificationState();
  }

  return ok;
}

void AddEditPropertyDialog::resetStateAfterParsing()
{
  propertyParsingProcessid = -1;
  ui->saveAndParseButton->setText(" Save and Parse ");
}

void AddEditPropertyDialog::abortPropertyParsing()
{
  if (propertyParsingProcessid >= 0)
  {
    /* we reset the state before aborting parsing so that parsingResult doesn't
     *   get triggered */
    int parsingid = propertyParsingProcessid;
    resetStateAfterParsing();
    processSystem->abortProcess(parsingid);
  }
}

void AddEditPropertyDialog::actionSaveAndParse()
{
  /* if a parsing process is running, abort it */
  if (propertyParsingProcessid >= 0)
  {
    abortPropertyParsing();
  }
  /* else parse the current property */
  else
  {
    if (saveProperty())
    {
      /* save the property, start a parsing process and wait for a reply */
      Property property = getProperty();
      lastParsingPropertyIsMucalculus = property.mucalculus;
      propertyParsingProcessid = processSystem->parseProperty(property);
      ui->saveAndParseButton->setText(" Abort Parsing ");
    }
  }
}

void AddEditPropertyDialog::parseResults(int processid)
{
  /* check if the process that has finished is the parsing process of this
   *   dialog */
  if (processid == propertyParsingProcessid)
  {
    /* get the result and notify the user */
    QString message = "";
    QString result = processSystem->getResult(processid);
    QString inputType = lastParsingPropertyIsMucalculus ? "mu-calculus formula"
                                                        : "process expressions";

    if (result == "valid")
    {
      message = "The entered " + inputType +
                (lastParsingPropertyIsMucalculus ? " is" : " are") +
                " well-formed.";
    }
    else if (result.startsWith("invalid"))
    {
      if (!lastParsingPropertyIsMucalculus)
      {
        inputType = (result.endsWith(SPECTYPEEXTENSION.at(SpecType::First))
                         ? "first process expression"
                         : "second process expression");
      }
      message =
          "The entered " + inputType +
          " is not well-formed. See the parsing console for more information";
    }
    else
    {
      message = "Could not parse the entered " + inputType +
                ". See the parsing console for more information";
    }

    executeInformationBox(this, windowTitle, message);
    resetStateAfterParsing();
  }
}

void AddEditPropertyDialog::actionSaveAndClose()
{
  if (saveProperty())
  {
    accept();
  }
}

void AddEditPropertyDialog::setEquivalenceTabToModified()
{
  ui->equivalenceTab->setWindowModified(true);
}

void AddEditPropertyDialog::resetModificationState()
{
  ui->propertyNameField->setModified(false);
  ui->mucalculusTab->setWindowModified(false);
  ui->equivalenceTab->setWindowModified(false);
}

void AddEditPropertyDialog::done(int r)
{
  /* check for modifications */
  if (ui->propertyNameField->isModified() ||
      ui->tabWidget->currentWidget()->isWindowModified())
  {
    QMessageBox::StandardButton result =
        executeQuestionBox(this, windowTitle,
                           "There are unsaved changes in the current property, "
                           "do you want to save?");
    switch (result)
    {
    case QMessageBox::Yes:
      if (saveProperty())
      {
        break;
      }
      return;
    case QMessageBox::No:
      break;
    default:
      return;
    }
  }

  /* reset the state */
  abortPropertyParsing();
  oldProperty = Property();
  ui->propertyNameField->clear();
  ui->formulaTextField->clear();
  ui->initTextField1->clear();
  ui->initTextField2->clear();
  ui->equivalenceComboBox->setCurrentIndex(0);

  QDialog::done(r);
}
