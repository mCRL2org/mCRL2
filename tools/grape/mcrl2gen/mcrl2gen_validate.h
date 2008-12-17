// Author(s): VitaminB100
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2gen_validate.h
//
// Declares internal functions used when validating process diagrams and architecture diagrams.

#ifndef MCRL2GEN_MCRL2GEN_VALIDATE_H
#define MCRL2GEN_MCRL2GEN_VALIDATE_H

#include <wx/wx.h>
#include <wx/xml/xml.h>
#include "libgrape/preamble.h"
#include "libgrape/label.h"
#include "libgrape/action.h"
#include "libgrape/decl.h"
#include "libgrape/varupdate.h"

#include <aterm2.h>

#define CONVERSION_ERROR            17

using namespace grape::libgrape;

namespace grape
{
  namespace mcrl2gen
  {
    /**
     * \short Represents a typed action.
     */
    struct action_type
    {
      wxString m_action;            /**< The name of the action. */
      wxString m_type;              /**< The type of the action. */
    };

    /**
     * \short Represents an array of typed actions.
     */
    WX_DECLARE_OBJARRAY(action_type, arr_action_type);

    /**
     * \short Represents a channel.
     */
    struct channel_id
    {
      wxString m_channel;           /**< The name of the channel. */
      wxString m_channel_id;        /**< The identifier of the channel. */
      wxString m_architecture_channel; /**< Name used for channels on architecture references (with correct id). */
    };

    /**
     * \short Represents an array of channels.
     */
    WX_DECLARE_OBJARRAY(channel_id, arr_channel_id);

    /**
     * \short Represents references and their associated actions.
     */
    struct action_reference
    {
      wxString m_reference;         /**< The name of the reference. */
      wxString m_reference_id;      /**< The identifier of the reference. */
      wxString m_diagram_id;        /**< The identifier of the diagram belonging to the reference. */
      bool m_is_process_reference;  /**< A flag indicating whether this represents a process reference or architecture reference. */
      list_of_action m_actions;      /**< An array of actions associated with this reference. */

      arr_channel_id m_channels;    /**< An array of channels associated with this reference. */
      wxArrayString m_blockeds;     /**< An array of blocked channels associated with this reference. */
      list_of_action m_hidden;      /**< An array of hidden channels associated with this reference. */
      arr_channel_id m_renamed;     /**< An array of renamed channels associated with this reference. */
    };

    /**
     * \short Represents an array of references with their associated actions.
     */
    WX_DECLARE_OBJARRAY(action_reference, arr_action_reference);

    /**
     * \short Represents a channel communication.
     */
    struct channel_comm
    {
      wxString m_name;              /**< The name of the channel communication. */
      wxString m_id;                /**< The identifier of the channel communication. */
      arr_channel_id m_channels;    /**< An array of channels associated with this channel communication. */
    };

    /**
     * \short Represents an array of channel communications.
     */
    WX_DECLARE_OBJARRAY(channel_comm, arr_channel_comm);

    /**
     * \short Represents a renamed channel or channel communication.
     */
    struct renamed
    {
      wxString m_old_name;          /**< The old name of this object. */
      wxString m_new_name;          /**< The new name of this object. */
    };

    /**
     * \short Represents an array of renamed channels or channel communications.
     */
    WX_DECLARE_OBJARRAY(renamed, arr_renamed);

    /**
     * XML node child retrieval function.
     * Retrieves a pointer to a child of an XML node.
     * @param p_parent The parent XML node.
     * @param p_child_name The name of the child node to retrieve.
     * @return A pointer to the specified child node, or 0 if the child node is not present.
     * @pre p_parent is a valid pointer to an XML node.
     * @post A pointer to the specified child node or 0 is returned.
     */
    wxXmlNode *get_child(wxXmlNode *p_parent, wxString p_child_name);

    /**
     * XML node child value retrieval function.
     * Retrieves the value from a child of an XML node.
     * @param p_parent The parent XML node.
     * @param p_child_name The name of the child to retrieve the value of.
     * @return The value of the specified child of the parent node, or the empty string if the child node is not present.
     * @pre p_parent is a valid pointer to an XML node.
     * @post The value of the specified child of p_parent or the empty string is returned.
     */
    wxString get_child_value(wxXmlNode *p_parent, wxString p_child_name);

    /**
     * XML process diagram name inference function.
     * Infers the name of a process diagram, based on its identifier.
     * @param p_doc_root The root node of the XML specification.
     * @param p_diagram_id The identifier of the process diagram to infer its name.
     * @return The name of the process diagram that has the given identifier.
     * @pre p_doc_root is a valid pointer to an XML specification.
     * @post The name of the inferred process diagram is returned or the empty string is returned and error messages are produced.
     */
    wxString infer_process_diagram_name(wxXmlNode *p_doc_root, wxString &p_diagram_id);

    /**
     * XML process diagram identifier inference function.
     * Infers the identifier of a process diagram, based on its name.
     * @param p_doc_root The root node of the XML specification.
     * @param p_diagram_name The name of the process diagram to infer its identifier of.
     * @return The identifier of the process diagram that matches the given name.
     * @pre p_doc_root is a valid pointer to an XML specification and p_diagram_name is a valid reference to a diagram name.
     * @post The identifier of the inferred process diagram is returned or the empty string is returned and error messages are produced.
     */
    wxString infer_process_diagram_id(wxXmlNode *p_doc_root, wxString &p_diagram_name);

    /**
     * XML process diagram name retrieval function.
     * Retrieves an XML process diagram from an XML specification, based on its identifier.
     * @param p_doc_root The root node of the XML specification.
     * @param p_diagram_id The identifier of the process diagram to retrieve.
     * @return The XML process diagram that has the given identifier.
     * @pre p_doc_root is a valid pointer to an XML specification.
     * @post The XML process diagram is returned or NULL and error messages are produced.
     */
    wxXmlNode *get_process_diagram(wxXmlNode *p_doc_root, wxString &p_diagram_id);

    /**
     * Architecture diagram name inference function.
     * Infers the name of an architecture diagram, based on its identifier.
     * @param p_doc_root An XML specification.
     * @param p_id The identifier of the architecture name to infer.
     * @return The inferred name of the architecture diagram.
     * @pre p_doc_root is a valid XML specification.
     * @post The name of the inferred architecture diagram is returned or the empty string is returned and error messages are produced.
     */
    wxString infer_architecture_name(wxXmlNode *p_doc_root, wxString &p_id);

    /**
     * Architecture diagram identifier inference function.
     * Infers the identifier of an architecture diagram, based on its name.
     * @param p_doc_root An XML specification.
     * @param p_diagram_name The name of the diagram to infer its identifier of.
     * @pre p_doc_root is a valid XML specification and p_diagram_name is a valid reference to a diagram name.
     * @post The identifier of the inferred diagram is returned or the empty string is returned and error messages are produced.
     */
    wxXmlNode *get_architecture_diagram(wxXmlNode *p_doc_root, wxString &p_diagram_name);

    /**
     * Datatype specification validation function.
     * Validates the datatype specification.
     * @param p_doc_root An XML specification.
     * @param datatype_spec The datatype specification
     * @return True if the datatype specification contained in p_doc_root is valid, false otherwise.
     * @pre p_doc_root is a valid pointer to an XML specification.
     * @post True is returned if the datatype specification is valid, false otherwise and error messages are produced.
     */
    bool validate_datatype_specification(wxXmlNode *p_doc_root, ATermAppl &datatype_spec);

    /**
     * Initial designator list validation function.
     * Validates the list of initial designators.
     * @param p_process_diagram The XML process diagram containing the list of initial designators.
     * @param p_designator_list The XML list of initial designators.
     * @return True if the list of initial designators is valid, false otherwise.
     * @pre p_process_diagram is a valid pointer to the XML process diagram containing the list of initial designators and p_designator_list is a valid pointer to an XML list of initial designators.
     * @post The validity of the list of initial designators is returned and error messages are produced if necessary.
     */
    bool validate_initial_designator_list(wxXmlNode *p_process_diagram, wxXmlNode *p_designator_list);

    /**
     * XML process diagram validation function.
     * Validates an XML process diagram and produces error messages if necessary.
     * @pre p_doc_root is a valid reference to a GraPE XML specification and p_process_diagram is a valid reference to an XML process diagram contained in p_doc_root.
     * @post The process diagram is validated and error messages are produced if necessary.
     * @param p_doc_root The GraPE XML specification containing the diagram to validate.
     * @param p_process_diagram The XML process diagram to validate.
     * @param datatype_spec The datatype specification against which data expressions are checked
     * @return True if the diagram is valid, false otherwise.
     */
    bool validate_process_diagram(wxXmlNode *p_doc_root, wxXmlNode *p_process_diagram, ATermAppl &datatype_spec);

    /**
     * XML architecture diagram validation function.
     * Validates an XML architecture diagram and produces error messages if necessary.
     * @pre p_doc_root is a valid pointer to a GraPE XML specification and p_architecture_diagram is a valid reference to an XML architecture diagram contained in p_doc_root.
     * @post The architecture diagram is validated and error messages are produced if necessary.
     * @param p_doc_root The GraPE XML specification containing the diagram to validate.
     * @param p_architecture_diagram The XML architecture diagram to validate.
     * @param datatype_spec The datatype specification.
     * @return True is the architecture diagram is valid, false otherwise.
     */
    bool validate_architecture_diagram(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, ATermAppl &datatype_spec);

    /**
     * Reference state list validation function.
     * Validates the list of reference states.
     * @param p_doc_root The XML GraPE specification containing the list of reference states.
     * @param p_process_diagram The XML process diagram containing the list to validate.
     * @param p_ref_state_list The XML list of reference states.
     * @param datatype_spec The datatype specification.
     * @return True if the list of reference states is valid, false otherwise.
     * @pre p_doc_root is a valid pointer the the XML GraPE specification containing the list of reference states and p_ref_state_list is a valid pointer to an XML list of reference states.
     * @post The validity of the list of reference states is returned and error messages are produced if necessary.
     */
    bool validate_reference_state_list(wxXmlNode *p_doc_root, wxXmlNode *p_process_diagram, wxXmlNode *p_ref_state_list, ATermAppl &datatype_spec);

    /**
     * State list validation function.
     * Validates the list of states.
     * @param p_process_diagram The XML process diagram containing the list to validate.
     * @param p_state_list The XML list of states.
     * @return True if the list of states is valid, false otherwise.
     * @pre p_state_list is a valid pointer to an XML list of states.
     * @post The validity of the list of states is returned and error messages are produced if necessary.
     */
    bool validate_state_list(wxXmlNode *p_process_diagram, wxXmlNode *p_state_list);

    /**
     * Terminating transition list validation function.
     * Validates the list of terminating transitions.
     * @param p_process_diagram The XML process diagram containing the list of terminating transitions.
     * @param p_term_trans_list The XML list of terminating transitions.
     * @param p_preamble_parameters The parameters in the preamble.
     * @param p_preamble_variables The variables in the preamble.
     * @param datatype_spec The datatype specification
     * @return True if the list of terminating transitions is valid, false otherwise.
     * @pre p_process_diagram is a valid pointer to an XML process diagram containing the list of terminating transitions and p_term_trans_list is a valid pointer to an XML list of terminating transitions.
     * @post The validity of the list of terminating transitions is returned and error messages are produced if necessary.
     */
    bool validate_terminating_transition_list(wxXmlNode *p_process_diagram, wxXmlNode *p_term_trans_list, list_of_decl &p_preamble_parameters, list_of_decl_init &p_preamble_variables, ATermAppl &datatype_spec);

    /**
     * Non-terminating transition list validation function.
     * Validates a list of non-terminating transitions.
     * @param p_process_diagram The XML process diagram containing the list of non-terminating transitions.
     * @param p_trans_list The XML list of non-terminating transitions.
     * @param p_preamble_parameters The parameters in the preamble.
     * @param p_preamble_variables The variables in the preamble.
     * @param datatype_spec The datatype specification
     * @return True if the list of non-terminating transitions is valid, false otherwise.
     * @pre p_process_diagram is a valid pointer to an XML process diagram containing the list of non-terminating transitions and p_trans_list is a valid pointer to an XML list of non-terminating transitions.
     * @post The validity of the list of non-terminating transitions is returned and error messages are produced if necessary.
     */
    bool validate_nonterminating_transition_list(wxXmlNode *p_process_diagram, wxXmlNode *p_trans_list, list_of_decl &p_preamble_parameters, list_of_decl_init &p_preamble_variables, ATermAppl &datatype_spec);

    /**
     * XML process diagram preamble parsing function.
     * Parses a valid XML preamble.
     * @param p_process_diagram A valid XML process diagram node.
     * @param p_preamble_parameter_decls A list of parameter declarations.
     * @param p_preamble_local_var_decls A list of local variable declarations.
     * @param datatype_spec The datatype specification against which data expressions are checked
     * @return True if parsing went successfully, false otherwise.
     * @pre p_process_diagram is a valid XML process diagram preamble.
     * @post p_preamble_parameter_decls contains the parameter declarations present in the preamble and p_preamble_local_var_decls contains the local variable declarations present in the preamble or error messages are produced.
     */
    bool parse_preamble(wxXmlNode *p_process_diagram, list_of_decl &p_preamble_parameter_decls, list_of_decl_init &p_preamble_local_var_decls, ATermAppl &datatype_spec);

    /**
     * Transition label parsing function.
     * Parses the transition label.
     * @param p_process_diagram The XML process diagram containing the transition to be parsed.
     * @param p_preamble_parameter_decls The parameter declarations in the preamble.
     * @param p_preamble_local_var_decls The local variable declarations in the preamble.
     * @param p_diagram_name The name of the process diagram.
     * @param p_actions List of actions with parameters present in the multiaction.
     * @param datatype_spec The datatype specification
     * @return label.
     */
    label parse_transition_label(wxXmlNode *p_process_diagram, list_of_decl &p_preamble_parameter_decls, list_of_decl_init &p_preamble_local_var_decls, wxString p_diagram_name, ATermAppl &datatype_spec);

    /**
     * Type inference function.
     * Returns the type associated with an action or identifier, based on the transition's
     * parameter declarations and the parameter declarations and local variable
     * declarations in the preamble.
     * @param p_type The variable to be inferred.
     * @param p_preamble_parameter_decls The parameter declarations in the preamble.
     * @param p_preamble_local_var_decls The local variable declarations in the preamble.
     * @param p_trans_var_decls The variable declarations in the transition label of the action.
     * @return A string containing the type associated with the action, the empty string if no type could be inferred.
     * @pre True.
     * @post True.
     */
    wxString infer_type(wxString &p_type, list_of_decl &p_preamble_parameter_decls, list_of_decl_init &p_preamble_local_var_decls, list_of_decl &p_trans_var_decls);

    /**
     * Blocked list validation function.
     * Validates a list of blocked properties.
     * @param p_architecture_diagram The XML architecture diagram containing the list of blocked properties.
     * @param p_blocked_list The XML list of blocked properties.
     * @return True if the list of blocked properties is valid, false otherwise.
     * @pre p_architecture_diagram is a valid pointer to an XML architecture diagram containing the list of blocked properties and p_blocked_list is a valid pointer to an XML list of blocked properties.
     * @post The validity of the list of blocked properties is returned and error messages are produced if necessary.
     */
    bool validate_blocked_list(wxXmlNode *p_architecture_diagram, wxXmlNode *p_blocked_list);

    /**
     * Visible list validation function.
     * Validates a list of visible properties.
     * @param p_architecture_diagram The XML architecture diagram containing the list of visible properties.
     * @param p_visible_list The XML list of visible properties.
     * @return True if the list of visible properties is valid, false otherwise.
     * @pre p_architecture_diagram is a valid pointer to an XML architecture diagram containing the list of visible properties and p_blocked_list is a valid pointer to an XML list of visible properties.
     * @post The validity of the list of visible properties is returned and error messages are produced if necessary.
     */
    bool validate_visible_list(wxXmlNode *p_architecture_diagram, wxXmlNode *p_visible_list);

    /**
     * Channel communication list validation function.
     * Validates a list of channel communications.
     * @param p_architecture_diagram The XML architecture diagram containing the list of channel communications.
     * @param p_channel_communication_list The XML list of channel communications.
     * @return True if the list of channel communications is valid, false otherwise.
     * @pre p_architecture_diagram is a valid pointer to an XML architecture diagram containing the list of channel communications and p_channel_communication_list is a valid pointer to an XML list of channel communications.
     * @post The validity of the list of channel communications is returned and error messages are produced if necessary.
     */
    bool validate_channel_communication_list(wxXmlNode *p_architecture_diagram, wxXmlNode *p_channel_communication_list);

    /**
     * Channel list validation function.
     * Validates a list of channels.
     * @param p_doc_root The XML GraPE specification containing the list of channels.
     * @param p_architecture_diagram The XML architecture diagram containing the list of channels.
     * @param p_channel_list The XML list of channels.
     * @param datatype_spec The datatype specification.
     * @return True if the list of channels is valid, false otherwise.
     * @pre p_doc_root is a valid pointer to an XML GraPE specification containing the list of channels, p_architecture_diagram is a valid pointer to an XML architecture diagram containing the list of channels and p_channel_list is a valid pointer to an XML list of channels.
     * @post The validity of the list of channels is returned and error messages are produced if necessary.
     */
    bool validate_channel_list(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, wxXmlNode *p_channel_list, ATermAppl &datatype_spec);

    /**
     * Architecture reference list validation function.
     * Validates a list of architecture references.
     * @param p_doc_root The XML GraPE specification containing the list of architecture references.
     * @param p_architecture_diagram The XML architecture diagram containing the list of architecture references.
     * @param p_reference_list The XML list of architecture references.
     * @return True if the list of architecture references is valid, false otherwise.
     * @pre p_doc_root is a valid pointer to an XML GraPE specification containing the list of architecture references, p_architecture_diagram is a valid pointer to the XML architecture diagram containing the list of architecture references and p_reference_list is a valid pointer to an XML list of architecture references.
     * @post The validity of the list of architecture references is returned and error messages are produced if necessary.
     */
    bool validate_architecture_reference_list(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, wxXmlNode *p_reference_list);

    /**
     * Architecture diagram acyclicy checking function.
     * Recursively checks if an architecture diagram is referencing itself (direct or indirect).
     * @param p_doc_root The XML GraPE specification containing the architecture diagrams.
     * @param p_checked The array of identifiers checked recursively.
     * @return True if the last element in the array does not refer to itself, false otherwise.
     * @pre p_doc_root is a valid pointer to an XML GraPE specification and p_checked is an array of already checked architecture diagram identifiers.
     * @post True is returned if the last element in p_checked does not refer to itself, false is returned otherwise and error messages are produced.
     */
    bool infer_architecture_reference_acyclic(wxXmlNode *p_doc_root, wxArrayString p_checked);

    /**
     * Process reference list validation function.
     * Validates a list of process references.
     * @param p_doc_root The XML GraPE specification containing the list of process references.
     * @param p_architecture_diagram The XML architecture diagram containing the list of process references.
     * @param p_reference_list The XML list of process references.
     * @param datatype_spec The datatype specification.
     * @return True if the list of process references is valid, false otherwise.
     * @pre p_doc_root is a valid pointer to an XML GraPE specification containing the list of process references, p_architecture_diagram is a valid pointer to the XML architecture diagram containing the list of process references and p_reference_list is a valid pointer to an XML list of process references.
     * @post The validity of the list of process references is returned and error messages are produced if necessary.
     */
    bool validate_process_reference_list(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, wxXmlNode *p_reference_list, ATermAppl &datatype_spec);

    /**
     * Architecture diagram visibles inference function.
     * Infers all visibles of an architecture diagram.
     * @param p_architecture_diagram The XML architecture diagram to infer the visibles of.
     * @return An array of strings containing the names of the visibles.
     * @pre p_architecture_diagram is a valid XML architecture diagram.
     * @post An array of strings containing the names of the inferred visibles.
     */
    list_of_action infer_architecture_visibles(wxXmlNode *p_architecture_diagram);

    /**
     * Process diagram actions inference function.
     * Infers all actions occuring in a process diagram, including those in process references.
     * @param p_doc_root An XML specification.
     * @param p_diagram_id The identifier of the diagram to infer the actions of.
     * @param datatype_spec The datatype specification.
     * @return An array of strings containing all the actions occuring in this diagram and any contained process references, or the empty string.
     * @pre p_doc_root is a valid pointer to an XML specification and p_diagram_id is a valid reference to a diagram identifier.
     * @post The actions occuring in this diagram and any contained process references are returned or the empty string is returned and error messages are produced.
     */
    list_of_action infer_process_actions(wxXmlNode *p_doc_root, wxString &p_diagram_id, ATermAppl &datatype_spec);

    /**
     * XML process diagram to mCRL2 action convertor function.
     * Extracts all actions of an XML process diagram and infers
     * their type.
     * @param p_process_diagram A valid XML process diagram.
     * @param p_preamble_parameter_decls The parameter declarations in the preamble of the process diagram.
     * @param p_preamble_local_var_decls The local variable declaration in the preamble of the process diagram.
     * @param datatype_spec The datatype specification.
     * @return A list of actions (and their inferred types) present the process diagram.
     * @pre p_process_diagram is a valid XML process diagram.
     * @post A list containing the actions (and their inferred types) present in the process diagram or error messages are produced.
     */
    list_of_action process_diagram_mcrl2_action(wxXmlNode *p_process_diagram, list_of_decl &p_preamble_parameter_decls, list_of_decl_init &p_preamble_local_var_decls, ATermAppl &datatype_spec);

    /**
     * Parameter initialisation parsing function.
     * Parses the parameter initialisation as it occurs inside a process reference or reference state.
     * @param p_parameter_initialisation The string containing the parameter initialisation.
     * @param datatype_spec The datatype specification.
     * @return An array of declarations and initialisations extracted from the parameter initialisation string.
     * @pre p_parameter_initialisation is a valid reference to a string containing the parameter initialisation.
     * @post An array of declarations and initialisations extracted from the parameter initialisation string is returned.
     */
    bool parse_reference_parameters(wxXmlNode *p_process_reference, wxString &p_diagram_name, list_of_varupdate &p_parameter_initialisation, ATermAppl &datatype_spec);

    /**
     * Diagram name validation function.
     * Validates all diagram names (e.g. check if they are unique)
     * @param p_doc_root The XML GraPE specification to check.
     * @return True if all diagram names are unique, false otherwise.
     * @pre p_doc_root is a valid pointer to a GraPE XML specification.
     * @post True is returned if all diagram names are unique, false otherwise and error messages are produced.
     */
    bool validate_diagram_names(wxXmlNode *p_doc_root);

  } // namespace mcrl2gen
} // namespace grape

#endif // MCRL2GEN_MCRL2GEN_VALIDATE_H
