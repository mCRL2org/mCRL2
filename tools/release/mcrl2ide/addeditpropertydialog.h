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
#include "findandreplacedialog.h"

#include <QDialog>
#include <QRegularExpressionValidator>

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
   * @param findAndReplaceDialog The find and replace dialog
   * @param parent The parent of this widget
   */
  AddEditPropertyDialog(bool add, ProcessSystem* processSystem,
                        FileSystem* fileSystem,
                        FindAndReplaceDialog* findAndReplaceDialog,
                        QWidget* parent = 0);
  ~AddEditPropertyDialog();

  /**
   * @brief activateDialog Activates the dialog, making it visible
   * @param property Fill in a property if applicable
   */
  void activateDialog(const Property& property = Property());

  /**
   * @brief getProperty Gets the property as entered in the text fields
   * @return The new or edited property
   */
  Property getProperty();

  public slots:
  /**
   * @brief actionSaveAndParse Saves and parses the filled in property
   */
  void actionSaveAndParse();

  /**
   * @brief parseResults Handles the result of parsing the property
   * @param processid The id of a finished process
   */
  void parseResults(int processid);

  /**
   * @brief clearParseLabel Clears the parse label
   */
  void clearParseLabel();

  /**
   * @brief actionSaveAndClose Saves the filled in property and then closes the
   *   dialog
   */
  void actionSaveAndClose();

  /**
   * @brief setEquivalenceTabToModified Sets the equivalence tab to modified to
   *   when one of its child widgets becomes modified
   */
  void setEquivalenceTabToModified();

  /**
   * @brief done When done (accepted or rejected), abort the last parsing
   *   process
   * @param r Equals 0 if rejected or 1 of accepted
   */
  void done(int r) override;

  protected:
  /**
   * @brief keyPressEvent Adds key events for find next and find previous
   * @param event The key event
   */
  void keyPressEvent(QKeyEvent* event) override;

  /**
   * @brief event Handles events
   * - On closing, save the location of the dialog
   * @return Whether the event is accepted
   */
  bool event(QEvent* event) override;

  private:
  Ui::AddEditPropertyDialog* ui;

  ProcessSystem* processSystem;
  FileSystem* fileSystem;
  FindAndReplaceDialog* findAndReplaceDialog;
  QString windowTitle;
  Property oldProperty;
  int propertyParsingProcessid;
  bool lastParsingPropertyIsMucalculus;
  QRegularExpressionValidator* propertyNameValidator;
  QByteArray geometry;

  /**
   * @brief checkInput Checks whether the fields are non-empty and whether the
   *   property name isn't already in use
   * @return Whether the fields are non-empty and whether the property name
   *   isn't already in use
   */
  bool checkInput();

  /**
   * @brief saveProperty Saves the filled in property and removes the old one if
   *   applicable
   * @return Whether the property could be saved
   */
  bool saveProperty();

  /**
   * @brief resetStateAfterParsing Resets the state of the dialog after parsing
   *   is done or aborted
   */
  void resetStateAfterParsing();

  /**
   * @brief abortPropertyParsing Abort the current property parsing process
   */
  void abortPropertyParsing();

  /**
   * @brief resetModificationState Puts all fields to not modified
   */
  void resetModificationState();
};

#endif // ADDEDITPROPERTYDIALOG_H
