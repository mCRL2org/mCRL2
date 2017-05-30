// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <QtGui>
#include <QTextEdit>

#include "findreplacedialog.h"

FindReplaceDialog::FindReplaceDialog(QWidget *parent) :
  QDialog(parent), m_textEdit(nullptr)
{
  m_ui.setupUi(this);

  m_ui.errorLabel->setText("");

  connect(m_ui.textToFind, SIGNAL(textChanged(QString)), this, SLOT(textToFindChanged()));

  connect(m_ui.findButton, SIGNAL(clicked()), this, SLOT(find()));
  connect(m_ui.closeButton, SIGNAL(clicked()), this, SLOT(close()));

  connect(m_ui.replaceButton, SIGNAL(clicked()), this, SLOT(replace()));
  connect(m_ui.replaceAllButton, SIGNAL(clicked()), this, SLOT(replaceAll()));
}

void FindReplaceDialog::setTextEdit(QTextEdit *textEdit)
{
  if (m_textEdit)
  {
    disconnect(m_textEdit, 0, m_ui.replaceButton, 0);
  }

  m_textEdit = textEdit;
  if (m_textEdit)
  {
    connect(m_textEdit, SIGNAL(copyAvailable(bool)), m_ui.replaceButton, SLOT(setEnabled(bool)));
  }
}

void FindReplaceDialog::textToFindChanged()
{
  m_ui.findButton->setEnabled(m_ui.textToFind->text().size() > 0);
}

void FindReplaceDialog::showError(const QString &error)
{
  if (error == "")
  {
    m_ui.errorLabel->setText("");
  } 
  else 
  {
    m_ui.errorLabel->setText("<span style=\" font-weight:600; color:#ff0000;\">" +
                             error +
                             "</spn>");
  }
}

void FindReplaceDialog::showMessage(const QString &message)
{
  if (message == "")
  {
    m_ui.errorLabel->setText("");
  } 
  else 
  {
    m_ui.errorLabel->setText("<span style=\" font-weight:600; color:green;\">" +
                             message +
                             "</span>");
  }
}

void FindReplaceDialog::find()
{
  find(m_ui.downRadioButton->isChecked());
}

void FindReplaceDialog::find(bool next)
{
  if (!m_textEdit)
  {
    return;
  }

  bool back = !next;
  const QString &toSearch = m_ui.textToFind->text();
  bool result = false;

  QTextDocument::FindFlags flags;

  if (back)
    flags |= QTextDocument::FindBackward;
  if (m_ui.caseCheckBox->isChecked())
    flags |= QTextDocument::FindCaseSensitively;
  if (m_ui.wholeCheckBox->isChecked())
    flags |= QTextDocument::FindWholeWords;

  result = m_textEdit->find(toSearch, flags);

  if (result)
  {
    showError("");
    return;
  } 
  
  // The string was not found, try to wrap around begin/end of file
  const QTextCursor originalPosition = m_textEdit->textCursor();
  m_textEdit->moveCursor(back ? QTextCursor::End : QTextCursor::Start);
  result = m_textEdit->find(toSearch, flags);

  if(result)
  {
    showError("");
    return;
  }

  // The string was still not found
  showError(tr("No match found"));
  // Reset the position of the cursor
  m_textEdit->setTextCursor(originalPosition);
  
}

void FindReplaceDialog::replace()
{
  if (!m_textEdit)
  {
    return;
  }
  if (!m_textEdit->textCursor().hasSelection())
  {
    find();
  } 
  else 
  {
    m_textEdit->textCursor().insertText(m_ui.textToReplace->text());
    find();
  }
}

void FindReplaceDialog::replaceAll()
{
  if (!m_textEdit)
  { 
    return;
  }

  m_textEdit->moveCursor(QTextCursor::Start);
  find(true);

  int i=0;
  while (m_textEdit->textCursor().hasSelection())
  {
    m_textEdit->textCursor().insertText(m_ui.textToReplace->text());
    find();
    i++;
  }
  showMessage(tr("Replaced %1 occurrence(s)").arg(i));
}
