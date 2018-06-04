#include "filesystem.h"

#include <QMessageBox>
#include <QFileInfo>
#include <QTextStream>

FileSystem::FileSystem(CodeEditor *specificationEditor)
{
    this->specificationEditor = specificationEditor;
    this->consoleDock = consoleDock;
    projectOpen = false;

    /* check if the projects folder exists, if not create it */
    if (!projectsFolder->exists()) {
        QDir().mkdir("projects");
    }
}

QString FileSystem::specificationFileName()
{
    return projectName + "_spec.mcrl";
}

QString FileSystem::lpsFileName()
{
    return projectName + "_lps.lps";
}

QString FileSystem::propertyFileName(QString propertyName)
{
    return propertyName + ".mcf";
}

QString FileSystem::pbesFileName(QString propertyName)
{
    return projectName + "_" + propertyName + "_pbes.pbes";
}


void FileSystem::setConsoleDock(ConsoleDock *consoleDock)
{
    this->consoleDock = consoleDock;
}

bool FileSystem::projectOpened()
{
    return projectOpen;
}

const QDir *FileSystem::getExecutablesFolder()
{
    return executablesFolder;
}

QString FileSystem::getCurrentSpecification()
{
    return specificationEditor->toPlainText();
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
    QString specificationFilePath = projectFolder->path() + QDir::separator() + specificationFileName();
    QFile *specificationFile = new QFile(specificationFilePath);
    specificationFile->open(QIODevice::WriteOnly);
    QTextStream *saveStream = new QTextStream(specificationFile);
    *saveStream << specificationEditor->toPlainText();
    specificationFile->close();
}

void FileSystem::saveProperty(QString propertyName, QString propertyText)
{
    QString propertyFilePath = propertiesFolder->path() + QDir::separator() + propertyFileName(propertyName);
    QFile *propertyFile = new QFile(propertyFilePath);
    propertyFile->open(QIODevice::WriteOnly);
    QTextStream *saveStream = new QTextStream(propertyFile);
    *saveStream << propertyText;
    propertyFile->close();
}

void FileSystem::saveProjectAs(QString projectName)
{
    /* Not implemented yet */
}

QProcess *FileSystem::mcrl22lps(bool verification)
{
    /* Not implemented yet */
    return new QProcess();
}

QProcess *FileSystem::lpsxsim()
{
    /* Not implemented yet */
    return new QProcess();
}

QProcess *FileSystem::lps2lts()
{
    /* Not implemented yet */
    return new QProcess();
}

QProcess *FileSystem::ltsconvert()
{
    /* Not implemented yet */
    return new QProcess();
}

QProcess *FileSystem::lps2pbes(QString propertyName)
{
    /* Not implemented yet */
    return new QProcess();
}

QProcess *FileSystem::pbes2bool(QString propertyName)
{
    /* Not implemented yet */
    return new QProcess();
}
