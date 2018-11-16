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

#include "processsystem.h"
#include "filesystem.h"

#include <QDialog>
#include <QRegExpValidator>

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
   * @param processSystem The process system (to parse formulas)
   * @param fileSystem The file system (to check property names)
   * @param parent The parent of this widget
   */
  AddEditPropertyDialog(bool add, ProcessSystem* processSystem,
                        FileSystem* fileSystem, QWidget* parent = 0);
  ~AddEditPropertyDialog();

  /**
   * @brief resetFocus Puts the focus on the add/edit button and the property
   *   name field
   */
  void resetFocus();

  /**
   * @brief clearFields Empties the property name and text fields
   */
  void clearFields();

  /**
   * @brief setProperty Sets the property in the text fields
   * @param property The property to fill in
   */
  void setProperty(const Property& property);

  /**
   * @brief getProperty Gets the property as entered in the text fields
   * @return The new or edited property
   */
  Property getProperty();

  /**
   * @brief setOldProperty Records the property as when editing began
   * @param oldProperty The property as when editing began
   */
  void setOldProperty(const Property& oldProperty);

  public slots:
  /**
   * @brief parseProperty Parse the filled in property
   */
  void parseProperty();

  /**
   * @brief parseResults Handles the result of parsing the property
   * @param processid The id of a finished process
   */
  void parseResults(int processid);

  /**
   * @brief addEditProperty Finishes adding/editing the property by saving the
   *   property (if the input is ok)
   */
  void addEditProperty();

  /**
   * @brief onRejected On rejected (Cancel, escape, "X"), abort the last parsing
   *   process, reset the property and clean up the properties folder
   */
  void onRejected();

  private:
  Ui::AddEditPropertyDialog* ui;

  ProcessSystem* processSystem;
  FileSystem* fileSystem;
  QString windowTitle;
  Property oldProperty;
  int propertyParsingProcessid;
  QRegExpValidator* propertyNameValidator;

  /**
   * @brief checkInput Checks whether the fields are non-empty and whether the
   *   property name isn't already in use
   * @return Whether the fields are non-empty and whether the property name
   *   isn't already in use
   */
  bool checkInput();

  /**
   * @brief abortPropertyParsing Abort the current property parsing process
   */
  void abortPropertyParsing();
};

#endif // ADDEDITPROPERTYDIALOG_H
