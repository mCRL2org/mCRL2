// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef CONSOLEDOCK_H
#define CONSOLEDOCK_H

#include <QDockWidget>
#include <QPlainTextEdit>
#include <QTabWidget>
#include <QProcess>

enum class ProcessType
{
  Parsing,
  LtsCreation,
  Verification
};

const std::vector<ProcessType> PROCESSTYPES = {
    ProcessType::Parsing, ProcessType::LtsCreation, ProcessType::Verification};

/**
 * @brief The ConsoleDock class defines the dock that shows console output
 */
class ConsoleDock : public QDockWidget
{
  Q_OBJECT

  public:
  const Qt::DockWidgetArea defaultArea = Qt::BottomDockWidgetArea;

  /**
   * @brief ConsoleDock Constructor
   * @param parent The parent of this widget
   */
  explicit ConsoleDock(QWidget* parent);

  /**
   * @brief setConsoleTab Set which tab should be visible
   * @param tab The tab to make visible
   */
  void setConsoleTab(ProcessType processType);

  /**
   * @brief writeToConsole Write something to the console
   * @param tab Which console tab to write to
   * @param output What to write
   */
  void writeToConsole(ProcessType processType, QString output);

  public slots:
  /**
   * @brief logToConsole Write output in the parsing console
   */
  void logToParsingConsole();

  /**
   * @brief logToConsole Write output in the lts creation console
   */
  void logToLtsCreationConsole();

  /**
   * @brief logToConsole Write output in the verification console
   */
  void logToVerificationConsole();

  private:
  QTabWidget* consoleTabs;
  QPlainTextEdit* parsingConsole;
  QPlainTextEdit* ltsCreationConsole;
  QPlainTextEdit* verificationConsole;

  /**
   * @brief logToConsole Write output in the console
   * @param tab Which console tab to write to
   */
  void logToConsole(QPlainTextEdit* console, QProcess* process);
};

#endif // CONSOLEDOCK_H
