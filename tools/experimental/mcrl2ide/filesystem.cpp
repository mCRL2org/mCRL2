// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "filesystem.h"
#include "addeditpropertydialog.h"

#include <QFileInfo>
#include <QDateTime>
#include <QTextStream>
#include <QDirIterator>
#include <QMessageBox>
#include <QInputDialog>

Property::Property(QString name, QString text)
{
  this->name = name;
  this->text = text;
}

Project::Project(QString projectName, std::list<Property*> properties)
{
  this->projectName = projectName;
  this->properties = properties;
}

FileSystem::FileSystem(CodeEditor* specificationEditor, QWidget* parent)
{
  this->specificationEditor = specificationEditor;
  this->parent = parent;
  specificationModified = false;
  connect(specificationEditor, SIGNAL(textChanged()), this,
          SLOT(setSpecificationModified()));

  projectOpen = false;

  makeSureProjectsFolderExists();
}

void FileSystem::makeSureProjectsFolderExists()
{
  if (!QDir(projectsFolderPath).exists())
  {
    QDir().mkpath(projectsFolderPath);
  }
}

void FileSystem::makeSurePropertiesFolderExists()
{
  if (!QDir(propertiesFolderPath(projectName)).exists())
  {
    QDir().mkpath(propertiesFolderPath(projectName));
  }
}

QString FileSystem::projectFolderPath(QString projectName)
{
  return projectsFolderPath + QDir::separator() + projectName;
}

QString FileSystem::propertiesFolderPath(QString projectName)
{
  return projectFolderPath(projectName) + QDir::separator() +
         propertiesFolderName;
}

QString FileSystem::specificationFilePath()
{
  return projectFolderPath(projectName) + QDir::separator() + projectName +
         "_spec.mcrl";
}

QString FileSystem::lpsFilePath()
{
  return projectFolderPath(projectName) + QDir::separator() + projectName +
         "_lps.lps";
}

QString FileSystem::ltsFilePath(LtsReduction reduction)
{
  return projectFolderPath(projectName) + QDir::separator() + projectName +
         "_lts_" + LTSREDUCTIONNAMES.at(reduction) + ".lts";
}

QString FileSystem::propertyFilePath(QString propertyName)
{
  return propertiesFolderPath(projectName) + QDir::separator() + propertyName +
         ".mcf";
}

QString FileSystem::pbesFilePath(QString propertyName)
{
  return propertiesFolderPath(projectName) + QDir::separator() + projectName +
         "_" + propertyName + "_pbes.pbes";
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

bool FileSystem::isPropertyModified(Property* property)
{
  return propertyModified[property->name];
}

void FileSystem::setPropertyModified(Property* property)
{
  propertyModified[property->name] = true;
  emit hasChanges(true);
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

QString FileSystem::newProject(QString context)
{
  makeSureProjectsFolderExists();

  QString error = "";

  /* ask the user for a project name */
  bool ok;
  QString projectName = QInputDialog::getText(
      parent, "New project", "Project name:", QLineEdit::Normal, "", &ok,
      Qt::WindowCloseButtonHint);

  /* if user pressed ok, create the project and save the specification and
   *   properties in it */
  if (ok)
  {
    /* the project name may not be empty */
    if (projectName.isEmpty())
    {
      error = "The project name may not be empty";
    }

    /* create the folder for this project */
    if (QDir(projectsFolderPath).mkdir(projectName))
    {
      /* if successful, create the properties folder too */
      this->projectName = projectName;
      QDir(projectFolderPath(projectName)).mkdir(propertiesFolderName);
      projectOpen = true;
    }
    else
    {
      /* if unsuccessful, there already is a project folder with this name */
      error = "A project with this name already exists";
    }
  }

  if (error.isEmpty())
  {
    return projectName;
  }
  else
  {
    /* if there was an error, tell the user */
    QMessageBox* msgBox =
        new QMessageBox(QMessageBox::Information, "New Project", error,
                        QMessageBox::Ok, parent, Qt::WindowCloseButtonHint);
    msgBox->exec();
    return "";
  }
}

Property* FileSystem::newProperty()
{
  makeSurePropertiesFolderExists();

  Property* property = NULL;
  AddEditPropertyDialog* addPropertyDialog = new
      AddEditPropertyDialog(true, this, parent);

  /* if successful (Add button was pressed), create the new property */
  if (addPropertyDialog->exec())
  {
    property = new Property(addPropertyDialog->getPropertyName(),
                            addPropertyDialog->getPropertyText());
    properties.push_back(property);
    setPropertyModified(property);
  }

  delete addPropertyDialog;
  return property;
}

Property* FileSystem::editProperty(Property* oldProperty)
{
  makeSurePropertiesFolderExists();

  Property* newProperty = oldProperty;
  AddEditPropertyDialog* editPropertyDialog = new AddEditPropertyDialog(
      false, this, parent, oldProperty->name, oldProperty->text);

  /* if editing was successful (Edit button was pressed), change the property */
  if (editPropertyDialog->exec())
  {
    newProperty = new Property(editPropertyDialog->getPropertyName(),
                               editPropertyDialog->getPropertyText());

    for (Property* property : properties)
    {
      if (property == oldProperty)
      {
        property = newProperty;
        break;
      }
    }
    setPropertyModified(newProperty);
  }
  
  delete editPropertyDialog;
  return newProperty;
}

Project FileSystem::openProject()
{
  makeSureProjectsFolderExists();

  QStringList projects =
      QDir(projectsFolderPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
  Project project = Project("", {});

  /* if there are no projects give an error, else ask a project name from the
   *   user */
  if (projects.isEmpty())
  {
    QMessageBox* msgBox = new QMessageBox(
        QMessageBox::Information, "Open Project", "No projects found",
        QMessageBox::Ok, parent, Qt::WindowCloseButtonHint);
    msgBox->exec();
  }
  else
  {
    bool ok;
    QString projectName =
        QInputDialog::getItem(parent, "Open Project", "Project name:", projects,
                              0, false, &ok, Qt::WindowCloseButtonHint);

    /* if user pressed ok, open the project */
    if (ok)
    {
      this->projectName = projectName;
      projectOpen = true;

      /* read the specification and put it in the specification editor */
      QFile* specificationFile = new QFile(specificationFilePath());
      specificationFile->open(QIODevice::ReadOnly);
      QTextStream* openStream = new QTextStream(specificationFile);
      QString spec = openStream->readAll();
      specificationEditor->setPlainText(spec);

      /* read all properties */
      properties.clear();
      QDirIterator* dirIterator =
          new QDirIterator(QDir(propertiesFolderPath(projectName)));

      while (dirIterator->hasNext())
      {
        QFile* propertyFile = new QFile(dirIterator->next());
        QFileInfo* propertyFileInfo = new QFileInfo(*propertyFile);
        QString fileName = propertyFileInfo->fileName();

        if (propertyFileInfo->isFile() && fileName.endsWith(".mcf"))
        {
          fileName.chop(4);
          propertyFile->open(QIODevice::ReadOnly);
          QTextStream* openStream = new QTextStream(propertyFile);
          QString propertyText = openStream->readAll();
          properties.push_back(new Property(fileName, propertyText));
        }
      }
      this->properties = properties;
      project = Project(projectName, properties);
    }
  }
  return project;
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
      QFile* specificationFile = new QFile(specificationFilePath());
      specificationFile->open(QIODevice::WriteOnly);
      QTextStream* saveStream = new QTextStream(specificationFile);
      *saveStream << specificationEditor->toPlainText();
      specificationFile->close();
      specificationModified = false;
    }

    /* save all properties (when there are changes) */
    for (Property* property : properties)
    {
      if (isPropertyModified(property) || forceSave)
      {
        QFile* propertyFile = new QFile(propertyFilePath(property->name));
        propertyFile->open(QIODevice::WriteOnly);
        QTextStream* saveStream = new QTextStream(propertyFile);
        *saveStream << property->text;
        propertyFile->close();
        propertyModified[property->name] = false;
      }
    }
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
  QString projectName = newProject("Save Project As");

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
