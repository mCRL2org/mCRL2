#ifndef PROCESSSYSTEM_H
#define PROCESSSYSTEM_H

#include "filesystem.h"
#include "consoledock.h"

#include <QProcess>

class FileSystem;

class ProcessSystem
{
public:
    ProcessSystem(FileSystem *fileSystem);

    /**
     * @brief setConsoleDock Assigns the console dock to the file system for logging
     * @param consoleDock The console dock
     */
    void setConsoleDock(ConsoleDock *consoleDock);

    /**
     * @brief mcrl22lps Executes mcrl22lps on the current specification
     * @param verification Determines what console dock tab to use
     */
    QProcess *mcrl22lps(bool verification);

    /**
     * @brief lpsxsim Executes lpsxsim on the lps that corresponds to the current specification
     */
    QProcess *lpsxsim();

    /**
     * @brief lps2lts Executes lps2lts on the lps that corresponds to the current specification
     */
    QProcess *lps2lts();

    /**
     * @brief ltsconvert Executes ltsconvert on the lts that corresponds to the current specification
     */
    QProcess *ltsconvert();

    /**
     * @brief lps2pbes Executes lps2pbes on the lps that corresponds to the current specification and the given property
     * @param propertyName The name of the property to include
     */
    QProcess *lps2pbes(QString propertyName);

    /**
     * @brief pbes2bool Executes pbes2bool on the pbes that corresponds to the current specification and the given property
     * @param propertyName The name of the property to include
     */
    QProcess *pbes2bool(QString propertyName);

private:
    FileSystem *fileSystem;
    ConsoleDock *consoleDock;
};

#endif // PROCESSSYSTEM_H
