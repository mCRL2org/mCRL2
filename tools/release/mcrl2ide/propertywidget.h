// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef PROPERTYWIDGET_H
#define PROPERTYWIDGET_H

#include "processsystem.h"
#include "filesystem.h"
#include "addeditpropertydialog.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QProcess>
#include <QStackedWidget>
#include <QScrollArea>

/**
 * @brief The PropertyWidget class defines a widget for a single property in the
 *   properties dock
 */
class PropertyWidget : public QWidget
{
  Q_OBJECT

  public:
  /**
   * @brief PropertyWidget Constructor
   * @param property The property of this widget
   * @param processSystem The process system
   * @param fileSystem The file system
   * @param parent The parent of this widget
   */
  PropertyWidget(Property property, ProcessSystem* processSystem,
                 FileSystem* fileSystem, QWidget* parent);
  ~PropertyWidget();

  /**
   * @brief getPropertyName Gets the property of this widget
   * @return The property
   */
  Property getProperty();

  /**
   * @brief resetWidget Resets the widget to unverified state
   */
  void resetWidget();

  public slots:
  /**
   * @brief actionVerify Allows the user to verify this property
   */
  void actionVerify();

  /**
   * @brief actionVerifyResult Applies the result of the verification
   * @param processid The id of a finished process
   */
  void actionVerifyResult(int processid);

  /**
   * @brief actionShowEvidence Allows the user to show evidence for this
   *   property
   */
  void actionShowEvidence();

  /**
   * @brief actionShowEvidenceResult Changes widget when showing evidence has
   *   finished
   * @param processid The id of a finished process
   */
  void actionShowEvidenceResult(int processid);

  /**
   * @brief actionAbortVerification Allows the user to abort the verification
   */
  void actionAbortVerification();

  /**
   * @brief actionEdit Allows the user to edit this property
   */
  void actionEdit();

  /**
   * @brief actionEditResult Handles the result of editing a property
   */
  void actionEditResult();

  /**
   * @brief actionDelete Allows the user to delete this property
   */
  void actionDelete();

  signals:
  /**
   * @brief deleteMe Signals the properties dock that this widgets needs to be
   *   deleted
   * @param propertyWidget This widget
   */
  void deleteMe(PropertyWidget* thisWidget);

  protected:
  /**
   * @brief paintEvent Makes sure that the widget is painted correctly
   * @param event A paint event
   */
  void paintEvent(QPaintEvent* event) override;

  private:
  QWidget* parent;
  ProcessSystem* processSystem;
  FileSystem* fileSystem;
  Property property;

  QHBoxLayout* propertyLayout;
  QLabel* propertyNameLabel;
  QScrollArea* propertyNameScrollArea;
  QStackedWidget* verificationWidgets;
  QPushButton* editButton;
  QPushButton* deleteButton;

  AddEditPropertyDialog* editPropertyDialog;

  int lastRunningProcessId;
  bool lastProcessIsVerification;
  bool evidenceIsWitness;
};

#endif // PROPERTYWIDGET_H
