// Author: Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_UTILITIES_QT_TOOL_H
#define MCRL2_UTILITIES_QT_TOOL_H

#include <memory>
#include <QAction>
#include <QApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QString>
#include <QUrl>
#include <QtGlobal>

#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/platform.h"
#include "mcrl2/utilities/toolset_version.h"

#ifdef MCRL2_PLATFORM_WINDOWS
#include <windows.h>
#endif // MCRL2_PLATFORM_WINDOWS

namespace mcrl2::gui::qt
{

class HelpMenu : public QMenu
{
  Q_OBJECT

  private:
    QMainWindow* m_window;
    QString m_name;
    QString m_author;
    QString m_description;
    QString m_manualUrl;

    QAction m_actionContents;
    QAction m_actionAbout;

  public:
    HelpMenu(QMainWindow* window, const std::string& name, const std::string& author, const std::string& description, const std::string& manualUrl):
      QMenu(window->menuBar()),
      m_window(window),
      m_name(QString::fromStdString(name)),
      m_author(QString::fromStdString(author)),
      m_description(QString::fromStdString(description)),
      m_manualUrl(QString::fromStdString(manualUrl)),
      m_actionContents(window),
      m_actionAbout(window)
    {
      m_actionContents.setText("&Contents");
      m_actionContents.setShortcut(QString("F1"));
      connect(&m_actionContents, SIGNAL(triggered()), this, SLOT(showContents()));

      m_actionAbout.setText("&About");
      connect(&m_actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));

      setTitle("&Help");
      addAction(&m_actionContents);
      window->addAction(&m_actionContents);
      addSeparator();
      addAction(&m_actionAbout);
    }

  protected slots:
    void showContents()
    {
      QDesktopServices::openUrl(QUrl(m_manualUrl));
    }

    void showAbout()
    {
      QString version = QString(mcrl2::utilities::get_toolset_version().c_str());
      QString description = m_description;
      description.replace("\n", "<br>");
      QString message;
      message += "<h1>" + m_name + "</h1>";
      message += "<p>" + description + "</p>";
      message += "<p>Written by " + m_author + "</p>";
      message += "<\br>";
      message += "<p>Version: " + version + "</p>";
      QMessageBox::about(m_window, QString("About ") + m_name, message);
    }
};

inline
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
}

template <typename Tool>
class qt_tool: public Tool
{
  protected:
    std::unique_ptr<QApplication> m_application;
    std::string m_name;
    std::string m_author;
    std::string m_about_description;
    std::string m_manual_url;

  public:
    qt_tool(const std::string& name,
            const std::string& author,
            const std::string& what_is,
            const std::string& tool_description,
            const std::string& about_description,
            const std::string& manual_url,
            std::string known_issues = ""
           )
      : Tool(name, author, what_is, tool_description, known_issues),
        m_name(name),
        m_author(author), 
        m_about_description(about_description),
        m_manual_url(manual_url)
    {
    }

    int execute(int& argc, char** argv)
    {
#ifdef MCRL2_PLATFORM_WINDOWS
      if (GetConsoleWindow() == 0)
      {
        if (AttachConsole(ATTACH_PARENT_PROCESS) == 0)
        {
          // Failed to attach a console, we could spawn one or just ignore it.
        }
      }
#endif // MCRL2_PLATFORM_WINDOWS


      return Tool::execute(argc, argv);
    }

    bool pre_run(int& argc, char** argv)
    {
#ifdef MCRL2_PLATFORM_WINDOWS
      // Disable the dark mode on Windows 11 until the icons have been adapted
      qputenv("QT_QPA_PLATFORM", "windows:darkmode=0");
#endif // MCRL2_PLATFORM_WINDOWS

      // The instantiation of QApplication has been postponed
      // to here, since creating it in execute would make it
      // impossible to view the help text in an environment
      // without display server
      try
      {
        qInstallMessageHandler(myMessageOutput); // Install the handler
        m_application = std::unique_ptr<QApplication>(new QApplication(argc, argv));
#ifdef MCRL2_PLATFORM_WINDOWS
        m_application->setStyle("windowsvista");
#endif // MCRL2_PLATFORM_WINDOWS
      }
      catch (...)
      {
        mCRL2log(mcrl2::log::debug) << "Creating QApplication failed." << std::endl;
      }
      return true;
    }

    bool show_main_window(QMainWindow& window)
    {
      HelpMenu menu(&window, m_name, m_author, m_about_description, m_manual_url);
      window.menuBar()->addAction(menu.menuAction());
      window.menuBar()->setNativeMenuBar(true);
      window.show();
      return m_application->exec() == 0;
    }

    virtual ~qt_tool() {}
};

} // namespace mcrl2::gui::qt

#endif // MCRL2_UTILITIES_QT_TOOL_H
