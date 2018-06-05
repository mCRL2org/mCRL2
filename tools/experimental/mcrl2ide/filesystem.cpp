#include "filesystem.h"

#include <QObject>
#include <QMessageBox>
#include <QFileInfo>
#include <QDateTime>
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
    QFile *specificationFile = new QFile(specificationFilePath());
    specificationFile->open(QIODevice::WriteOnly);
    QTextStream *saveStream = new QTextStream(specificationFile);
    *saveStream << specificationEditor->toPlainText();
    specificationFile->close();
}

void FileSystem::saveProperty(QString propertyName, QString propertyText)
{
    QFile *propertyFile = new QFile(propertyFilePath(propertyName));
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
    ConsoleDock::ConsoleTab consoleTab = verification ? ConsoleDock::Verification : ConsoleDock::LTSCreation;
    consoleDock->setConsoleTab(consoleTab);
    consoleDock->writeToConsole(consoleTab, "##### CREATING LPS #####\n");

    /* check if we need to run this (specification is modified or lps file does not exist) */
    if (specificationEditor->isWindowModified() || !QFile(lpsFilePath()).exists()) {
        /* save the specification */
        saveSpecification();

        /* create the process */
        QProcess *mcrl22lpsProcess = new QProcess();
        QString program = getExecutablesFolder()->path() + QDir::separator() + "mcrl22lps.exe";
        QStringList arguments;
        arguments << specificationFilePath() << lpsFilePath() << "--lin-method=regular" << "--rewriter=jitty" << "--verbose";

        /* connect to logger */
        if (verification) {
            QObject::connect(mcrl22lpsProcess, SIGNAL(readyReadStandardError()), consoleDock, SLOT(logToVerificationConsole()));
        } else {
            QObject::connect(mcrl22lpsProcess, SIGNAL(readyReadStandardError()), consoleDock, SLOT(logToLTSCreationConsole()));
        }

        /* start the process */
        mcrl22lpsProcess->start(program, arguments);
        return mcrl22lpsProcess;
    } else {
        consoleDock->writeToConsole(consoleTab, "Up to date lps already exists\n");
        return NULL;
    }
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
    consoleDock->writeToConsole(ConsoleDock::Verification, "##### CREATING PBES #####\n");
    /* check if we need to run this (lps file does not exist or lps file is newer than pbes file) */
    if (!QFile(pbesFilePath(propertyName)).exists() || QFileInfo(lpsFilePath()).fileTime(QFileDevice::FileModificationTime) > QFileInfo(pbesFilePath(propertyName)).fileTime(QFileDevice::FileModificationTime)) {
        /* create the process */
        QProcess *lps2pbesProcess = new QProcess();
        QString program = getExecutablesFolder()->path() + QDir::separator() + "lps2pbes.exe";
        QStringList arguments;
        arguments << lpsFilePath() << pbesFilePath(propertyName) << "--formula=" + propertyFilePath(propertyName) << "--out=pbes" << "--verbose";

        /* connect to logger */
        QObject::connect(lps2pbesProcess, SIGNAL(readyReadStandardError()), consoleDock, SLOT(logToVerificationConsole()));

        /* start the process */
        lps2pbesProcess->start(program, arguments);
        return lps2pbesProcess;
    } else {
        consoleDock->writeToConsole(ConsoleDock::Verification, "Up to date pbes already exists for property " + propertyName + "\n");
        return NULL;
    }
}

QProcess *FileSystem::pbes2bool(QString propertyName)
{
    /* create the process */
    QProcess *pbes2boolProcess = new QProcess();
    QString program = getExecutablesFolder()->path() + QDir::separator() + "pbes2bool.exe";
    QStringList arguments;
    arguments << pbesFilePath(propertyName) << "--erase=none" << "--in=pbes" << "--rewriter=jitty" << "--search=breadth-first" << "--solver=lf" << "--strategy=0" <<"--verbose";

    /* connect to logger */
    consoleDock->writeToConsole(ConsoleDock::Verification, "##### SOLVING PBES #####\n");
    QObject::connect(pbes2boolProcess, SIGNAL(readyReadStandardError()), consoleDock, SLOT(logToVerificationConsole()));

    /*start the process */
    pbes2boolProcess->start(program, arguments);
    return pbes2boolProcess;
}
