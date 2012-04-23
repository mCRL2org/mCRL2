#include "documentmanager.h"
#include "ui_documentmanager.h"

DocumentManager::DocumentManager(QWidget *parent) :
  QWidget(parent)
{
  ui.setupUi(this);
}

void DocumentManager::newFile()
{
  QWidget widget(ui.tabWidget);
  ui.tabWidget->addTab(&widget, QString("Untitled"));
}
