// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "toolinstance.h"
#include "ui_toolinstance.h"

#include "filepicker.h"

#include <limits>
#include <QDebug>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QSpacerItem>
#include <QFileDialog>
#include "mcrl2/utilities/logger.h"

ToolInstance::ToolInstance(QString filename, ToolInformation information, QWidget *parent) :
  QWidget(parent),
  m_filename(filename),
  m_info(information),
  ui(new Ui::ToolInstance)
{
  ui->setupUi(this);

  connect(&m_process, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(onStateChange(QProcess::ProcessState)));
  connect(&m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(onStandardOutput()));
  connect(&m_process, SIGNAL(readyReadStandardError()), this, SLOT(onStandardError()));
  connect(ui->btnRun, SIGNAL(clicked()), this, SLOT(onRun()));
  connect(ui->btnAbort, SIGNAL(clicked()), this, SLOT(onAbort()));
  connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(onSave()));

  QFileInfo fileInfo(filename);

  m_process.setWorkingDirectory(fileInfo.absoluteDir().absolutePath());
  ui->lblDirectoryValue->setText(fileInfo.absoluteDir().absolutePath());
  ui->lblFileValue->setText(fileInfo.fileName());

  if (m_info.hasOutput())
  {
    QDir dir = fileInfo.absoluteDir();
    QString newfile = fileInfo.baseName().append(".%1").arg(m_info.output);
    int filenr = 0;
    while(dir.exists(newfile))
    {
      filenr++;
      newfile = fileInfo.baseName().append("_%1.%2").arg(filenr).arg(m_info.output);
    }
    ui->pckFileOut->setText(newfile);
  }
  else
  {
    ui->lblFileOut->setVisible(false);
    ui->pckFileOut->setVisible(false);
  }

  for (int i = 0; i < m_info.options.count(); i++)
  {
    ToolOption option = m_info.options.at(i);
    QCheckBox *cbOpt = NULL;

    if (option.argument.type != EnumArgument)
    {
        cbOpt = new QCheckBox(option.nameLong, this);
        cbOpt->setChecked(option.standard);
    }

    QLabel *lblOpt = new QLabel(option.description, this);
    lblOpt->setWordWrap(true);

    if (!option.hasArgument())
    {
      ui->frmOptions->addRow(cbOpt, lblOpt);
      m_optionValues.append(OptionValue(option, cbOpt));
    }
    else
    {
      QVBoxLayout *lytOpt = new QVBoxLayout();
      lytOpt->setSpacing(10);

      lytOpt->addWidget(lblOpt);

      switch (option.argument.type)
      {
        case StringArgument:
        case LevelArgument:
        case IntegerArgument:
        case RealArgument:
        case BooleanArgument:
          {
            QHBoxLayout *lytArg = new QHBoxLayout();
            lytArg->setSpacing(6);

//            QLabel *lblArg = new QLabel(option.argument.name, this);
//            lblArg->setMinimumWidth(100);
//            lytArg->addWidget(lblArg);

            QWidget *edtArg = NULL;

            switch (option.argument.type)
            {
              case LevelArgument:
                {
                  QLineEdit *edtLdt = new QLineEdit("verbose", this);
                  m_optionValues.append(OptionValue(option, cbOpt, edtLdt));
                  edtArg = edtLdt;
                }
                break;
              case IntegerArgument:
                {
                  QSpinBox *edtSpb = new QSpinBox(this);
                  edtSpb->setRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
                  if (option.argument.optional)
                  {
                    QCheckBox *cbOptional = new QCheckBox(this);
                    lytArg->addWidget(cbOptional);
                    m_optionValues.append(OptionValue(option, cbOpt, edtSpb, cbOptional));
                  }
                  else
                  {
                    m_optionValues.append(OptionValue(option, cbOpt, edtSpb));
                  }
                  edtArg = edtSpb;
                }
                break;
              case RealArgument:
                {
                  QDoubleSpinBox *edtSpb = new QDoubleSpinBox(this);
                  edtSpb->setRange(std::numeric_limits<double>::min(), std::numeric_limits<double>::max());
                  if (option.argument.optional)
                  {
                    QCheckBox *cbOptional = new QCheckBox(this);
                    lytArg->addWidget(cbOptional);
                    m_optionValues.append(OptionValue(option, cbOpt, edtSpb, cbOptional));
                  }
                  else
                  {
                    m_optionValues.append(OptionValue(option, cbOpt, edtSpb));
                  }
                  edtArg = edtSpb;
                }
                break;
              case BooleanArgument:
                {
                  QCheckBox *edtChb = new QCheckBox("Yes", this);
                  m_optionValues.append(OptionValue(option, cbOpt, edtChb));
                  edtArg = edtChb;
                }
                break;
              case StringArgument:
              default:
                {
                  QLineEdit *edtLdt = new QLineEdit(this);
                  m_optionValues.append(OptionValue(option, cbOpt, edtLdt));
                  edtArg = edtLdt;
                }
                break;
            }
            edtArg->setMinimumWidth(300);

            lytArg->addWidget(edtArg);

            if (!option.argument.optional && option.argument.type != BooleanArgument)
            {
              QLabel *lblReq = new QLabel("*", this);
              lytArg->addWidget(lblReq);
            }

            QSpacerItem *spacer = new QSpacerItem(100, 20, QSizePolicy::Expanding);
            lytArg->addItem(spacer);
            lytOpt->addLayout(lytArg);
          }
          break;
        case FileArgument:
          {
            QHBoxLayout *lytArg = new QHBoxLayout();
            lytArg->setSpacing(6);

//            QLabel *lblArg = new QLabel(option.argument.name, this);
//            lblArg->setMinimumWidth(100);
//            lytArg->addWidget(lblArg);

            FilePicker *edtArg = new FilePicker(this);
            lytArg->addWidget(edtArg);
            m_optionValues.append(OptionValue(option, cbOpt, edtArg));

            if (!option.argument.optional)
            {
              QLabel *lblReq = new QLabel("*", this);
              lytArg->addWidget(lblReq);
            }

            QSpacerItem *spacer = new QSpacerItem(100, 20, QSizePolicy::Expanding);
            lytArg->addItem(spacer);
            lytOpt->addLayout(lytArg);
          }
          break;
        case EnumArgument:
          {
            QFormLayout *lytValues = new QFormLayout();
            lytValues->setSpacing(6);

            QButtonGroup *grpValues = new QButtonGroup(this);

            for (int j = 0; j < option.argument.values.count(); j++)
            {
              ToolValue val = option.argument.values.at(j);
              QRadioButton *rbVal = new QRadioButton(val.nameLong, this);
              rbVal->setChecked(val.standard);
              grpValues->addButton(rbVal);

              QLabel *lblVal = new QLabel(val.description, this);
              lblVal->setWordWrap(true);

              lytValues->addRow(rbVal, lblVal);
            }
            m_optionValues.append(OptionValue(option, cbOpt, grpValues));

            lytOpt->addLayout(lytValues);
          }
          break;
        default:
          break;
      }
      if (cbOpt != NULL)
      {
        ui->frmOptions->addRow(cbOpt, lytOpt);
      }
      else
      {
        ui->frmOptions->addRow(option.nameLong, lytOpt);
      }
    }
  }
}

ToolInstance::~ToolInstance()
{
  delete ui;
}

QString ToolInstance::executable()
{
  return m_info.path;
}

QString ToolInstance::arguments()
{
  QFileInfo info(m_filename);
  QString result = info.fileName();

  if (result.contains(" "))
  {
    result = QString("\"%1\"").arg(result);
  }

  QString fileOut = ui->pckFileOut->text();
  if (!fileOut.isEmpty())
  {
    if (fileOut.contains(" "))
    {
      fileOut = QString("\"%1\"").arg(fileOut);
    }
    result.append(" ").append(fileOut);
  }

  for (int i = 0; i < m_optionValues.count(); i++)
  {
    OptionValue val =  m_optionValues.at(i);
    if (!val.value().isEmpty())
    {
      result.append(" ").append(val.value());
    }
  }

  return result;
}

void ToolInstance::onStateChange(QProcess::ProcessState state)
{
  QFileInfo fileInfo(m_filename);

  switch (state)
  {
    case QProcess::NotRunning:
      emit(titleChanged(fileInfo.fileName().append(" [Ready]")));
      ui->btnAbort->setEnabled(false);
      ui->btnRun->setEnabled(true);
      break;
    case QProcess::Starting:
      emit(titleChanged(fileInfo.fileName().append(" [Starting]")));
      ui->btnRun->setEnabled(false);
      ui->btnAbort->setEnabled(true);
      break;
    case QProcess::Running:
    default:
      emit(titleChanged(fileInfo.fileName().append(" [Running]")));
      ui->btnRun->setEnabled(false);
      ui->btnAbort->setEnabled(true);
      break;
  }
}

void ToolInstance::onStandardOutput()
{
  QByteArray outText = m_process.readAllStandardOutput();
  ui->edtOutput->appendPlainText(QString(outText));
}

void ToolInstance::onStandardError()
{
  QByteArray outText = m_process.readAllStandardError();
  ui->edtOutput->appendPlainText(QString(outText));
}

void ToolInstance::onRun()
{
  m_process.start(executable().append(" ").append(arguments()), QIODevice::ReadOnly);
  if (m_process.waitForStarted(1000))
  {
    mCRL2log(mcrl2::log::info) << "Started " << executable().append(" ").append(arguments()).toStdString() << std::endl;
    ui->tabWidget->setCurrentIndex(1);
  }
  else
  {
    mCRL2log(mcrl2::log::error) << m_process.errorString().toStdString() << " (" << executable().append(" ").append(arguments()).toStdString() << ")" << std::endl;
  }
}

void ToolInstance::onAbort()
{
  mCRL2log(mcrl2::log::info) << "Attempting to terminate " << executable().toStdString() << std::endl;
  m_process.terminate();

  if (!m_process.waitForFinished(1000) && m_process.state() == QProcess::Running)
  {
    mCRL2log(mcrl2::log::info) << "Killing " << executable().toStdString() << std::endl;
    m_process.kill();
  }
}

void ToolInstance::onSave()
{
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save output"), QString(),
                                          tr("Text file (*.txt ) ;; All files (*.* )"));

  if (!fileName.isNull()) {
    QFile file(fileName);

    if (file.open(QFile::WriteOnly | QFile::Text))
    {
      file.write((const char *)ui->edtOutput->toPlainText().toAscii().data());
      file.close();
    }
  }
}











