// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "filepicker.h"

#include <QFileDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QList>
#include <QUrl>

FilePicker::FilePicker(mcrl2::gui::qt::PersistentFileDialog* fileDialog, QWidget *parent, bool save) :
  QWidget(parent),
  m_fileDialog(fileDialog)
{
  m_ui.setupUi(this);

  m_save=save;

  connect(m_ui.btnBrowse, SIGNAL(clicked()), this, SLOT(onBrowse()));
  connect(m_ui.value, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));

}

void FilePicker::onBrowse()
{
  QString fileName;

  if(m_save)
  {
    fileName = m_fileDialog->getSaveFileName(tr("Select file"),
                                                tr("All files (*.* )"));
  }
  else
  {
     fileName = m_fileDialog->getOpenFileName(tr("Select file"),
                                                   tr("All files (*.* )"));
  }

  if (!fileName.isNull())
    m_ui.value->setText(fileName);
}

void FilePicker::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->mimeData()->hasUrls())
    event->acceptProposedAction();
}

void FilePicker::dropEvent(QDropEvent *event)
{
  if (event->mimeData()->hasUrls())
  {
    QList<QUrl> urls = event->mimeData()->urls();

    if (urls.count() == 1)
    {
      m_ui.value->setText(urls.at(0).toLocalFile());
      event->accept();
    }
  }
}
