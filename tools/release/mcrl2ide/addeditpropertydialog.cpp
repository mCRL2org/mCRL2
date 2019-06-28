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

#include <QMessageBox>
#include <QStandardItemModel>

EquivalenceComboBox::EquivalenceComboBox(QWidget* parent) : QComboBox(parent)
{
  /* add equivalences to the combobox, including some seperators to indicate the
   *   use of abstraction */
  QStringList items;
  int secondSeparatorIndex = 2;
  items << "----- CHOOSE REDUCTION -----"
        << "--- WITHOUT ABSTRACTION ---";
  for (std::pair<mcrl2::lts::lts_equivalence, std::pair<QString, bool>> item :
       LTSREDUCTIONINFO)
  {
    if (!item.second.second &&
        item.first != mcrl2::lts::lts_equivalence::lts_eq_none)
    {
      items << item.second.first;
      secondSeparatorIndex++;
    }
  }

  items << "--- WITH ABSTRACTION ---";
  for (std::pair<mcrl2::lts::lts_equivalence, std::pair<QString, bool>> item :
       LTSREDUCTIONINFO)
  {
    if (item.second.second)
    {
      items << item.second.first;
    }
  }

  this->addItems(items);

  /* Set separators to be unselectable */
  QStandardItemModel* model = qobject_cast<QStandardItemModel*>(this->model());
  model->item(0)->setFlags(model->item(0)->flags() & ~Qt::ItemIsEnabled);
  model->item(1)->setFlags(model->item(1)->flags() & ~Qt::ItemIsEnabled);
  model->item(secondSeparatorIndex)
      ->setFlags(model->item(secondSeparatorIndex)->flags() &
                 ~Qt::ItemIsEnabled);
}

AddEditPropertyDialog::AddEditPropertyDialog(bool add,
                                             ProcessSystem* processSystem,
                                             FileSystem* fileSystem,
                                             QWidget* parent)
    : QDialog(parent), ui(new Ui::AddEditPropertyDialog),
      processSystem(processSystem), fileSystem(fileSystem),
      propertyParsingProcessid(-1)
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
  ui->propertyTextField->clear();
}

void AddEditPropertyDialog::setProperty(const Property& property)
{
  ui->propertyNameField->setText(property.name);
  ui->propertyTextField->setPlainText(property.text);
}

Property AddEditPropertyDialog::getProperty()
{
  return Property(ui->propertyNameField->text(),
                  ui->propertyTextField->toPlainText());
}

void AddEditPropertyDialog::setOldProperty(const Property& oldProperty)
{
  this->oldProperty = oldProperty;
}

bool AddEditPropertyDialog::checkInput()
{
  QString propertyName = ui->propertyNameField->text();
  QString error = "";

  /* both input fields may not be empty and the propertyname may not exist
   * already */
  if (propertyName.count() == 0)
  {
    error = "The property name may not be empty";
  }
  else if (oldProperty.name != propertyName &&
           fileSystem->propertyNameExists(propertyName))
  {
    error = "A property with this name already exists";
  }

  if (!error.isEmpty())
  {
    QMessageBox msgBox(QMessageBox::Information, windowTitle, error,
                       QMessageBox::Ok, this, Qt::WindowCloseButtonHint);
    msgBox.exec();
    return false;
  }
  else
  {
    return true;
  }
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
      fileSystem->saveProperty(property);
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
    QString text = "";
    QString result = processSystem->getResult(processid);

    if (result == "valid")
    {
      text = "The entered formula is a valid mu-calculus formula.";
    }
    else if (result == "invalid")
    {
      text = "The entered formula is not a valid mu-calculus formula. See the "
             "parsing console for more information";
    }
    else
    {
      text = "Could not parse the entered formula. See the parsing console "
             "for more information";
    }

    QMessageBox msgBox(QMessageBox::Information, windowTitle, text,
                       QMessageBox::Ok, this, Qt::WindowCloseButtonHint);
    msgBox.exec();
    ui->parseButton->setText("Parse");
    propertyParsingProcessid = -1;
  }
}

void AddEditPropertyDialog::addEditProperty()
{
  if (checkInput())
  {
    accept();
  }
}

void AddEditPropertyDialog::onRejected()
{
  /* abort the parsing process */
  abortPropertyParsing();

  /* save the original property and clean up */
  fileSystem->saveProperty(oldProperty);
  fileSystem->deleteUnlistedPropertyFiles();
}
