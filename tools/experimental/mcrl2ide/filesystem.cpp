#include "filesystem.h"

#include <QMessageBox>

FileSystem::FileSystem(CodeEditor *specificationEditor, ConsoleDock *consoleDock)
{
    this->specificationEditor = specificationEditor;
    this->consoleDock = consoleDock;

    /* check if the projects folder exists, if not create it */
    if (!projectsFolder->exists()) {
        QDir().mkdir("projects");
    }
}

QString FileSystem::specificationFileName()
{
    return projectName.append("_spec.mcrl");
}

QString FileSystem::lpsFileName()
{
    return projectName.append("_lps.lps");
}

QString FileSystem::propertyFileName(QString propertyName)
{
    return propertyName.append(".mcf");
}

QString FileSystem::pbesFileName(QString propertyName)
{
    return projectName.append("_").append(propertyName).append("_pbes.pbes");
}

QDir *FileSystem::projectFolder()
{
    QDir *projectFolder = new QDir(*projectsFolder);
    projectFolder->cd(projectName);
    return projectFolder;
}

QDir *FileSystem::propertiesFolder()
{
    QDir *propertiesFolder = projectFolder();
    propertiesFolder->cd(propertiesFolderName);
    return propertiesFolder;
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
        projectFolder()->mkdir(propertiesFolderName);
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

void FileSystem::saveProject()
{
    /* Not implemented yet */
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
