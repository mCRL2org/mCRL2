// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include "toolinstance.h"
#include "ui_toolinstance.h"

#include "filepicker.h"

#include <limits>
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
#include <QScrollBar>
#include "mcrl2/utilities/logger.h"

ToolInstance::ToolInstance(QString filename, ToolInformation information, mcrl2::gui::qt::PersistentFileDialog* fileDialog, QWidget *parent) :
  QWidget(parent),
  m_filename(filename),
  m_info(information),
  m_fileDialog(fileDialog)
{
  m_ui.setupUi(this);

  connect(this, SIGNAL(colorChanged(QColor)), this, SLOT(onColorChanged(QColor)));

  connect(&m_process, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(onStateChange(QProcess::ProcessState)));
  connect(&m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(onStandardOutput()));
  connect(&m_process, SIGNAL(readyReadStandardError()), this, SLOT(onStandardError()));
  connect(m_ui.btnRun, SIGNAL(clicked()), this, SLOT(onRun()));
  connect(m_ui.btnAbort, SIGNAL(clicked()), this, SLOT(onAbort()));
  connect(m_ui.btnSave, SIGNAL(clicked()), this, SLOT(onSave()));
  connect(m_ui.btnClear, SIGNAL(clicked()), m_ui.edtOutput, SLOT(clear()));

  QFileInfo fileInfo(filename);

  m_process.setWorkingDirectory(fileInfo.absoluteDir().absolutePath());
  m_ui.lblDirectoryValue->setText(fileInfo.absoluteDir().absolutePath());
  m_ui.lblFileValue->setText(fileInfo.fileName());

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
    m_pckFileOut = new FilePicker(m_fileDialog, m_ui.pckFileOut);
    m_ui.pckFileOut->layout()->addWidget(m_pckFileOut);
    m_pckFileOut->setText(newfile);
  }
  else
  {
    m_pckFileOut = NULL;
    m_ui.lblFileOut->setVisible(false);
    m_ui.pckFileOut->setVisible(false);
  }

  if (m_info.hasSecondInput())
  {
    m_pckFileIn = new FilePicker(m_fileDialog, m_ui.pckFileIn, false);
    m_ui.pckFileIn->layout()->addWidget(m_pckFileIn);
  }
  else
  {
    m_pckFileIn = NULL;
    m_ui.lblFileIn->setVisible(false);
    m_ui.pckFileIn->setVisible(false);
  }

  QFormLayout *formLayout = new QFormLayout();
  formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
  for (int i = 0; i < m_info.options.count(); i++)
  {
    ToolOption option = m_info.options.at(i);
    QWidget *nameOpt = NULL;
    QCheckBox* cbOpt = NULL;
    QVBoxLayout *lytOpt = new QVBoxLayout();

    if (option.argument.type == EnumArgument)
    {
      nameOpt = new QLabel("<b>"+option.nameLong+": </b>");
    }
    else
    {
      cbOpt = new QCheckBox(option.nameLong + ": ", this);
      cbOpt->setChecked(option.standard);
      QFont font(cbOpt->font());
      font.setBold(true);
      cbOpt->setFont(font);
      nameOpt = cbOpt;
    }

    formLayout->addRow(nameOpt, lytOpt);

    QLabel *lblOpt = new QLabel(option.description, this);
    lblOpt->setAlignment(Qt::AlignJustify | Qt::AlignTop);
    lblOpt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    lblOpt->setWordWrap(true);
    lytOpt->addWidget(lblOpt);

    if (!option.hasArgument())
    {
      m_optionValues.append(new OptionValue(option, cbOpt));
    }
    else
    {
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

            QWidget *edtArg = NULL;

            switch (option.argument.type)
            {
              case LevelArgument:
                {
                  QLineEdit *edtLdt = new QLineEdit("verbose", this);
                  m_optionValues.append(new OptionValue(option, cbOpt, edtLdt));
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
                    m_optionValues.append(new OptionValue(option, cbOpt, edtSpb, cbOptional));
                  }
                  else
                  {
                    m_optionValues.append(new OptionValue(option, cbOpt, edtSpb));
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
                    m_optionValues.append(new OptionValue(option, cbOpt, edtSpb, cbOptional));
                  }
                  else
                  {
                    m_optionValues.append(new OptionValue(option, cbOpt, edtSpb));
                  }
                  edtArg = edtSpb;
                }
                break;
              case BooleanArgument:
                {
                  QCheckBox *edtChb = new QCheckBox("Yes", this);
                  m_optionValues.append(new OptionValue(option, cbOpt, edtChb));
                  edtArg = edtChb;
                }
                break;
              case StringArgument:
              default:
                {
                  QLineEdit *edtLdt = new QLineEdit(this);
                  m_optionValues.append(new OptionValue(option, cbOpt, edtLdt));
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

            FilePicker *edtArg = new FilePicker(m_fileDialog, this, false);
            lytArg->addWidget(edtArg);
            m_optionValues.append(new OptionValue(option, cbOpt, edtArg));

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
            lytValues->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

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
              lytValues->setLabelAlignment(Qt::AlignLeft);
            }
            m_optionValues.append(new OptionValue(option, cbOpt, grpValues));

            lytOpt->addLayout(lytValues);
          }
          break;
        default:
          break;
      }
    }
  }
  m_ui.scrollWidget->setLayout(formLayout);
}

ToolInstance::~ToolInstance()
{
  for (auto option = m_optionValues.begin(); option != m_optionValues.end(); ++option)
  {
    delete *option;
  }
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

  if (m_pckFileOut)
  {
    QString fileOut = m_pckFileOut->text();
    if (fileOut.contains(" "))
    {
      fileOut = QString("\"%1\"").arg(fileOut);
    }
    if (!fileOut.isEmpty())
    {
      result.append(" ").append(fileOut);
    }
  }

  if (m_pckFileIn)
  {
    QString fileIn = m_pckFileIn->text();
    if (fileIn.contains(" "))
    {
      fileIn = QString("\"%1\"").arg(fileIn);
    }
    if (!fileIn.isEmpty())
    {
      result.append(" ").append(fileIn);
    }
  }

  for (int i = 0; i < m_optionValues.count(); i++)
  {
    OptionValue& val =  *m_optionValues[i];
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
      if (m_process.exitCode() == 0 && m_process.error() != QProcess::FailedToStart && m_process.error() != QProcess::Crashed && m_process.error() != QProcess::Timedout)
      {
        m_ui.lblState->setText("[Ready]");
        emit(titleChanged(fileInfo.fileName().append(" [Ready]")));
        emit(colorChanged(Qt::green));
      }
      else
      {
        m_ui.lblState->setText("[Error]");
        emit(titleChanged(fileInfo.fileName().append(" [Error]")));
        emit(colorChanged(Qt::red));
      }
      m_ui.btnAbort->setEnabled(false);
      m_ui.btnRun->setEnabled(true);
      break;
    case QProcess::Starting:
      m_ui.lblState->setText("[Starting]");
      emit(titleChanged(fileInfo.fileName().append(" [Starting]")));
      emit(colorChanged(Qt::yellow));
      m_ui.btnRun->setEnabled(false);
      m_ui.btnAbort->setEnabled(true);
      break;
    case QProcess::Running:
    default:
      m_ui.lblState->setText("[Running]");
      emit(titleChanged(fileInfo.fileName().append(" [Running]")));
      emit(colorChanged(Qt::yellow));
      m_ui.btnRun->setEnabled(false);
      m_ui.btnAbort->setEnabled(true);
      break;
  }
}

void ToolInstance::onStandardOutput()
{
  QScrollBar* scrollbar = m_ui.edtOutput->verticalScrollBar();
  bool end = scrollbar->value() == scrollbar->maximum();

  m_ui.edtOutput->setTextColor(Qt::black);
  QByteArray outText = m_process.readAllStandardOutput();
  m_ui.edtOutput->append(QString(outText).replace("\n\n", "\n"));

  if (end)
  {
    scrollbar->setValue(scrollbar->maximum());
  }
}

void ToolInstance::onStandardError()
{
  QScrollBar* scrollbar = m_ui.edtOutput->verticalScrollBar();
  bool end = scrollbar->value() == scrollbar->maximum();

  m_ui.edtOutput->setTextColor(Qt::black);
  QByteArray outText = m_process.readAllStandardError();
  m_ui.edtOutput->append(QString(outText).replace("\n\n", "\n"));

  if (end)
  {
    scrollbar->setValue(scrollbar->maximum());
  }
}

void ToolInstance::onRun()
{
  QScrollBar* scrollbar = m_ui.edtOutput->verticalScrollBar();
  bool end = scrollbar->value() == scrollbar->maximum();
  int oldValue = scrollbar->value();

  m_ui.edtOutput->setTextColor(Qt::gray);
  m_ui.edtOutput->setPlainText(m_ui.edtOutput->toPlainText());

  if (end)
  {
    scrollbar->setValue(scrollbar->maximum());
  }
  else
  {
    scrollbar->setValue(oldValue);
  }

  QString exec = executable();
  if (exec.contains(" "))
  {
    exec = QString("\"%1\"").arg(exec);
  }
  exec.append(" ").append(arguments());

  m_process.start(exec, QIODevice::ReadOnly);
  if (m_process.waitForStarted(1000))
  {
    mCRL2log(mcrl2::log::info) << "Started " << exec.toStdString() << std::endl;
    m_ui.tabWidget->setCurrentIndex(1);
  }
  else
  {
    mCRL2log(mcrl2::log::error) << m_process.errorString().toStdString() << " (" << exec.toStdString() << ")" << std::endl;
    onStateChange(QProcess::NotRunning);
  }
}

void ToolInstance::onAbort()
{
  mCRL2log(mcrl2::log::info) << "Attempting to terminate " << executable().toStdString() << std::endl;
  m_process.terminate();

  if (!m_process.waitForFinished(10000) && m_process.state() == QProcess::Running)
  {
    mCRL2log(mcrl2::log::warning) << "Killing " << executable().toStdString() << std::endl;
    m_process.kill();
  }
}

void ToolInstance::onSave()
{
  QString fileName = m_fileDialog->getSaveFileName(tr("Save output"),
                                                  tr("Text file (*.txt ) ;; All files (*.* )"));

  if (!fileName.isNull()) {
    QFile file(fileName);

    if (file.open(QFile::WriteOnly | QFile::Text))
    {
      file.write((const char *)m_ui.edtOutput->toPlainText().toLatin1().data());
      file.close();
    }
  }
}

void ToolInstance::onColorChanged(QColor color)
{
  m_ui.lblState->setStyleSheet(QString("background: %1;").arg(color.name()));
}










