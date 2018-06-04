#include "filesystem.h"

#include <QMessageBox>

FileSystem::FileSystem(CodeEditor *specificationEditor)
{
    this->specificationEditor = specificationEditor;

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
