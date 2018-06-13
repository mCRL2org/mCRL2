#include "processsystem.h"

ProcessSystem::ProcessSystem(FileSystem *fileSystem)
{
    this->fileSystem = fileSystem;
    pid = 0;
}

void ProcessSystem::setConsoleDock(ConsoleDock *consoleDock)
{
    this->consoleDock = consoleDock;
}

QProcess *ProcessSystem::createMcrl22lpsProcess(bool verification)
{
    QProcess *mcrl22lpsProcess = new QProcess();
    /* check if we need to run this */
    if (!fileSystem->upToDateLpsFileExists()) {

        /* create the process */
        mcrl22lpsProcess->setProgram("mcrl22lps");
        mcrl22lpsProcess->setArguments({fileSystem->specificationFilePath(), fileSystem->lpsFilePath(), "--lin-method=regular", "--rewriter=jitty", "--verbose"});

        /* connect to logger */
        if (verification) {
            connect(mcrl22lpsProcess, SIGNAL(readyReadStandardError()), consoleDock, SLOT(logToVerificationConsole()));
        } else {
            connect(mcrl22lpsProcess, SIGNAL(readyReadStandardError()), consoleDock, SLOT(logToLTSCreationConsole()));
        }

        return mcrl22lpsProcess;
    } else {
        return mcrl22lpsProcess;
    }
}

QProcess *ProcessSystem::createLpsxsimProcess()
{
    /* Not implemented yet */
    return new QProcess();
}

QProcess *ProcessSystem::createLps2ltsProcess()
{
    /* Not implemented yet */
    return new QProcess();
}

QProcess *ProcessSystem::createLtsconvertProcess()
{
    /* Not implemented yet */
    return new QProcess();
}

QProcess *ProcessSystem::createLps2pbesProcess(QString propertyName)
{
    QProcess *lps2pbesProcess = new QProcess();
    /* check if we need to run this */
    if (!fileSystem->upToDatePbesFileExists(propertyName)) {

        /* create the process */
        lps2pbesProcess->setProgram("lps2pbes");
        lps2pbesProcess->setArguments({fileSystem->lpsFilePath(), fileSystem->pbesFilePath(propertyName), "--formula=" + fileSystem->propertyFilePath(propertyName), "--out=pbes", "--verbose"});

        /* connect to logger */
        connect(lps2pbesProcess, SIGNAL(readyReadStandardError()), consoleDock, SLOT(logToVerificationConsole()));

        return lps2pbesProcess;
    } else {
        return NULL;
    }
}

QProcess *ProcessSystem::createPbes2boolProcess(QString propertyName)
{
    /* create the process */
    QProcess *pbes2boolProcess = new QProcess();
    pbes2boolProcess->setProgram("pbes2bool");
    pbes2boolProcess->setArguments({fileSystem->pbesFilePath(propertyName), "--erase=none", "--in=pbes", "--rewriter=jitty", "--search=breadth-first", "--solver=lf", "--strategy=0", "--verbose"});

    /* connect to logger */
    connect(pbes2boolProcess, SIGNAL(readyReadStandardError()), consoleDock, SLOT(logToVerificationConsole()));

    return pbes2boolProcess;
}


int ProcessSystem::verifyProperty(Property *property)
{
    if (fileSystem->isSpecificationModified()) {
        fileSystem->saveSpecification();
    }
    if (fileSystem->isPropertyModified(property->name)) {
        fileSystem->saveProperty(property);
    }

    consoleDock->setConsoleTab(ConsoleDock::Verification);

    /* create the subprocesses */
    int processid = pid++;

    QProcess *mcrl22lpsProcess = createMcrl22lpsProcess(true);
    mcrl22lpsProcess->setProperty("pid", processid);
    connect(mcrl22lpsProcess, SIGNAL(finished(int)), this, SLOT(verifyProperty2()));

    QProcess *lps2pbesProcess = createLps2pbesProcess(property->name);
    lps2pbesProcess->setProperty("pid", processid);
    connect(lps2pbesProcess, SIGNAL(finished(int)), this, SLOT(verifyProperty3()));

    QProcess *pbes2boolProcess = createPbes2boolProcess(property->name);
    pbes2boolProcess->setProperty("pid", processid);
    connect(pbes2boolProcess, SIGNAL(finished(int)), this, SLOT(verifyPropertyResult()));

    processes[processid] = {mcrl22lpsProcess, lps2pbesProcess, pbes2boolProcess};

    consoleDock->writeToConsole(ConsoleDock::Verification, "##### CREATING LPS #####\n");
    if (mcrl22lpsProcess->program() == "") {
        consoleDock->writeToConsole(ConsoleDock::Verification, "Up to date lps already exists\n");
        emit mcrl22lpsProcess->finished(0);
    } else {
        mcrl22lpsProcess->start();
    }

    return processid;
}

void ProcessSystem::verifyProperty2()
{
    consoleDock->writeToConsole(ConsoleDock::Verification, "##### CREATING PBES #####\n");
    int processid = qobject_cast<QProcess*>(sender())->property("pid").toInt();
    QProcess *lps2pbesProcess = processes[processid][1];
    if (lps2pbesProcess->program() == "") {
        consoleDock->writeToConsole(ConsoleDock::Verification, "Up to date pbes already exists");
        emit lps2pbesProcess->finished(0);
    } else {
        processes[processid][1]->start();
    }
}

void ProcessSystem::verifyProperty3()
{
    consoleDock->writeToConsole(ConsoleDock::Verification, "##### SOLVING PBES #####\n");
    int processid = qobject_cast<QProcess*>(sender())->property("pid").toInt();
    processes[processid][2]->start();
}

void ProcessSystem::verifyPropertyResult()
{
    int processid = qobject_cast<QProcess*>(sender())->property("pid").toInt();
    std::string output = processes[processid][2]->readAllStandardOutput().toStdString();
    if (output.find("true") == 0) {
        results[processid] = "true";
    } else if (output.find("false") == 0) {
        results[processid] = "false";
    } else {
        results[processid] = "";
    }
    emit processFinished(processid);
}

QString ProcessSystem::getResult(int processid)
{
    return results[processid];
}
