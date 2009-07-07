// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
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
#include "mcrl2/atermpp/table.h"

#define CONVERSION_ERROR            17

using namespace grape::libgrape;

namespace grape
{
  namespace mcrl2gen
  {
    /**
     * \short Represents a channel.
     */
    struct channel_id
    {
      action   m_channel;              /**< The action of the channel. */
      wxString m_channel_id;           /**< The identifier of the channel. */
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
      list_of_action m_actions;     /**< An array of actions associated with this reference. */

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
      action m_new;
    };

    /**
     * \short Represents an array of renamed channels or channel communications.
     */
    WX_DECLARE_OBJARRAY(renamed, arr_renamed);

    /**
     * Identifier parse function.
     * This function converts the wxString and gives it to the mcrl2 library identifier parser.
     * @param p_identifier The wxString with an identifier.
     * @return ATermAppl containing the parsed identifier or 0 if it could not be parsed.
     */
    ATermAppl parse_identifier(wxString p_identifier);

    /**
     * Sort expression parse function.
     * This function converts the wxString and gives it to the mcrl2 library sort expression parser.
     * @param p_sort_expression The wxString with a sort expression.
     * @return ATermAppl containing the parsed sort expression or 0 if it could not be parsed.
     */
    ATermAppl parse_sort_expr(wxString p_sort_expression);

    /**
     * Data expression parse function.
     * This function converts the wxString and gives it to the mcrl2 library data expression parser.
     * @param p_data_expression The wxString with a data expression.
     * @return ATermAppl containing the parsed data expression or 0 if it could not be parsed.
     */
    ATermAppl parse_data_expr(wxString p_data_expression);

    /**
     * Process specification parse function.
     * This function converts the wxString and gives it to the mcrl2 library process specification parser.
     * @param p_proc_spec The wxString with a process specification.
     * @return ATermAppl containing the parsed process specification or 0 if it could not be parsed.
     */
    ATermAppl parse_proc_spec(wxString p_proc_spec);

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
     * XML diagram retrieval function.
     * Retrieves an XML diagram from an XML specification, based on its identifier.
     * @param p_doc_root The root node of the XML specification.
     * @param p_diagram_id The identifier of the diagram to retrieve.
     * @return The XML diagram that has the given identifier.
     * @pre p_doc_root is a valid pointer to an XML specification.
     * @post The XML process diagram is returned or 0 and error messages are produced.
     */
    wxXmlNode *get_diagram(wxXmlNode *p_doc_root, wxString &p_diagram_id);

    /**
     * Table of variables retrieval function.
     * This function makes a variable table of the variables of the provided preamble parameter declarations,
     * preamble local variable declarations and transition local variable declarations.
     * @param p_preamble_parameter_decls The preamble parameter declarations list.
     * @param p_preamble_local_var_decls The preamble local variable declarations list.
     * @param p_trans_var_decls The transition local variable declarations list.
     * @param datatype_spec The datatype specification.
     * @return A table with the variables of the provided lists.
     */
    atermpp::table get_variable_table(list_of_decl &p_preamble_parameter_decls, list_of_decl_init &p_preamble_local_var_decls, list_of_decl &p_trans_var_decls, ATermAppl &datatype_spec);

    /**
     * Architecture diagram visibles retrieval function.
     * Retrieves all visibles of an architecture diagram.
     * @param p_doc_root The XML GraPE specification containing the list of architecture diagram visibles.
     * @param p_architecture_diagram The XML architecture diagram to retrieve the visibles of.
     * @param datatype_spec The datatype specification.
     * @return A list of actions representing the names and types of the visibles.
     * @pre p_architecture_diagram is a valid XML architecture diagram.
     * @post A list of actions containing the names and types of the retrieved visibles.
     */
    list_of_action get_architecture_visibles(wxXmlNode *p_doc_root, wxString &p_diagram_id, ATermAppl &datatype_spec);

    /**
     * Architecture diagram visible channels retrieval function.
     * Retrieves all visible channels of a diagram.
     * @param p_doc_root The XML GraPE specification containing the list of diagrams.
     * @param p_diagram_name The name of the diagram to retrieve the visibles of.
     * @param p_objects The XML list of objects containing visibles, process references and channels.
     * @param datatype_spec The datatype specification.
     * @return A list of actions representing the names and types of the visible channels.
     * @pre p_diagram_name is a valid architecture diagram name.
     * @post A list of actions containing the names and types of the retrieved visible channels.
     */
    list_of_action get_architecture_visible_channels(wxXmlNode *p_doc_root, wxString &p_diagram_name, wxXmlNode *p_objects, ATermAppl &datatype_spec);

    /**
     * Architecture diagram visible channel communications retrieval function.
     * Retrieves all visible channel communications of a diagram.
     * @param p_doc_root The XML GraPE specification containing the list of diagrams.
     * @param p_diagram_name The name of the diagram to retrieve the visible channel communications of.
     * @param p_objects The XML list of objects containing visibles, process references, channels and channel communications.
     * @param datatype_spec The datatype specification.
     * @return A list of actions representing the names and types of the visible channel communications.
     * @pre p_diagram_name is a valid architecture diagram name.
     * @post A list of actions containing the names and types of the retrieved visible channel communications.
     */
    list_of_action get_architecture_visible_channel_communications(wxXmlNode *p_doc_root, wxString &p_diagram_name, wxXmlNode *p_objects, ATermAppl &datatype_spec);

    /**
     * Process diagram actions retrieval function.
     * Retrieve all actions occuring in a process diagram, including those in process references.
     * @param p_doc_root The XML GraPE specification containing the list of diagrams.
     * @param p_diagram_id The identifier of the diagram to retrieve the actions of.
     * @param datatype_spec The datatype specification.
     * @return A list of all the actions occuring in this diagram and any contained process references.
     * @pre p_doc_root is a valid pointer to an XML specification and p_diagram_id is a valid reference to a diagram identifier.
     * @post The actions occuring in this diagram and any contained process references are returned or an empty list is returned and error messages are produced.
     */
    list_of_action get_process_actions(wxXmlNode *p_doc_root, wxString &p_diagram_id, ATermAppl &datatype_spec);

    /**
     * Action list compacting function.
     * Removes duplicate actions from the list.
     * @param p_actions The array of actions.
     * @param new_actions The array of new actions.
     * @pre p_actions and new_actions are valid lists of actions.
     * @post All actions of p_actions that are not in new_actions are add to new_actions.
     */
    void compact_list_action(list_of_action &p_actions, list_of_action &new_actions);

    /**
     * Diagram acyclicy checking function.
     * Recursively checks if a diagram is referencing itself (direct or indirect).
     * @param p_doc_root The XML GraPE specification containing the diagrams.
     * @param p_checked The array of identifiers checked recursively.
     * @return True if the last element in the array does not refer to itself, false otherwise.
     * @pre p_doc_root is a valid pointer to an XML GraPE specification and p_checked is an array of already checked diagram identifiers.
     * @post True is returned if the last element in p_checked does not refer to itself, false is returned otherwise and error messages are produced.
     */
    bool is_reference_acyclic(wxXmlNode *p_doc_root, wxArrayString p_checked);

    /**
     * XML specification validation function.
     * Validates an XML GraPE specification and produces error messages if necessary.
     * @param p_spec The specification to validate.
     * @return True if the specification is valid, false otherwise.
     * @pre True.
     * @post The XML specification is validated and error messages are produced if necessary.
     */
    bool validate(wxXmlDocument &p_spec);

    /**
     * Datatype specification validation function.
     * Validates the datatype specification.
     * @param p_spec The GraPE XML specification containing the diagram to validate.
     * @return True if the datatype specification contained in p_spec is valid, false otherwise.
     * @pre p_spec is a valid reference to a GraPE XML specification.
     * @post True is returned if the datatype specification is valid, false otherwise and error messages are produced.
     */
    bool validate_datatype_specification(wxXmlDocument &p_spec);

    /**
     * Datatype specification validation function.
     * Validates the datatype specification.
     * @param p_doc_root An XML specification.
     * @param datatype_spec The datatype specification.
     * @return True if the datatype specification contained in p_doc_root is valid, false otherwise.
     * @pre p_doc_root is a valid pointer to an XML specification.
     * @post True is returned if the datatype specification is valid, false otherwise and error messages are produced.
     */
    bool validate_datatype_specification(wxXmlNode *p_doc_root, ATermAppl &datatype_spec);

    /**
     * XML process diagram validation function.
     * Validates an XML process diagram and produces error messages if necessary.
     * @param p_spec The GraPE XML specification containing the diagram to validate.
     * @param p_diagram_id The identifier of the process diagram to validate.
     * @return True if the diagram is valid, false otherwise.
     * @pre p_spec is a valid reference to a GraPE XML specification and p_diagram_id is a valid reference to a process diagram contained in p_spec.
     * @post The process diagram is validated and error messages are produced if necessary.
     */
    bool validate_process_diagram(wxXmlDocument &p_spec, wxString &p_diagram_id);

    /**
     * XML process diagram validation function.
     * Validates an XML process diagram and produces error messages if necessary.
     * @param p_doc_root The GraPE XML specification containing the diagram to validate.
     * @param p_process_diagram The XML process diagram to validate.
     * @param datatype_spec The datatype specification against which data expressions are checked
     * @return True if the diagram is valid, false otherwise.
     * @pre p_doc_root is a valid reference to a GraPE XML specification and p_process_diagram is a valid reference to an XML process diagram contained in p_doc_root.
     * @post The process diagram is validated and error messages are produced if necessary.
     */
    bool validate_process_diagram(wxXmlNode *p_doc_root, wxXmlNode *p_process_diagram, ATermAppl &datatype_spec);

    /**
     * XML process diagram preamble validation function.
     * Validates an XML preamble and produces error messages if necessary.
     * @param p_process_diagram A valid XML process diagram node.
     * @param p_preamble_parameter_decls A list of parameter declarations.
     * @param p_preamble_local_var_decls A list of local variable declarations.
     * @param datatype_spec The datatype specification against which data expressions are checked
     * @return True if the preamble is valid, false otherwise.
     * @pre p_process_diagram is a valid XML process diagram.
     * @post p_preamble_parameter_decls contains the parameter declarations present in the preamble and p_preamble_local_var_decls contains the local variable declarations present in the preamble or error messages are produced.
     */
    bool validate_preamble(wxXmlNode *p_process_diagram, list_of_decl &p_preamble_parameter_decls, list_of_decl_init &p_preamble_local_var_decls, ATermAppl &datatype_spec);

    /**
     * XML process diagram preamble local variables validation function.
     * Validates an XML preamble local variables list and produces error messages if necessary.
     * @param p_preamble A valid XML preamble node.
     * @param p_diagram_name The name of the process diagram wich contains the preamble.
     * @param p_preamble_local_var_decls A list of local variable declarations.
     * @param datatype_spec The datatype specification against which data expressions are checked
     * @return True if the preamble local variables are valid, false otherwise.
     * @pre p_process_diagram is a valid XML process diagram and p_preamble_local_var_decls is empty.
     * @post p_preamble_local_var_decls contains the local variable declarations present in the preamble or error messages are produced.
     */
    bool validate_preamble_local_variables(wxXmlNode *p_preamble, wxString &p_diagram_name, list_of_decl_init &p_preamble_local_var_decls, ATermAppl &datatype_spec);

    /**
     * XML process diagram preamble parameters validation function.
     * Validates an XML preamble parameters list and produces error messages if necessary.
     * @param p_preamble A valid XML process diagram node.
     * @param p_diagram_name The name of the process diagram wich contains the preamble.
     * @param p_preamble_parameter_decls A list of parameter declarations.
     * @param datatype_spec The datatype specification against which data expressions are checked
     * @return True if the preable is valid, false otherwise.
     * @pre p_process_diagram is a valid XML process diagram and p_preamble_parameter_decls is empty.
     * @post p_preamble_parameter_decls contains the parameter declarations present in the preamble or error messages are produced.
     */
    bool validate_preamble_parameters(wxXmlNode *p_preamble, wxString &p_diagram_name, list_of_decl &p_preamble_parameter_decls, ATermAppl &datatype_spec);

    /**
     * Initial designator list validation function.
     * Validates the list of initial designators.
     * @param p_doc_root The GraPE XML specification containing the list of initial designators.
     * @param p_process_diagram The XML process diagram containing the list of initial designators.
     * @param p_designator_list The XML list of initial designators.
     * @return True if the list of initial designators is valid, false otherwise.
     * @pre p_process_diagram is a valid pointer to the XML process diagram containing the list of initial designators and p_designator_list is a valid pointer to an XML list of initial designators.
     * @post The validity of the list of initial designators is returned and error messages are produced if necessary.
     */
    bool validate_initial_designator_list(wxXmlNode *p_doc_root, wxXmlNode *p_process_diagram, wxXmlNode *p_designator_list);

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
     * Parameter initialisation validation function.
     * Validates the parameter initialisation as it occurs inside a process reference or reference state.
     * @param p_doc_root The XML GraPE specification containing the list of reference states.
     * @param p_reference The XML process reference.
     * @param p_diagram_name The name of the process diagram the reference is in.
     * @param p_parameter_initialisation The list of variable updates representing the parameter initialisation.
     * @param datatype_spec The datatype specification.
     * @return True if the list of reference parameter initialisations is valid, false otherwise.
     * @pre p_parameter_initialisation is a valid reference to a string containing the parameter initialisation.
     * @post p_parameter_initialisation contains the list of paramter initialisations and error messages are produced if necessary.
     */
    bool validate_reference_parameters(wxXmlNode *p_doc_root, wxXmlNode *p_reference, wxString &p_diagram_name, list_of_varupdate &p_parameter_initialisation, ATermAppl &datatype_spec);

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
     * Connection between state and transition validation function.
     * Validates the connection between a state and its transitions.
     * @param p_process_diagram The XML process diagram containing the state and transitions for wich the connections are to validate.
     * @param p_state_id The id of the state to validate.
     * @return True if the connections between the state and its transitions are valid, false otherwise.
     * @pre p_state_id is a valid id of a state.
     * @post The validity of the connections is returned and error messages are produced if necessary.
     */
    bool validate_transition_connection(wxXmlNode *p_process_diagram, wxString &p_state_id);

    /**
     * Connection between transition and state validation function.
     * Validates the connection between a transition and its states.
     * @param p_process_diagram The XML process diagram containing the transition and states for wich the connections are to validate.
     * @param p_transition_id The id of the transition to validate.
     * @return True if the connections between the transition and its states are valid, false otherwise.
     * @pre p_state_id is a valid id of a transition.
     * @post The validity of the connections is returned and error messages are produced if necessary.
     */
    bool validate_state_connection(wxXmlNode *p_process_diagram, wxString &p_transition_id);

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
     * Transition label validation function.
     * Validates the transition label.
     * @param p_process_diagram The XML process diagram containing the transition label to be validated.
     * @param p_preamble_parameter_decls The parameter declarations in the preamble.
     * @param p_preamble_local_var_decls The local variable declarations in the preamble.
     * @param p_trans_label The label of the transition.
     * @param p_diagram_name The name of the process diagram.
     * @param datatype_spec The datatype specification
     * @return True if the transition label is valid, false otherwise.
     * @pre p_process_diagram is a valid pointer to an XML process diagram containing the transition label and p_trans_label is empty.
     * @post The validity of the transition label is returned and p_trans_label contains the valid label and error messages are produced if necessary.
     */
    bool validate_transition_label(wxXmlNode *p_process_diagram, list_of_decl &p_preamble_parameter_decls, list_of_decl_init &p_preamble_local_var_decls, label &p_trans_label, wxString p_diagram_name, ATermAppl &datatype_spec);

    /**
     * Transition label variable declarations validation function.
     * Validates the transition label variable declarations.
     * @param p_transition_label The XML transition label to be validated.
     * @param p_diagram_name The name of the process diagram the transition label is in.
     * @param p_local_var_decls The local variable declarations in the preamble.
     * @param datatype_spec The datatype specification.
     * @return True if the transition label variable declarations are valid, false otherwise.
     * @pre p_transition_label is a valid pointer to an XML transition label and p_local_var_decls is empty.
     * @post The validity of the transition label variable declarations is returned and p_local_var_decls contains the valid label variable declarations and error messages are produced if necessary.
     */
    bool validate_transition_label_variable_declarations(wxXmlNode *p_transition_label, wxString &p_diagram_name, list_of_decl &p_local_var_decls, ATermAppl &datatype_spec);

    /**
     * Transition label condition validation function.
     * Validates the transition label condition.
     * @param p_transition_label The XML transition label to be validated.
     * @param p_diagram_name The name of the process diagram the transition label is in.
     * @param p_condition The local variable declarations in the preamble.
     * @param datatype_spec The datatype specification.
     * @param vars Table of known variables to typecheck the condition with.
     * @return True if the transition label condition is valid, false otherwise.
     * @pre p_transition_label is a valid pointer to an XML transition label and p_condition is empty.
     * @post The validity of the transition label condition is returned and p_condition contains the valid label condition and error messages are produced if necessary.
     */
    bool validate_transition_label_condition(wxXmlNode *p_transition_label, wxString &p_diagram_name, wxString &p_condition, ATermAppl &datatype_spec, atermpp::table &vars);

    /**
     * Transition label actions validation function.
     * Validates the transition label actions.
     * @param p_transition_label The XML transition label to be validated.
     * @param p_diagram_name The name of the process diagram the transition label is in.
     * @param p_actions The local variable actions in the preamble.
     * @param datatype_spec The datatype specification.
     * @param vars Table of known variables to typecheck the action parameters with.
     * @return True if the transition label actions are valid, false otherwise.
     * @pre p_transition_label is a valid pointer to an XML transition label and p_actions is empty.
     * @post The validity of the transition label actions is returned and p_actions contains the valid label actions and error messages are produced if necessary.
     */
    bool validate_transition_label_actions(wxXmlNode *p_transition_label, wxString &p_diagram_name, list_of_action &p_actions, ATermAppl &datatype_spec, atermpp::table &vars);

    /**
     * Transition label timestamp validation function.
     * Validates the transition label timestamp.
     * @param p_transition_label The XML transition label to be validated.
     * @param p_diagram_name The name of the process diagram the transition label is in.
     * @param p_timestamp The local variable timestamp in the preamble.
     * @param datatype_spec The datatype specification.
     * @param vars Table of known variables to typecheck the action parameters with.
     * @return True if the transition label timestamp is valid, false otherwise.
     * @pre p_transition_label is a valid pointer to an XML transition label and p_timestamp is empty.
     * @post The validity of the transition label timestamp is returned and p_timestamp contains the valid label timestamp and error messages are produced if necessary.
     */
    bool validate_transition_label_timestamp(wxXmlNode *p_transition_label, wxString &p_diagram_name, wxString &p_timestamp, ATermAppl &datatype_spec, atermpp::table &vars);

    /**
     * Transition label variable updates validation function.
     * Validates the transition label variable updates.
     * @param p_transition_label The XML transition label to be validated.
     * @param p_diagram_name The name of the process diagram the transition label is in.
     * @param p_variable_updates The local variable variable updates in the preamble.
     * @param datatype_spec The datatype specification.
     * @param vars Table of known variables to typecheck the action parameters with.
     * @return True if the transition label variable updates are valid, false otherwise.
     * @pre p_transition_label is a valid pointer to an XML transition label and p_variable_updates is empty.
     * @post The validity of the transition label variable updates is returned and p_variable_updates contains the valid label variable updates and error messages are produced if necessary.
     */
    bool validate_transition_label_variable_updates(wxXmlNode *p_transition_label, wxString &p_diagram_name, list_of_varupdate &p_variable_updates, ATermAppl &datatype_spec, atermpp::table &vars);

    /**
     * XML architecture diagram validation function.
     * Validates an XML architecture diagram and produces error messages if necessary.
     * @param p_spec The GraPE XML specification containing the diagram to validate.
     * @param p_diagram_id The identifier of the architecture diagram to validate.
     * @return True if the diagram is valid, false otherwise
     * @pre p_spec is a valid reference to a GraPE XML specification and p_diagram is a valid reference to an architecture diagram contained in p_spec.
     * @post The architecture diagram is validated and error messages are produced if necessary.
     */
    bool validate_architecture_diagram(wxXmlDocument &p_spec, wxString &p_diagram_id);

    /**
     * XML architecture diagram validation function.
     * Validates an XML architecture diagram and produces error messages if necessary.
     * @param p_doc_root The GraPE XML specification containing the diagram to validate.
     * @param p_architecture_diagram The XML architecture diagram to validate.
     * @param datatype_spec The datatype specification.
     * @return True is the architecture diagram is valid, false otherwise.
     * @pre p_doc_root is a valid pointer to a GraPE XML specification and p_architecture_diagram is a valid reference to an XML architecture diagram contained in p_doc_root.
     * @post The architecture diagram is validated and error messages are produced if necessary.
     */
    bool validate_architecture_diagram(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, ATermAppl &datatype_spec);

    /**
     * Architecture reference list validation function.
     * Validates a list of architecture references.
     * @param p_doc_root The XML GraPE specification containing the list of architecture references.
     * @param p_architecture_diagram The XML architecture diagram containing the list of architecture references.
     * @param p_reference_list The XML list of architecture references.
     * @param datatype_spec The datatype specification
     * @return True if the list of architecture references is valid, false otherwise.
     * @pre p_doc_root is a valid pointer to an XML GraPE specification containing the list of architecture references, p_architecture_diagram is a valid pointer to the XML architecture diagram containing the list of architecture references and p_reference_list is a valid pointer to an XML list of architecture references.
     * @post The validity of the list of architecture references is returned and error messages are produced if necessary.
     */
    bool validate_architecture_reference_list(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, wxXmlNode *p_reference_list, ATermAppl &datatype_spec);

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
     * Channel communication list validation function.
     * Validates a list of channel communications.
     * @param p_doc_root The XML GraPE specification containing the list of channels.
     * @param p_architecture_diagram The XML architecture diagram containing the list of channel communications.
     * @param p_channel_communication_list The XML list of channel communications.
     * @param datatype_spec The datatype specification.
     * @return True if the list of channel communications is valid, false otherwise.
     * @pre p_doc_root is a valid pointer to an XML GraPE specification containing the list of channels, p_architecture_diagram is a valid pointer to an XML architecture diagram containing the list of channel communications and p_channel_communication_list is a valid pointer to an XML list of channel communications.
     * @post The validity of the list of channel communications is returned and error messages are produced if necessary.
     */
    bool validate_channel_communication_list(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, wxXmlNode *p_channel_communication_list, ATermAppl &datatype_spec);

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
