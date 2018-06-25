// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ADDEDITPROPERTYDIALOG_H
#define ADDEDITPROPERTYDIALOG_H

#include "propertiesdock.h"

#include <QDialog>

namespace Ui
{
class AddEditPropertyDialog;
}

/**
 * @brief The AddEditPropertyDialog class defines the dialog used to add or edit
 *   a property
 */
class AddEditPropertyDialog : public QDialog
{
  Q_OBJECT

  public:
  /**
   * @brief AddEditPropertyDialog Constructor
   * @param add Whether this should be an add (or else an edit) window
   * @param propertiesDock The 
   * @param parent The parent of this widget
   * @param propertyName The current name of the property (in case of edit)
   * @param propertyText The current text of the property (in case of edit)
   */
  AddEditPropertyDialog(bool add, FileSystem* fileSystem,
                        QWidget* parent = 0, QString propertyName = "",
                        QString propertyText = "");
  ~AddEditPropertyDialog();

  /**
   * @brief getPropertyName Gets the string filled in as the property name
   * @return The property name string
   */
  QString getPropertyName();

  /**
   * @brief getPropertyText Gets the string filled in as the property text
   * @return The preoperty text string
   */
  QString getPropertyText();

  public slots:
  /**
   * @brief parseAndAccept checks whether the fields are non-empty and the
   *   property is valid before accepting (parsing has yet to be implemented)
   */
  void parseAndAccept();

  private:
  Ui::AddEditPropertyDialog* ui;

  FileSystem* fileSystem;
  QString windowTitle;
};

#endif // ADDEDITPROPERTYDIALOG_H
