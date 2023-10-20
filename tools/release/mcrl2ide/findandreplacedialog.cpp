// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "findandreplacedialog.h"
#include "ui_findandreplacedialog.h"

FindAndReplaceDialog::FindAndReplaceDialog(QPlainTextEdit* editor,
                                           QWidget* parent)
    : QDialog(parent), ui(new Ui::FindAndReplaceDialog), editor(editor)
{
  ui->setupUi(this);

  setWindowFlags(Qt::Dialog);

  /* update the text editor whenever the focus changes */
  connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this,
          SLOT(updateEditor(QWidget*, QWidget*)));

  /* update the enabledness of the buttons */
  connect(ui->textToFind, SIGNAL(textChanged(QString)), this,
          SLOT(setFindEnabled()));
  connect(ui->textToFind, SIGNAL(textChanged(QString)), this,
          SLOT(setReplaceEnabled()));
  connect(editor, SIGNAL(selectionChanged()), this, SLOT(setReplaceEnabled()));
  connect(ui->caseCheckBox, SIGNAL(stateChanged(int)), this,
          SLOT(setReplaceEnabled()));
  connect(ui->textToFind, SIGNAL(textChanged(QString)), this,
          SLOT(setReplaceAllEnabled()));

  /* map the functionality of the buttons */
  connect(ui->findButton, SIGNAL(clicked()), this, SLOT(actionFind()));
  connect(ui->replaceButton, SIGNAL(clicked()), this, SLOT(actionReplace()));
  connect(ui->replaceAllButton, SIGNAL(clicked()), this,
          SLOT(actionReplaceAll()));
  connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

void FindAndReplaceDialog::showMessage(const QString& message, bool error)
{
  if (error)
  {
    ui->infoLabel->setStyleSheet("color:red");
  }
  else
  {
    ui->infoLabel->setStyleSheet("color:green");
  }
  ui->infoLabel->setText(message);
}

void FindAndReplaceDialog::resetFocus()
{
  ui->textToFind->setFocus();
  QString selection = editor->textCursor().selectedText();
  if (!selection.isEmpty())
  {
    ui->textToFind->setText(selection);
  }

  if (isVisible())
  {
    setFocus();
    ui->textToFind->setFocus();
    ui->textToFind->selectAll();
    activateWindow();
    raise(); // for MacOS
  }
  else
  {
    restoreGeometry(geometry);
    show();
  }
}

void FindAndReplaceDialog::setFindEnabled()
{
  ui->findButton->setEnabled(ui->textToFind->text().size() > 0);
}

void FindAndReplaceDialog::setReplaceEnabled()
{
  Qt::CaseSensitivity flag =
      ui->caseCheckBox->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
  bool enable = !editor->isReadOnly() && ui->textToFind->text().size() > 0 &&
                QString::compare(editor->textCursor().selectedText(),
                                 ui->textToFind->text(), flag) == 0;
  ui->replaceButton->setEnabled(enable);
}

void FindAndReplaceDialog::setReplaceAllEnabled()
{
  bool enable = !editor->isReadOnly() && ui->textToFind->text().size() > 0;
  ui->replaceAllButton->setEnabled(enable);
}

void FindAndReplaceDialog::actionFind(bool forReplaceAll)
{
  bool back = !forReplaceAll && ui->upRadioButton->isChecked();
  QString toSearch = ui->textToFind->text();
  bool result = false;
  QTextCursor originalPosition = editor->textCursor();

  QTextDocument::FindFlags flags;

  if (back)
  {
    flags |= QTextDocument::FindBackward;
  }
  if (ui->caseCheckBox->isChecked())
  {
    flags |= QTextDocument::FindCaseSensitively;
  }
  if (ui->wholeCheckBox->isChecked())
  {
    flags |= QTextDocument::FindWholeWords;
  }

  result = editor->find(toSearch, flags);

  /* if found, we are done */
  if (result)
  {
    showMessage("");
    return;
  }

  if (!forReplaceAll)
  {
    /* if the string was not found, try to wrap around begin/end of file */
    editor->moveCursor(back ? QTextCursor::End : QTextCursor::Start);
    result = editor->find(toSearch, flags);

    /* if found, we are done and tell the user that we have wrapped around */
    if (result)
    {
      if (back)
      {
        showMessage("Found the last occurrence");
      }
      else
      {
        showMessage("Found the first occurrence");
      }
      return;
    }
  }

  /* if the string was still not found, mention it and reset the cursor */
  showMessage("No match found", true);
  editor->setTextCursor(originalPosition);
}

void FindAndReplaceDialog::findNext(bool down)
{
  if (!ui->textToFind->text().isEmpty())
  {
    if (!this->isVisible())
    {
      this->show();
    }
    if (down)
    {
      ui->downRadioButton->setChecked(true);
    }
    else
    {
      ui->upRadioButton->setChecked(true);
    }
    this->actionFind();
  }
}

void FindAndReplaceDialog::actionReplace()
{
  editor->textCursor().insertText(ui->textToReplace->text());
  actionFind();
}

void FindAndReplaceDialog::actionReplaceAll()
{
  QTextCursor originalPosition = editor->textCursor();
  originalPosition.beginEditBlock();

  editor->moveCursor(QTextCursor::Start);
  actionFind(true);

  int i = 0;
  while (editor->textCursor().hasSelection())
  {
    editor->textCursor().insertText(ui->textToReplace->text());
    actionFind(true);
    i++;
  }

  originalPosition.endEditBlock();
  showMessage(tr("Replaced %1 occurrence(s)").arg(i));

  editor->setTextCursor(originalPosition);
}

void FindAndReplaceDialog::updateEditor(QWidget*, QWidget* widget)
{
  QPlainTextEdit* textwidget = qobject_cast<QPlainTextEdit*>(widget);
  if (textwidget != nullptr)
  {
    disconnect(editor, SIGNAL(selectionChanged()), this,
               SLOT(setReplaceEnabled()));
    editor = textwidget;
    connect(editor, SIGNAL(selectionChanged()), this,
            SLOT(setReplaceEnabled()));

    setFindEnabled();
    setReplaceEnabled();
    setReplaceAllEnabled();
  }
}

void FindAndReplaceDialog::keyPressEvent(QKeyEvent* event)
{
  /* Do find next or find previous if the corresponding keys are pressed */
  if (event->matches(QKeySequence::FindNext))
  {
    this->findNext(true);
  }
  else if (event->matches(QKeySequence::FindPrevious))
  {
    this->findNext(false);
  }
  else
  {
    QDialog::keyPressEvent(event);
  }
}

bool FindAndReplaceDialog::event(QEvent* event)
{
  if (event->type() == QEvent::Close)
  {
    geometry = saveGeometry();
  }
  return QDialog::event(event);
}

FindAndReplaceDialog::~FindAndReplaceDialog()
{
  delete ui;
}
