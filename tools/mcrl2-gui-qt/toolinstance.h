// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef TOOLINSTANCE_H
#define TOOLINSTANCE_H

#include <QWidget>
#include "ui_toolinstance.h"

#include "mcrl2/utilities/persistentfiledialog.h"

#include "toolinformation.h"
#include "optionvalue.h"

class ToolInstance : public QWidget
{
    Q_OBJECT
    
  public:
    explicit ToolInstance(QString filename, ToolInformation information, mcrl2::utilities::qt::PersistentFileDialog* fileDialog, QWidget *parent = 0);

    ToolInformation information() { return m_info; }
    QString executable();
    QString arguments();

  public slots:
    void onStateChange(QProcess::ProcessState state);
    void onStandardOutput();
    void onStandardError();
    void onRun();
    void onAbort();
    void onSave();

    void onColorChanged(QColor color);

  private:
    QString m_filename;
    ToolInformation m_info;
    Ui::ToolInstance m_ui;

    QList<OptionValue> m_optionValues;
    QProcess m_process;

    FilePicker* m_pckFileOut;
    mcrl2::utilities::qt::PersistentFileDialog* m_fileDialog;

  signals:
    void titleChanged(QString title);
    void colorChanged(QColor color);

};

#endif // TOOLINSTANCE_H
