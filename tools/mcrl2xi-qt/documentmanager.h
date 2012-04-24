#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include <QWidget>
#include <QTextEdit>

#include "ui_documentmanager.h"

class DocumentManager : public QWidget
{
    Q_OBJECT
    
  public:
    DocumentManager(QWidget *parent = 0);
    //~DocumentManager();

    void newFile();
    void openFile(QString fileName);
    void saveFile(QString fileName);
    QTextEdit* currentEditor();
    QWidget* currentTab();
    
  signals:
    void documentCreated(QTextEdit *editor);
    void documentSwitched(QTextEdit *editor);

  private:
    QTextEdit* createEditor(QString title);

    Ui::DocumentManager m_ui;
};

#endif // DOCUMENTMANAGER_H
