// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "propertywidget.h"

#include <QSpacerItem>
#include <QStyleOption>
#include <QPainter>

PropertyWidget::PropertyWidget(Property property, ProcessSystem* processSystem,
                               FileSystem* fileSystem,
                               FindAndReplaceDialog* findAndReplaceDialog,
                               QWidget* parent)
    : QWidget(parent), parent(parent), processSystem(processSystem),
      fileSystem(fileSystem), property(property), lastRunningProcessId(-1)
{
  editPropertyDialog = new AddEditPropertyDialog(
      false, processSystem, fileSystem, findAndReplaceDialog, this);

  /* create the label for the property name */
  propertyNameLabel = new QLabel(property.name);

  /* create a scrollarea to reduce the name label when needed */
  propertyNameScrollArea = new QScrollArea(this);
  propertyNameScrollArea->setWidget(propertyNameLabel);
  propertyNameScrollArea->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
  propertyNameScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  propertyNameScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  propertyNameScrollArea->setMinimumWidth(16);
  propertyNameScrollArea->setMaximumHeight(propertyNameLabel->height());
  propertyNameScrollArea->setFrameShape(QFrame::NoFrame);

  /* create the verify button */
  QPushButton* verifyButton = new QPushButton();
  verifyButton->setIcon(QIcon(":/icons/verify.png"));
  verifyButton->setIconSize(QSize(24, 24));
  verifyButton->setStyleSheet("QPushButton { padding:5px; }");
  verifyButton->setToolTip("Verify");
  connect(verifyButton, SIGNAL(clicked()), this, SLOT(actionVerify()));

  /* create the abort button for when a property is being verified */
  QPushButton* abortButton = new QPushButton();
  abortButton->setIcon(QIcon(":/icons/abort.png"));
  abortButton->setIconSize(QSize(24, 24));
  abortButton->setStyleSheet("QPushButton { padding:5px; }");
  abortButton->setToolTip("Abort verification");
  connect(abortButton, SIGNAL(clicked()), this,
          SLOT(actionAbortVerification()));

  /* create the witness button for when a property is true */
  QPushButton* witnessButton = new QPushButton();
  witnessButton->setIcon(QIcon(":/icons/witness.png"));
  witnessButton->setIconSize(QSize(24, 24));
  witnessButton->setStyleSheet("QPushButton { padding:5px; }");
  witnessButton->setToolTip("Show witness");
  connect(witnessButton, SIGNAL(clicked()), this, SLOT(actionShowEvidence()));

  /* create the counterexample button for when a property is false */
  QPushButton* counterexampleButton = new QPushButton();
  counterexampleButton->setIcon(QIcon(":/icons/counterexample.png"));
  counterexampleButton->setIconSize(QSize(24, 24));
  counterexampleButton->setStyleSheet("QPushButton { padding:5px; }");
  counterexampleButton->setToolTip("Show counterexample");
  connect(counterexampleButton, SIGNAL(clicked()), this,
          SLOT(actionShowEvidence()));

  /* stack the verification buttons */
  verificationWidgets = new QStackedWidget(this);
  verificationWidgets->setMaximumWidth(40);
  verificationWidgets->addWidget(verifyButton);         /* index = 0 */
  verificationWidgets->addWidget(abortButton);          /* index = 1 */
  verificationWidgets->addWidget(witnessButton);        /* index = 2 */
  verificationWidgets->addWidget(counterexampleButton); /* index = 3 */
  verificationWidgets->setCurrentIndex(0);

  /* create the edit button */
  editButton = new QPushButton();
  editButton->setIcon(QIcon(":/icons/edit.png"));
  editButton->setIconSize(QSize(24, 24));
  editButton->setStyleSheet("QPushButton { padding:5px; }");
  editButton->setToolTip("Edit property");
  connect(editButton, SIGNAL(clicked()), this, SLOT(actionEdit()));

  /* create the delete button */
  deleteButton = new QPushButton();
  deleteButton->setIcon(QIcon(":/icons/delete.png"));
  deleteButton->setIconSize(QSize(24, 24));
  deleteButton->setStyleSheet("QPushButton { padding:5px; }");
  deleteButton->setToolTip("Delete property");
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(actionDelete()));

  /* lay them out */
  propertyLayout = new QHBoxLayout();
  propertyLayout->addWidget(propertyNameScrollArea, 1);
  propertyLayout->addStretch();
  propertyLayout->addWidget(verificationWidgets);
  propertyLayout->addWidget(editButton);
  propertyLayout->addWidget(deleteButton);

  this->setLayout(propertyLayout);

  connect(processSystem, SIGNAL(processFinished(int)), this,
          SLOT(actionVerifyResult(int)));
  connect(processSystem, SIGNAL(processFinished(int)), this,
          SLOT(actionShowEvidenceResult(int)));
  connect(fileSystem, SIGNAL(propertyEdited(QString, Property)), this,
          SLOT(updateProperty(QString, Property)));
}

PropertyWidget::~PropertyWidget()
{
  editPropertyDialog->deleteLater();
  propertyNameLabel->deleteLater();
  for (int i = 0; i < verificationWidgets->count(); i++)
  {
    verificationWidgets->widget(i)->deleteLater();
  }
  verificationWidgets->deleteLater();
  editButton->deleteLater();
  deleteButton->deleteLater();
  propertyLayout->deleteLater();
  editPropertyDialog->deleteLater();
}

void PropertyWidget::paintEvent(QPaintEvent* event)
{
  Q_UNUSED(event);
  QStyleOption o;
  o.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
}

Property PropertyWidget::getProperty()
{
  return this->property;
}

void PropertyWidget::resetWidget()
{
  verificationWidgets->setCurrentIndex(0);
  this->setStyleSheet("");
  propertyNameScrollArea->setStyleSheet("");
  propertyNameLabel->setStyleSheet("");
}

void PropertyWidget::actionVerify()
{
  /* only verify when it hasn't been verified yet for the current specification
   *   and property */
  if (verificationWidgets->currentIndex() == 0)
  {
    resetWidget();
    /* start the verification process */
    lastRunningProcessId = processSystem->verifyProperty(property);

    /* if starting the process was successful, change the buttons */
    if (lastRunningProcessId >= 0)
    {
      lastProcessIsVerification = true;
      verificationWidgets->setCurrentIndex(1);
    }
  }
}

void PropertyWidget::actionVerifyResult(int processid)
{
  /* check if the process that has finished is the verification process of this
   *   property */
  if (processid == lastRunningProcessId && lastProcessIsVerification)
  {
    /* get the result and apply it to the widget */
    QString result = processSystem->getResult(lastRunningProcessId);
    QString styleSheet = " {background-color:rgb(%1)}";
    if (result == "true")
    {
      verificationWidgets->setCurrentIndex(2);
      styleSheet = styleSheet.arg("153,255,153");
    }
    else if (result == "false")
    {
      verificationWidgets->setCurrentIndex(3);
      styleSheet = styleSheet.arg("255,153,153");
    }
    else
    {
      styleSheet = "";
    }

    if (styleSheet.isEmpty())
    {
      resetWidget();
    }
    else
    {
      this->setStyleSheet(".PropertyWidget" + styleSheet);
      propertyNameScrollArea->setStyleSheet(".QScrollArea" + styleSheet);
      propertyNameLabel->setStyleSheet(".QLabel" + styleSheet);
    }
  }
}

void PropertyWidget::actionShowEvidence()
{
  /* creating evidence is not (yet) enabled for equivalence properties */
  if (!property.mucalculus)
  {
    executeInformationBox(this, "Not yet implemented",
                          "Creating evidence for equivalence properties has "
                          "not yet been implemented");
    return;
  }

  /* check if the property has been verified */
  if (verificationWidgets->currentIndex() > 1)
  {
    /* start the evidence creation process */
    lastRunningProcessId = processSystem->showEvidence(property);
    evidenceIsWitness = verificationWidgets->currentIndex() == 2;

    /* if starting the process was successful, change the buttons */
    if (lastRunningProcessId >= 0)
    {
      lastProcessIsVerification = false;
      verificationWidgets->setCurrentIndex(1);
    }
  }
}

void PropertyWidget::actionShowEvidenceResult(int processid)
{
  /* check if the process that has finished is the verification process of this
   *   property */
  if (processid == lastRunningProcessId && !lastProcessIsVerification)
  {
    verificationWidgets->setCurrentIndex(evidenceIsWitness ? 2 : 3);
  }
}

void PropertyWidget::actionAbortVerification()
{
  if (lastRunningProcessId > -1)
  {
    processSystem->abortProcess(lastRunningProcessId);
  }
}

void PropertyWidget::actionEdit()
{
  editPropertyDialog->activateDialog(property);
}

void PropertyWidget::updateProperty(const QString& oldPropertyName,
                                    const Property& newProperty)
{
  /* check if it was the property of this widget that changed */
  if (oldPropertyName == property.name)
  {
    property = newProperty;
    propertyNameLabel->setText(property.name);
    propertyNameLabel->adjustSize();
    actionAbortVerification();
    resetWidget();
  }
}

void PropertyWidget::actionDelete()
{
  /* show a message box to ask the user whether he is sure to delete the
   *   property */
  if (executeBinaryQuestionBox(
          parent, "Delete Property",
          "Are you sure you want to delete the property '" + property.name +
              "'?"))
  {
    actionAbortVerification();
    fileSystem->deleteProperty(property);
    emit deleteMe(this);
  }
}
