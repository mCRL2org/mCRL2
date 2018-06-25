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

AddEditPropertyDialog::AddEditPropertyDialog(bool add, FileSystem* fileSystem,
                                             QWidget* parent,
                                             QString propertyName,
                                             QString propertyText)
    : QDialog(parent), ui(new Ui::AddEditPropertyDialog)
{
  ui->setupUi(this);

  this->fileSystem = fileSystem;
  oldPropertyName = propertyName;

  /* change the ui depending on whether this should be an add or edit property
   *   window */
  if (add)
  {
    windowTitle = "Add Property";
    ui->addEditButton->setText("Add");
  }
  else
  {
    windowTitle = "Edit Property";
    ui->addEditButton->setText("Edit");
    ui->propertyNameField->setText(propertyName);
    ui->propertyTextField->setPlainText(propertyText);
  }

  setWindowTitle(windowTitle);
  setWindowFlags(Qt::Window);

  connect(ui->addEditButton, SIGNAL(clicked()), this, SLOT(parseAndAccept()));
  connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

QString AddEditPropertyDialog::getPropertyName()
{
  return ui->propertyNameField->text();
}

QString AddEditPropertyDialog::getPropertyText()
{
  return ui->propertyTextField->toPlainText();
}

void AddEditPropertyDialog::parseAndAccept()
{
  QString propertyName = ui->propertyNameField->text();
  /* show a message box if the property name field is empty */
  if (propertyName.count() == 0)
  {
    QMessageBox* msgBox =
        new QMessageBox(QMessageBox::Information, windowTitle,
                        "The property name may not be empty", QMessageBox::Ok,
                        this, Qt::WindowCloseButtonHint);
    msgBox->exec();
    return;
  }

  /* show a message box if this property name already exists */
  if (oldPropertyName != propertyName &&
      fileSystem->propertyNameExists(propertyName))
  {
    QMessageBox* msgBox =
        new QMessageBox(QMessageBox::Information, windowTitle,
                        "A property with this name already exists",
                        QMessageBox::Ok, this, Qt::WindowCloseButtonHint);
    msgBox->exec();
    return;
  }

  /* show a message box if the property text field is empty */
  if (ui->propertyTextField->toPlainText().count() == 0)
  {
    QMessageBox* msgBox =
        new QMessageBox(QMessageBox::Information, windowTitle,
                        "The property text may not be empty", QMessageBox::Ok,
                        this, Qt::WindowCloseButtonHint);
    msgBox->exec();
    return;
  }

  /* if everything is ok, accept the input */
  accept();
}

AddEditPropertyDialog::~AddEditPropertyDialog()
{
  delete ui;
}
