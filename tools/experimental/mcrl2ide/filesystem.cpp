#include "filesystem.h"

#include <QFileInfo>
#include <QDateTime>
#include <QTextStream>
#include <QDirIterator>

Property::Property(QString name, QString text)
{
    this->name = name;
    this->text = text;
}

FileSystem::FileSystem(CodeEditor *specificationEditor, QWidget *parent)
{
    this->specificationEditor = specificationEditor;
    this->parent = parent;
    specificationModified = false;
    connect(specificationEditor, SIGNAL(textChanged()), this, SLOT(setSpecificationModified()));

    projectOpen = false;

    makeSureProjectsFolderExists();
}

void FileSystem::makeSureProjectsFolderExists()
{
    if (!QDir(projectsFolderPath).exists()) {
        QDir().mkpath(projectsFolderPath);
    }
}

void FileSystem::makeSureProjectFolderExists()
{
    if (!QDir(projectFolderPath(projectName)).exists()) {
        QDir().mkpath(projectFolderPath(projectName));
    }
}

void FileSystem::makeSurePropertiesFolderExists()
{
    if (!QDir(propertiesFolderPath(projectName)).exists()) {
        QDir().mkpath(propertiesFolderPath(projectName));
    }
}

QString FileSystem::projectFolderPath(QString projectName)
{
    return projectsFolderPath + QDir::separator() + projectName;
}

QString FileSystem::propertiesFolderPath(QString projectName)
{
    return projectFolderPath(projectName) + QDir::separator() + propertiesFolderName;
}

QString FileSystem::specificationFilePath()
{
    return projectFolderPath(projectName) + QDir::separator() + projectName + "_spec.mcrl";
}

QString FileSystem::lpsFilePath()
{
    return projectFolderPath(projectName) + QDir::separator() + projectName + "_lps.lps";
}

QString FileSystem::propertyFilePath(QString propertyName)
{
    return propertiesFolderPath(projectName) + QDir::separator() + propertyName + ".mcf";
}

QString FileSystem::pbesFilePath(QString propertyName)
{
    return propertiesFolderPath(projectName) + QDir::separator() + projectName + "_" + propertyName + "_pbes.pbes";
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
    emit hasChanges(true);
}

bool FileSystem::isPropertyModified(QString propertyName)
{
    return propertymodified[propertyName];
}

void FileSystem::setPropertyModified(QString propertyName)
{
    propertymodified[propertyName] = true;
    emit hasChanges(true);
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


QString FileSystem::newProject(QString projectName)
{
    makeSureProjectsFolderExists();

    /* the project name may not be empty */
    if (projectName.isEmpty()) {
        return "The project name may not be empty";
    }

    /* create the folder for this project */
    if (QDir(projectsFolderPath).mkdir(projectName)) {
        /* if successful, create the properties folder too */
        this->projectName = projectName;
        QDir(projectFolderPath(projectName)).mkdir(propertiesFolderName);
        projectOpen = true;
        return "";
    } else {
        /* if not successful, there already is a project folder with this name */
        return "A project with this name already exists";
    }
}

QStringList FileSystem::getAllProjects()
{
    makeSureProjectsFolderExists();
    return QDir(projectsFolderPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
}

std::list<Property*> FileSystem::openProject(QString projectName)
{
    this->projectName = projectName;
    projectOpen = true;

    /* read the specification and put it in the specification editor */
    QFile *specificationFile = new QFile(specificationFilePath());
    specificationFile->open(QIODevice::ReadOnly);
    QTextStream *openStream = new QTextStream(specificationFile);
    QString spec = openStream->readAll();
    specificationEditor->setPlainText(spec);

    /* get all properties and return them */
    std::list<Property*> properties;

    QDirIterator *dirIterator = new QDirIterator(QDir(propertiesFolderPath(projectName)));
    while (dirIterator->hasNext()) {
        QFile *propertyFile = new QFile(dirIterator->next());
        QFileInfo *propertyFileInfo = new QFileInfo(*propertyFile);
        QString fileName = propertyFileInfo->fileName();
        if (propertyFileInfo->isFile() && fileName.endsWith(".mcf")) {
            fileName.chop(4);
            propertyFile->open(QIODevice::ReadOnly);
            QTextStream *openStream = new QTextStream(propertyFile);
            QString propertyText = openStream->readAll();
            properties.push_back(new Property(fileName, propertyText));
        }
    }
    return properties;
}

void FileSystem::saveSpecification()
{
    makeSureProjectFolderExists();

    QFile *specificationFile = new QFile(specificationFilePath());
    specificationFile->open(QIODevice::WriteOnly);
    QTextStream *saveStream = new QTextStream(specificationFile);
    *saveStream << specificationEditor->toPlainText();
    specificationFile->close();
    specificationModified = false;
}

void FileSystem::saveProperty(Property *property)
{
    makeSurePropertiesFolderExists();

    QFile *propertyFile = new QFile(propertyFilePath(property->name));
    propertyFile->open(QIODevice::WriteOnly);
    QTextStream *saveStream = new QTextStream(propertyFile);
    *saveStream << property->text;
    propertyFile->close();
    propertymodified[property->name] = false;
}
