// Author(s): Olav Bunte
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "iostream"
#include "filesystem.h"

#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QTextStream>
#include <QDirIterator>
#include <QMessageBox>
#include <QInputDialog>
#include <QDesktopServices>
#include <QCoreApplication>

Property::Property()
{
  this->name = "";
  this->text = "";
}

Property::Property(QString name, QString text)
{
  this->name = name;
  this->text = text;
}

bool Property::operator==(const Property& property) const
{
  return this->name == property.name && this->text == property.text;
}

bool Property::operator!=(const Property& property) const
{
  return !operator==(property);
}

FileSystem::FileSystem(CodeEditor* specificationEditor, QSettings* settings,
                       QWidget* parent)
{
  this->specificationEditor = specificationEditor;
  this->settings = settings;
  this->parent = parent;
  specificationModified = false;
  projectOpen = false;

  if (!temporaryFolder.isValid())
  {
    qDebug("Warning: could not create temporary folder");
  }

  connect(specificationEditor, SIGNAL(modificationChanged(bool)), this,
          SLOT(setSpecificationModified(bool)));
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
  return projectFolderPath + QDir::separator() + projectName +
         projectFileExtension;
}

QString FileSystem::propertiesFolderPath()
{
  return projectFolderPath + QDir::separator() + propertiesFolderName;
}

QString FileSystem::defaultSpecificationFilePath()
{
  return projectFolderPath + QDir::separator() + projectName + "_spec.mcrl2";
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

QString FileSystem::lpsFilePath(bool evidence, const QString& propertyName)
{
  return temporaryFolder.path() + QDir::separator() + projectName +
         (evidence ? "_" + propertyName + "_evidence" : "") + "_lps.lps";
}

QString FileSystem::ltsFilePath(LtsReduction reduction, bool evidence,
                                const QString& propertyName)
{
  return temporaryFolder.path() + QDir::separator() + projectName +
         (evidence ? "_" + propertyName + "_evidence" : "") + "_lts_" +
         QString(LTSREDUCTIONNAMES.at(reduction)).replace(' ', '_') + ".lts";
}

QString FileSystem::propertyFilePath(const QString& propertyName)
{
  return propertiesFolderPath() + QDir::separator() + propertyName + ".mcf";
}

QString FileSystem::pbesFilePath(const QString& propertyName, bool evidence)
{
  return temporaryFolder.path() + QDir::separator() + projectName + "_" +
         propertyName + (evidence ? "_evidence" : "") + "_pbes.pbes";
}

QString FileSystem::toolPath(const QString& tool)
{
  QDir toolDir = QDir(QCoreApplication::applicationDirPath());
  return toolDir.absoluteFilePath(tool);
}

QString FileSystem::parentFolderPath(const QString& folderPath)
{
  QDir folder(folderPath);
  folder.cdUp();
  return folder.path();
}

QString FileSystem::getProjectName()
{
  if (projectOpened())
  {
    return projectName;
  }
  else
  {
    return "";
  }
}

std::list<Property> FileSystem::getProperties()
{
  return properties;
}

bool FileSystem::projectOpened()
{
  return projectOpen;
}

bool FileSystem::isSpecificationModified()
{
  return specificationModified;
}

void FileSystem::setSpecificationModified(bool modified)
{
  specificationModified = modified;
}

void FileSystem::updateSpecificationModificationTime()
{
  lastKnownSpecificationModificationTime =
      QFileInfo(specificationFilePath()).lastModified();
}

bool FileSystem::isSpecificationNewlyModifiedFromOutside()
{
  QDateTime newestModificationTime =
      QFileInfo(specificationFilePath()).lastModified();
  bool newlyModified =
      lastKnownSpecificationModificationTime != newestModificationTime;
  lastKnownSpecificationModificationTime = newestModificationTime;
  return newlyModified;
}

bool FileSystem::propertyNameExists(const QString& propertyName)
{
  for (Property property : properties)
  {
    if (property.name == propertyName)
    {
      return true;
    }
  }
  return false;
}

bool FileSystem::upToDateLpsFileExists(bool evidence,
                                       const QString& propertyName)
{
  /* in case of not an evidence lps, an lps file is up to date if the lps file
   *   exists, the lps file is not empty and the lps file is created after the
   *   last time the specification file was modified
   * in case of an evidence lps, an lps file is up to date if the lps file
   *   exists, the lps file is not empty and the lps file is created after the
   *   last time the evidence pbes was modified */
  QFile lpsFile(lpsFilePath(evidence, propertyName));
  if (!evidence)
  {

    return lpsFile.exists() && lpsFile.size() > 0 &&
           QFileInfo(specificationFilePath()).lastModified() <=
               QFileInfo(lpsFile).lastModified();
  }
  else
  {
    return lpsFile.exists() && lpsFile.size() > 0 &&
           QFileInfo(pbesFilePath(propertyName, evidence)).lastModified() <=
               QFileInfo(lpsFile).lastModified();
  }
}

bool FileSystem::upToDateLtsFileExists(LtsReduction reduction, bool evidence,
                                       const QString& propertyName)
{
  /* in case of not a reduced lts, an lts file is up to date if the lts file
   *   exists, the lts file is not empty and the lts file is created after the
   *   last time the lps file was modified
   * in case of a reduced lts, an lts file is up to date if the lts file exists,
   *   the lts file is not empty and the lts file is created after the last time
   *   the unreduced lts file was modified
   */
  if (reduction == LtsReduction::None)
  {
    QFile unreducedLtsFile(ltsFilePath(reduction, evidence, propertyName));
    return unreducedLtsFile.exists() && unreducedLtsFile.size() > 0 &&
           QFileInfo(lpsFilePath(evidence, propertyName)).lastModified() <=
               QFileInfo(unreducedLtsFile).lastModified();
  }
  else
  {
    QFile reducedLtsFile(ltsFilePath(reduction));
    return reducedLtsFile.exists() && reducedLtsFile.size() > 0 &&
           QFileInfo(ltsFilePath(LtsReduction::None)).lastModified() <=
               QFileInfo(reducedLtsFile).lastModified();
  }
}

bool FileSystem::upToDatePbesFileExists(const QString& propertyName,
                                        bool evidence)
{
  /* a pbes file is up to date if the pbes file exists, the pbes is not empty
   *   and the pbes file is created after the last time both the lps and the
   *   property files were modified */
  QFile pbesFile(pbesFilePath(propertyName, evidence));
  return pbesFile.exists() && pbesFile.size() > 0 &&
         QFileInfo(lpsFilePath()).lastModified() <=
             QFileInfo(pbesFile).lastModified() &&
         QFileInfo(propertyFilePath(propertyName)).lastModified() <=
             QFileInfo(pbesFile).lastModified();
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
  QString fileDialogLocation =
      settings
          ->value("fileDialogLocation", QStandardPaths::writableLocation(
                                            QStandardPaths::DocumentsLocation))
          .toString();
  QFileDialog* fileDialog = new QFileDialog(parent, Qt::WindowCloseButtonHint);
  fileDialog->setDirectory(fileDialogLocation);
  switch (type)
  {
  case 0: /* New Project */
    fileDialog->setAcceptMode(QFileDialog::AcceptSave);
    fileDialog->setWindowTitle("New Project");
    fileDialog->setLabelText(QFileDialog::FileName, "Project name:");
    fileDialog->setLabelText(QFileDialog::Accept, "Create");
    break;
  case 1: /* Save Project As */
    fileDialog->setAcceptMode(QFileDialog::AcceptSave);
    fileDialog->setWindowTitle("Save Project As");
    fileDialog->setLabelText(QFileDialog::FileName, "Project name:");
    fileDialog->setLabelText(QFileDialog::Accept, "Save as");
    break;
  case 2: /* Open Project */
    fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog->setFileMode(QFileDialog::Directory);
    fileDialog->setWindowTitle("Open Project");
    fileDialog->setLabelText(QFileDialog::FileName, "Project folder:");
    fileDialog->setLabelText(QFileDialog::Accept, "Open");
    break;
  case 3: /* Import Property */
    fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    fileDialog->setWindowTitle("Import Property");
    fileDialog->setLabelText(QFileDialog::FileName, "Property file:");
    fileDialog->setLabelText(QFileDialog::Accept, "Import");
    fileDialog->setNameFilter("Mu-calculus file (*.mcf)");
    break;
  default:
    break;
  }
  return fileDialog;
}

void FileSystem::createProjectFile()
{
  QFile projectFile(projectFilePath());
  projectFile.open(QIODevice::WriteOnly);
  QTextStream saveStream(&projectFile);
  /* we add the relative path to the specication, which is simply the name of
   *   the specification file */
  saveStream << "SPEC " + QFileInfo(specificationFilePath()).fileName();
  projectFile.close();
}

bool FileSystem::newProject(bool askToSave, bool forNewProject)
{
  bool success = false;
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
      settings->setValue("fileDialogLocation",
                         parentFolderPath(newProjectFolderPath));
      projectFolderPath = newProjectFolderPath;
      projectName = newProjectName;
      specFilePath = defaultSpecificationFilePath();

      createProjectFile();
      QDir(projectFolderPath).mkdir(propertiesFolderName);

      /* also empty the editor and properties list if we are not saving as and
       *   if there was already a project open */
      if (forNewProject && projectOpened())
      {
        specificationEditor->clear();
        properties.clear();
      }

      projectOpen = true;
      emit newProjectOpened();
      success = true;

      /* if we are not saving as, create an empty specification file by saving
       *   the project */
      if (forNewProject)
      {
        saveProject(true);
      }
    }
    else
    {
      /* if unsuccessful, tell the user */
      error = "Could not create project";
    }
  }
  newProjectDialog->deleteLater();

  if (!error.isEmpty())
  {
    /* if there was an error, tell the user */
    QMessageBox msgBox(QMessageBox::Information, context, error,
                       QMessageBox::Ok, parent, Qt::WindowCloseButtonHint);
    msgBox.exec();
  }
  return success;
}

Property FileSystem::readPropertyFromFile(const QString& propertyFilePath)
{
  QFile propertyFile(propertyFilePath);
  QString fileName = QFileInfo(propertyFile).fileName();
  fileName.chop(4);

  propertyFile.open(QIODevice::ReadOnly);
  QTextStream propertyOpenStream(&propertyFile);
  QString propertyText = propertyOpenStream.readAll();
  propertyFile.close();

  return Property(fileName, propertyText);
}

bool FileSystem::deletePropertyFile(const QString& propertyName,
                                    bool showIfFailed)
{
  QFile propertyFile(propertyFilePath(propertyName));
  bool deleteSucceeded = true;
  if (!propertyFile.remove())
  {
    deleteSucceeded = false;
    if (showIfFailed)
    {
      /* if deleting the file failed, tell the user */
      QMessageBox msgBox(QMessageBox::Information, "Delete property",
                         "Could not delete property file: " +
                             propertyFile.errorString(),
                         QMessageBox::Ok, parent, Qt::WindowCloseButtonHint);
      msgBox.exec();
    }
  }

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

void FileSystem::newProperty(const Property& property)
{
  /* add to the properties list and save it */
  properties.push_back(property);
  saveProperty(property);

  /* delete any obsolete property files generated by the dialog */
  deleteUnlistedPropertyFiles();
}

std::list<Property> FileSystem::importProperties()
{
  std::list<Property> importedProperties;
  /* first ask the user for a property file */
  QFileDialog* importPropertyDialog = createFileDialog(3);
  if (importPropertyDialog->exec() == QDialog::Accepted)
  {
    /* if successful, extract the property and save it if it is a .mcf file */
    for (QString propertyFilePath : importPropertyDialog->selectedFiles())
    {
      if (propertyFilePath.endsWith(".mcf"))
      {
        Property importedProperty = readPropertyFromFile(propertyFilePath);
        importedProperties.push_back(importedProperty);
        saveProperty(importedProperty);
      }
    }
    /* add the imported properties to the list of properties */
    properties.insert(properties.end(), importedProperties.begin(),
                      importedProperties.end());
  }
  return importedProperties;
}

void FileSystem::editProperty(const Property& oldProperty,
                              const Property& newProperty)
{
  /* alter the properties list and save it */
  std::replace(properties.begin(), properties.end(), oldProperty, newProperty);
  saveProperty(newProperty);

  /* delete any obsolete property files generated by the dialog
   * this also deletes the original property file if the property name has
   *   changed */
  deleteUnlistedPropertyFiles();
}

bool FileSystem::deleteProperty(const Property& oldProperty)
{
  /* show a message box to ask the user whether he is sure to delete the
   *   property */
  bool deleteIt =
      QMessageBox::question(parent, "Delete Property",
                            "Are you sure you want to delete the property " +
                                oldProperty.name + "?",
                            QMessageBox::Yes | QMessageBox::Cancel) ==
      QMessageBox::Yes;

  /* if the user agrees, delete the property */
  if (deleteIt)
  {
    /* also delete the file if it exists */
    if (deletePropertyFile(oldProperty.name))
    {
      properties.remove(oldProperty);
    }
    else
    {
      deleteIt = false;
    }
  }

  return deleteIt;
}

bool FileSystem::loadSpecification()
{
  QFile specificationFile(specificationFilePath());
  if (specificationFile.exists())
  {
    specificationFile.open(QIODevice::ReadOnly);
    QTextStream specificationOpenStream(&specificationFile);
    QString spec = specificationOpenStream.readAll();
    specificationEditor->setPlainText(spec);
    specificationFile.close();
    specificationModified = false;
    updateSpecificationModificationTime();
    return true;
  }
  else
  {
    return false;
  }
}

void FileSystem::openProjectFromFolder(const QString& newProjectFolderPath)
{
  QString error = "";
  QDir projectFolder(newProjectFolderPath);

  if (!projectFolder.exists())
  {
    error = "Could not find provided project folder";
  }
  else
  {
    settings->setValue("fileDialogLocation",
                       parentFolderPath(newProjectFolderPath));

    /* find the project file */
    QStringList projectFiles;
    for (QString fileName : projectFolder.entryList())
    {
      if (fileName.endsWith(projectFileExtension))
      {
        projectFiles << fileName;
      }
    }

    if (projectFiles.length() == 0)
    {
      error = "Provided folder does not contain a project file (ending with " +
              projectFileExtension + ")";
    }
    else if (projectFiles.length() > 1)
    {
      error = "Provided folder contains more than one project file";
    }
    else
    {
      /* read the project file to get the specification path */
      QString newProjectFilePath =
          newProjectFolderPath + QDir::separator() + projectFiles.first();
      QFile projectFile(newProjectFilePath);
      projectFile.open(QIODevice::ReadOnly);
      QTextStream projectOpenStream(&projectFile);
      QString projectInfo = projectOpenStream.readAll();
      int specLineIndex = projectInfo.lastIndexOf("SPEC");
      if (specLineIndex < 0)
      {
        error = "Project file in provided project folder does not contain a "
                "specification (should contain SPEC <path_to_spec>).";
      }
      else
      {
        // Read the filename from the project file and remove control characters. 
        specFilePath =
            projectInfo.right(projectInfo.length() - specLineIndex - 5).simplified();
        projectFile.close();
        if (QFileInfo(specFilePath).isRelative())
        {
          specFilePath =
              newProjectFolderPath + QDir::separator() + specFilePath;
        }
std::cerr << "PAD |" << specFilePath.toStdString() << "|\n";

        /* read the specification and put it in the specification editor */
        if (!loadSpecification())
        {
          error = "Specification file given in the project file in the "
                  "provided project folder does not exist";
        }
        else
        {
          /* set propject folder and project name */
          projectFolderPath = QFileInfo(newProjectFilePath).path();
          this->projectName = QFileInfo(newProjectFilePath).baseName();

          /* read all properties */
          properties.clear();
          QDirIterator* dirIterator =
              new QDirIterator(QDir(propertiesFolderPath()));

          while (dirIterator->hasNext())
          {
            QString filePath = dirIterator->next();
            if (QFileInfo(filePath).isFile() && filePath.endsWith(".mcf"))
            {
              properties.push_back(readPropertyFromFile(filePath));
            }
          }
          delete dirIterator;

          projectOpen = true;
          emit newProjectOpened();
        }
      }
    }
  }

  /* if unsuccessful, tell the user */
  if (!error.isEmpty())
  {
    QMessageBox msgBox(QMessageBox::Information, "Open Project", error,
                       QMessageBox::Ok, parent, Qt::WindowCloseButtonHint);
    msgBox.exec();
  }
}

void FileSystem::openProject()
{
  /* ask the user for a project folder */
  QFileDialog* openProjectDialog = createFileDialog(2);
  if (openProjectDialog->exec() == QDialog::Accepted)
  {
    QString newProjectFolderPath = openProjectDialog->selectedFiles().first();
    openProjectFromFolder(newProjectFolderPath);
  }
  openProjectDialog->deleteLater();
}

bool FileSystem::saveProject(bool forceSave)
{
  makeSurePropertiesFolderExists();

  /* if we have a project open, we have a location to save in so we can simply
   *   save, else use save as */
  if (projectOpen)
  {
    /* save the specification (when there are changes) */
    if (isSpecificationModified() || forceSave)
    {
      QFile specificationFile(specFilePath);
      specificationFile.open(QIODevice::WriteOnly);
      QTextStream saveStream(&specificationFile);
      saveStream << specificationEditor->toPlainText();
      specificationFile.close();
      specificationModified = false;
      updateSpecificationModificationTime();
    }

    /* save all properties if necessary */
    if (forceSave)
    {
      for (Property property : properties)
      {
        saveProperty(property);
      }
    }

    specificationEditor->document()->setModified(false);
    return true;
  }
  else
  {
    return saveProjectAs();
  }
}

bool FileSystem::saveProjectAs()
{
  /* create a new project to save the current as, save if successful */
  if (newProject(false, false))
  {
    return saveProject(true);
  }
  else
  {
    return false;
  }
}

void FileSystem::saveProperty(const Property& property)
{
  makeSurePropertiesFolderExists();

  QFile propertyFile(propertyFilePath(property.name));
  propertyFile.open(QIODevice::WriteOnly);
  QTextStream saveStream(&propertyFile);
  saveStream << property.text;
  propertyFile.close();
}

void FileSystem::openProjectFolderInExplorer()
{
  QDesktopServices::openUrl(projectFolderPath);
}

void FileSystem::removeTemporaryFolder()
{
  temporaryFolder.remove();
}
