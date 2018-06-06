#include "processsystem.h"

ProcessSystem::ProcessSystem(FileSystem *fileSystem)
{
    this->fileSystem = fileSystem;
}

void ProcessSystem::setConsoleDock(ConsoleDock *consoleDock)
{
    this->consoleDock = consoleDock;
}

QProcess *ProcessSystem::mcrl22lps(bool verification)
{
    ConsoleDock::ConsoleTab consoleTab = verification ? ConsoleDock::Verification : ConsoleDock::LTSCreation;
    consoleDock->setConsoleTab(consoleTab);
    consoleDock->writeToConsole(consoleTab, "##### CREATING LPS #####\n");

    /* check if we need to run this */
    if (!fileSystem->upToDateLpsFileExists()) {
        /* save the specification */
        fileSystem->saveSpecification();

        /* create the process */
        QProcess *mcrl22lpsProcess = new QProcess();
        QString program = fileSystem->getExecutablesFolder()->path() + QDir::separator() + "mcrl22lps.exe";
        QStringList arguments;
        arguments << fileSystem->specificationFilePath() << fileSystem->lpsFilePath() << "--lin-method=regular" << "--rewriter=jitty" << "--verbose";

        /* connect to logger */
        if (verification) {
            QObject::connect(mcrl22lpsProcess, SIGNAL(readyReadStandardError()), consoleDock, SLOT(logToVerificationConsole()));
        } else {
            QObject::connect(mcrl22lpsProcess, SIGNAL(readyReadStandardError()), consoleDock, SLOT(logToLTSCreationConsole()));
        }

        /* queue the process */
        mcrl22lpsProcess->start(program, arguments);
        return mcrl22lpsProcess;
    } else {
        consoleDock->writeToConsole(consoleTab, "Up to date lps already exists\n");
        return NULL;
    }
}

QProcess *ProcessSystem::lpsxsim()
{
    /* Not implemented yet */
    return new QProcess();
}

QProcess *ProcessSystem::lps2lts()
{
    /* Not implemented yet */
    return new QProcess();
}

QProcess *ProcessSystem::ltsconvert()
{
    /* Not implemented yet */
    return new QProcess();
}

QProcess *ProcessSystem::lps2pbes(QString propertyName)
{
    consoleDock->writeToConsole(ConsoleDock::Verification, "##### CREATING PBES #####\n");
    /* check if we need to run this */
    if (!fileSystem->upToDatePbesFileExists(propertyName)) {
        /* create the process */
        QProcess *lps2pbesProcess = new QProcess();
        QString program = fileSystem->getExecutablesFolder()->path() + QDir::separator() + "lps2pbes.exe";
        QStringList arguments;
        arguments << fileSystem->lpsFilePath() << fileSystem->pbesFilePath(propertyName) << "--formula=" + fileSystem->propertyFilePath(propertyName) << "--out=pbes" << "--verbose";

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

QProcess *ProcessSystem::pbes2bool(QString propertyName)
{
    /* create the process */
    QProcess *pbes2boolProcess = new QProcess();
    QString program = fileSystem->getExecutablesFolder()->path() + QDir::separator() + "pbes2bool.exe";
    QStringList arguments;
    arguments << fileSystem->pbesFilePath(propertyName) << "--erase=none" << "--in=pbes" << "--rewriter=jitty" << "--search=breadth-first" << "--solver=lf" << "--strategy=0" <<"--verbose";

    /* connect to logger */
    consoleDock->writeToConsole(ConsoleDock::Verification, "##### SOLVING PBES #####\n");
    QObject::connect(pbes2boolProcess, SIGNAL(readyReadStandardError()), consoleDock, SLOT(logToVerificationConsole()));

    /* start the process */
    pbes2boolProcess->start(program, arguments);
    return pbes2boolProcess;
}
