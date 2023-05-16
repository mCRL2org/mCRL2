// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef TOOLINSTANCE_H
#define TOOLINSTANCE_H

#include "ui_toolinstance.h"

#include "mcrl2/gui/utilities.h"

#include "optionvalue.h"
#include "multiprocess.h"

class ToolInstance : public QWidget
{
    Q_OBJECT

  public:
    explicit ToolInstance(QString filename, ToolInformation information, mcrl2::gui::qt::PersistentFileDialog* fileDialog, QWidget *parent = 0);
    ~ToolInstance();

    ToolInformation information() { return m_info; }
    QString executable();
    QStringList arguments(bool addQuotesAroundValuesWithSpaces);

  public slots:
    void onStateChange(QProcess::ProcessState state);
    void onOutputLog(const QString &outText);
    void onErrorLog(const QString &outText);
    void onRun();
    void onAbort();
    void onSave();

    void onColorChanged(QColor color);

  private:
    QString m_filename;
    ToolInformation m_info;
    Ui::ToolInstance m_ui;

    QList<OptionValue*> m_optionValues;
    QProcess* m_process;
    QMultiProcess* m_mprocess;

    FilePicker* m_pckFileOut;
    FilePicker* m_pckFileIn;
    mcrl2::gui::qt::PersistentFileDialog* m_fileDialog;

  signals:
    void titleChanged(QString title);
    void colorChanged(QColor color);

};

#endif // TOOLINSTANCE_H
