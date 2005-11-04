#ifndef SPECIFICATION_H
#define SPECIFICATION_H

#include <string>
#include <vector>
#include <iostream>
#include <ostream>

/*
 * A specification is a container that specifies the dependencies on other
 * specifications and what tool is used to generate a set of new `output objects'.
 * Potentially an output object is a new specification, but the current
 * visualisation does not support it. A future more advanced visualisation
 * might, so the data is already gathered here.
 *
 * A better concept than specification would be Processor, because what is
 * captured here are input/output relations. This does not fit the current
 * visualisation at all so we stick to the name specification.
 */

#define UNSPECIFIED_TOOL ""

class XMLTextReader;
class Specification;

/* Type that is used to keep the status of a specification */
typedef enum {
  non_existent,         /* outputs do not exist */
  being_computed,       /* outputs are being generated */
  not_up_to_date,       /* outputs exist but are not up to date */
  up_to_date            /* outputs exist and are up-to-date */
} specification_status;

typedef union {
  Specification* pointer;
  unsigned int   identifier;
} PointerIdentifier;

/* Type to hold information about input objects */
typedef struct {
  PointerIdentifier derived_from;  /* Specification (with output objects) from which these outputs are derived */
  unsigned int      output_number; /* The number of an output object */
} SpecificationInputType;

/* Type to hold information about output objects */
typedef struct {
  std::string format;
  std::string file_name;
  std::string md5_hash;
} SpecificationOutputType;

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

    std::string  name;                        /* A name for the specification (need not be unique) */
    unsigned int identifier;                  /* A number that uniquely identifies this specification */
    std::string  tool_configuration;          /* A string of command line arguments that should be passed at tool execution */
    unsigned int tool_mode;                   /* What mode the tool is configured to run in */
    std::string  tool_identifier;             /* Identifies the tool that is required to run the command */
    std::string  description;                 /* Optional description */

    std::vector < SpecificationInputType >  input_objects;   /* Storage objects that this specification depends on */
    std::vector < SpecificationOutputType > output_objects;  /* Storage objects that this specification is comprised of */

    /* Whether the specification is up to date */
    specification_status status;

    /* Recursively checks whether specification is up to date (ignores the set status) */
    specification_status CheckStatus();

    /* Generate the specification (instantiation on storage) */
    bool Generate() throw (void*);

    /* Remove specification from storage */
    bool Delete();

    /* Read from XML using a libXML2 reader */
    bool Read(XMLTextReader& reader) throw (int);

    /* Write as XML to stream */
    bool Write(std::ostream& stream = std::cout);

    /* Store changes to the configuration of this specification to storage (TODO) */
    bool Commit();

  public:

    Specification();

    /* Get name */
    inline std::string GetName() const {
      return (name);
    }

    /* Set name */
    inline void SetName(std::string new_name) {
      name = new_name;
    }

    /* Get tool configuration */
    inline std::string GetToolConfiguration() const {
      return (tool_configuration);
    }

    /* Set tool configuration */
    inline void SetToolConfiguration(std::string new_configuration) {
      tool_configuration = new_configuration;
    }

    /* Get tool mode of operation */
    inline unsigned int GetToolMode() const {
      return (tool_mode);
    }

    /* Set tool mode of operation */
    inline void SetToolMode(unsigned int new_mode) {
      tool_mode = new_mode;
    }

    /* Get tool identifier */
    inline std::string GetToolIdentifier() const {
      return (tool_identifier);
    }

    /* Set tool idenfier */
    inline void SetToolIdentifier(std::string new_identifier) {
      tool_identifier = new_identifier;
    }

    /* Get description */
    inline std::string GetDescription() const {
      return (description);
    }

    /* Set description */
    inline void SetDescription(std::string new_description) {
      description = new_description;
    }

    /* Get input objects */
    inline const std::vector < SpecificationInputType >& GetInputObjects() const {
      return (input_objects);
    }

    /* Get input objects (modifiable) */
    std::vector < SpecificationInputType >& GetModifiableInputObjects() {
      /* TODO force changes to persistent storage */
      return (input_objects);
    }

    /* Get number of input objects */
    inline unsigned int GetNumberOfInputObjects() const {
      return (input_objects.size());
    }

    /* Set input objects */
    inline void SetInputObjects(std::vector < SpecificationInputType >& new_input_objects) {
      input_objects = new_input_objects;
    }

    /* Get output objects */
    inline const std::vector < SpecificationOutputType >& GetOutputObjects() const {
      return (output_objects);
    }

    /* Get output objects */
    std::vector < SpecificationOutputType >& GetModifiableOutputObjects() {
      /* TODO force changes to persistent storage */
      return (output_objects);
    }

    /* Get number output objects */
    inline unsigned int GetNumberOfOutputObjects() const {
      return (output_objects.size());
    }

    /* Set output objects */
    inline void SetOutputObjects(std::vector < SpecificationOutputType >& new_output_objects) {
      output_objects = new_output_objects;
    }

    /* Returns the value of uptodate */
    inline bool IsUpToDate() const {
      return (status == up_to_date);
    }

    /* Sets status to one of the not-available ones and removes the instances of specification from storage */
    inline void ResetAvailability() {
      status = non_existent;
    }

    /* If a specification is up-to-date this toggles the state to */
    inline void ToggleRegeneration() {
      if (status == up_to_date) {
        status = not_up_to_date;
      }
      else {
        status = CheckStatus();
      }
    }

    /* Pretty prints the fields of the specification */
    void Print(std::ostream& stream = std::cerr) const;
};

#endif

