// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include "toolinstance.h"

#include <cassert>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSpacerItem>
#include <QScrollBar>
#include "mcrl2/utilities/logger.h"

ToolInstance::ToolInstance(QString filename, ToolInformation information, mcrl2::gui::qt::PersistentFileDialog* fileDialog, QWidget *parent) :
  QWidget(parent),
  m_filename(filename),
  m_info(information),
  m_fileDialog(fileDialog)
{
  m_ui.setupUi(this);

  if (m_info.guiTool)
  {
    // GUI-based tools run half-detached in the sense they are not terminated
    // when the tab is closed, and multiple instance may be created from the
    // same tab.
    // m_process will be set to the last spawned process
    m_process = m_mprocess = new QMultiProcess();

    connect(m_mprocess, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(onStateChange(QProcess::ProcessState)));
    connect(m_mprocess, &QProcess::readyReadStandardOutput, this,
      [this]() { onOutputLog(m_mprocess->readAllStandardOutput()); });
    connect(m_process, &QProcess::readyReadStandardError, this,
      [this]() { onErrorLog(m_mprocess->readAllStandardError()); });
  }
  else
  {
    m_process = new QProcess();
    m_mprocess = nullptr;

    connect(m_process, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(onStateChange(QProcess::ProcessState)));
    connect(m_process, &QProcess::readyReadStandardOutput, this,
      [this]() { onOutputLog(m_process->readAllStandardOutput()); });
    connect(m_process, &QProcess::readyReadStandardError, this,
      [this]() { onErrorLog(m_process->readAllStandardError()); });
  }

  connect(this, SIGNAL(colorChanged(QColor)), this, SLOT(onColorChanged(QColor)));

  connect(m_ui.btnRun, SIGNAL(clicked()), this, SLOT(onRun()));
  connect(m_ui.btnAbort, SIGNAL(clicked()), this, SLOT(onAbort()));
  connect(m_ui.btnSave, SIGNAL(clicked()), this, SLOT(onSave()));
  connect(m_ui.btnClear, SIGNAL(clicked()), m_ui.edtOutput, SLOT(clear()));

  QFileInfo fileInfo(filename);

  m_process->setWorkingDirectory(fileInfo.absoluteDir().absolutePath());
  m_ui.lblDirectoryValue->setText(fileInfo.absoluteDir().absolutePath());
  m_ui.lblFileValue->setText(fileInfo.fileName());

  if (m_info.hasOutput())
  {
    QDir dir = fileInfo.absoluteDir();
    QString newfile = fileInfo.completeBaseName().append(".%1").arg(m_info.output);
    int filenr = 0;
    while(dir.exists(newfile))
    {
      filenr++;
      newfile = fileInfo.completeBaseName().append("_%1.%2").arg(filenr).arg(m_info.output);
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

  // Gui-based tools have a 'Open' button instead of a 'Run'.
  if (m_info.guiTool)
  {
    m_ui.btnRun->setText("Open");
  }

  QFormLayout *formLayout = new QFormLayout();
  formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
  for (int i = 0; i < m_info.options.count(); i++)
  {
    ToolOption option = m_info.options.at(i);
    QWidget *nameOpt = NULL;
    QCheckBox* cbOpt = NULL;
    QVBoxLayout *lytOpt = new QVBoxLayout();

    cbOpt = new QCheckBox(option.nameLong + ": ", this);
    cbOpt->setChecked(option.standard);
    QFont font(cbOpt->font());
    font.setBold(true);
    cbOpt->setFont(font);
    nameOpt = cbOpt;

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
              rbVal->setProperty("valueName", val.nameLong);
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
  if (m_info.guiTool)
    delete m_mprocess;
  else
    delete m_process;
  m_process = nullptr;
  m_mprocess = nullptr;
}

QString ToolInstance::executable()
{
  return m_info.path;
}

QStringList ToolInstance::arguments(bool addQuotesAroundValuesWithSpaces)
{
  QFileInfo info(m_filename);
  QStringList result(info.fileName());

  if (m_pckFileOut)
  {
    QString fileOut = m_pckFileOut->text();
    if (!fileOut.isEmpty())
    {
      result.append(fileOut);
    }
  }

  if (m_pckFileIn)
  {
    QString fileIn = m_pckFileIn->text();
    if (!fileIn.isEmpty())
    {
      result.append(fileIn);
    }
  }

  for (int i = 0; i < m_optionValues.count(); i++)
  {
    OptionValue& val =  *m_optionValues[i];
    QString valueAsString = val.value(addQuotesAroundValuesWithSpaces);
    if (!valueAsString.isEmpty())
    {
      result.append(valueAsString);
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
      if (m_process->exitCode() == 0
        && m_process->error() != QProcess::FailedToStart
        && m_process->error() != QProcess::Crashed
        && m_process->error() != QProcess::Timedout)
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

void ToolInstance::onOutputLog(const QString &outText)
{
  QScrollBar* scrollbar = m_ui.edtOutput->verticalScrollBar();
  bool end = scrollbar->value() == scrollbar->maximum();

  mcrl2::gui::qt::setTextEditTextColor(m_ui.edtOutput, Qt::black, Qt::white);
  m_ui.edtOutput->append(QString(outText).replace("\n\n", "\n"));

  if (end)
  {
    scrollbar->setValue(scrollbar->maximum());
  }
}

void ToolInstance::onErrorLog(const QString &outText)
{
  QScrollBar* scrollbar = m_ui.edtOutput->verticalScrollBar();
  bool end = scrollbar->value() == scrollbar->maximum();

  mcrl2::gui::qt::setTextEditTextColor(m_ui.edtOutput, Qt::black, Qt::white);
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

  if (m_info.guiTool)
  {
    // For GUI-based tools we spawn a new process
    m_mprocess->setProgram(executable());
    m_mprocess->setArguments(arguments(false));
    m_process = m_mprocess->start(QIODevice::ReadOnly);
  }
  else
  {
    m_process->setProgram(executable());
    m_process->setArguments(arguments(false));
    m_process->start(QIODevice::ReadOnly);
  }

  if (m_process->waitForStarted(1000))
  {
    mCRL2log(mcrl2::log::info) << "Started " << executable().toStdString() << arguments(true).join(" ").toStdString() << std::endl;
    m_ui.tabWidget->setCurrentIndex(1);
  }
  else
  {
    mCRL2log(mcrl2::log::error) << m_process->errorString().toStdString()
      << " (" << executable().toStdString() << arguments(true).join(" ").toStdString() << ")" << std::endl;
    onStateChange(QProcess::NotRunning);
  }
}

void ToolInstance::onAbort()
{
  assert(!m_info.guiTool);
  mCRL2log(mcrl2::log::info) << "Attempting to terminate " << executable().toStdString() << std::endl;
  m_process->terminate();

  if (!m_process->waitForFinished(10000)
    && m_process->state() == QProcess::Running)
  {
    mCRL2log(mcrl2::log::warning) << "Killing " << executable().toStdString() << std::endl;
    m_process->kill();
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
