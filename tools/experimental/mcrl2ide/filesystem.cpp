#include "filesystem.h"

#include <QMessageBox>
#include <QFileInfo>
#include <QDateTime>
#include <QTextStream>

Property::Property(QString name, QString text)
{
    this->name = name;
    this->text = text;
}

FileSystem::FileSystem(CodeEditor *specificationEditor)
{
    this->specificationEditor = specificationEditor;
    specificationModified = false;
    connect(specificationEditor, SIGNAL(textChanged()), this, SLOT(setSpecificationModified()));

    projectOpen = false;

    /* check if the projects folder exists, if not create it */
    if (!projectsFolder->exists()) {
        QDir().mkdir("projects");
    }
}

QString FileSystem::specificationFilePath()
{
    return projectFolder->path() + QDir::separator() + projectName + "_spec.mcrl";
}

QString FileSystem::lpsFilePath()
{
    return projectFolder->path() + QDir::separator() + projectName + "_lps.lps";
}

QString FileSystem::propertyFilePath(QString propertyName)
{
    return propertiesFolder->path() + QDir::separator() + propertyName + ".mcf";
}

QString FileSystem::pbesFilePath(QString propertyName)
{
    return propertiesFolder->path() + QDir::separator() + projectName + "_" + propertyName + "_pbes.pbes";
}

const QDir *FileSystem::getExecutablesFolder()
{
    return executablesFolder;
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
}

bool FileSystem::isPropertyModified(QString propertyName)
{
    return propertymodified[propertyName];
}

void FileSystem::setPropertyModified(QString propertyName)
{
    propertymodified[propertyName] = true;
}


bool FileSystem::upToDateLpsFileExists()
{
    /* an lps file is up to date if the lps file exists and the lps file is created after the the last time the specification file was modified */
    return QFile(lpsFilePath()).exists()
           && QFileInfo(specificationFilePath()).fileTime(QFileDevice::FileModificationTime) <= QFileInfo(lpsFilePath()).fileTime(QFileDevice::FileModificationTime);
}

bool FileSystem::upToDatePbesFileExists(QString propertyName)
{
    /* a pbes file is up to date if the pbes file exists and the pbes file is created after the last time both the lps and the property files were modified */
    return QFile(pbesFilePath(propertyName)).exists()
           && QFileInfo(lpsFilePath()).fileTime(QFileDevice::FileModificationTime) <= QFileInfo(pbesFilePath(propertyName)).fileTime(QFileDevice::FileModificationTime)
           && QFileInfo(propertyFilePath(propertyName)).fileTime(QFileDevice::FileModificationTime) <= QFileInfo(pbesFilePath(propertyName)).fileTime(QFileDevice::FileModificationTime);
}


void FileSystem::newProject(QString projectName)
{
    /* create the folder for this project */
    if (projectsFolder->mkdir(projectName)) {
        /* if successful, create the properties folder too */
        this->projectName = projectName;
        projectFolder = new QDir(projectsFolder->path() + QDir::separator() + projectName);
        projectFolder->mkdir(propertiesFolderName);
        propertiesFolder = new QDir(projectFolder->path() + QDir::separator() + propertiesFolderName);
        projectOpen = true;
    } else {
        /* if not succesful, tell the user */
        QMessageBox *msgBox = new QMessageBox();
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->setText("A project with this name already exists");
        msgBox->exec();
    }
}

void FileSystem::openProject(QString projectName)
{
    /* Not implemented yet */
}

void FileSystem::saveSpecification()
{
    QFile *specificationFile = new QFile(specificationFilePath());
    specificationFile->open(QIODevice::WriteOnly);
    QTextStream *saveStream = new QTextStream(specificationFile);
    *saveStream << specificationEditor->toPlainText();
    specificationFile->close();
    specificationModified = false;
}

void FileSystem::saveProperty(Property *property)
{
    QFile *propertyFile = new QFile(propertyFilePath(property->name));
    propertyFile->open(QIODevice::WriteOnly);
    QTextStream *saveStream = new QTextStream(propertyFile);
    *saveStream << property->text;
    propertyFile->close();
    propertymodified[property->name] = false;
}

void FileSystem::saveProjectAs(QString projectName)
{
    /* Not implemented yet */
}
