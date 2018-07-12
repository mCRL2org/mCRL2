// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "filesystem.h"

#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QTextStream>
#include <QDirIterator>
#include <QMessageBox>
#include <QInputDialog>
#include <QStandardPaths>

Property::Property(QString name, QString text)
{
  this->name = name;
  this->text = text;
}

bool Property::equals(Property* property)
{
  return this->name == property->name && this->text == property->text;
}

FileSystem::FileSystem(CodeEditor* specificationEditor, QWidget* parent)
{
  this->specificationEditor = specificationEditor;
  this->parent = parent;
  specificationModified = false;
  connect(specificationEditor, SIGNAL(textChanged()), this,
          SLOT(setSpecificationModified()));

  projectOpen = false;
}

void FileSystem::makeSurePropertiesFolderExists()
{
  if (!QDir(propertiesFolderPath()).exists())
  {
    QDir().mkpath(propertiesFolderPath());
  }
}

QString FileSystem::projectFilePath()
{
  return projectFolderPath + QDir::separator() + projectName + ".mcrl2proj";
}

QString FileSystem::propertiesFolderPath()
{
  return projectFolderPath + QDir::separator() + propertiesFolderName;
}

QString FileSystem::defaultSpecificationFilePath()
{
  return projectFolderPath + QDir::separator() + projectName + "_spec.mcrl";
}

QString FileSystem::specificationFilePath()
{
  if (specFilePath.isEmpty())
  {
    return defaultSpecificationFilePath();
  }
  else
  {
    return specFilePath;
  }
}

QString FileSystem::lpsFilePath()
{
  return projectFolderPath + QDir::separator() + projectName + "_lps.lps";
}

QString FileSystem::ltsFilePath(LtsReduction reduction)
{
  return projectFolderPath + QDir::separator() + projectName + "_lts_" +
         LTSREDUCTIONNAMES.at(reduction) + ".lts";
}

QString FileSystem::propertyFilePath(QString propertyName)
{
  return propertiesFolderPath() + QDir::separator() + propertyName + ".mcf";
}

QString FileSystem::pbesFilePath(QString propertyName)
{
  return propertiesFolderPath() + QDir::separator() + projectName + "_" +
         propertyName + "_pbes.pbes";
}

bool FileSystem::projectOpened()
{
  return projectOpen;
}

QString FileSystem::getCurrentSpecification()
{
  return specificationEditor->toPlainText();
}

bool FileSystem::isSpecificationModified()
{
  return specificationModified;
}

void FileSystem::setSpecificationModified()
{
  specificationModified = true;
  emit hasChanges(true);
}

bool FileSystem::propertyNameExists(QString propertyName)
{
  for (Property* property : properties)
  {
    if (property->name == propertyName)
    {
      return true;
    }
  }
  return false;
}

bool FileSystem::upToDateLpsFileExists()
{
  /* an lps file is up to date if the lps file exists and the lps file is
   *   created after the the last time the specification file was modified */
  return QFile(lpsFilePath()).exists() &&
         QFileInfo(specificationFilePath()).lastModified() <=
             QFileInfo(lpsFilePath()).lastModified();
}

bool FileSystem::upToDateLtsFileExists(LtsReduction reduction)
{
  /* in case not reduced, an lts file is up to date if the lts file exists and
   *   the lts file is created after the the last time the lps file was modified
   * in case reduced, an lts file is up to date if the lts file exists and
   *   the lts file is created after the the last time the unreduced lts file
   *   was modified */
  if (reduction == LtsReduction::None)
  {
    return QFile(ltsFilePath(reduction)).exists() &&
           QFileInfo(lpsFilePath()).lastModified() <=
               QFileInfo(ltsFilePath(reduction)).lastModified();
  }
  else
  {
    return QFile(ltsFilePath(reduction)).exists() &&
           QFileInfo(ltsFilePath(LtsReduction::None)).lastModified() <=
               QFileInfo(ltsFilePath(reduction)).lastModified();
  }
}

bool FileSystem::upToDatePbesFileExists(QString propertyName)
{
  /* a pbes file is up to date if the pbes file exists and the pbes file is
   *   created after the last time both the lps and the property files were
   *   modified */
  return QFile(pbesFilePath(propertyName)).exists() &&
         QFileInfo(lpsFilePath()).lastModified() <=
             QFileInfo(pbesFilePath(propertyName)).lastModified() &&
         QFileInfo(propertyFilePath(propertyName)).lastModified() <=
             QFileInfo(pbesFilePath(propertyName)).lastModified();
}

void FileSystem::setSpecificationEditorCursor(int row, int column)
{
  QTextCursor cursor = specificationEditor->textCursor();
  cursor.movePosition(QTextCursor::Start);
  cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, row - 1);
  cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, column);
  specificationEditor->setTextCursor(cursor);
}

QFileDialog* FileSystem::createFileDialog(int type)
{
  QFileDialog* fileDialog = new QFileDialog(parent, Qt::WindowCloseButtonHint);
  fileDialog->setDirectory(
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
  fileDialog->setOption(QFileDialog::DontUseNativeDialog);
  fileDialog->setNameFilters(QStringList({"Directory"}));
  fileDialog->setLabelText(QFileDialog::FileName, "Project name:");
  switch (type)
  {
  case 0:
    fileDialog->setAcceptMode(QFileDialog::AcceptSave);
    fileDialog->setWindowTitle("New Project");
    fileDialog->setLabelText(QFileDialog::Accept, "Create");
    break;
  case 1:
    fileDialog->setAcceptMode(QFileDialog::AcceptSave);
    fileDialog->setWindowTitle("Save Project As");
    fileDialog->setLabelText(QFileDialog::Accept, "Save as");
    break;
  case 2:
    fileDialog->setFileMode(QFileDialog::Directory);
    fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog->setWindowTitle("Open Project");
    fileDialog->setLabelText(QFileDialog::Accept, "Open");
  }
  return fileDialog;
}

void FileSystem::createProjectFile()
{
  QFile* projectFile = new QFile(projectFilePath());
  projectFile->open(QIODevice::WriteOnly);
  QTextStream* saveStream = new QTextStream(projectFile);
  *saveStream << "SPEC " + defaultSpecificationFilePath();
  projectFile->close();
  delete projectFile;
  delete saveStream;
}

QString FileSystem::newProject(bool askToSave, bool forNewProject)
{
  QString error = "";
  QString context = forNewProject ? "New Project" : "Save Project As";

  /* if there are changes in the current project, ask to save first */
  if (askToSave && isSpecificationModified())
  {
    QMessageBox::StandardButton result = QMessageBox::question(
        parent, "New Project",
        "There are changes in the current project, do you want to save?",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    switch (result)
    {
    case QMessageBox::Yes:
      saveProject();
      break;
    case QMessageBox::Cancel:
      return "";
    default:
      break;
    }
  }

  /* ask the user for a project folder */
  QFileDialog* newProjectDialog = createFileDialog(forNewProject ? 0 : 1);
  if (newProjectDialog->exec() == QDialog::Accepted)
  {
    /* if successful, create the new project */
    QString newProjectFolderPath = newProjectDialog->selectedFiles().first();
    QString newProjectName = QFileInfo(newProjectFolderPath).fileName();

    /* create the folder for this project */
    if (QDir().mkpath(newProjectFolderPath))
    {
      /* if successful, set the current projectFolder and create the project
       *   file and properties folder */
      projectFolderPath = newProjectFolderPath;
      projectName = newProjectName;
      specFilePath = defaultSpecificationFilePath();

      createProjectFile();
      QDir(projectFolderPath).mkdir(propertiesFolderName);
      projectOpen = true;

      /* also empty the editor and properties list if we are not saving as */
      if (forNewProject)
      {
        specificationEditor->clear();
        properties.clear();
      }
    }
    else
    {
      /* if unsuccessful, tell the user */
      error = "Could not create project";
    }
  }
  delete newProjectDialog;

  if (error.isEmpty())
  {
    return projectName;
  }
  else
  {
    /* if there was an error, tell the user */
    QMessageBox* msgBox =
        new QMessageBox(QMessageBox::Information, context, error,
                        QMessageBox::Ok, parent, Qt::WindowCloseButtonHint);
    msgBox->exec();
    delete msgBox;
    return "";
  }
}

bool FileSystem::deletePropertyFile(QString propertyName, bool showIfFailed)
{
  QFile* propertyFile = new QFile(propertyFilePath(propertyName));
  bool deleteSucceeded = true;
  if (propertyFile->exists() && !propertyFile->remove())
  {
    /* if deleting the file failed, tell the user */
    QMessageBox* msgBox = new QMessageBox(
        QMessageBox::Information, "Delete property",
        "Could not delete property file: " + propertyFile->errorString(),
        QMessageBox::Ok, parent, Qt::WindowCloseButtonHint);
    msgBox->exec();
    deleteSucceeded = false;
    delete msgBox;
  }

  delete propertyFile;
  return deleteSucceeded;
}

void FileSystem::deleteUnlistedPropertyFiles()
{
  QDir propertiesFolder = QDir(propertiesFolderPath());
  QStringList fileNames = propertiesFolder.entryList();
  for (QString fileName : fileNames)
  {
    if (fileName.endsWith(".mcf"))
    {
      QString propertyName = fileName.left(fileName.length() - 4);
      if (!propertyNameExists(propertyName))
      {
        deletePropertyFile(propertyName, false);
      }
    }
  }
}

void FileSystem::newProperty(Property* property)
{
  /* add to the properties list */
  properties.push_back(property);
  /* delete any obsolete property files generated by the dialog */
  deleteUnlistedPropertyFiles();
}

void FileSystem::editProperty(Property* oldProperty, Property* newProperty)
{
  /* alter the properties list */
  std::list<Property*>::iterator it;
  for (it = properties.begin(); it != properties.end(); it++)
  {
    if ((*it)->equals(oldProperty))
    {
      (*it) = newProperty;
      break;
    }
  }

  /* delete any obsolete property files generated by the dialog
   * this also deletes the original property file if the property name has
   *   changed */
  deleteUnlistedPropertyFiles();
}

bool FileSystem::deleteProperty(Property* oldProperty)
{
  /* show a message box to ask the user whether he is sure to delete the
   *   property */
  bool deleteIt =
      QMessageBox::question(parent, "Delete Property",
                            "Are you sure you want to delete the property " +
                                oldProperty->name + "?",
                            QMessageBox::Yes | QMessageBox::Cancel) ==
      QMessageBox::Yes;

  /* if the user agrees, delete the property */
  if (deleteIt)
  {
    Property* toRemove = NULL;
    for (Property* property : properties)
    {
      if (property->equals(oldProperty))
      {
        toRemove = property;
        break;
      }
    }
    if (toRemove != NULL)
    {
      properties.remove(toRemove);
    }

    /* also delete the file if it exists */
    if (!deletePropertyFile(oldProperty->name))
    {
      deleteIt = false;
    }
  }

  return deleteIt;
}

void FileSystem::openProject(QString* newProjectName,
                             std::list<Property*>* newProperties)
{
  /* ask the user for a project */
  QString error = "";
  QFileDialog* openProjectDialog = createFileDialog(2);
  if (openProjectDialog->exec() == QDialog::Accepted)
  {
    QString newProjectFolderPath = openProjectDialog->selectedFiles().first();
    /* check if it is a valid project folder (exactly one project file must
     *   exist) */
    QStringList projectFiles =
        QDir(newProjectFolderPath).entryList().filter(".mcrl2proj");
    if (projectFiles.length() < 1)
    {
      error = "This is not a valid project folder because no project file was "
              "found.";
    }
    else if (projectFiles.length() > 1)
    {
      error = "This is not a valid project folder because more than one "
              "project file was found.";
    }
    else
    {
      /* if ok, set project folder and project name and read all files */
      projectFolderPath = newProjectFolderPath;
      this->projectName = QFileInfo(projectFiles.first()).baseName();
      projectOpen = true;

      /* read the project file to get the specification path */
      QFile* projectFile = new QFile(newProjectFolderPath + QDir::separator() +
                                     projectFiles.first());
      projectFile->open(QIODevice::ReadOnly);
      QTextStream* projectOpenStream = new QTextStream(projectFile);
      QString projectInfo = projectOpenStream->readAll();
      specFilePath = projectInfo.right(
          projectInfo.length() - projectInfo.lastIndexOf("SPEC") - 5);
      std::string test = specFilePath.toStdString();
      projectFile->close();
      delete projectFile;
      delete projectOpenStream;

      /* read the specification and put it in the specification editor */
      QFile* specificationFile = new QFile(specFilePath);
      specificationFile->open(QIODevice::ReadOnly);
      QTextStream* specificationOpenStream = new QTextStream(specificationFile);
      QString spec = specificationOpenStream->readAll();
      specificationEditor->setPlainText(spec);
      specificationFile->close();
      delete specificationFile;
      delete specificationOpenStream;
      specificationModified = false;

      /* read all properties */
      properties.clear();
      QDirIterator* dirIterator =
          new QDirIterator(QDir(propertiesFolderPath()));

      while (dirIterator->hasNext())
      {
        QFile* propertyFile = new QFile(dirIterator->next());
        QFileInfo* propertyFileInfo = new QFileInfo(*propertyFile);
        QString fileName = propertyFileInfo->fileName();

        if (propertyFileInfo->isFile() && fileName.endsWith(".mcf"))
        {
          fileName.chop(4);
          propertyFile->open(QIODevice::ReadOnly);
          QTextStream* propertyOpenStream = new QTextStream(propertyFile);
          QString propertyText = propertyOpenStream->readAll();
          properties.push_back(new Property(fileName, propertyText));
          propertyFile->close();
          delete propertyOpenStream;
        }

        delete propertyFile;
        delete propertyFileInfo;
      }
      delete dirIterator;

      this->properties = properties;
      *newProjectName = projectName;
      *newProperties = properties;
      emit hasChanges(false);
    }
  }

  /* if there was an error, tell the user */
  if (!error.isEmpty())
  {
    QMessageBox* msgBox =
        new QMessageBox(QMessageBox::Information, "Open Project", error,
                        QMessageBox::Ok, parent, Qt::WindowCloseButtonHint);
    msgBox->exec();
    delete msgBox;
  }
}

QString FileSystem::saveProject(bool forceSave)
{
  makeSurePropertiesFolderExists();

  /* if we have a project open, we have a location to save in so we can simply
   *   save, else use save as */
  if (projectOpen)
  {
    /* save the specification (when there are changes) */
    if (isSpecificationModified() || forceSave)
    {
      QFile* specificationFile = new QFile(specFilePath);
      specificationFile->open(QIODevice::WriteOnly);
      QTextStream* saveStream = new QTextStream(specificationFile);
      *saveStream << specificationEditor->toPlainText();
      specificationFile->close();
      specificationModified = false;
      delete specificationFile;
      delete saveStream;
      emit changesSaved();
    }

    /* save all properties if necessary */
    if (forceSave)
    {
      for (Property* property : properties)
      {
        saveProperty(property);
      }
    }

    emit hasChanges(false);
    return projectName;
  }
  else
  {
    return saveProjectAs();
  }
}

QString FileSystem::saveProjectAs()
{
  /* ask the user for a new project name */
  QString projectName = newProject(false, false);

  /* save if successful, else return the empty string */
  if (projectName.isEmpty())
  {
    return "";
  }
  else
  {
    saveProject(true);
    return projectName;
  }
}

void FileSystem::saveProperty(Property* property)
{
  makeSurePropertiesFolderExists();

  QFile* propertyFile = new QFile(propertyFilePath(property->name));
  propertyFile->open(QIODevice::WriteOnly);
  QTextStream* saveStream = new QTextStream(propertyFile);
  *saveStream << property->text;
  propertyFile->close();
  propertyModified[property->name] = false;
  delete propertyFile;
  delete saveStream;
}
