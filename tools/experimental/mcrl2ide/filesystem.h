#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "codeeditor.h"

#include <QDir>

class FileSystem
{
public:
    /**
     * @brief FileSystem Constructor
     * @param specificationEditor
     */
    explicit FileSystem(CodeEditor *specificationEditor);

    /**
     * @brief getExecutablesFolder Gets the folder containing the mCRL2 tool executables
     * @return The folder with the executables
     */
    const QDir *getExecutablesFolder();

    /**
     * @brief getCurrentSpecification Gets the specification that is in the specification editor
     * @return the current specification ion the specification editor
     */
    QString getCurrentSpecification();

    /**
     * @brief newProject Creates a new project with the corresponding file structure
     */
    void newProject(QString projectName);

private:
    const QDir *projectsFolder = new QDir("projects");
    const QString propertiesFolderName = "properties";
    const QDir *executablesFolder = new QDir("C:\\Users\\s123188\\Documents\\QtProjects\\mCRL2-IDE\\execs");

    CodeEditor *specificationEditor;
    QString projectName;

    /**
     * @brief specificationFileName Defines the file anem of a specification
     * @return The file name of the specification
     */
    QString specificationFileName();

    /**
     * @brief lpsFileName Defines the file name of a lps
     * @return The file name of the lps
     */
    QString lpsFileName();

    /**
     * @brief propertyFileName Defines the file name of a property
     * @param propertyName The name of the property
     * @return The file name of the property
     */
    QString propertyFileName(QString propertyName);

    /**
     * @brief pbesFileName Defines the file name of a pbes
     * @param propertyName The name of the property this pbes correspeonds to
     * @return The file name of the pbes
     */
    QString pbesFileName(QString propertyName);

    /**
     * @brief projectFolder Defines the project folder
     * @return The project folder
     */
    QDir *projectFolder();

    /**
     * @brief propertiesFolder Defines the properties folder
     * @return The properties folder
     */
    QDir *propertiesFolder();
};

#endif // FILESYSTEM_H
