#ifndef TOOL_H
#define TOOL_H

#include <map>
#include <list>
#include <vector>
#include <string>

class XMLTextReader;

/* Distinguished object types */
typedef enum { input, intermediate, output } ToolObjectType;

class ToolObject;
class ToolInputObject;
class ToolOutputObject;
class ToolMode;

/*
 * The object that is used to communicate tool information with the GUI component
 *
 */
class Tool {
  friend class ToolManager;

  protected:
    std::string name;               /* A, not necessarily unique, name for the tool */
    std::string location;           /* A location where the tool can be found (URI to the tool) */
    std::string description;        /* A description for the tool */

  private:
    std::string identifier;         /* A unique identifier for the tool as: [a-z][a-z0-9_]+ */

    /* Modes connected to the input/output descriptor objects */
    std::vector < ToolMode* > modes;

    /* Read from XML using a libXML2 reader */
    void Read(XMLTextReader& reader);

    /* Write as XML to stream */
    bool Write(std::ostream& stream = std::cout) const;

  public:

    Tool();
    ~Tool();

    /* Pretty print tool configuration */
    void Print(std::ostream& stream = std::cerr) const;

    /* Get the name */
    inline const std::string& GetName() const {
      return (name);
    }

    /* Get the indentifier */
    inline const std::string& GetIdentifier() const {
      return (identifier);
    }

    /* Get the location */
    inline const std::string& GetLocation() const {
      return (location);
    }

    /* Get the description */
    inline const std::string& GetDescription() const {
      return (description);
    }

    /* Get the operational mode specifications of the tool */
    inline const std::vector < ToolMode* >& GetModes() const {
      return (modes);
    }

    /* Get the operational mode specifications of the tool */
    inline const ToolMode& GetMode(const size_t mode_number) const {
      return (*modes[mode_number]);
    }
};

/*
 * Type for the interchange of information about a tool between GUI and Tool Manager
 *
 * Notes:
 *  - The selector string may be empty if there is only one tool mode of operation
 */
class ToolMode {
  friend class Tool;

  private:
    std::string selector;    /* Command line argument that selects the mode */
    std::string description; /* Description of what a tool does in this mode */
    std::string category;    /* A category for the tools function e.g. conversion, transformation, visualisation, ... */

    /* The list of input/intermediate/output objects a tool requires or creates */
    std::list < ToolObject* > objects;

    /* Read from XML using a libXML2 reader */
    void Read(XMLTextReader& reader);

    /* Write as XML to stream */
    bool Write(std::ostream& stream = std::cout) const;

  public:

    /* Destructor */
    ~ToolMode();

    /* Returns: objects */
    const std::list < ToolObject* > GetObjectList() const;

    /* Pretty print tool mode configuration */
    void Print(std::ostream& stream = std::cerr) const;

    inline bool HasSelector() const {
      return (selector != "");
    }

    /* Get the current selector */
    inline const std::string& GetSelector() const {
      return (selector);
    }

    /* Get the current selector */
    inline const std::string& GetCategory() const {
      return (category);
    }

    /* Get the description */
    inline const std::string& GetDescription() const {
      return (description);
    }

    /* Get the list of objects */
    inline const std::list < ToolObject* >& GetObjects() const {
      return (objects);
    }

    /* Return output object number <|object_number|> (must exist) */
    const ToolOutputObject& GetOutputObject(size_t object_number) const;

    /* Return input object number <|object_number|> (must exist) */
    const ToolInputObject& GetInputObject(size_t object_number) const;

    /* Returns whether the mode produces output objects */
    const bool HasOutputObjects() const;

    /* Returns whether the mode requires input objects */
    const bool HasInputObjects() const;

    /* Generates a name based on <|name|> and the restrictions of output with respect to input objects of this mode */
    const std::string ChooseName(size_t output_number, std::string name) const;
};

/* Abstract type that describes an object of a tool */
class ToolObject {
  friend class ToolMode;

  protected:

    std::string name;           /* An optional name for the object with `%' or '*' */
    std::string selector;       /* Optional command line argument that selects this object (e.g. `--input-file') */
    std::string formalism;      /* The formalism of the object */
    std::string default_format; /* An optional default format */
    bool        obligatory;     /* Whether the input should be represented in the final command */
 
    /* Maps a format to a selector (e.g. trs is mapped to `--type trs') */
    std::map < std::string, std::string > format_selectors;

  private:

    /* Write as XML to stream */
    virtual bool Write(std::ostream& stream = std::cout) const = 0;

    /* Read from XML using a libXML2 reader */
    void Read(XMLTextReader& reader);

  public:

    ToolObjectType type;

    virtual ~ToolObject();

    /* Pretty print tool mode configuration */
    virtual void Print(std::ostream& stream = std::cerr) const;

    /* Returns the type of the object */
    virtual ToolObjectType GetType() const = 0;

    /* Returns the name of the object */
    inline const std::string& GetName() const {
      return (name);
    }

    /* Returns whether the object has a name that depends on for instance that of other objects */
    inline bool HasName() const {
      return (name != "");
    }

    /* Get formalism of the object */
    inline const std::string& GetFormalism() const {
      return (formalism);
    }

    /* Get the map of format selectors */
    inline const std::map < std::string, std::string >& GetFormatSelectors() const {
      return (format_selectors);
    }

    /* Get selectors */
    inline const std::string& GetSelector() const {
      return (selector);
    }

    /* Returns whether the object name should be preceded by an option (selector) */
    inline bool HasSelector() const {
      return (selector != "");
    }

    /* Get the objects default format (command line default) */
    inline const std::string& GetDefaultFormat() const {
      return (default_format);
    }

    /* Get the default format, or some other format if there is no default */
    inline const std::string& GetSomeFormat() const {
      if (default_format != "") {
        return (default_format);
      }
      else {
        std::map < std::string, std::string >::const_iterator i = format_selectors.begin();

        return ((*i).first);
      }
    }

    /* Whether the option has a default format, or a format must be specified */
    inline const bool HasDefaultFormat() const {
      return (default_format != "");
    }

    /* Print argument string prefixed with a space, such as it would occur on the command line */
    inline const std::string String(const std::string& file_name, const std::string& format) const {
      std::string return_value(" ");

      if (selector != "") {
        return_value.append(selector);
       
        /* May also add a `=' here, or quotes ... */
        return_value.append(" ");
      }

      /* Could use the format in the name */
      return_value.append(file_name);

      return(return_value);
    }

    inline bool IsObligatory() const {
      return(obligatory);
    }

    inline bool IsVirtual() const {
      return(!obligatory);
    }
};

/* Type that describes an input object of a tool */
class ToolInputObject : public ToolObject {
  friend class ToolMode;

  private:

    /* Write as XML to stream */
    inline bool Write(std::ostream& stream = std::cout) const;

    /* Read from XML using a libXML2 reader */
    inline void Read(XMLTextReader& reader);

  public:

    ToolInputObject();
    ~ToolInputObject();

    /* Pretty print tool mode configuration */
    void Print(std::ostream& stream = std::cerr) const;

    /* Returns the type of the object */
    inline ToolObjectType GetType() const {
      return (input);
    }
};

/* Type that describes an output object of a tool */
class ToolOutputObject : public ToolObject {
  friend class ToolMode;

  private:

    /* Write as XML to stream */
    inline bool Write(std::ostream& stream = std::cout) const;

    /* Read from XML using a libXML2 reader */
    inline void Read(XMLTextReader& reader);

  public:

    ToolOutputObject();
    ~ToolOutputObject();

    /* Pretty print tool mode configuration */
    void Print(std::ostream& stream = std::cerr) const;

    /* Returns the type of the object */
    inline ToolObjectType GetType() const {
      return (output);
    }
};

#endif
