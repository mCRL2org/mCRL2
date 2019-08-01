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
#include <QInputDialog>
#include <QDesktopServices>
#include <QCoreApplication>
#include <QDomNode>
#include <QDomElement>
#include <QDomNodeList>

Property::Property()
    : name(""), text(""), mucalculus(true), equivalence(mcrl2::lts::lts_eq_none)
{
}

Property::Property(QString name, QString text, bool mucalculus,
                   mcrl2::lts::lts_equivalence equivalence)
    : name(name), text(text), mucalculus(mucalculus), equivalence(equivalence)
{
}

bool Property::operator==(const Property& property) const
{
  return this->name == property.name && this->text == property.text &&
         this->mucalculus == property.mucalculus &&
         this->equivalence == property.equivalence;
}

bool Property::operator!=(const Property& property) const
{
  return !operator==(property);
}

FileSystem::FileSystem(CodeEditor* specificationEditor, QSettings* settings,
                       QWidget* parent)
    : parent(parent), specificationEditor(specificationEditor),
      settings(settings), projectOpen(false), specificationModified(false),
      specificationOnlyMode(false)
{
  for (std::pair<IntermediateFileType, QString> item :
       INTERMEDIATEFILETYPENAMES)
  {
    saveIntermediateFilesOptions[item.first] = false;
  }

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

void FileSystem::makeSureArtefactsFolderExists()
{
  if (!QDir(artefactsFolderPath()).exists())
  {
    QDir().mkpath(artefactsFolderPath());
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

QString FileSystem::artefactsFolderPath()
{
  return projectFolderPath + QDir::separator() + artefactsFolderName;
}

QString FileSystem::intermediateFilesFolderPath(IntermediateFileType fileType)
{
  if (saveIntermediateFilesOptions.at(fileType))
  {
    makeSureArtefactsFolderExists();
    return artefactsFolderPath();
  }
  else
  {
    return temporaryFolder.path();
  }
}

QString FileSystem::defaultSpecificationFilePath()
{
  return projectFolderPath + QDir::separator() + projectName + "_spec.mcrl2";
}

QString FileSystem::specificationFilePath(const QString& propertyName)
{
  if (propertyName.isEmpty())
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
  else
  {
    return temporaryFolder.path() + QDir::separator() + projectName + "_" +
           propertyName + "_spec.mcrl2";
  }
}

QString FileSystem::lpsFilePath(const QString& propertyName, bool evidence)
{
  return intermediateFilesFolderPath(IntermediateFileType::Lps) +
         QDir::separator() + projectName +
         (propertyName.isEmpty() ? "" : "_" + propertyName) +
         (evidence ? "_evidence" : "") + "_lps.lps";
}

QString FileSystem::ltsFilePath(mcrl2::lts::lts_equivalence equivalence,
                                const QString& propertyName, bool evidence)
{
  return intermediateFilesFolderPath(IntermediateFileType::Lts) +
         QDir::separator() + projectName +
         (propertyName.isEmpty() ? "" : "_" + propertyName) +
         (evidence ? "_evidence" : "") + "_lts_" +
         getEquivalenceName(equivalence, true) + ".lts";
}

QString FileSystem::propertyFilePath(const Property& property, bool forParsing)
{
  return (forParsing ? temporaryFolder.path() : propertiesFolderPath()) +
         QDir::separator() + property.name +
         (property.mucalculus ? ".mcf" : ".equ");
}

QString FileSystem::pbesFilePath(const QString& propertyName, bool evidence)
{
  return intermediateFilesFolderPath(IntermediateFileType::Pbes) +
         QDir::separator() + projectName + "_" + propertyName +
         (evidence ? "_evidence" : "") + "_pbes.pbes";
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

QString FileSystem::getSpecificationFileName()
{
  if (specFilePath.isEmpty())
  {
    return "";
  }
  else
  {
    return QFileInfo(specFilePath).baseName();
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

bool FileSystem::inSpecificationOnlyMode()
{
  return specificationOnlyMode;
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
  QString propertyNameLower = propertyName.toLower();
  for (Property property : properties)
  {
    if (property.name.toLower() == propertyNameLower)
    {
      return true;
    }
  }
  return false;
}

void FileSystem::setSaveIntermediateFilesOptions(bool checked)
{
  IntermediateFileType fileType =
      static_cast<IntermediateFileType>(sender()->property("filetype").toInt());
  saveIntermediateFilesOptions[fileType] = checked;
}

bool FileSystem::upToDateOutputFileExists(const QString& inputFile,
                                          const QString& outputFile,
                                          const QString& inputFile2)
{
  /* An output file is up to date if it exists and if its modification time is
   *   larger than the modification time of the input file(s) */
  QFileInfo outFileInfo(outputFile);
  return outFileInfo.exists() &&
         QFileInfo(inputFile).lastModified() <= outFileInfo.lastModified() &&
         (inputFile2.isEmpty() ? true
                               : QFileInfo(inputFile2).lastModified() <=
                                     outFileInfo.lastModified());
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

  QString fileType = specificationOnlyMode ? "Specification" : "Project";
  switch (type)
  {
  case 0: /* New Project */
    fileDialog->setAcceptMode(QFileDialog::AcceptSave);
    fileDialog->setWindowTitle("New Project");
    fileDialog->setLabelText(QFileDialog::FileName, "Project name:");
    fileDialog->setLabelText(QFileDialog::Accept, "Create");
    break;
  case 1: /* Save As */
    fileDialog->setAcceptMode(QFileDialog::AcceptSave);
    fileDialog->setWindowTitle("Save " + fileType + " As");
    fileDialog->setLabelText(QFileDialog::FileName, fileType + " name:");
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
    fileDialog->setNameFilter("Property (*.mcf *.equ)");
    break;
  default:
    break;
  }
  return fileDialog;
}

QDomDocument FileSystem::createNewProjectOptions()
{
  QDomDocument newProjectOptions;

  QDomElement rootElement = newProjectOptions.createElement("root");
  newProjectOptions.appendChild(rootElement);

  QDomElement specElement = newProjectOptions.createElement("spec");
  rootElement.appendChild(specElement);

  QDomElement propertiesElement = newProjectOptions.createElement("properties");
  rootElement.appendChild(propertiesElement);

  return newProjectOptions;
}

void FileSystem::updateProjectFile()
{
  /* save the project options to the project file */
  QFile projectFile(projectFilePath());
  projectFile.open(QIODevice::WriteOnly);
  QTextStream saveStream(&projectFile);
  saveStream << projectOptions.toString();
  projectFile.close();
}

QDomDocument
FileSystem::convertProjectFileToNewFormat(const QString& newProjectFolderPath,
                                          const QString& newProjectFilePath,
                                          const QString& oldFormat)
{
  /* notify the user of the conversion */
  executeInformationBox(parent, "Open Project",
                        "The project file of this project has an older format. "
                        "It will be converted to the newest format.");

  QDomDocument newFormat = createNewProjectOptions();

  /* read the specification path from the old format */
  int specLineIndex = oldFormat.lastIndexOf("SPEC");
  QString specFilePathEntry =
      oldFormat.right(oldFormat.length() - specLineIndex - 5).simplified();
  QDomText specPathNode =
      newFormat.createTextNode(QFileInfo(specFilePathEntry).fileName());
  newFormat.elementsByTagName("spec").at(0).appendChild(specPathNode);

  /* get all properties */
  QDir propertiesFolder(newProjectFolderPath + QDir::separator() +
                        propertiesFolderName);
  if (propertiesFolder.exists())
  {
    QDirIterator dirIterator(propertiesFolder);
    QDomNode propertiesNode = newFormat.elementsByTagName("properties").at(0);

    while (dirIterator.hasNext())
    {
      QString filePath = dirIterator.next();
      if (QFileInfo(filePath).isFile() && filePath.endsWith(".mcf"))
      {
        QDomElement propertyElement = newFormat.createElement("property");
        propertiesNode.appendChild(propertyElement);
        QDomText propertyPathNode =
            newFormat.createTextNode(QFileInfo(filePath).baseName());
        propertyElement.appendChild(propertyPathNode);
      }
    }
  }

  /* save the new format to file */
  QFile projectFile(newProjectFilePath);
  projectFile.open(QIODevice::WriteOnly);
  QTextStream saveStream(&projectFile);
  saveStream << newFormat.toString();
  projectFile.close();

  return newFormat;
}

bool FileSystem::newProject(bool askToSave, bool forNewProject)
{
  bool success = false;
  QString context = forNewProject ? "New Project" : "Save Project As";

  /* if there are changes in the current project, ask to save first */
  if (askToSave && isSpecificationModified())
  {
    QMessageBox::StandardButton result = executeQuestionBox(
        parent, "New Project",
        "There are changes in the current project, do you want to save?");
    switch (result)
    {
    case QMessageBox::Yes:
      save();
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

      projectOptions = createNewProjectOptions();
      /* we add the relative path to the specification to the project options,
       *   which is simply the name of the specification file */
      QDomText specPathNode =
          projectOptions.createTextNode(QFileInfo(specFilePath).fileName());
      projectOptions.elementsByTagName("spec").at(0).appendChild(specPathNode);
      updateProjectFile();

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
        save(true);
      }
    }
    else
    {
      /* if unsuccessful, tell the user */
      executeInformationBox(parent, context, "Could not create project");
    }
  }

  newProjectDialog->deleteLater();
  return success;
}

QString FileSystem::readSpecification(QString specPath)
{
  if (specPath.isEmpty())
  {
    specPath = specificationFilePath();
  }
  QFile specificationFile(specPath);
  if (specificationFile.exists())
  {
    specFilePath = specPath;
    specificationFile.open(QIODevice::ReadOnly);
    QTextStream specificationOpenStream(&specificationFile);
    QString spec = specificationOpenStream.readAll();
    specificationFile.close();
    return spec;
  }
  else
  {
    return "";
  }
}

bool FileSystem::loadSpecification(QString specPath)
{
  QString spec = readSpecification(specPath);
  if (!spec.isEmpty())
  {
    specificationEditor->setPlainText(spec);
    specificationModified = false;
    updateSpecificationModificationTime();
    return true;
  }
  else
  {
    return false;
  }
}

void FileSystem::openFromArgument(const QString& inputFilePath)
{
  QFileInfo inputFileInfo(inputFilePath);
  if (!inputFileInfo.exists())
  {
    executeInformationBox(parent, "Open project",
                          "Could not find the provided input " + inputFilePath);
    return;
  }

  if (inputFileInfo.isDir())
  {
    openProjectFromFolder(inputFilePath);
  }
  else if (inputFileInfo.suffix() == "mcrl2")
  {
    loadSpecification(inputFilePath);
    specificationOnlyMode = true;
    emit enterSpecificationOnlyMode();
  }
  else
  {
    executeInformationBox(parent, "Open project",
                          "The provided input argument should be a project "
                          "folder or an mCRL2 specification (.mcrl2)");
  }
}

void FileSystem::openProjectFromFolder(const QString& newProjectFolderPath)
{
  QString context = "Open Project";
  QDir projectFolder(newProjectFolderPath);

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

  /* check if there is exactly one project file */
  if (projectFiles.length() == 0)
  {
    executeInformationBox(
        parent, context,
        "Provided folder does not contain a project file (ending with '" +
            projectFileExtension + "')");
    return;
  }
  if (projectFiles.length() > 1)
  {
    executeInformationBox(
        parent, context, "Provided folder contains more than one project file");
    return;
  }

  /* read the project file to get the specification path */
  QString newProjectFilePath =
      newProjectFolderPath + QDir::separator() + projectFiles.first();
  QFile projectFile(newProjectFilePath);
  projectFile.open(QIODevice::ReadOnly);
  QTextStream projectOpenStream(&projectFile);
  QString newProjectFileContents = projectOpenStream.readAll();
  projectFile.close();

  bool successfullyParsed = true;
  QString parseError = "";
  int parseErrorRow = 0;
  int parseErrorColumn = 0;
  QDomDocument newProjectOptions;
  /* if the project file begins with SPEC, it is old type of project file and
   *   should be converted to the new format
   * this should be removed in the future */
  if (newProjectFileContents.startsWith("SPEC"))
  {
    newProjectOptions = convertProjectFileToNewFormat(
        newProjectFolderPath, newProjectFilePath, newProjectFileContents);
  }
  else
  {
    successfullyParsed = newProjectOptions.setContent(
        newProjectFileContents, &parseError, &parseErrorRow, &parseErrorColumn);
  }

  if (!successfullyParsed)
  {
    executeInformationBox(
        parent, context,
        "Project file could not be parsed correctly: " + parseError +
            " on line " + QString::number(parseErrorRow) + " and column " +
            QString::number(parseErrorColumn));
    return;
  }

  /* check if the project info contains a path to the specification */
  QDomElement specElement =
      newProjectOptions.elementsByTagName("spec").at(0).toElement();
  if (specElement.isNull())
  {
    executeInformationBox(
        parent, context,
        "Project file in provided project folder does not contain a "
        "specification (should contain a \"spec\" element with the path to the "
        "specification as value).");
    projectFile.close();
    return;
  }

  /* get the path to the specification */
  QString newSpecFilePath = specElement.text();
  if (QFileInfo(newSpecFilePath).isRelative())
  {
    newSpecFilePath =
        newProjectFolderPath + QDir::separator() + newSpecFilePath;
  }

  /* read the specification and put it in the specification editor */
  if (!loadSpecification(newSpecFilePath))
  {
    executeInformationBox(parent, context,
                          "Specification file given in the project file in the "
                          "provided project folder does not exist");
    return;
  }

  /* opening is successful, so set project variables */
  projectFolderPath = QFileInfo(newProjectFilePath).path();
  projectName = QFileInfo(newProjectFilePath).baseName();
  projectOptions = newProjectOptions;

  /* read all properties */
  properties.clear();

  QDomNodeList propertyNodes = projectOptions.elementsByTagName("property");
  for (int i = 0; i < propertyNodes.length(); i++)
  {
    QString propertyName = propertyNodes.at(i).toElement().text();

    /* find the file that corresponds to this property */
    QString propFilePath = findPropertyFilePath(propertyName);
    if (propFilePath.isEmpty())
    {
      if (executeBinaryQuestionBox(
              parent, context,
              "Could not find a property file with basename '" + propertyName +
                  "', do you want to remove it from the project file?"))
      {
        removePropertyFromProjectFile(propertyName);
        i--;
      }
      continue;
    }

    /* read the file and create the corresponding property */
    Property readProperty = readPropertyFromFile(propFilePath, context);
    if (!readProperty.name.isEmpty())
    {
      properties.push_back(readProperty);
    }
  }

  projectOpen = true;
  emit newProjectOpened();
}

void FileSystem::openProject()
{
  /* ask to save changes if there are any */
  if (isSpecificationModified())
  {
    QMessageBox::StandardButton result = executeQuestionBox(
        parent, "mCRL2 IDE",
        "There are changes in the current project, do you want to save?");
    switch (result)
    {
    case QMessageBox::Yes:
      if (!save())
      {
        return;
      }
      break;
    case QMessageBox::Cancel:
      return;
    default:
      break;
    }
  }

  /* ask the user for a project folder */
  QFileDialog* openProjectDialog = createFileDialog(2);
  if (openProjectDialog->exec() == QDialog::Accepted)
  {
    QString newProjectFolderPath = openProjectDialog->selectedFiles().first();
    openProjectFromFolder(newProjectFolderPath);
  }
  openProjectDialog->deleteLater();
}

QString FileSystem::findPropertyFilePath(const QString& propertyName)
{
  QDir propertiesFolder = QDir(propertiesFolderPath());
  QStringList fileNames =
      propertiesFolder.entryList(QStringList({"*.mcf", "*.equ"}));
  for (QString fileName : fileNames)
  {
    if (propertyName == fileName.left(fileName.length() - 4))
    {
      return propertiesFolderPath() + QDir::separator() + fileName;
    }
  }
  return "";
}

Property FileSystem::readPropertyFromFile(const QString& propertyFilePath,
                                          const QString& context)
{
  QFile propertyFile(propertyFilePath);
  QString fileName = QFileInfo(propertyFile).fileName();
  QString fileExtension = fileName.right(3);
  QString propertyName = fileName.left(fileName.size() - 4);

  if (propertyFile.open(QIODevice::ReadOnly))
  {
    QTextStream propertyOpenStream(&propertyFile);
    QString propertyText = propertyOpenStream.readAll();
    propertyFile.close();

    if (fileExtension == "mcf")
    {
      return Property(propertyName, propertyText);
    }
    else // fileExtension == "equ"
    {
      /* extract equivalence and alternate process */
      int firstNewlineIndex = propertyText.indexOf("\n");
      if (firstNewlineIndex != -1)
      {
        mcrl2::lts::lts_equivalence equivalence =
            getEquivalenceFromName(propertyText.left(firstNewlineIndex));
        if (equivalence != mcrl2::lts::lts_eq_none)
        {
          QString altProcess =
              propertyText.right(propertyText.size() - firstNewlineIndex)
                  .trimmed();

          return Property(propertyName, altProcess, false, equivalence);
        }
      }
      executeInformationBox(
          parent, context,
          "Could not read the file '" + propertyFilePath +
              "': an equivalence property file should have the name of the "
              "equivalence on the first line and the alternate process "
              "starting from the second line");
    }
  }
  else
  {
    executeInformationBox(parent, context,
                          "Could not read the file '" + propertyFilePath + "'");
  }

  return Property();
}

void FileSystem::newProperty(const Property& property)
{
  /* add to the properties list and save it */
  properties.push_back(property);
  saveProperty(property);

  /* edit the project options and save it */
  QDomNode propertiesNode =
      projectOptions.elementsByTagName("properties").at(0);

  QDomElement propertyElement = projectOptions.createElement("property");
  propertiesNode.appendChild(propertyElement);
  QDomText propertyPathNode = projectOptions.createTextNode(property.name);
  propertyElement.appendChild(propertyPathNode);

  updateProjectFile();
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
      if (propertyFilePath.endsWith(".mcf") ||
          propertyFilePath.endsWith(".equ"))
      {
        Property importedProperty =
            readPropertyFromFile(propertyFilePath, "Import Properties");
        if (propertyNameExists(importedProperty.name))
        {
          executeInformationBox(parent, "Import properties",
                                "A property with name '" +
                                    importedProperty.name +
                                    "' already exists in this project");
        }
        else
        {
          if (!importedProperty.name.isEmpty())
          {
            importedProperties.push_back(importedProperty);
            newProperty(importedProperty);
          }
        }
      }
    }
  }
  return importedProperties;
}

void FileSystem::editProperty(const Property& oldProperty,
                              const Property& newProperty)
{
  /* alter the properties list and save it */
  std::replace(properties.begin(), properties.end(), oldProperty, newProperty);
  saveProperty(newProperty);

  /* edit the project options if the name has changed and save it */
  if (oldProperty.name != newProperty.name)
  {
    QDomNodeList propertyNodes = projectOptions.elementsByTagName("property");
    for (int i = 0; i < propertyNodes.length(); i++)
    {
      QDomElement propertyElement = propertyNodes.at(i).toElement();
      if (propertyElement.text() == oldProperty.name)
      {
        QDomText newPropertyNameNode =
            projectOptions.createTextNode(newProperty.name);
        propertyElement.replaceChild(newPropertyNameNode,
                                     propertyElement.firstChild());
      }
    }

    updateProjectFile();
  }
}

void FileSystem::removePropertyFromProjectFile(const QString& propertyName)
{
  QDomNodeList propertyNodes = projectOptions.elementsByTagName("property");
  for (int i = 0; i < propertyNodes.length(); i++)
  {
    QDomElement propertyElement = propertyNodes.at(i).toElement();
    if (propertyElement.text() == propertyName)
    {
      propertyElement.parentNode().removeChild(propertyElement);
    }
  }

  updateProjectFile();
}

void FileSystem::deletePropertyFile(const Property& property)
{
  QFile(propertyFilePath(property)).remove();
}

void FileSystem::deleteProperty(const Property& oldProperty)
{
  properties.remove(oldProperty);
  removePropertyFromProjectFile(oldProperty.name);
  deletePropertyFile(oldProperty);
}

bool FileSystem::save(bool forceSave)
{
  /* if we have a project open or if we are in specification only mode, we have
   *   a location to save in so we can simply save, else use save as */
  if (projectOpen || specificationOnlyMode)
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
    if (forceSave && !specificationOnlyMode)
    {
      makeSurePropertiesFolderExists();
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
    return saveAs();
  }
}

bool FileSystem::saveAs()
{
  /* if in specification only mode, ask the user for a name and location */
  if (specificationOnlyMode)
  {
    QFileDialog* saveAsDialog = createFileDialog(1);
    if (saveAsDialog->exec() == QDialog::Accepted)
    {
      /* if successful, check if we can create a new specification, if so save
       *   it */
      QString newSpecFilePath = saveAsDialog->selectedFiles().first();
      QFile specFile(newSpecFilePath);
      bool success = specFile.open(QIODevice::WriteOnly);
      specFile.close();
      if (success)
      {
        specFilePath = newSpecFilePath;
        return save();
      }
      else
      {
        /* if failed, tell the user */
        executeInformationBox(parent, "Save Specification As",
                              "Could not create specification");
      }
    }
    return false;
  }
  /* if not in specification only mode, create a new project to save the
   *   current as and save if successful */
  else
  {
    if (newProject(false, false))
    {
      return save(true);
    }
    else
    {
      return false;
    }
  }
}

void FileSystem::saveProperty(const Property& property, bool forParsing)
{
  makeSurePropertiesFolderExists();

  QFile propertyFile(propertyFilePath(property, forParsing));
  propertyFile.open(QIODevice::WriteOnly);
  QTextStream saveStream(&propertyFile);

  if (property.mucalculus)
  {
    saveStream << property.text;
  }
  else
  {
    saveStream << getEquivalenceName(property.equivalence) << "\n"
               << property.text;
  }

  propertyFile.close();
}

void FileSystem::createReinitialisedSpecification(const Property& property,
                                                  bool forParsing)
{
  if (!upToDateOutputFileExists(specificationFilePath(),
                                specificationFilePath(property.name)) ||
      !upToDateOutputFileExists(propertyFilePath(property, forParsing),
                                specificationFilePath(property.name)))
  {
    QString spec = readSpecification();
    int initIndex = spec.indexOf(QRegExp("(^|[; \\t\\n\\r])init[ \\t\\n\\r]"));
    QString alternateSpec = spec;
    int initSize = spec.indexOf(";", initIndex + 5) - initIndex +
                   (spec.at(initIndex) == 'i' ? 1 : 0);
    alternateSpec.replace(initIndex + (spec.at(initIndex) == 'i' ? 0 : 1),
                          initSize, property.text);

    QFile alternateSpecFile(specificationFilePath(property.name));
    alternateSpecFile.open(QIODevice::WriteOnly);
    QTextStream saveStream(&alternateSpecFile);
    saveStream << alternateSpec;
    alternateSpecFile.close();
  }
}

void FileSystem::openProjectFolderInExplorer()
{
  QDesktopServices::openUrl(projectFolderPath);
}

void FileSystem::removeTemporaryFolder()
{
  temporaryFolder.remove();
}
