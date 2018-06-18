// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "addeditpropertydialog.h"

#include <QMessageBox>
#include <QSpacerItem>
#include <QStyleOption>
#include <QPainter>

PropertyWidget::PropertyWidget(Property* property, ProcessSystem* processSystem,
                               FileSystem* fileSystem, PropertiesDock* parent)
    : QWidget(parent)
{
  this->property = property;
  this->processSystem = processSystem;
  this->fileSystem = fileSystem;
  this->parent = parent;
  verificationProcessId = -1;

  fileSystem->setPropertyModified(property->name);

  /* create the label for the property name */
  propertyNameLabel = new QLabel(property->name);

  /* create the verify button */
  QPushButton* verifyButton = new QPushButton();
  verifyButton->setIcon(QIcon(":/icons/verify.png"));
  verifyButton->setIconSize(QSize(24, 24));
  verifyButton->setStyleSheet("border:none;");
  connect(verifyButton, SIGNAL(clicked()), this, SLOT(actionVerify()));

  /* create the abort button for when a property is being verified */
  QPushButton* abortButton = new QPushButton();
  abortButton->setIcon(QIcon(":/icons/abort.png"));
  abortButton->setIconSize(QSize(24, 24));
  abortButton->setStyleSheet("border:none;");
  connect(abortButton, SIGNAL(clicked()), this,
          SLOT(actionAbortVerification()));

  /* create the label for when a property is true */
  QPixmap* trueIcon = new QPixmap(":/icons/true.png");
  QLabel* trueLabel = new QLabel();
  trueLabel->setPixmap(trueIcon->scaled(QSize(24, 24)));

  /* create the label for when a property is false */
  QPixmap* falseIcon = new QPixmap(":/icons/false.png");
  QLabel* falseLabel = new QLabel();
  falseLabel->setPixmap(falseIcon->scaled(QSize(24, 24)));

  /* stack the verification widgets */
  verificationWidgets = new QStackedWidget(this);
  verificationWidgets->setMaximumSize(QSize(30, 30));
  verificationWidgets->addWidget(verifyButton); /* index = 0 */
  verificationWidgets->addWidget(abortButton);  /* index = 1 */
  verificationWidgets->addWidget(trueLabel);    /* index = 2 */
  verificationWidgets->addWidget(falseLabel);   /* index = 3 */
  verificationWidgets->setCurrentIndex(0);

  /* create the edit button */
  editButton = new QPushButton();
  editButton->setIcon(QIcon(":/icons/edit.png"));
  editButton->setIconSize(QSize(24, 24));
  editButton->setStyleSheet("border:none;");
  connect(editButton, SIGNAL(clicked()), this, SLOT(actionEdit()));

  /* create the delete button */
  deleteButton = new QPushButton();
  deleteButton->setIcon(QIcon(":/icons/delete.png"));
  deleteButton->setIconSize(QSize(24, 24));
  deleteButton->setStyleSheet("border:none;");
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(actionDelete()));

  /* lay them out */
  propertyLayout = new QHBoxLayout();
  propertyLayout->addWidget(propertyNameLabel);
  propertyLayout->addStretch();
  propertyLayout->addWidget(verificationWidgets);
  propertyLayout->addWidget(editButton);
  propertyLayout->addWidget(deleteButton);

  this->setLayout(propertyLayout);
}

void PropertyWidget::paintEvent(QPaintEvent* pe)
{
  Q_UNUSED(pe);
  QStyleOption o;
  o.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
}

Property* PropertyWidget::getProperty()
{
  return this->property;
}

void PropertyWidget::setPropertyName(QString name)
{
  this->property->name = name;
  propertyNameLabel->setText(name);
}

void PropertyWidget::setPropertyText(QString text)
{
  this->property->text = text;
}

void PropertyWidget::saveProperty()
{
  fileSystem->saveProperty(property);
}

void PropertyWidget::actionVerify()
{
  /* check if the property isn't already being verified or has been verified */
  if (verificationWidgets->currentIndex() == 0)
  {
    

    /* start the verification process */
    connect(processSystem, SIGNAL(processFinished(int)), this,
            SLOT(actionVerifyResult(int)));
    verificationProcessId = processSystem->verifyProperty(property);

    /* if starting the process was successful, change the buttons */
    if (verificationProcessId >= 0)
    {
      verificationWidgets->setCurrentIndex(1);
      editButton->setEnabled(false);
      deleteButton->setEnabled(false);
    }
  }
}

void PropertyWidget::actionVerifyResult(int processid)
{
  /* check if the process that is finished is the verification process of this
   *   property */
  if (processid == verificationProcessId)
  {
    /* get the result and apply it to the widget */
    QString result = processSystem->getResult(verificationProcessId);
    if (result == "true")
    {
      verificationWidgets->setCurrentIndex(2);
      this->setStyleSheet("background-color:rgb(153,255,153)");
    }
    else if (result == "false")
    {
      verificationWidgets->setCurrentIndex(3);
      this->setStyleSheet("background-color:rgb(255,153,153)");
    }
    else
    {
      verificationWidgets->setCurrentIndex(0);
    }
    editButton->setEnabled(true);
    deleteButton->setEnabled(true);
  }
}

void PropertyWidget::actionAbortVerification()
{
  qDebug("abort");
}

void PropertyWidget::actionEdit()
{
  AddEditPropertyDialog* editPropertyDialog = new AddEditPropertyDialog(
      false, parent, this, property->name, property->text);

  /* if editing was succesful (Edit button was pressed), update the property and
   *   its widget */
  if (editPropertyDialog->exec())
  {
    property->name = editPropertyDialog->getPropertyName();
    property->text = editPropertyDialog->getPropertyText();
    propertyNameLabel->setText(property->name);
    fileSystem->setPropertyModified(property->name);
  }
}

void PropertyWidget::actionDelete()
{
  /* show a message box to ask the user whether he is sure to delete the
   *   property, only delete the property if the user agrees */
  if (QMessageBox::question(this, "Delete Property",
                            "Are you sure you want to delete the property " +
                                property->name + "?",
                            QMessageBox::Yes | QMessageBox::Cancel) ==
      QMessageBox::Yes)
  {
    parent->deleteProperty(this);
    delete this;
  }
}
