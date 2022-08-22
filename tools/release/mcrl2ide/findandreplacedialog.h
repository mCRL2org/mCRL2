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
   * @param editor The editor to find/replace in
   * @param parent The parent of this widget
   */
  explicit FindAndReplaceDialog(QPlainTextEdit* editor, QWidget* parent = 0);
  ~FindAndReplaceDialog();

  /**
   * @brief resetAndFocus Resets the focus to the find and replace window. The
   *   find and replace fields are not reset for reuse, unless there is selected
   *   text, in which case this is put in the find field.
   */
  void resetFocus();

  /**
   * @brief findNext Find the next string in the editor
   * @param down Whether the direction to find is down
   */
  void findNext(bool down);

  public slots:
  /**
   * @brief setFindEnabled Enables or disables the "Find" button whenever the
   *   text in the find field changes
   */
  void setFindEnabled();

  /**
   * @brief setReplaceEnabled Enables or disables the "Replace" button whenever
   *   the text in the find field changes, whenever the selection in the text
   *   editor has changed and whenever the "Case sensitive" box is (un)ticked
   */
  void setReplaceEnabled();

  /**
   * @brief setReplaceAllEnabled Enables or disables the "Replace All" button
   * whenever the text in the find field changes
   */
  void setReplaceAllEnabled();

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

  /**
   * @brief updateEditorAndActions Update the referred to editor whenever the
   *   focus changes to a new widget
   * @param widget The widget that is in focus
   */
  void updateEditor(QWidget*, QWidget* widget);

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
  Ui::FindAndReplaceDialog* ui;

  QPlainTextEdit* editor;
  QByteArray geometry;

  /**
   * @brief showMessage Shows a message on the dialog
   * @param message The message to show
   * @param error Whether the message is an error message
   */
  void showMessage(const QString& message, bool error = false);
};

#endif // FINDANDREPLACEDIALOG_H
