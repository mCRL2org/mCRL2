// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef FINDANDREPLACEDIALOG_H
#define FINDANDREPLACEDIALOG_H

#include <QDialog>
#include "mcrl2/gui/codeeditor.h"

namespace Ui
{
class FindAndReplaceDialog;
}

/**
 * @brief The FindAndReplaceDialog class defines a dialog for finding and
 *   replacing text in a given code editor
 */
class FindAndReplaceDialog : public QDialog
{
  Q_OBJECT

  public:
  /**
   * @brief FindAndReplaceDialog Constructor
   * @param codeEditor The editor to find/replace in
   * @param parent The parent of this widget
   */
  explicit FindAndReplaceDialog(mcrl2::gui::qt::CodeEditor* codeEditor,
                                QWidget* parent = 0);
  ~FindAndReplaceDialog();

  /**
   * @brief resetAndFocus Resets the focus to the find and replace window. The
   *   find and replace fields are not reset for reuse, unless there is selected
   *   text, in which case this is put in the find field.
   */
  void resetFocus();

  public slots:
  /**
   * @brief setFindEnabled Is called when the text in the find field changes
   * Enables or disables the find button
   */
  void setFindEnabled();

  /**
   * @brief setReplaceEnabled Is called when the selection in the text editor
   *   has changed
   * Enables or disables the replace button
   */
  void setReplaceEnabled();

  /**
   * @brief actionFind Allows the user to find a string in the editor
   * @param forReplaceAll Whether we are finding for replace all
   * This means that searching must go down and no wrap around is done
   */
  void actionFind(bool forReplaceAll = false);

  /**
   * @brief actionReplace Allows the user to replace a string in the editor
   */
  void actionReplace();

  /**
   * @brief actionReplaceAll Allows the user to replace all occurrences of a
   *   string in the editor
   */
  void actionReplaceAll();

  private:
  Ui::FindAndReplaceDialog* ui;

  mcrl2::gui::qt::CodeEditor* codeEditor;

  /**
   * @brief showMessage Shows a message on the dialog
   * @param message The message to show
   * @param error Whether the message is an error message
   */
  void showMessage(const QString& message, bool error = false);
};

#endif // FINDANDREPLACEDIALOG_H
