#ifndef TOOL_MANAGER_H_
#define TOOL_MANAGER_H_

#include <string>
#include <list>
#include <ostream>

extern "C" {
 #include <libxml/xmlreader.h>
}

#define UNSPECIFIED_TOOL ""

/*
 * A project is a collection of specifications objects along with meta
 * information about those specifications and how specifications that are not
 * initially in the project can be obtained by applying tools to specifications
 * that are in the project.
 *
 */
class Specification;

/* Pairs to assign storage locations to output objects */
typedef std::pair < Specification*, std::string > ObjectPair;

/*
 * A specification is either provided, in this case it is not generated from
 * other specifications, or generated.
 *
 * A generated specification is build using some tool with a particular
 * configuration from some input objects. The result is one or more output
 * objects that represent this specification on storage.
 */
class Specification {
  friend class ProjectManager;

  private:
    /* Whether the specification is up to date */
    bool uptodate;

    /* Checks if specification is up to date */
    bool CheckStatus();

    /* Generate the specification (instantiation on storage) */
    bool Generate() throw (void*);

    /* Remove specification from storage */
    bool Delete();

    /* Read from XML using a libXML2 reader */
    bool Read(xmlTextReaderPtr reader) throw (int);

    /* Write as XML to stream */
    bool Write(std::ostream& stream = std::cout);

    /* Store changes to the configuration of this specification to storage (TODO) */
    bool Commit();

  public:

    std::string  name;                        /* A name for the specification (need not be unique) */
    unsigned int identifier;                  /* A number that uniquely identifies this specification */
    std::string  tool_configuration;          /* A string of command line arguments that should be passed at tool execution */
    std::string  tool_identifier;             /* Identifies the tool that is required to run the command */
    std::string  description;                 /* Optional description */

    std::list < ObjectPair >  input_objects;  /* Specifications that this specification depends on */
    std::list < std::string > output_objects; /* Specifications that this specification depends on */

    Specification();

    /* Returns the value of uptodate */
    bool IsUpToDate();

    /* Resets up-to-date status to false */
    void SetNotUpToDate();

    /* Pretty prints the fields of the specification */
    void Print(std::ostream& stream = std::cerr);
};

class ProjectManager {
  private:

    std::list <Specification> specifications;

    /* Keeps the smallest natural number that has not yet been assigned as identifier */
    unsigned int free_identifier;

    std::string  project_root;
    std::string  project_description;

  public:

    ProjectManager();

    /* Set a project directory */
    void ProjectManager::SetProjectDirectory(std::string directory);

    /* Get project directory */
    std::string ProjectManager::GetProjectDirectory();

    /* Set project description */
    void ProjectManager::SetDescription(std::string description);

    /* Get project description */
    const std::string ProjectManager::GetDescription();

    /* Get a pointer to the list of specifications in this project */
    const std::list < Specification >* GetSpecifications();

    /* Close project, writing out changes if necessary */
    bool Close();

    /* Read project information from project_directory */
    bool Load();

    /* Write project information to storage */
    bool Store();

    /* Write project information to storage */
    void Print(std::ostream& stream = std::cerr);

    /* Add a new specification to the project */
    Specification* Add(Specification& specification);

    /* Remove a specification from the project */
    bool Remove(Specification* specification);

    /* Remove instance from a specification from storage */
    bool Flush(Specification* specification);

    /* Update information about a specification that is already in the project */
    bool Update(Specification* specification);

    /* Make all specifications in the project up to date */
    bool UpdateAll();
};

#endif
