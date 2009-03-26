// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2gen.cpp
//
// Implements the functions to convert process diagrams and architecture diagrams to mCRL2.

#include "mcrl2gen.h"
#include "mcrl2gen_validate.h"
#include "libgrape/label.h"
#include "libgrape/preamble.h"
#include "libgrape/decl.h"

#include <sstream>
#include <string>
#include <wx/wx.h>
#include <wx/arrstr.h>
#include <wx/tokenzr.h>
#include <wx/file.h>

#include <aterm2.h>

// mCRL2 core libraries
#include "mcrl2/core/detail/struct.h"                      // ATerm building blocks.
#include "mcrl2/core/messaging.h"                   // Library for messaging.
#include "mcrl2/core/print.h"                       // Printing library.    <-- we need Boost to enable this, unfortunately :(...

// mCRL2 core/detail libraries
#include "mcrl2/core/parse.h"                // Parse library.
#include "mcrl2/core/typecheck.h"            // Type check library.

// mCRL2 utility libraries
#include "mcrl2/core/aterm_ext.h"

using namespace grape::mcrl2gen;
using namespace grape::libgrape;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2;
using namespace std;

/**
 * XML process diagram to mCRL2 sort expression convertor function.
 * Extracts all state/reference state names of an XML process diagram
 * and constructs an mCRL2 sort expression containing a struct of all
 * encountered names.
 * @param p_process_diagram A valid XML process diagram node.
 * @return A string containing the constructed mCRL2 expression if conversion went successfully, the empty string otherwise.
 * @pre p_process_diagram is a valid XML process diagram.
 * @post A string containing the constructed mCRL2 expression is returned or the empty string is returned and error messages are produced.
 */
wxString process_diagram_mcrl2_sort(wxXmlNode *p_process_diagram)
{
  if(p_process_diagram->GetName() == _T("processdiagram"))
  {
    // <processdiagram> node
    wxArrayString struct_names;
    wxString diagram_name = wxEmptyString;
    // process states and reference states of this process diagram
    diagram_name = get_child_value(p_process_diagram, _T("name"));
    if(diagram_name == wxEmptyString)
    {
      // ERROR: <processdiagram> does not contain child <name>
      cerr << "mCRL2 conversion error: process diagram does not have a name." << endl;
      throw CONVERSION_ERROR;
      return wxEmptyString;
    }
    wxXmlNode *child = get_child(p_process_diagram, _T("objectlist"));
    if(child == 0)
    {
      // ERROR: <processdiagram> does not contain <objectlist>
      cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " does not have any objects in it." << endl;
      throw CONVERSION_ERROR;
      return wxEmptyString;
    }
    // child = <objectlist>
    for(child = child->GetChildren(); child != 0; child = child->GetNext())
    {
      // process objectlist children
      if(child->GetName() == _T("statelist"))
      {
        // add names of all states in this <statelist> to struct_names
        for(wxXmlNode *child_state = child->GetChildren(); child_state != 0; child_state = child_state->GetNext())
        {
          if(child_state->GetName() != _T("state"))
          {
            // WARNING: <statelist> has no child <state>
            cerr << "mCRL2 conversion warning: process diagram " << diagram_name.ToAscii() << " does not contain any states." << endl;
            break;
          }
          wxString child_state_name = get_child_value(child_state, _T("name"));
          wxString child_state_id = get_child_value(child_state, _T("id"));
          if(child_state_name == wxEmptyString)
          {
            // ERROR: <state> does not contain <name>
            cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii()
                 << " has an unnamed state." << endl;
            throw CONVERSION_ERROR;
            return wxEmptyString;
          }
          if(child_state_id == wxEmptyString)
          {
            // ERROR: <state> does not contain <id>
            cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii()
                 << " has an invalid identifier for state " << child_state_name.ToAscii() << "." << endl;
            throw CONVERSION_ERROR;
            return wxEmptyString;
          }
          // child_state_name = <name>
          struct_names.Add(child_state_name + child_state_id);
        }
      }
      else if(child->GetName() == _T("referencestatelist"))
      {
        // add names of all reference states in this <referencestatelist> to struct_names
        for(wxXmlNode *child_refstate = child->GetChildren(); child_refstate != 0; child_refstate = child_refstate->GetNext())
        {
          if(child_refstate->GetName() != _T("referencestate"))
          {
            // WARNING: <referencestatelist> has no child <referencestate>
            cerr << "mCRL2 conversion warning: process diagram " << diagram_name.ToAscii() << " does not contain any process references." << endl;
            break;
          }
          wxString child_refstate_name = get_child_value(child_refstate, _T("name"));
          wxString child_refstate_id = get_child_value(child_refstate, _T("id"));
          if(child_refstate_name == wxEmptyString)
          {
            // ERROR: <refstate> does not contain <name>
            cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii()
                 << " has an unnamed process reference." << endl;
            throw CONVERSION_ERROR;
            return wxEmptyString;
          }
          if(child_refstate_id == wxEmptyString)
          {
            // ERROR: <refstate> does not contain <id>
            cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii()
                 << " has an invalid identifier for process reference " << child_refstate_name.ToAscii() << "." << endl;
            throw CONVERSION_ERROR;
            return wxEmptyString;
          }
          // child_refstate_name = <name>
          wxString ref_name = child_refstate_name + child_refstate_id;
          struct_names.Add(_T("S") + ref_name + _T("_in"), 1);
          struct_names.Add(_T("S") + ref_name + _T("_out"), 1);
        }
      }
    }

    // structnames now contains the names of all states in this diagram, along with 2 names of all reference states in this diagram
    wxString sort_struct_expr = wxEmptyString;
    sort_struct_expr += _T("sort ") + diagram_name + _T("_state = struct ");
    for(unsigned int i=0; i<struct_names.GetCount(); ++i)
    {
      if(i > 0)
      {
        sort_struct_expr += _T("|");
      }
      //sort_struct_expr += struct_names[i];
      sort_struct_expr += struct_names[i];
    }
    sort_struct_expr += _T(";\n");

    return sort_struct_expr;
  }

  cerr << "mCRL2 conversion error: process_diagram_mcrl2_sort called without a valid XML process diagram." << endl;
  return wxEmptyString;
}

/**
 * XML state identifier inference function.
 * Infers the identifier of a state (or process reference) in a process diagram, based on its name.
 * @param p_process_diagram The XML process diagram that contains the state to infer.
 * @param p_state_name The name of the state to infer.
 * @return The identifier of the inferred state (or process reference) or the empty string if the state was not found.
 * @pre p_process_diagram is a valid pointer to an XML process diagram and p_state_name is a valid reference to a state name.
 * @post The identifier of the inferred state (or process reference) is returned or the empty string is returned and error messages are produced.
 */
wxString infer_state_id(wxXmlNode *p_process_diagram, wxString &p_state_name)
{
  wxString diagram_name = get_child_value(p_process_diagram, _T("name"));

  if(p_state_name == wxEmptyString)
  {
    // ERROR: empty state name
    cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii()
         << " has an unnamed state." << endl;
    throw CONVERSION_ERROR;
    return wxEmptyString;
  }

  wxXmlNode *child = get_child(p_process_diagram, _T("objectlist"));
  if(child == 0)
  {
    // ERROR: <processdiagram> does not contain <objectlist>
    cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii()
         << " does not contain any objects." << endl;
    throw CONVERSION_ERROR;
    return wxEmptyString;
  }

  // child = <objectlist>
  for(child = child->GetChildren(); child != 0; child = child->GetNext())
  {
    // process objectlist children
    if(child->GetName() == _T("statelist"))
    {
      for(wxXmlNode *t_state = child->GetChildren(); t_state != 0; t_state = t_state->GetNext())
      {
        // t_state = <state>
        wxString state_name = get_child_value(t_state, _T("name"));
        wxString state_id = get_child_value(t_state, _T("id"));
        if(state_id == wxEmptyString)
        {
          // ERROR: <state> does not contain <id>
          cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii()
               << " has a state " << state_name.ToAscii() << " which has no identifier." << endl;
          throw CONVERSION_ERROR;
          return wxEmptyString;
        }

        if(state_name == p_state_name)
        {
          return state_id;
        }
      }
    }
    if(child->GetName() == _T("referencestatelist"))
    {
      for(wxXmlNode *t_refstate = child->GetChildren(); t_refstate != 0; t_refstate = t_refstate->GetNext())
      {
        // t_refstate = <referencestate>
        wxString state_name = get_child_value(t_refstate, _T("name"));
        wxString state_id = get_child_value(t_refstate, _T("id"));
        if(state_id == wxEmptyString)
        {
          // ERROR: <referencestate> does not contain <id>
          cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii()
               << " has a process reference to " << state_name.ToAscii()
               << " which has no identifier." << endl;
          throw CONVERSION_ERROR;
          return wxEmptyString;
        }

        if(state_name == p_state_name)
        {
          return state_id;
        }
      }
    }
  }

  // nothing found, return emptystring
  return wxEmptyString;
}

/**
 * XML state name inference function.
 * Infers the name of a state (or process reference) in a process diagram, based on its identifier.
 * @param p_process_diagram The XML process diagram that contains the state to infer.
 * @param p_id The identifier of the state to infer.
 * @param p_is_ref Reference to a boolean which indicates whether the inferred state is actually a process reference.
 * @return The name of the inferred state (or process reference) or the empty string if the state was not found.
 * @pre p_process_diagram is a valid pointer to an XML process diagram and p_id is a valid reference to an identifier.
 * @post The name of the inferred state (or process reference) is returned and p_is_ref is true if the inferred state is a process reference and false otherwise, or error messages are produced.
 */
wxString infer_state_name(wxXmlNode *p_process_diagram, wxString &p_id, bool &p_is_ref)
{
  wxString diagram_name = get_child_value(p_process_diagram, _T("name"));

  if(p_id == _T("-1"))
  {
    // ERROR: unconnected transition
    cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " has an unconnected transition." << endl;
    throw CONVERSION_ERROR;
    return wxEmptyString;
  }

  wxXmlNode *child = get_child(p_process_diagram, _T("objectlist"));
  if(child == 0)
  {
    // ERROR: <processdiagram> does not contain <objectlist>
    cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " does not contain any objects." << endl;
    throw CONVERSION_ERROR;
    return wxEmptyString;
  }
  // child = <objectlist>
  for(child = child->GetChildren(); child != 0; child = child->GetNext())
  {
    // process objectlist children
    if(child->GetName() == _T("statelist"))
    {
      for(wxXmlNode *t_state = child->GetChildren(); t_state != 0; t_state = t_state->GetNext())
      {
        // t_state = <state>
        wxString state_name = get_child_value(t_state, _T("name"));
        wxXmlNode *t_state_id = get_child(t_state, _T("id"));
        if(t_state_id == 0)
        {
          // ERROR: <state> does not contain <id>
          cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii()
               << " has a state " << state_name.ToAscii() << " which has no identifier." << endl;
          throw CONVERSION_ERROR;
          return wxEmptyString;
        }
        // t_state_id = <id>
        if(t_state_id->GetNodeContent() == p_id)
        {
          p_is_ref = false;
          return state_name;
        }
      }
    }
    if(child->GetName() == _T("referencestatelist"))
    {
      for(wxXmlNode *t_refstate = child->GetChildren(); t_refstate != 0; t_refstate = t_refstate->GetNext())
      {
        // t_refstate = <referencestate>
        wxString ref_name = get_child_value(t_refstate, _T("name"));
        wxXmlNode *t_refstate_id = get_child(t_refstate, _T("id"));
        if(t_refstate_id == 0)
        {
          // ERROR: <referencestate> does not contain <id>
          cerr << "mCRL2 conversion error: process diaram " << diagram_name.ToAscii()
               << " has a process reference " << ref_name.ToAscii() << " which has no identifier." << endl;
          throw CONVERSION_ERROR;
          return wxEmptyString;
        }
        // t_refstate_id = <id>
        if(t_refstate_id->GetNodeContent() == p_id)
        {
          p_is_ref = true;
          return ref_name;
        }
      }
    }
  }

  // nothing found, return emptystring
  return wxEmptyString;
}

/**
 * XML transition parsing function.
 * Parses an XML transition to construct an mCRL2 choice expression
 * for the internal mCRL2 process definition.
 * @param p_process_diagram The XML process diagram that contains the transition to be parsed.
 * @param p_transition The XML transition to parse.
 * @param p_declaration The mCRL2 process definition to parse the XML transition to.
 * @param p_alternative Boolean flag to indicate if the constructed mCRL2 process definition already has preceding choice expressions.
 * @param p_is_terminating Boolean flag to indicate if the transition to be parsed is terminating.
 * @param p_diagram_name The name of the process diagram that contains the transition to be parsed.
 * @param p_preamble_parameter_decls The preamble parameter declarations of the process diagram that contains the transition to be parsed.
 * @param p_preamble_local_var_decls The preamble local variable declarations of the process diagram that contains the transition to be parse.
 * @param datatype_spec The datatype specification.
 * @pre p_process_diagram is a valid pointer to an XML process diagram, p_transition is a valid pointer to an XML transition, p_declaration is a valid reference to an mCRL2 process definition, p_diagram_name is a valid process diagram name, p_preamble_parameter_decls is a valid reference to a parameter declaration and p_preamble_local_var_decls is a valid reference to a local variable declaration.
 * @post The transition is parsed and the results are appended to p_declaration or error messages are produced.
 */
void parse_transition(wxXmlNode *p_process_diagram, wxXmlNode *p_transition, wxString &p_declaration, bool p_alternative, bool p_is_terminating, wxString &p_diagram_name, list_of_decl &p_preamble_parameter_decls, list_of_decl_init &p_preamble_local_var_decls, ATermAppl &datatype_spec)
{
  wxString state_from, state_to, id_from, id_to;
  bool is_ref_from, is_ref_to;

  id_to = _T("-1");

  // determine begin- and endstate identifiers of this transition
  for(wxXmlNode *child = p_transition->GetChildren(); child != 0; child = child->GetNext())
  {
    if(child->GetName() == _T("from"))
    {
      id_from = child->GetNodeContent();
    }
    else if(child->GetName() == _T("to"))
    {
      id_to = child->GetNodeContent();
    }
  }

  // determine begin- and endstate names of this transition
  state_from = infer_state_name(p_process_diagram, id_from, is_ref_from);
  if(!p_is_terminating)
  {
    state_to = infer_state_name(p_process_diagram, id_to, is_ref_to);
  }

  wxString diagram_name = get_child_value(p_process_diagram, _T("name"));

  // load label
  label trans_label;
  try
  {
    trans_label = parse_transition_label(p_transition, p_preamble_parameter_decls, p_preamble_local_var_decls, diagram_name, datatype_spec);
  }
  catch(...)
  {
    return;
  }
  wxString variables = trans_label.get_declarations_text();
  wxString condition = trans_label.get_condition();
  wxString actions = trans_label.get_actions_text();
  if(actions == wxEmptyString)
  {
    // an empty multiaction is translated as a tau
    actions = _T("tau");
  }
  wxString timestamp = trans_label.get_timestamp();
  list_of_varupdate updates = trans_label.get_variable_updates();

  p_declaration += _T("\n");

  if(p_alternative)
  {
    p_declaration += _T("     + ");
  }
  else
  {
    p_declaration += _T("       ");
  }
  p_declaration += _T("(s == ");
  if(is_ref_from)
  {
    p_declaration += _T("S") + state_from + id_from + _T("_out) -> ");
  }
  else
  {
    p_declaration += state_from + id_from + _T(") -> ");
  }

  // add optional variables
  if(variables != wxEmptyString)
  {
    p_declaration += _T("sum ") + variables + _T(".");
  }
  // add optional condition
  if(condition != wxEmptyString)
  {
    p_declaration += _T("(") + condition + _T(") -> ");
  }

  // add multiaction
  if(actions != wxEmptyString)
  {
    p_declaration += actions;
  }
  if(!p_is_terminating)
  {
    p_declaration += _T(".");
    // add end state with optional variable updates
    p_declaration += p_diagram_name + _T("_internal(");
    if(is_ref_to)
    {
      p_declaration += _T("S") + state_to + id_to + _T("_in");
    }
    else
    {
      p_declaration += state_to + id_to;
    }

    // process parameters, local variables and variable updates
    for(unsigned int i=0; i<p_preamble_parameter_decls.GetCount(); ++i)
    {
      wxString variable_update = _T(", ") + p_preamble_parameter_decls[i].get_name();
      // check for updates
      for(unsigned int j=0; j<updates.GetCount(); ++j)
      {
        if(updates[j].get_lhs() == p_preamble_parameter_decls[i].get_name())
        {
          variable_update = _T(", ") + updates[j].get_rhs();
          break;
        }
      }
      p_declaration += variable_update;
    }
    for(unsigned int i=0; i<p_preamble_local_var_decls.GetCount(); ++i)
    {
      wxString variable_update = _T(", ") + p_preamble_local_var_decls[i].get_name();
      // check for updates
      for(unsigned int j=0; j<updates.GetCount(); ++j)
      {
        if(updates[j].get_lhs() == p_preamble_local_var_decls[i].get_name())
        {
          variable_update = _T(", ") + updates[j].get_rhs();
          break;
        }
      }
      p_declaration += variable_update;
    }
    p_declaration += _T(")");
  }
}

/**
 * Process diagram parameter sorting function.
 * Sorts a list of process diagram parameters, based on the sequence in which
 * they appear in the process diagram's preamble.
 * @param p_doc_root The root node of the XML specification.
 * @param p_diagram_name The name of the diagram that contains the parameters.
 * @param p_inits The parameters (with values) to sort.
 * @param datatype_spec The datatype specification.
 * @return The list of parameters (with values) sorted in the order in which the parameters appear in the process diagram's preamble.
 * @pre p_doc_root is a valid pointer to an XML node, p_diagram_name is a valid name of a process diagram, p_inits is a valid reference to a list of process diagram parameters.
 * @post The list of parameters (with values) is returned in sorted order or error messages are produced.
 */
list_of_decl_init sort_parameters(wxXmlNode *p_doc_root, wxString &p_diagram_name, list_of_decl_init &p_inits, ATermAppl &datatype_spec)
{
  list_of_decl_init decls;
  decls.Empty();

  wxXmlNode *diagram_list = get_child(p_doc_root, _T("processdiagramlist"));
  if(diagram_list == 0)
  {
    // ERROR: XML document does not contain child <processdiagramlist>
    cerr << "mCRL2 conversion error: the specification does not contain any process diagrams." << endl;
    throw CONVERSION_ERROR;
    return decls;
  }
  // search appropriate process diagram
  wxXmlNode *proc_diag;
  for(proc_diag = diagram_list->GetChildren(); proc_diag != 0; proc_diag = proc_diag->GetNext())
  {
    if(get_child_value(proc_diag, _T("name")) == p_diagram_name)
    {
      // found appropriate diagram
      break;
    }
  }
  if(proc_diag == 0)
  {
    // ERROR: XML file does not contain process diagram p_diagram_name
    cerr << "mCRL2 conversion error: the specification does not contain a process diagram for process reference "
         << p_diagram_name.ToAscii() << "." << endl;
    throw CONVERSION_ERROR;
    return decls;
  }

  // parse preamble of diagram
  list_of_decl preamble_params;
  list_of_decl_init preamble_vars;
  parse_preamble(proc_diag, preamble_params, preamble_vars, datatype_spec);

  // sort p_inits
  if(p_inits.GetCount() != preamble_params.GetCount())
  {
    // ERROR: preamble of p_diagram name does not contain the same parameters as its process reference
    cerr << "mCRL2 conversion error: the process reference to process diagram " << p_diagram_name.ToAscii()
         << " does not contain the same parameters." << endl;
    throw CONVERSION_ERROR;
    return decls;
  }
  for(unsigned int i=0; i<preamble_params.GetCount(); ++i)
  {
    bool found = false;
    for(unsigned int j=0; j<p_inits.GetCount(); ++j)
    {
      if(p_inits[j].get_name() == preamble_params[i].get_name())
      {
        decls.Add(p_inits[j]);
        found = true;
        break;
      }
    }
    if(!found)
    {
      // ERROR: process reference of diagram_name does not contain parameter preamble_params[i].get_name()
      cerr << "mCRL2 conversion error: the process reference to process diagram " << p_diagram_name.ToAscii()
           << " does not contain parameter " << preamble_params[i].get_name().ToAscii() << "." << endl;
      throw CONVERSION_ERROR;
      return decls;
    }
  }
  return decls;
}

list_of_varupdate sort_parameters(wxXmlNode *p_doc_root, wxString &p_diagram_name, list_of_varupdate &p_inits, ATermAppl &datatype_spec)
{
  list_of_varupdate decls;
  decls.Empty();

  wxXmlNode *diagram_list = get_child(p_doc_root, _T("processdiagramlist"));
  if(diagram_list == 0)
  {
    // ERROR: XML document does not contain child <processdiagramlist>
    cerr << "mCRL2 conversion error: the specification does not contain any process diagrams." << endl;
    throw CONVERSION_ERROR;
    return decls;
  }
  // search appropriate process diagram
  wxXmlNode *proc_diag;
  for(proc_diag = diagram_list->GetChildren(); proc_diag != 0; proc_diag = proc_diag->GetNext())
  {
    if(get_child_value(proc_diag, _T("name")) == p_diagram_name)
    {
      // found appropriate diagram
      break;
    }
  }
  if(proc_diag == 0)
  {
    // ERROR: XML file does not contain process diagram p_diagram_name
    cerr << "mCRL2 conversion error: the specification does not contain a process diagram for process reference "
         << p_diagram_name.ToAscii() << "." << endl;
    throw CONVERSION_ERROR;
    return decls;
  }

  // parse preamble of diagram
  list_of_decl preamble_params;
  list_of_decl_init preamble_vars;
  parse_preamble(proc_diag, preamble_params, preamble_vars, datatype_spec);

  // sort p_inits
  if(p_inits.GetCount() != preamble_params.GetCount())
  {
    // ERROR: preamble of p_diagram name does not contain the same parameters as its process reference
    cerr << "mCRL2 conversion error: the process reference to process diagram " << p_diagram_name.ToAscii()
         << " does not contain the same parameters." << endl;
    throw CONVERSION_ERROR;
    return decls;
  }
  for(unsigned int i=0; i<preamble_params.GetCount(); ++i)
  {
    bool found = false;
    for(unsigned int j=0; j<p_inits.GetCount(); ++j)
    {
      if(p_inits[j].get_lhs() == preamble_params[i].get_name())
      {
        decls.Add(p_inits[j]);
        found = true;
        break;
      }
    }
    if(!found)
    {
      // ERROR: process reference of diagram_name does not contain parameter preamble_params[i].get_name()
      cerr << "mCRL2 conversion error: the process reference to process diagram " << p_diagram_name.ToAscii()
           << " does not contain parameter " << preamble_params[i].get_name().ToAscii() << "." << endl;
      throw CONVERSION_ERROR;
      return decls;
    }
  }
  return decls;
}

/**
 * XML process reference transition parsing function.
 * Parses a transition (which is not visible in the GraPE specification) for a process reference
 * of a process diagram to construct an mCRL2 choice expression for the internal mCRL2 process definition.
 * @param p_doc_root The root node of the XML specification.
 * @param p_reference_state The XML reference state to construct a hidden transition for.
 * @param p_diagram_name The name of the process diagram that contains the process reference.
 * @param p_declaration The mCRL2 process definition to parse the XML transition to.
 * @param p_alternative Boolean flag to indicate if the constructed mCRL2 process definition already has preceding choice expressions.
 * @param p_preamble_parameter_decls The preamble parameter declarations of the process diagram that contains the process reference to be parsed.
 * @param p_preamble_local_var_decls The preamble local variable declarations of the process diagram that contains the process reference to be parsed.
 * @pre p_doc_root is a valid pointer to an XML node, p_reference_state is a valid pointer to an XML reference state, p_diagram name is a valid process diagram name, p_declarations is a valid reference to an mCRL2 process definition, p_preamble_parameter_decls is a valid reference to a parameter declaration, p_preamble_local_var_decls is a valid reference to a loval variable declaration.
 * @post The hidden transition of the process reference is parsed and the results are appended to p_declarations or error messages are produced.
 */
void parse_transition_reference(wxXmlNode *p_doc_root, wxXmlNode *p_reference_state, wxString &p_diagram_name, wxString &p_declaration, bool p_alternative, list_of_decl &p_preamble_parameter_decls, list_of_decl_init &p_preamble_local_var_decls, ATermAppl &datatype_spec)
{
  wxString ref_name = get_child_value(p_reference_state, _T("name"));
  if(ref_name == wxEmptyString)
  {
    // ERROR: <referencestate> contains invalid <name> child
    cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii()
         << " contains a process reference without a name." << endl;
    throw CONVERSION_ERROR;
    return;
  }
  // ref_name contains the name of this process reference

  wxString ref_id = get_child_value(p_reference_state, _T("id"));
  if(ref_id == wxEmptyString)
  {
    // ERROR: <referencestate> contains invalid <id> child
    cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii()
         << " contains an invalid identifier for process reference "
         << ref_name.ToAscii() << "." << endl;
    throw CONVERSION_ERROR;
    return;
  }

  list_of_varupdate ref_inits;
  try
  {
    parse_reference_parameters(p_reference_state, p_diagram_name, ref_inits, p_preamble_parameter_decls, datatype_spec);
  }
  catch(...)
  {
    return;
  }

  // ref_inits contains the parameter initialisation of this process reference

  p_declaration += _T("\n");

  if(p_alternative)
  {
    p_declaration += _T("     + ");
  }
  else
  {
    p_declaration += _T("       ");
  }
  p_declaration += _T("(s == ");
  p_declaration += _T("S");
  p_declaration += ref_name;
  p_declaration += ref_id + _T("_in) -> ");
  p_declaration += ref_name;
  if(ref_inits.GetCount() > 0)
  {
    p_declaration += _T("(");
    ref_inits = sort_parameters(p_doc_root, ref_name, ref_inits, datatype_spec);
  }
  for(unsigned int i=0; i<ref_inits.GetCount(); ++i)
  {
    if(i > 0)
    {
      p_declaration += _T(", ");
    }
    p_declaration += ref_inits[i].get_rhs();
  }
  if(ref_inits.GetCount() > 0)
  {
    p_declaration += _T(")");
  }
  p_declaration += _T(".") + p_diagram_name + _T("_internal(") + _T("S") + ref_name + ref_id + _T("_out");
  if(p_preamble_parameter_decls.GetCount() > 0)
  {
    for(unsigned int i=0; i<p_preamble_parameter_decls.GetCount(); ++i)
    {
      p_declaration += _T(", ");
      p_declaration += p_preamble_parameter_decls[i].get_name();
    }
  }
  if(p_preamble_local_var_decls.GetCount() > 0)
  {
    for(unsigned int i=0; i<p_preamble_local_var_decls.GetCount(); ++i)
    {
      p_declaration += _T(", ");
      p_declaration += p_preamble_local_var_decls[i].get_name();
    }
  }
  p_declaration += _T(")");
}

/**
 * XML process diagram to mCRL2 internal process specification convertor function.
 * Constructs an mCRL2 internal process specification out of a valid XML process diagram.
 * @param p_doc_root The root node of the XML specification.
 * @param p_process_diagram The XML process diagram to construct an internal process specification for.
 * @param p_preamble_parameter_decls The preamble's parameter declaration of the process diagram to construct an internal process specification for.
 * @param p_preamble_local_var_decls The preamble's local variable declarations of the process diagram to construct an internal process specification for.
 * @return The internal process specification or the empty string if construction failed.
 * @pre p_doc_root is a valid pointer to an XML node, p_process_diagram is a valid pointer to an XML process diagram, p_preamble_parameter_decls is a valid reference to a parameter declaration and p_preamble_local_var_decls is a valid reference to a local variable declaration.
 * @post The internal process specification is returned or the empty string is returned and error messages are produced.
 */
wxString process_diagram_mcrl2_internal_proc(wxXmlNode *p_doc_root, wxXmlNode *p_process_diagram, list_of_decl &p_preamble_parameter_decls, list_of_decl_init &p_preamble_local_var_decls, ATermAppl &datatype_spec)
{
  if(p_process_diagram->GetName() == _T("processdiagram"))
  {
    // construct the first part of the declaration
    wxString decl_internal = _T("proc ");

    // <processdiagram> node
    wxString diagram_name = get_child_value(p_process_diagram, _T("name"));
    if(diagram_name == wxEmptyString)
    {
      cerr << "mCRL2 conversion error: cannot construct an internal process specification for an unnamed process diagram." << endl;
      throw CONVERSION_ERROR;
      return wxEmptyString;
    }

    decl_internal += diagram_name + _T("_internal(");

    // add state parameter to declaration
    decl_internal += _T("s: ") + diagram_name + _T("_state");
    // add preamble parameters and local variable declarations
    for(unsigned int i=0; i<p_preamble_parameter_decls.GetCount(); ++i)
    {
      decl_internal += _T(", ");
      decl_internal += p_preamble_parameter_decls[i].get_name();
      decl_internal += _T(": ");
      decl_internal += p_preamble_parameter_decls[i].get_type();
    }
    // add preamble local variables to declaration
    for(unsigned int i=0; i<p_preamble_local_var_decls.GetCount(); ++i)
    {
      decl_internal += _T(", ");
      decl_internal += p_preamble_local_var_decls[i].get_name();
      decl_internal += _T(": ");
      decl_internal += p_preamble_local_var_decls[i].get_type();
    }
    decl_internal += _T(") = ");

    // construct internal specification
    bool first_line = true;
    // child = <objectlist>
    wxXmlNode *child = get_child(p_process_diagram, _T("objectlist"));
    if(child == 0)
    {
      // ERROR: <processdiagram> has no child <objectlist>
      cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii()
           << " does not contain any objects." << endl;
      return wxEmptyString;
    }
    wxString decl_trans = wxEmptyString;
    for(child = child->GetChildren(); child != 0; child = child->GetNext())
    {
      // process objectlist children
      if(child->GetName() == _T("terminatingtransitionlist"))
      {
        // parse transition labels of all terminating transitions in this <terminatingtransitionlist> to actions
        for(wxXmlNode *child_trans = child->GetChildren(); child_trans != 0; child_trans = child_trans->GetNext())
        {
          if(child_trans->GetName() != _T("terminatingtransition"))
          {
            // WARNING: <terminatingtransitionlist> has no child <terminatingtransition>
            cerr << "mCRL2 conversion warning: process diagram " << diagram_name.ToAscii()
                 << " has no terminating transitions." << endl;
            break;
          }

          parse_transition(p_process_diagram, child_trans, decl_trans, !first_line, true, diagram_name, p_preamble_parameter_decls, p_preamble_local_var_decls, datatype_spec);
          if(first_line)
          {
            first_line = false;
          }
        }
      }
      if(child->GetName() == _T("nonterminatingtransitionlist"))
      {
        // parse transition labels of all nonterminating transitions in this <nonterminatingtransitionlist> to actions
        for(wxXmlNode *child_trans = child->GetChildren(); child_trans != 0; child_trans = child_trans->GetNext())
        {
          if(child_trans->GetName() != _T("nonterminatingtransition"))
          {
            // WARNING: <nonterminatingtransitionlist> has no child <nonterminatingtransition>
            cerr << "mCRL2 conversion warning: process diagram " << diagram_name.ToAscii()
                 << " has no non-terminating transitions." << endl;
            break;
          }

          parse_transition(p_process_diagram, child_trans, decl_trans, !first_line, false, diagram_name, p_preamble_parameter_decls, p_preamble_local_var_decls, datatype_spec);
          if(first_line)
          {
            first_line = false;
          }
        }
      }
      if(child->GetName() == _T("referencestatelist"))
      {
        // parse all reference states in this <referencestatelist>
        for(wxXmlNode *child_ref = child->GetChildren(); child_ref != 0; child_ref = child_ref->GetNext())
        {
          if(child_ref->GetName() != _T("referencestate"))
          {
            // WARNING: <referencestatelist> has no child <referencestate>
            cerr << "mCRL2 conversion warning: process diagram " << diagram_name.ToAscii()
                 << " has no process references." << endl;
            break;
          }
          parse_transition_reference(p_doc_root,  child_ref, diagram_name, decl_trans, !first_line, p_preamble_parameter_decls, p_preamble_local_var_decls, datatype_spec);
          if(first_line)
          {
            first_line = false;
          }
        }
      }
    }
    if(decl_trans == wxEmptyString)
    {
      decl_internal += _T("delta");
    }
    else
    {
      decl_internal += decl_trans;
    }
    decl_internal += _T(";\n");
    return decl_internal;
  }

  cerr << "mCRL2 conversion error: process_diagram_mcrl2_internal_proc called without a valid XML process diagram." << endl;
  throw CONVERSION_ERROR;
  return wxEmptyString;
}

/**
 * XML initial designator inference function.
 * Infers the initial designator of a process diagram.
 * @param p_process_diagram The process diagram to infer the initial designator of.
 * @return The name of the state (or process reference) of the process diagram that contains the initial designator.
 * @pre p_process_diagram is a valid pointer to an XML process diagram.
 * @post The name of the inferred state (or process reference) connected to the initial designator is returned or error messages are produced.
 */
wxString infer_initial_designator(wxXmlNode *p_process_diagram)
{
  if(p_process_diagram->GetName() == _T("processdiagram"))
  {
    wxString diagram_name = get_child_value(p_process_diagram, _T("name"));

    wxXmlNode *obj_list = get_child(p_process_diagram, _T("objectlist"));
    if(obj_list == 0)
    {
      // ERROR: <processdiagram> does not contain <objectlist>
      cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii()
           << " does not contain any objects." << endl;
      throw CONVERSION_ERROR;
      return wxEmptyString;
    }
    wxXmlNode *des_list = get_child(obj_list, _T("initialdesignatorlist"));
    if(des_list == 0)
    {
      // ERROR: <objectlist> does not contain <initialdesignatorlist>
      cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii()
           << " does not contain an initial designator." << endl;
      throw CONVERSION_ERROR;
      return wxEmptyString;
    }
    wxXmlNode *des = get_child(des_list, _T("initialdesignator"));
    if(des == 0)
    {
      // ERROR: <initialdesignatorlist> does not contain <initialdesignator>
      cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii()
           << " does not contain an initial designator." << endl;
      throw CONVERSION_ERROR;
      return wxEmptyString;
    }

    // extract id of (reference-)state initial designator is connected to
    wxString des_id = get_child_value(des, _T("propertyof"));
    if(des_id == wxEmptyString)
    {
      // ERROR: <initialdesignator> is not connected!
      cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii()
           << " contains an unconnected initial designator." << endl;
      throw CONVERSION_ERROR;
      return wxEmptyString;
    }
    bool is_ref;
    wxString des_name = infer_state_name(p_process_diagram, des_id, is_ref) + des_id;
    if(is_ref)
    {
      des_name = _T("S") + des_name + _T("_in");
    }
    return des_name;
  }

  cerr << "mCRL2 conversion error: infer_initial_designator called without a valid XML process diagram." << endl;
  throw CONVERSION_ERROR;
  return wxEmptyString;
}

/**
 * XML process diagram to mCRL2 process specification convertor function.
 * Constructs an mCRL2 process specification out of a valid XML process diagram.
 * @param p_process_diagram The XML process diagram to construct a process specification for.
 * @param p_preamble_parameter_decls The preamble's parameter declaration of the process diagram to construct a process specification for.
 * @param p_preamble_local_var_decls The preamble's local variable declaration of the process diagram to construct a process specification for.
 * @return The process specification or the empty string if construction failed.
 * @pre p_process diagram is a valid pointer to an XML process diagram, p_preamble_parameter_decls is a valid reference to a parameter declaration and p_preamble_local_var_decls is a valid reference to a local variable declaration.
 * @post The process specification is returned, or the empty string is returned and error messages are produced.
 */
wxString process_diagram_mcrl2_proc(wxXmlNode *p_process_diagram, list_of_decl &p_preamble_parameter_decls, list_of_decl_init &p_preamble_local_var_decls)
{
  if(p_process_diagram->GetName() == _T("processdiagram"))
  {
    // construct declaration
    wxString proc_decl = _T("proc ");
    wxString diagram_name = get_child_value(p_process_diagram, _T("name"));
    if(diagram_name == wxEmptyString)
    {
      // ERROR: No name could be found for process diagram p_process_diagram.
      cerr << "mCRL2 conversion error: the specification contains an unnamed process diagram." << endl;
      throw CONVERSION_ERROR;
      return wxEmptyString;
    }
    proc_decl += diagram_name;
    if(p_preamble_parameter_decls.GetCount() > 0)
    {
      proc_decl += _T("(");
    }
    for(unsigned int i=0; i<p_preamble_parameter_decls.GetCount(); ++i)
    {
      if(i > 0)
      {
        proc_decl += _T(", ");
      }
      proc_decl += p_preamble_parameter_decls[i].get_name();
      proc_decl += _T(": ");
      proc_decl += p_preamble_parameter_decls[i].get_type();
    }
    if(p_preamble_parameter_decls.GetCount() > 0)
    {
      proc_decl += _T(")");
    }
    proc_decl += _T(" =\n");
    proc_decl += _T("     ");
    proc_decl += diagram_name + _T("_internal(");

    // extract initial designator
    wxString designator = infer_initial_designator(p_process_diagram);
    if(designator == wxEmptyString)
    {
      // ERROR: no initial designator present for this diagram
      cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii()
           << " contains an invalid initial designator." << endl;
      throw CONVERSION_ERROR;
      return wxEmptyString;
    }
    proc_decl += designator;
    for(unsigned int i=0; i<p_preamble_parameter_decls.GetCount(); ++i)
    {
      proc_decl += _T(", ");
      proc_decl += p_preamble_parameter_decls[i].get_name();
    }
    for(unsigned int i=0; i<p_preamble_local_var_decls.GetCount(); ++i)
    {
      proc_decl += _T(", ");
      proc_decl += p_preamble_local_var_decls[i].get_value();
    }
    proc_decl += _T(");");

    return proc_decl;
  }

  // ERROR: not a valid XML process diagram
  cerr << "mCRL2 conversion error: process_diagram_mcrl2_proc called without a valid XML process diagram." << endl;
  throw CONVERSION_ERROR;
  return wxEmptyString;
}

/**
 * XML process diagram to mCRL2 initial process specification convertor function.
 * Constructs an mCRL2 initial process specification out of a parameter initialisation.
 * @param p_doc_root The root of a valid XML specification.
 * @param p_diagram_name The name of the exported process diagram.
 * @param p_parameter_init The parameter initialisation for the exported process diagram.
 * @param datatype_spec The datatype specification.
 * @return The initial process specification for the exported process diagram.
 * @pre p_doc_root is a valid pointer to an XML specification, p_diagram_name is a valid reference to a process diagram and p_parameter init is a valid reference to a parameter initialisation.
 * @post The initial process specification is returned or error messages are produced.
 */
wxString process_diagram_mcrl2_init(wxXmlNode *p_doc_root, wxString &p_diagram_name, list_of_decl_init &p_parameter_init, ATermAppl &datatype_spec)
{
  wxString init_decl = wxEmptyString;
  init_decl += _T("init ");
  init_decl += p_diagram_name;
  if(p_parameter_init.GetCount() > 0)
  {
    init_decl += _T("(");

    // sort parameter initialisation and print to init declaration
    list_of_decl_init decls = sort_parameters(p_doc_root, p_diagram_name, p_parameter_init, datatype_spec);
    for(unsigned int i=0; i<decls.GetCount(); ++i)
    {
      if(i > 0)
      {
        init_decl += _T(", ");
      }
      init_decl += decls[i].get_value();
    }
    init_decl += _T(")");
  }
  init_decl += _T(";");
  return init_decl;
}
/**
 * XML datatype specification to mCRL2 datatype specification convertor function.
 * Constructs an mCRL2 datatype specification out of an XML datatype specification.
 * @param p_doc_root The root of a valid XML specification.
 * @return The datatype specification extracted from the XML specification or the empty string if it is not present.
 * @pre p_doc_root is a valid XML specification.
 * @post The datatype specification is returned or the empty string is returned and error messages are produced.
 */
wxString datatype_specification_mcrl2(wxXmlNode *p_doc_root)
{
  wxXmlNode *spec_list = get_child(p_doc_root, _T("datatypespecificationlist"));
  if(spec_list == 0)
  {
    // ERROR: specification does not contain datatypespecificationlist
    cerr << "mCRL2 conversion error: the specification does not contain a datatypespecification." << endl;
    throw CONVERSION_ERROR;
    return wxEmptyString;
  }
  wxString datatype_spec = get_child_value(spec_list, _T("datatypespecification"));
  return datatype_spec;
}

/**
 * Process reference inference function.
 * Infers the process references in a given process or architecture diagram.
 * @param p_diagram The XML process or architecture diagram to infer the process references for.
 * @return An array of identifiers of all process references in this diagram.
 * @pre p_diagram is a valid pointer to an XML process or architecture diagram.
 * @post An array of identifiers of all process references in p_diagram is returned or the empty array is returned and error messages are produced.
 */
wxArrayString infer_process_references(wxXmlNode *p_diagram)
{
  wxArrayString refs;
  refs.Empty();

  wxXmlNode *objects = get_child(p_diagram, _T("objectlist"));
  wxString diagram_name = get_child_value(p_diagram, _T("name"));
  wxXmlNode *ref_list;
  if (p_diagram->GetName() == _T("processdiagram"))
  {
    if(objects == 0)
    {
      // ERROR: diagram has no objects
      cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii()
           << " contains no objects." << endl;
      throw CONVERSION_ERROR;
      return refs;
    }
    ref_list = get_child(objects, _T("referencestatelist"));
    if(ref_list == 0)
    {
      // ERROR: no references
      cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii()
           << " contains no process references." << endl;
      throw CONVERSION_ERROR;
      return refs;
    }
  }
  else if (p_diagram->GetName() == _T("architecturediagram"))
  {
    if(objects == 0)
    {
      // ERROR: diagram has no objects
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " contains no objects." << endl;
      throw CONVERSION_ERROR;
      return refs;
    }
    ref_list = get_child(objects, _T("processreferencelist"));
    if(ref_list == 0)
    {
      // ERROR: no references
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " contains no process references." << endl;
      throw CONVERSION_ERROR;
      return refs;
    }
  }
  else
  {
    // ERROR: no valid XML code
    return refs;
  }

  for(wxXmlNode *ref = ref_list->GetChildren(); ref != 0; ref = ref->GetNext())
  {
    wxString ref_id = get_child_value(ref, _T("propertyof"));
    refs.Add(ref_id);
  }
  return refs;
}

/**
 * Architecture reference inference function.
 * Infers the architecture references in a given architecture diagram.
 * @param p_diagram The XML architecture diagram to infer the architecture references for.
 * @return An array of identifiers of all architecture references in this diagram.
 * @pre p_diagram is a valid pointer to an XML architecture diagram.
 * @post An array of identifiers of all architecture references in p_diagram is returned or the empty array is returned and error messages are produced.
 */
wxArrayString infer_architecture_references(wxXmlNode *p_diagram)
{
  wxArrayString refs;
  refs.Empty();

  wxXmlNode *objects = get_child(p_diagram, _T("objectlist"));
  wxString diagram_name = get_child_value(p_diagram, _T("name"));
  wxXmlNode *ref_list;
  if (p_diagram->GetName() == _T("architecturediagram"))
  {
    if(objects == 0)
    {
      // ERROR: diagram has no objects
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " contains no objects." << endl;
      throw CONVERSION_ERROR;
      return refs;
    }
    ref_list = get_child(objects, _T("architecturereferencelist"));
    if(ref_list == 0)
    {
      // ERROR: no references
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " contains no architecture references." << endl;
      throw CONVERSION_ERROR;
      return refs;
    }
  }
  else
  {
    // ERROR: no valid XML code
    return refs;
  }

  for(wxXmlNode *ref = ref_list->GetChildren(); ref != 0; ref = ref->GetNext())
  {
    wxString ref_id = get_child_value(ref, _T("propertyof"));
    refs.Add(ref_id);
  }
  return refs;
}

/**
 * Architecture diagram to mCRL2 possible actions convertor function.
 * Extracts all possible actions out of an architecture diagram and
 * adds them to an array.
 * @param p_doc_root The XML specification.
 * @param p_architecture_diagram The architecture diagram.
 * @param p_possibles An array containing all possible actions in this architecture diagram.
 * @pre p_doc_root is a valid pointer to an XML specification, p_architecture_diagram is a valid pointer to an XML architecture diagram and p_possibles is a valid reference to an array of actions.
 * @post All possible actions in this architecture diagram are stored in p_possibles, sorted by reference.
 */
void architecture_diagram_mcrl2_actions(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, arr_action_reference &p_possibles, ATermAppl &datatype_spec)
{
  if(p_architecture_diagram->GetName() == _T("architecturediagram"))
  {
    wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));

    // go through all references and determine possible actions
    wxXmlNode *objects = get_child(p_architecture_diagram, _T("objectlist"));
    wxXmlNode *archs = get_child(objects, _T("architecturereferencelist"));
    wxXmlNode *procs = get_child(objects, _T("processreferencelist"));
    if(archs == 0)
    {
      // ERROR: <architecturediagram> does not contain architecture references.
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " does not contain a list of architecture references." << endl;
      throw CONVERSION_ERROR;
      return;
    }
    if(procs == 0)
    {
      // ERROR: <architecturediagram> does not contain process references.
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " does not contain a list of process references." << endl;
      throw CONVERSION_ERROR;
      return;
    }

    // process process references
    for(wxXmlNode *proc = procs->GetChildren(); proc != 0; proc = proc->GetNext())
    {
      action_reference aref;

      wxString ref_id = get_child_value(proc, _T("propertyof"));
      if(ref_id == wxEmptyString)
      {
        // ERROR: could not find reference
        cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
             << " contains an empty process reference." << endl;
        throw CONVERSION_ERROR;
        return;
      }
      wxString ref_name = infer_process_diagram_name(p_doc_root, ref_id);
      if(ref_name == wxEmptyString)
      {
        // ERROR: could not infer name
        cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
             << " contains a process reference to an unnamed process diagram." << endl;
        throw CONVERSION_ERROR;
        return;
      }
      wxString reference_id = get_child_value(proc, _T("id"));
      aref.m_reference = ref_name;
      aref.m_reference_id = reference_id;
      aref.m_diagram_id = ref_id;
      aref.m_is_process_reference = true;

      // infer possible actions for this reference
      aref.m_actions = infer_process_actions(p_doc_root, ref_id, datatype_spec);
      p_possibles.Add(aref);
    }

    // process architecture references
    for(wxXmlNode *arch = archs->GetChildren(); arch != 0; arch = arch->GetNext())
    {
      action_reference aref;

      wxString ref_id = get_child_value(arch, _T("propertyof"));
      if(ref_id == wxEmptyString)
      {
        // ERROR: could not find reference
        cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
             << " contains an empty architecture reference." << endl;
        throw CONVERSION_ERROR;
        return;
      }
      wxString ref_name = infer_architecture_name(p_doc_root, ref_id);
      if(ref_name == wxEmptyString)
      {
        // ERROR: could not infer name
        cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
             << " contains an architecture reference to an unnamed architecture diagram." << endl;
        throw CONVERSION_ERROR;
        return;
      }

      // infer possibles for this reference
      wxXmlNode *arch_diagram = get_architecture_diagram(p_doc_root, ref_name);
      if(arch_diagram == 0)
      {
        // ERROR: could not find architecture diagram
        cerr << "mCRL2 conversion error: could not find architecture diagram "
             << ref_name.ToAscii() << "." << endl;
        throw CONVERSION_ERROR;
        return;
      }
      // update vars for referenced architecture diagram
      wxString reference_id = get_child_value(arch, _T("id"));
      aref.m_reference = ref_name;
      aref.m_reference_id = reference_id;
      aref.m_diagram_id = ref_id;
      aref.m_is_process_reference = false;
      aref.m_actions = infer_architecture_visibles(p_doc_root, arch_diagram, datatype_spec);
      p_possibles.Add(aref);
    }
    // update vars for architecture diagram
    action_reference aref;
    wxString reference_id = get_child_value(p_architecture_diagram, _T("id"));
    aref.m_reference = diagram_name;
    aref.m_reference_id = _T("-1");
    aref.m_diagram_id = reference_id;
    aref.m_is_process_reference = false;
    aref.m_actions = infer_architecture_visibles(p_doc_root, p_architecture_diagram, datatype_spec);
    p_possibles.Add(aref);
  }

  // ERROR: not called with valid XML node
  return;
}

/**
 * Action list compacting function.
 * Removes duplicate actions from the list.
 * @param p_action The array of actions.
 * @pre p_action is a valid list of action.
 * @post All duplicates in p_actions are removed.
 */
list_of_action compact_list_action(list_of_action &p_actions)
{
  list_of_action new_actions;

  // compact members
  for(unsigned int j=0; j<p_actions.GetCount(); ++j)
  {
    bool found = false;
    for(unsigned int k=0; k<new_actions.GetCount(); ++k)
    {
      if(new_actions[k].get_name() == p_actions[j].get_name())
      {
        if ( new_actions[k].get_parameters().GetCount() == p_actions[j].get_parameters().GetCount() )
        {
          if ( new_actions[k].get_parameters().GetCount() == 0 )
          {
	          found = true;
            break;
          }
	        else
	        {
	          list_of_dataexpression acts_params = new_actions[k].get_parameters();
	          list_of_dataexpression actions_params = p_actions[j].get_parameters();
            found = true;
            unsigned int l = 0;
            while ( found && l < acts_params.GetCount() )
	          {
              found = acts_params[l].get_type() == actions_params[l].get_type();
              ++l;
		        }
            if (found)
            {
              break;
            }
  	      }
        }
      }
    }
    if(!found)
    {
      new_actions.Add(p_actions[j]);
    }
  }

  return new_actions;
}

/**
 * Channel of reference inference function.
 * Determines whether a given channel lies on a given reference in an architecture diagram.
 * @param p_architecture_diagram The XML architecture diagram which contains the reference.
 * @param p_channel_id The identifier of the channel.
 * @param p_reference_id The identifier of the reference supposed to contain the channel.
 * @return True if the channel lies on the given reference, false otherwise.
 * @pre p_architecture_diagram is a valid pointer to an XML architecture diagram, p_channel_id is a valid reference to a channel identifier and p_reference_id is a valid reference to a reference identifier.
 * @post A flag indicating whether the given channel lies on the given reference is returned or error messages are produced.
 */
bool is_channel_of_reference(wxXmlNode *p_architecture_diagram, wxString &p_channel_id, wxString &p_reference_id)
{
  if(p_architecture_diagram->GetName() == _T("architecturediagram"))
  {
    wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));
    wxXmlNode *objects = get_child(p_architecture_diagram, _T("objectlist"));
    if(objects == 0)
    {
      // ERROR: contains no objects
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " does not contain any objects." << endl;
      throw CONVERSION_ERROR;
      return false;
    }

    wxXmlNode *channels = get_child(objects, _T("channellist"));
    if(channels == 0)
    {
      // ERROR: no list of channels in spec
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " has no list of channels." << endl;
      throw CONVERSION_ERROR;
      return false;
    }

    // determine if there is a channel p_channel_id which belongs to p_reference_id
    for(wxXmlNode *channel_child = channels->GetChildren(); channel_child != 0; channel_child = channel_child->GetNext())
    {
      wxString channel_id = get_child_value(channel_child, _T("id"));
      if(channel_id == p_channel_id)
      {
        // found matching channel
        wxString channel_of = get_child_value(channel_child, _T("onreference"));
        if(channel_of == p_reference_id)
        {
          // found matching reference
          return true;
        }
      }
    }
  }
  else
  {
    // ERROR: not called with valid <architecturediagram>
    cerr << "mCRL2 conversion error: is_channel_of_reference called with an invalid XML specification." << endl;
    throw CONVERSION_ERROR;
    return false;
  }

  // nothing found, return false
  return false;
}

/**
 * Channel name inference function.
 * Infers the name of a given channel.
 * @param p_architecture_diagram The XML architecture diagram containing the channel.
 * @param p_channel_id The identifier of the channel.
 * @return The name of the channel or the empty string if p_channel_id is not valid.
 * @pre p_architecture_diagram is a valid pointer to an XML architecture diagram, p_channel_id is a valid reference to a channel identifier.
 * @post The name of the channel is returned or the empty string is returned and error messages are produced.
 */
wxString infer_channel_name(wxXmlNode *p_architecture_diagram, wxString &p_channel_id)
{
  if(p_architecture_diagram->GetName() == _T("architecturediagram"))
  {
    wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));
    wxXmlNode *objects = get_child(p_architecture_diagram, _T("objectlist"));
    if(objects == 0)
    {
      // ERROR: contains no objects
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " does not contain any objects." << endl;
      throw CONVERSION_ERROR;
      return wxEmptyString;
    }

    wxXmlNode *channels = get_child(objects, _T("channellist"));
    if(channels == 0)
    {
      // ERROR: no list of channels in spec
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " has no list of channels." << endl;
      throw CONVERSION_ERROR;
      return wxEmptyString;
    }

    // determine name of channel
    for(wxXmlNode *channel_child = channels->GetChildren(); channel_child != 0; channel_child = channel_child->GetNext())
    {
      wxString channel_id = get_child_value(channel_child, _T("id"));
      if(channel_id == p_channel_id)
      {
        // return name of channel
        wxString channel_name = get_child_value(channel_child, _T("name"));
        return channel_name;
      }
    }
  }
  else
  {
    // ERROR: not called with valid <architecturediagram>
    cerr << "mCRL2 conversion error: infer_channel_name called with an invalid XML specification." << endl;
    throw CONVERSION_ERROR;
    return wxEmptyString;
  }

  // nothing found, return the empty string
  return wxEmptyString;
}

/**
 * Reference channel inference function.
 * Infers the channels of a given reference.
 * @param p_architecture_diagram The XML architecture diagram containing the reference.
 * @param p_reference_id The identifier of the reference to infer the channels of.
 * @return An array of channel names, tupled with unique identifiers, containing the names of the channels present on the reference p_reference_id.
 * @pre p_architecture_diagram is a valid pointer to an XML architecture diagram and p_reference_id is a valid reference to a reference identifier.
 * @post An array containing the names and identifiers of all channels of the given reference is returned or error messages are produced.
 */
arr_channel_id infer_reference_channels(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, action_reference &p_reference, ATermAppl &datatype_spec)
{
  arr_channel_id channels;
  channels.Empty();

  if(p_architecture_diagram->GetName() == _T("architecturediagram"))
  {
    wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));
    wxXmlNode *objects = get_child(p_architecture_diagram, _T("objectlist"));
    if(objects == 0)
    {
      // ERROR: contains no objects
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " does not contain any objects." << endl;
      throw CONVERSION_ERROR;
      return channels;
    }

    wxXmlNode *channel_list = get_child(objects, _T("channellist"));
    if(channel_list == 0)
    {
      // ERROR: no list of channels in spec
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " has no list of channels." << endl;
      throw CONVERSION_ERROR;
      return channels;
    }

    list_of_action reference_actions = infer_process_actions(p_doc_root, p_reference.m_diagram_id, datatype_spec);
    // determine channels associated with reference
    for(wxXmlNode *channel_child = channel_list->GetChildren(); channel_child != 0;  channel_child = channel_child->GetNext())
    {
      wxString channel_ref = get_child_value(channel_child, _T("onreference"));
      if(channel_ref == p_reference.m_reference_id)
      {
        for (unsigned int i = 0; i < reference_actions.GetCount(); ++i)
        {
          wxString channel_name = get_child_value(channel_child, _T("name"));
          if (channel_name == reference_actions[i].get_name())
          {
            channel_id channel;
            channel.m_channel = reference_actions[i];
            channel.m_channel_id = get_child_value(channel_child, _T("id"));
            channels.Add(channel);
          }
        }
      }
    }
  }
  else
  {
    // ERROR: invalid XML data
    cerr << "mCRL2 conversion error: infer_reference_channels called with an invalid XML specification." << endl;
    throw CONVERSION_ERROR;
    return channels;
  }

  return channels;
}

/**
 * Blocked channels inference function.
 * Infers the blocked channels of a given reference.
 * @param p_architecture_diagram The XML architecture diagram containing the channels and references.
 * @param p_reference_id The identifier of the reference to infer its blocked channels of.
 * @return An array of strings containing the names of the blocked channels of the reference.
 * @pre p_architecture_diagram is a valid pointer to an XML architecture diagram and p_reference_id is a valid reference to a reference identifier.
 * @post An array containing the names of the blocked channels associated with the reference is returned or error messages are produced.
 */
wxArrayString infer_reference_blocked_channels(wxXmlNode *p_architecture_diagram, wxString &p_reference_id)
{
  wxArrayString blockeds;
  blockeds.Empty();

  if(p_architecture_diagram->GetName() == _T("architecturediagram"))
  {
    wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));
    wxXmlNode *objects = get_child(p_architecture_diagram, _T("objectlist"));
    if(objects == 0)
    {
      // ERROR: contains no objects
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " does not contain any objects." << endl;
      throw CONVERSION_ERROR;
      return blockeds;
    }

    wxXmlNode *channel_list = get_child(objects, _T("channellist"));
    if(channel_list == 0)
    {
      // ERROR: no list of channels in spec
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " has no list of channels." << endl;
      throw CONVERSION_ERROR;
      return blockeds;
    }

    // determine if reference p_reference_id has any associated blockeds
    for(wxXmlNode *channel_child = channel_list->GetChildren(); channel_child != 0; channel_child = channel_child->GetNext())
    {
      // get channel type
      wxString channel_type = get_child_value(channel_child, _T("channeltype"));
      if (channel_type == _T("blocked"))
      {
        // get channel id
        wxString channel_id = get_child_value(channel_child, _T("id"));
        if(is_channel_of_reference(p_architecture_diagram, channel_id, p_reference_id))
        {
          // get channel name
          wxString channel_name = get_child_value(channel_child, _T("name"));
          // the blocked channel belongs to this reference, add name of channel to list
          blockeds.Add(channel_name);
        }
      }
    }
  }
  else
  {
    // ERROR: not called with valid <architecturediagram>
    cerr << "mCRL2 conversion error: infer_reference_blocked_channels called with an invalid XML specification." << endl;
    throw CONVERSION_ERROR;
    return blockeds;
  }

  return blockeds;
}

/**
 * Hidden actions inference function.
 * Infers the hidden actions of a reference.
 * @param p_actions The actions occuring inside the reference.
 * @param p_channels The channels on the reference.
 * @param p_blockeds The channels on the reference that are blocked.
 * @return An array of strings containing all the actions inside the reference that are hidden (not visible).
 * @pre p_actions is a valid reference to an array of actions, p_channels is a valid reference to an array of channels and p_blockeds is a valid reference to an array of blocked channels.
 * @post An array containing all hidden actions of this reference is returned.
 */
list_of_action infer_reference_hidden_actions(list_of_action &p_actions, arr_channel_id &p_channels, wxArrayString &p_blockeds)
{
  list_of_action hidden;

  // for each action in p_actions, if it is not in p_channels or p_blockeds, it is hidden
  for(unsigned int i=0; i<p_actions.GetCount(); ++i)
  {
    bool is_hidden = true;
    for(unsigned int j=0; j<p_channels.GetCount(); ++j)
    {
      if(p_channels[j].m_channel.get_name() == p_actions[i].get_name())
      {
        is_hidden = false;
        break;
      }
    }

    for(unsigned int j=0; j<p_blockeds.GetCount(); ++j)
    {
      if(p_blockeds[j] == p_actions[i].get_name())
      {
        is_hidden = false;
        break;
      }
    }

    if(is_hidden)
    {
      hidden.Add(p_actions[i]);
    }
  }

  return hidden;
}

/**
 * Renamed actions inference function.
 * Infers the renamed actions of a reference.
 * @param p_channels The channels on the reference.
 * @param p_blockeds The channels on the reference that are blocked.
 * @return An array of channels with their unique identifiers of all actions that are to be renemaed.
 * @pre p_channels is a valid reference to an array of channels and p_blockeds is a valid reference to an array of blocked actions.
 * @post An array containing the names and unique identifiers of all actions which are to be renamed is returned.
 */
arr_channel_id infer_reference_renamed_actions(arr_channel_id &p_channels, wxArrayString &p_blockeds)
{
  arr_channel_id ren;
  ren.Empty();

  // for each action in p_channels, if it is not blocked, rename it
  for(unsigned int i=0; i<p_channels.GetCount(); ++i)
  {
    bool is_renamed = true;
    for(unsigned int j=0; j<p_blockeds.GetCount(); ++j)
    {
      if(p_blockeds[j] == p_channels[i].m_channel.get_name())
      {
        is_renamed = false;
        break;
      }
    }

    if(is_renamed)
    {
      ren.Add(p_channels[i]);
    }
  }

  arr_channel_id new_ren;
  for (unsigned int i=0; i<ren.GetCount(); ++i)
  {
    bool found = false;
    for (unsigned int j=0; j<new_ren.GetCount(); ++j)
    {
      if ( ren[i].m_channel.get_name() == new_ren[j].m_channel.get_name() )
      {
        list_of_dataexpression channel_params = ren[i].m_channel.get_parameters();
        list_of_dataexpression new_channel_params = new_ren[j].m_channel.get_parameters();
        if (channel_params.GetCount() == new_channel_params.GetCount())
        {
          if (channel_params.GetCount() == 0)
          {
            found = true;
            break;
          }
          else
          {
            found = true;
            unsigned int k = 0;
            while ( found && k < channel_params.GetCount() )
	          {
              found = channel_params[k].get_type() == new_channel_params[k].get_type();
              ++k;
		        }
            if (found)
            {
              break;
            }
  	      }
        }
      }
    }
    if(!found)
    {
      new_ren.Add(ren[i]);
    }
  }
  return new_ren;
}

/**
 * Channel communication to mCRL2 convertor function.
 * Extracts all channel communications out of an architecture diagram and matches them
 * with the already extracted channels.
 * @param p_architecture_diagram An XML architecture diagram.
 * @param p_renamed All renamed channels in the architecture diagram.
 * @return An array of channel communications in this diagram.
 * @pre p_architecture_diagram is a valid pointer to an XML architecture diagram and p_renamed is a valid reference to an array of renamed channels.
 * @post An array of channel communications in this diagram is returned or error messages are produced.
 */
arr_channel_comm architecture_diagram_mcrl2_communication(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, arr_action_reference &p_refs, ATermAppl &datatype_spec)
{
  arr_channel_comm comms;
  comms.Empty();

  if(p_architecture_diagram->GetName() == _T("architecturediagram"))
  {
    wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));
    wxXmlNode *objects = get_child(p_architecture_diagram, _T("objectlist"));
    if(objects == 0)
    {
      // ERROR: does not contain any objects
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " does not contain any objects." << endl;
      throw CONVERSION_ERROR;
      return comms;
    }

    wxXmlNode *channels = get_child(objects, _T("channellist"));
    if(channels == 0)
    {
      // ERROR: no list of channels
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " does not contain a list of channels." << endl;
      throw CONVERSION_ERROR;
      return comms;
    }

    wxXmlNode *channel_comms = get_child(objects, _T("channelcommunicationlist"));
    if(channel_comms == 0)
    {
      // ERROR: no list of channel communications
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " does not contain a list of channel communications." << endl;
      throw CONVERSION_ERROR;
      return comms;
    }

    // get process references
    wxXmlNode *proc_refs = get_child(objects, _T("processreferencelist"));

    for(wxXmlNode *child = channel_comms->GetChildren(); child != 0; child = child->GetNext())
    {
      channel_comm current;
      current.m_name = wxEmptyString;
      current.m_channels.Empty();

      // store identifier of channel communication
      wxString comm_id = get_child_value(child, _T("id"));
      current.m_id = comm_id;

      wxXmlNode *connectionlist = get_child(child, _T("connectionlist"));
      if(connectionlist == 0)
      {
        // ERROR: no list of connections
        cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
             << " contains a channel communication that does not contain a list of connections." << endl;
        throw CONVERSION_ERROR;
        return comms;
      }

      for(wxXmlNode *connection = connectionlist->GetChildren(); connection != 0; connection = connection->GetNext())
      {
        wxString conn_id = connection->GetNodeContent();
//        cerr << "conn id: " << conn_id.ToAscii() << endl;
        // search through renamed channels to find correct channel
        bool found = false;
//        cerr << "refs: " << p_refs.GetCount() << endl;
        for(unsigned int i=0; i<p_refs.GetCount(); ++i)
        {
//          cerr << "renameds: " << p_refs[i].m_renamed.GetCount() << endl;
          for(unsigned int j=0; j<p_refs[i].m_renamed.GetCount(); ++j)
          {
//            cerr << "renamed: " << p_refs[i].m_renamed[j].m_channel.get_name().ToAscii() << ": ";
            for (unsigned int k=0; k<p_refs[i].m_renamed[j].m_channel.get_parameters().GetCount(); ++k)
            {
//              cerr << p_refs[i].m_renamed[j].m_channel.get_parameters()[k].get_type().ToAscii();
              if (k < p_refs[i].m_renamed[j].m_channel.get_parameters().GetCount()-1)
              {
//                cerr << " # ";
              }
            }
//            cerr << endl;
//            cerr << "renamed id: " << p_refs[i].m_renamed[j].m_channel_id << endl;
            if(p_refs[i].m_renamed[j].m_channel_id == conn_id)
            {
//TODO: veel meer uitbreiden met type enzo.
              found = true;
//              cerr << "added" << endl;
              current.m_channels.Add(p_refs[i].m_renamed[j]);
//              break;
            }
          }
          if (found)
          {
            break;
          }
        }
        if(!found)
        {
          // ERROR: could not find channel
          cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
               << " contains a channel communication connected with an invalid channel." << endl;
          throw CONVERSION_ERROR;
          return comms;
        }
      }

      comms.Add(current);
    }
  }
  else
  {
    // ERROR: not called with valid XML node
    cerr << "mCRL2 conversion error: architecture_diagram_mcrl2_communication called with an invalid XML specification." << endl;
    throw CONVERSION_ERROR;
    return comms;
  }

  return comms;
}

/**
 * Blocked channel communication inference function.
 * Infers all the blocked channel communications inside a given architecture diagram.
 * @param p_architecture_diagram The XML architecture diagram containing the channel communications.
 * @param p_communications The array of channel communications inside the architecture diagram.
 * @return An array of channel communications that are blocked inside the given architecture diagram.
 * @pre p_architecture_diagram is a valid pointer to an XML architecture diagram containing the channel communications and p_communications is a valid reference to an array of channel communications inside the given XML architecture diagram.
 * @post An array of channel communications that are blocked inside the given architecture diagram is returned and error messages are produced if necessary.
 */
arr_channel_comm infer_communication_blocked(wxXmlNode *p_architecture_diagram, arr_channel_comm &p_communications)
{
  arr_channel_comm comms;
  comms.Empty();

  if(p_architecture_diagram->GetName() == _T("architecturediagram"))
  {
    wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));
    wxXmlNode *objects = get_child(p_architecture_diagram, _T("objectlist"));
    if(objects == 0)
    {
      // ERROR: <architecturediagram> does not contain any objects.
      cerr << "mCLR2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " does not contain a list of objects." << endl;
      throw CONVERSION_ERROR;
      return comms;
    }

    // get channel communications
    wxXmlNode *channel_communications = get_child(objects, _T("channelcommunicationlist"));
    if (channel_communications == 0)
    {
      // ERROR: <objectlist> has no <channelcommunicationlist>
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " does not contain any channel communications." << endl;
      throw CONVERSION_ERROR;
      return comms;
    }

    // for each channel communication in p_communications, go through the list of visibles
    // in this specification and add it to comms if there is a visible property for the
    // channel communication.
    for(wxXmlNode *channel_communication_child = channel_communications->GetChildren(); channel_communication_child != 0; channel_communication_child = channel_communication_child->GetNext())
    {
      // get channel communication type
      wxString channel_communication_type = get_child_value(channel_communication_child, _T("channelcommunicationtype"));
      if (channel_communication_type == _T("blocked"))
      {
        // get communication id
        wxString channel_communication_id = get_child_value(channel_communication_child, _T("id"));
        for(unsigned int i=0; i<p_communications.GetCount(); ++i)
        {
          if(channel_communication_id == p_communications[i].m_id)
          {
            comms.Add(p_communications[i]);
            break;
          }
        }
      }
    }
  }
  else
  {
    // ERROR: not called with valid XML node
    cerr << "mCRL2 conversion error: infer_communication_blocked called with an invalid XML specification." << endl;
    throw CONVERSION_ERROR;
    return comms;
  }

  return comms;
}

/**
 * Non-visible channel communications inference function.
 * Infers all non-visible channel communications in an architecture diagram.
 * @param p_architecture_diagram An XML architecture diagram.
 * @param p_communications An array of channel communications contained in the architecture diagram.
 * @return An array of channel communications which are not connected to the visibility frame of the architecture diagram.
 * @pre p_architecture_diagram is a valid pointer to an XML architecture diagram and p_communications is a valid reference to an array of channel communications.
 * @post An array containing all non-visible channel communications in the given architecture diagram is returned or error messages are produced.
 */
arr_channel_comm infer_communication_non_visible(wxXmlNode *p_architecture_diagram, arr_channel_comm &p_communications)
{
  arr_channel_comm comms;
  comms.Empty();

  if(p_architecture_diagram->GetName() == _T("architecturediagram"))
  {
    wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));
    wxXmlNode *objects = get_child(p_architecture_diagram, _T("objectlist"));
    if(objects == 0)
    {
      // ERROR: <architecturediagram> does not contain any objects.
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " does not contain a list of objects." << endl;
      throw CONVERSION_ERROR;
      return comms;
    }

    // get channel communications
    wxXmlNode *channel_communications = get_child(objects, _T("channelcommunicationlist"));
    if (channel_communications == 0)
    {
      // ERROR: <objectlist> has no <channelcommunicationlist>
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " does not contain any channel communications." << endl;
      throw CONVERSION_ERROR;
      return comms;
    }

    // for each channel communication in p_communications, go through the list of visibles
    // in this specification and add it to comms if there is a visible property for the
    // channel communication.
    for(wxXmlNode *channel_communication_child = channel_communications->GetChildren(); channel_communication_child != 0; channel_communication_child = channel_communication_child->GetNext())
    {
      // get channel communication type
      wxString channel_communication_type = get_child_value(channel_communication_child, _T("channelcommunicationtype"));
      if (channel_communication_type != _T("visible"))
      {
        // get communication id
        wxString channel_communication_id = get_child_value(channel_communication_child, _T("id"));
        for(unsigned int i=0; i<p_communications.GetCount(); ++i)
        {
          if(channel_communication_id == p_communications[i].m_id)
          {
            comms.Add(p_communications[i]);
            break;
          }
        }
      }
    }
  }
  else
  {
    // ERROR: not called with valid XML node
    cerr << "mCRL2 conversion error: infer_communication_visible called with an invalid XML specification." << endl;
    throw CONVERSION_ERROR;
    return comms;
  }

  return comms;
}

/**
 * Blocked channel communications removal function.
 * Removes the blocked channel communications from an array of channel communications.
 * @param p_communications The array of channel communications.
 * @param p_blocked_communications The array of blocked channel communications.
 * @return An array of channel communications that contains the channel communications in p_communications that do not occur in p_blocked_communications is returned.
 * @pre p_communications is a valid reference to an array of channel communications and p_blocked_communications is a valid reference to an array of channel communications.
 * @post An array of channel communications that contains the channel communications in p_communications that do not occur in p_blocked_communications is returned.
 */
arr_channel_comm remove_blocked_comms(arr_channel_comm &p_communications, arr_channel_comm &p_blocked_communications)
{
  arr_channel_comm comms;
  comms.Empty();

  for(unsigned int i=0; i<p_communications.GetCount(); ++i)
  {
    bool is_blocked = false;
    for(unsigned int j=0; j<p_blocked_communications.GetCount(); ++j)
    {
      if(p_communications[i].m_id == p_blocked_communications[j].m_id)
      {
        is_blocked = true;
        break;
      }
    }
    if(!is_blocked)
    {
      comms.Add(p_communications[i]);
    }
  }

  return comms;
}

/**
 * Architecture reference action fixing function.
 * Fixes actions for an architecture diagram, they need to be associated with visibles inside the actual diagram.
 * @param p_doc_root The XML GraPE specification.
 * @param p_refs The references to fix.
 * @pre p_doc_root is a valid pointer to an XML GraPE specification and p_refs is a valid reference to an array of references.
 * @post The references are fixed.
 */
void fix_architecture_references(wxXmlNode *p_doc_root, arr_action_reference &p_refs)
{
  for(unsigned int i=0; i<p_refs.GetCount(); ++i)
  {
    if(!p_refs[i].m_is_process_reference)
    {
      wxString arch_name = p_refs[i].m_reference;

      wxXmlNode *arch_diag = get_architecture_diagram(p_doc_root, arch_name);
      wxXmlNode *objects = get_child(arch_diag, _T("objectlist"));
      wxXmlNode *visibles = get_child(objects, _T("visiblelist"));

      for(unsigned int j=0; j<p_refs[i].m_renamed.GetCount(); ++j)
      {
        wxString channel_name = p_refs[i].m_renamed[j].m_channel.get_name();
        for(wxXmlNode *vis = visibles->GetChildren(); vis != 0; vis = vis->GetNext())
        {
          wxString vis_name = get_child_value(vis, _T("name"));
          wxString vis_id = get_child_value(vis, _T("id"));
          if(vis_name == channel_name)
          {
            // found match, save new name
            p_refs[i].m_renamed[j].m_architecture_channel = channel_name + vis_id;
            break;
          }
        }
      }
      for(unsigned int j=0; j<p_refs[i].m_hidden.GetCount(); ++j)
      {
        wxString channel_name = p_refs[i].m_hidden[j].get_name();
        for(wxXmlNode *vis = visibles->GetChildren(); vis != 0; vis = vis->GetNext())
        {
          wxString vis_name = get_child_value(vis, _T("name"));
          wxString vis_id = get_child_value(vis, _T("id"));
          if(vis_name == channel_name)
          {
            // found match, save new name
            p_refs[i].m_hidden[j].set_name( channel_name + vis_id );
            break;
          }
        }
      }
      for(unsigned int j=0; j<p_refs[i].m_blockeds.GetCount(); ++j)
      {
        wxString channel_name = p_refs[i].m_blockeds[j];
        for(wxXmlNode *vis = visibles->GetChildren(); vis != 0; vis = vis->GetNext())
        {
          wxString vis_name = get_child_value(vis, _T("name"));
          wxString vis_id = get_child_value(vis, _T("id"));
          if(vis_name == channel_name)
          {
            // found match, save new name
            p_refs[i].m_blockeds[j] = channel_name + vis_id;
            break;
          }
        }
      }
    }
  }
}

/**
 * Renamed channels and channel communications inference function.
 * Infers all channels and channel communications connected to the visibility frame (so they have to be renamed).
 * @param p_doc_root The root of the XML document.
 * @param p_architecture_diagram An XML architecture diagram.
 * @param p_refs An array of references containing the channels per contained reference.
 * @param p_communications An array of contained channel communications.
 * @return An array of "renameds", where each element contains both the old and new name of the renamed channel/channel-communication.
 * @pre p_architecture_diagram is a valid pointer to an XML architecture diagram, p_refs is a valid reference to an array of references and p_communications is a valid reference to an array of channel communications.
 * @post An array of "renameds" is returned or error messages are produced.
 */
arr_renamed infer_communication_channel_renamed(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, arr_action_reference &p_refs, arr_channel_comm &p_communications)
{
  arr_renamed ren;
  ren.Empty();

  if(p_architecture_diagram->GetName() == _T("architecturediagram"))
  {
    wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));
    wxXmlNode *objects = get_child(p_architecture_diagram, _T("objectlist"));
    if(objects == 0)
    {
      // ERROR: <architecturediagram> does not contain any objects.
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " does not contain a list of objects." << endl;
      throw CONVERSION_ERROR;
      return ren;
    }

    // get channels
    wxXmlNode *channels = get_child(objects, _T("channellist"));
    if (channels == 0)
    {
      // ERROR: <objectlist> has no <channellist>
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " does not contain any channels." << endl;
      throw CONVERSION_ERROR;
      return ren;
    }

    // get channel communications
    wxXmlNode *channel_communications = get_child(objects, _T("channelcommunicationlist"));
    if (channel_communications == 0)
    {
      // ERROR: <objectlist> has no <channelcommunicationlist>
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " does not contain any channel communications." << endl;
      throw CONVERSION_ERROR;
      return ren;
    }

    // for each channel in p_refs, go through the list of visibles
    // in this specification and add it to ren if there is a visible property for the
    // channel.
    for(wxXmlNode *channel_child = channels->GetChildren(); channel_child != 0; channel_child = channel_child->GetNext())
    {
      // get channel type
      wxString channel_type = get_child_value(channel_child, _T("channeltype"));
      if (channel_type == _T("visible"))
      {
        bool is_found = false;
        // get channel id
        wxString channel_id = get_child_value(channel_child, _T("id"));
        for(unsigned int i=0; i<p_refs.GetCount(); ++i)
        {
          // only renamed channels are candidates
          for(unsigned int j=0; j<p_refs[i].m_renamed.GetCount(); ++j)
          {
            if(p_refs[i].m_renamed[j].m_channel_id == channel_id)
            {
              // found match
              // get channel rename
              wxString visible_channel_name = get_child_value(channel_child, _T("rename"));
              if(visible_channel_name.IsEmpty())
              {
                visible_channel_name = get_child_value(channel_child, _T("name"));
              }
              renamed vis_ren;
              vis_ren.m_old_name = p_refs[i].m_renamed[j].m_channel.get_name() + p_refs[i].m_renamed[j].m_channel_id;
              vis_ren.m_new.set_name( visible_channel_name );
              vis_ren.m_new.set_parameters( p_refs[i].m_renamed[j].m_channel.get_parameters() );
              ren.Add(vis_ren);
              is_found = true;
              break;
            }
          }
          if(is_found)
          {
            // stop search
            break;
          }
        }
      }
    }

    // for each channel communication in p_communications, go through the list of visibles
    // in this specification and add it to ren if there is a visible property for the
    // channel communication.
    for(wxXmlNode *channel_communication_child = channel_communications->GetChildren(); channel_communication_child != 0; channel_communication_child = channel_communication_child->GetNext())
    {
      // get channel communication type
      wxString channel_communication_type = get_child_value(channel_communication_child, _T("channelcommunicationtype"));
      if (channel_communication_type == _T("visible"))
      {
        // get channel communication id
        wxString channel_communication_id = get_child_value(channel_communication_child, _T("id"));
        for(unsigned int i=0; i<p_communications.GetCount(); ++i)
        {
          if(channel_communication_id == p_communications[i].m_id)
          {
            // found match
            for (unsigned int j=0; j<p_communications[i].m_channels.GetCount(); ++j)
            {
              bool found = false;
              for (unsigned int k=j+1; k<p_communications[i].m_channels.GetCount(); ++k)
              {
                if (p_communications[i].m_channels[j].m_channel.get_parameters().GetCount() == p_communications[i].m_channels[k].m_channel.get_parameters().GetCount())
                {
                  if (p_communications[i].m_channels[j].m_channel.get_parameters().GetCount() == 0)
                  {
                    found = true;
                    break;
                  }
                  else
                  {
                    found = true;
                    for (unsigned int l=0; l<p_communications[i].m_channels[j].m_channel.get_parameters().GetCount(); ++l)
                    {
                      if (p_communications[i].m_channels[j].m_channel.get_parameters()[l].get_type() != p_communications[i].m_channels[k].m_channel.get_parameters()[l].get_type())
                      {
                        found = false;
                        break;
                      }
                    }
                    if (found)
                    {
                      break;
                    }
                  }
                }
              }
              if (!found)
              {
                // get channel communication rename
                wxString visible_channel_communication_name = get_child_value(channel_communication_child, _T("rename"));
                renamed vis_ren;
                vis_ren.m_old_name = p_communications[i].m_name;
                vis_ren.m_new.set_name( visible_channel_communication_name );
                vis_ren.m_new.set_parameters( p_communications[i].m_channels[j].m_channel.get_parameters() );
                ren.Add(vis_ren);
              }
            }
            break;
          }
        }
      }
    }
  }
/*  
  
  
  wxXmlNode *visibles = get_child(objects, _T("visiblelist"));
    if(visibles == 0)
    {
      // ERROR: <objectlist> does not contain <visiblelist>
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " does not contain a list of visible properties." << endl;
      throw CONVERSION_ERROR;
      return ren;
    }

    // for each visible property in the diagram, search for the object in p_refs and p_communications (by identifier),
    // and add the old name with the new name to ren
    for(wxXmlNode *vis_child = visibles->GetChildren(); vis_child != 0; vis_child = vis_child->GetNext())
    {
      wxString vis_name = get_child_value(vis_child, _T("name"));
      wxString vis_prop_id = get_child_value(vis_child, _T("propertyof"));
      wxString vis_id = get_child_value(vis_child, _T("id"));

      // search p_communications first
      bool is_found = false;
      for(unsigned int i=0; i<p_communications.GetCount(); ++i)
      {
        if(p_communications[i].m_id == vis_prop_id)
        {
          // found match
          for (unsigned int j=0; j<p_communications[i].m_channels.GetCount(); ++j)
          {
            bool found = false;
            for (unsigned int k=j+1; k<p_communications[i].m_channels.GetCount(); ++k)
            {
              if (p_communications[i].m_channels[j].m_channel.get_parameters().GetCount() == p_communications[i].m_channels[k].m_channel.get_parameters().GetCount())
              {
                if (p_communications[i].m_channels[j].m_channel.get_parameters().GetCount() == 0)
                {
                  found = true;
                  break;
                }
                else
                {
                  found = true;
                  for (unsigned int l=0; l<p_communications[i].m_channels[j].m_channel.get_parameters().GetCount(); ++l)
                  {
                    if (p_communications[i].m_channels[j].m_channel.get_parameters()[l].get_type() != p_communications[i].m_channels[k].m_channel.get_parameters()[l].get_type())
                    {
                      found = false;
                      break;
                    }
                  }
                  if (found)
                  {
                    break;
                  }
                }
              }
            }
            if (!found)
            {
              renamed vis_ren;
              vis_ren.m_old_name = p_communications[i].m_name;
              vis_ren.m_new.set_name( vis_name );
//              cerr << "1 action found: " << vis_name.ToAscii() << " ";
              vis_ren.m_new.set_parameters( p_communications[i].m_channels[j].m_channel.get_parameters() );
/*              for (unsigned int k=0; k<p_communications[i].m_channels[j].m_channel.get_parameters().GetCount(); ++k)
              {
                if (k>0)
                {
                  cerr << " # ";
                }
                cerr << p_communications[i].m_channels[j].m_channel.get_parameters()[k].get_type().ToAscii();
              }
              cerr << endl;

              ren.Add(vis_ren);
            }
          }
          is_found = true;
          break;
        }
      }
      if(is_found)
      {
        // continue with next visible property
        continue;
      }

      // now search p_refs
      for(unsigned int i=0; i<p_refs.GetCount(); ++i)
      {
        // only renamed channels are candidates
        for(unsigned int j=0; j<p_refs[i].m_renamed.GetCount(); ++j)
        {
          if(p_refs[i].m_renamed[j].m_channel_id == vis_prop_id)
          {
            // found match
            renamed vis_ren;
            vis_ren.m_old_name = p_refs[i].m_renamed[j].m_channel.get_name() + p_refs[i].m_renamed[j].m_channel_id;
            vis_ren.m_new.set_name( vis_name );
            vis_ren.m_new.set_parameters( p_refs[i].m_renamed[j].m_channel.get_parameters() );
            ren.Add(vis_ren);
            is_found = true;
            break;
          }
        }
        if(is_found)
        {
          // stop search
          break;
        }
      }

      if(!is_found)
      {
        // ERROR: could not find a matching channel or channel communication for this visible property!
        cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
             << " contains an item on the visibility frame with name " << vis_name.ToAscii()
             << " that does not belong to an existing channel or channel communication." << endl;
        throw CONVERSION_ERROR;
        return ren;
      }
    }
  }
*/
  else
  {
    // ERROR: not called with valid XML node
    cerr << "mCRL2 conversion error: infer_communication_channel_renamed called with an invalid XML specification." << endl;
    throw CONVERSION_ERROR;
    return ren;
  }

  return ren;
}

/**
 * Process reference initialisation inference function.
 * Infers the parameter initialisation of a process reference in an architecture diagram.
 * @param p_doc_root An XML specification.
 * @param p_architecture_diagram An XML architecture diagram.
 * @param p_reference_id The identifier of the process reference to infer the parameter initialisation of.
 * @param p_reference_name The name of the process reference to infer the parameter initialisation of.
 * @return A list containing the parameter declarations and initialisations of the inferred process reference.
 * @pre p_doc_root is a valid pointer to an XML specification, p_architecture_diagram is a valid pointer to an XML architecture diagram, p_reference_id is a valid reference to an identifier of a process reference and p_reference_name is a valid reference to a name of a process reference.
 * @post A list containing the parameter declarations and initialisations of the inferred process reference is returned or error messages are produced.
 */
list_of_varupdate infer_process_reference_initialisation(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, wxString &p_reference_id, wxString &p_reference_name, ATermAppl &datatype_spec)
{
  list_of_varupdate inits;
  inits.Empty();

  if(p_architecture_diagram->GetName() == _T("architecturediagram"))
  {
    wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));
    wxXmlNode *objects = get_child(p_architecture_diagram, _T("objectlist"));

    if(objects == 0)
    {
      // ERROR: <architecturediagram> does not contain any objects.
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " does not contain a list of objects." << endl;
      throw CONVERSION_ERROR;
      return inits;
    }

    wxXmlNode *process_references = get_child(objects, _T("processreferencelist"));
    if(process_references == 0)
    {
      // ERROR: <objectlist> does not contain <processreferencelist>
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " does not contain a list of process references." << endl;
      throw CONVERSION_ERROR;
      return inits;
    }

    // find matching process reference
    wxXmlNode *reference;
    for(reference = process_references->GetChildren(); reference != 0; reference = reference->GetNext())
    {
      wxString ref_id = get_child_value(reference, _T("id"));
      if(ref_id == p_reference_id)
      {
        break;
      }
    }
    // reference = <processreference> with matching id
    if(reference == 0)
    {
      // ERROR: reference not found
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " contains a process reference that could not be inferred." << endl;
      throw CONVERSION_ERROR;
      return inits;
    }
    

    list_of_decl preamble_params;
    list_of_decl_init preamble_vars;
    wxString proc_ref_prop = get_child_value(reference, _T("propertyof"));
    wxXmlNode *referenced_diagram = 0;
    referenced_diagram = get_process_diagram(p_doc_root, proc_ref_prop);
    if (referenced_diagram == 0) {
      // ERROR: process_reference not found
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " contains a process reference to a process that could not be inferred." << endl;
      throw CONVERSION_ERROR;
      return inits;
    }
    // parsed parameter initialisation to ref_inits
    try
    {
      parse_preamble(referenced_diagram, preamble_params, preamble_vars, datatype_spec);
    }
    catch(...)
    {
      return inits;
    }

    try
    {
      parse_reference_parameters(reference, diagram_name, inits, preamble_params, datatype_spec);
    }
    catch(...)
    {
      return inits;
    }

    if(inits.GetCount() > 0)
    {
      inits = sort_parameters(p_doc_root, p_reference_name, inits, datatype_spec);
    }

    return inits;
  }
  else
  {
    // ERROR: not called with valid XML node
    cerr << "mCRL2 conversion error: infer_process_reference_initialisation called with an invalid XML specification." << endl;
    throw CONVERSION_ERROR;
    return inits;
  }

  return inits;
}

/**
 * XML architecture diagram to mCRL2 convertor function.
 * Converts an XML architecture diagram to an mCRL2 specification.
 * @param p_doc_root An XML specification containing the architecture diagram.
 * @param p_diagram_id The identifier of the architecture diagram to convert.
 * @param p_verbose Flag to set for verbose output.
 * @param p_refs An array of references, containing the actions inside each reference in this architecture diagram.
 * @param p_renameds An array of renamed actions.
 * @param p_channel_comms An array of channel communications.
 * @return A string containing the mCRL2 specification of the architecture diagram.
 * @pre p_doc_root is a valid pointer to an XML specification, p_diagram_id is a valid reference to an identifier of a valid architecture diagram and p_refs is a valid reference to an array of references (may be empty).
 * @post A string containing the mCRL2 specification for the converted architecture diagram is returned and p_refs, p_renameds and p_channel_comms are updated according to the diagram or error messages are produced and the empty string is returned.
 */
wxString architecture_diagram_mcrl2(wxXmlNode *p_doc_root, wxString &p_diagram_id, bool p_verbose, arr_action_reference &p_refs, arr_renamed &p_renameds, arr_channel_comm &p_channel_comms, ATermAppl &datatype_spec)
{
  unsigned int comm_counter = 0;

  wxString diagram_name = infer_architecture_name(p_doc_root, p_diagram_id);
  if(p_verbose)
  {
    cerr << "+exporting architecture diagram " << diagram_name.ToAscii() << "." << endl;
  }

  wxXmlNode *diagram = get_architecture_diagram(p_doc_root, diagram_name);
  if(diagram == 0)
  {
    // ERROR: the diagram to export could not be found.
    cerr << "mCRL2 conversion error: could not find architecture diagram "
         << diagram_name.ToAscii() << " in the exported specification." << endl;
    throw CONVERSION_ERROR;
    return wxEmptyString;
  }

  // infer all possible actions in this diagram
  if(p_verbose)
  {
    cerr << "+inferred possible actions in " << diagram_name.ToAscii() << ":" << endl;
  }
  p_refs.Empty();
  architecture_diagram_mcrl2_actions(p_doc_root, diagram, p_refs, datatype_spec);
  for(unsigned int i=0; i<p_refs.GetCount(); ++i)
  {
    p_refs[i].m_actions = compact_list_action(p_refs[i].m_actions);
  }

  if(p_verbose)
  {
    for(unsigned int i=0; i<p_refs.GetCount(); ++i)
    {
      cerr << " -" << p_refs[i].m_reference.ToAscii() << ":" << endl;
      for(unsigned int j=0; j<p_refs[i].m_actions.GetCount(); ++j)
      {
        cerr << "  " << p_refs[i].m_actions[j].get_name().ToAscii();
        wxString action_text = wxEmptyString;
        for(unsigned int k=0; k<p_refs[i].m_actions[j].get_parameters().GetCount(); ++k)
        {
          if (!p_refs[i].m_actions[j].get_parameters()[k].get_type().IsEmpty())
          {
            action_text += p_refs[i].m_actions[j].get_parameters()[k].get_type();
            if (k != p_refs[i].m_actions[j].get_parameters().GetCount()-1)
            {
              action_text += _T(" # ");
            }
          }
        }
        if (!action_text.IsEmpty())
        {
          cerr << ": " << action_text.ToAscii();
        }
        cerr << endl;
	    }    
    }
  }

  // infer all blocked channels in this diagram
  if(p_verbose)
  {
    cerr << "+inferred blocked channels in " << diagram_name.ToAscii() << ":" << endl;
  }
  for(unsigned int i=0; i<p_refs.GetCount(); ++i)
  {
    if (p_refs[i].m_reference_id != _T("-1"))
    {
      p_refs[i].m_blockeds = infer_reference_blocked_channels(diagram, p_refs[i].m_reference_id);
    }
  }
  if(p_verbose)
  {
    for(unsigned int i=0; i<p_refs.GetCount(); ++i)
    {
      if (p_refs[i].m_reference_id != _T("-1"))
      {
        cerr << " -" << p_refs[i].m_reference.ToAscii() << ":" << endl;
        for(unsigned int j=0; j<p_refs[i].m_blockeds.GetCount(); ++j)
        {
          cerr << "  " << p_refs[i].m_blockeds[j].ToAscii() << endl;
        }
      }
    }
  }

  // infer all channels in this diagram
  if(p_verbose)
  {
    cerr << "+inferred channels in " << diagram_name.ToAscii() << ":" << endl;
  }
  for(unsigned int i=0; i<p_refs.GetCount(); ++i)
  {
    if (p_refs[i].m_reference_id != _T("-1"))
    {
      p_refs[i].m_channels = infer_reference_channels(p_doc_root, diagram, p_refs[i], datatype_spec);
    }
  }
  if(p_verbose)
  {
    for(unsigned int i=0; i<p_refs.GetCount(); ++i)
    {
      if (p_refs[i].m_reference_id != _T("-1"))
      {
        cerr << " -" << p_refs[i].m_reference.ToAscii() << ":" << endl;
        for(unsigned int j=0; j<p_refs[i].m_channels.GetCount(); ++j)
        {
          cerr << "  " << p_refs[i].m_channels[j].m_channel.get_name().ToAscii();
          wxString action_text = wxEmptyString;
          for(unsigned int k=0; k<p_refs[i].m_channels[j].m_channel.get_parameters().GetCount(); ++k)
          {
            if (!p_refs[i].m_channels[j].m_channel.get_parameters()[k].get_type().IsEmpty())
            {
              action_text += p_refs[i].m_channels[j].m_channel.get_parameters()[k].get_type();
              if (k != p_refs[i].m_channels[j].m_channel.get_parameters().GetCount()-1)
              {
                action_text += _T(" # ");
              }
            }
          }
          if (!action_text.IsEmpty())
          {
            cerr << ": " << action_text.ToAscii();
          }
          cerr << endl;
        }
      }
    }
  }

  // infer all hidden channels in this diagram
  if(p_verbose)
  {
    cerr << "+inferred hidden channels in " << diagram_name.ToAscii() << ":" << endl;
  }
  for(unsigned int i=0; i<p_refs.GetCount(); ++i)
  {
    if (p_refs[i].m_reference_id != _T("-1"))
    {
      p_refs[i].m_hidden = infer_reference_hidden_actions(p_refs[i].m_actions, p_refs[i].m_channels, p_refs[i].m_blockeds);
    }
  }
  if(p_verbose)
  {
    for(unsigned int i=0; i<p_refs.GetCount(); ++i)
    {
      if (p_refs[i].m_reference_id != _T("-1"))
      {
        cerr << " -" << p_refs[i].m_reference.ToAscii() << ":" << endl;
        for(unsigned int j=0; j<p_refs[i].m_hidden.GetCount(); ++j)
        {
          bool found = false;
          for (unsigned int k=j+1; k<p_refs[i].m_hidden.GetCount(); ++k)
          {
            if (p_refs[i].m_hidden[j].get_name() == p_refs[i].m_hidden[k].get_name())
            {
              found = true;
              break;
            }
          }
          if (!found)
          {
            cerr << "  " << p_refs[i].m_hidden[j].get_name().ToAscii() << endl;
          }
        }
      }
    }
  }

  // infer all renamed actions in this diagram
  if(p_verbose)
  {
    cerr << "+inferred renamed actions in " << diagram_name.ToAscii() << ":" << endl;
  }
  for(unsigned int i=0; i<p_refs.GetCount(); ++i)
  {
    if (p_refs[i].m_reference_id != _T("-1"))
    {
      p_refs[i].m_renamed = infer_reference_renamed_actions(p_refs[i].m_channels, p_refs[i].m_blockeds);
    }
  }
  if(p_verbose)
  {
    for(unsigned int i=0; i<p_refs.GetCount(); ++i)
    {
      if (p_refs[i].m_reference_id != _T("-1"))
      {
        cerr << " -" << p_refs[i].m_reference.ToAscii() << ":" << endl;
        for(unsigned int j=0; j<p_refs[i].m_renamed.GetCount(); ++j)
        {
          bool found = false;
          for (unsigned int k=j+1; k< p_refs[i].m_renamed.GetCount(); ++k)
          {
            if (p_refs[i].m_renamed[j].m_channel.get_name() == p_refs[i].m_renamed[k].m_channel.get_name())
            {
              if (p_refs[i].m_renamed[j].m_channel_id == p_refs[i].m_renamed[k].m_channel_id)
              {
                found = true;
                break;
              }
            }
          }
          if (!found)
          {
            cerr << "  " << p_refs[i].m_renamed[j].m_channel.get_name().ToAscii() << " -> "
                 << p_refs[i].m_renamed[j].m_channel.get_name().ToAscii() << p_refs[i].m_renamed[j].m_channel_id.ToAscii() << endl;
          }
        }
      }
    }
  }

  // infer all channel communications in this diagram
  if(p_verbose)
  {
    cerr << "+channel communications in " << diagram_name.ToAscii() << ":" << endl;
  }
  p_channel_comms = architecture_diagram_mcrl2_communication(p_doc_root, diagram, p_refs, datatype_spec);
  for(unsigned int i=0; i<p_channel_comms.GetCount(); ++i)
  {
    wxString comm_id;
    comm_id.Printf(_T("%d"), comm_counter++);
    p_channel_comms[i].m_name = _T("comm") + diagram_name + comm_id;
  }
  if(p_verbose)
  {
    int unique_channel_comm_counter = 0;
    for(unsigned int i=0; i<p_channel_comms.GetCount(); ++i)
    {
//      cerr << "comms: " << p_channel_comms.GetCount() << endl;
//      cerr << " ";
      for(unsigned int j=0; j<p_channel_comms[i].m_channels.GetCount(); ++j)
      {
//        cerr << "channels: " << p_channel_comms[i].m_channels.GetCount() << endl;
        bool found = false;
        for (unsigned int k=j+1; k<p_channel_comms[i].m_channels.GetCount(); ++k)
        {
/*          if (p_channel_comms[i].m_channels[j].m_channel_id == p_channel_comms[i].m_channels[k].m_channel_id)
          {
            found = true;
            break;
          }
          else
          {
*/
          if (p_channel_comms[i].m_channels[j].m_channel.get_parameters().GetCount() == p_channel_comms[i].m_channels[k].m_channel.get_parameters().GetCount())
            {
              if (p_channel_comms[i].m_channels[j].m_channel.get_parameters().GetCount() == 0)
              {
                found = true;
                break;
              }
              else
              {
                found = true;
                for(unsigned int l=0; l<p_channel_comms[i].m_channels[j].m_channel.get_parameters().GetCount(); ++l)
                {
                  if (p_channel_comms[i].m_channels[j].m_channel.get_parameters()[l].get_type() != p_channel_comms[i].m_channels[k].m_channel.get_parameters()[l].get_type())
                  {
                    found = false;
                    break;
                  }
                }
                if (found)
                {
                  break;
                }
              }
            }
//          }
        }
        if (!found)
        {
          int channel_counter = 0;
          for (unsigned int k=0; k<p_channel_comms[i].m_channels.GetCount(); ++k)
          {
            bool found2 = false;
            for (unsigned int l=k+1; l<p_channel_comms[i].m_channels.GetCount(); ++l)
            {
              if (p_channel_comms[i].m_channels[k].m_channel_id == p_channel_comms[i].m_channels[l].m_channel_id)
              {
                found2 = true;
                break;
              }
            }
            if (!found2)
            {

            if(channel_counter > 0)
              {
                cerr << "|";
              }
              cerr << p_channel_comms[i].m_channels[k].m_channel.get_name().ToAscii()
                   << p_channel_comms[i].m_channels[k].m_channel_id.ToAscii();
              ++channel_counter;
            }
          }
          cerr << " -> " << p_channel_comms[i].m_name.ToAscii();

          wxString channel_comm_type_text = wxEmptyString;
          for(unsigned int l=0; l<p_channel_comms[i].m_channels[j].m_channel.get_parameters().GetCount(); ++l)
          {
            if (!p_channel_comms[i].m_channels[j].m_channel.get_parameters()[l].get_type().IsEmpty())
            {
              channel_comm_type_text += p_channel_comms[i].m_channels[j].m_channel.get_parameters()[l].get_type();
              if (l != p_channel_comms[i].m_channels[j].m_channel.get_parameters().GetCount()-1)
              {
                channel_comm_type_text += _T(" # ");
              }
            }
          }
          if (!channel_comm_type_text.IsEmpty())
          {
            cerr << ": " << channel_comm_type_text.ToAscii();
          }
          cerr << endl;
        }
      }
    }
  }

  // infer all non-visible communications in this diagram
  if(p_verbose)
  {
    cerr << "+non-visible channel communications in " << diagram_name.ToAscii() << ":" << endl;
  }
  arr_channel_comm non_visible_channel_comms = infer_communication_non_visible(diagram, p_channel_comms);
  if(p_verbose)
  {
    for(unsigned int i=0; i<non_visible_channel_comms.GetCount(); ++i)
    {
      cerr << " " << non_visible_channel_comms[i].m_name.ToAscii() << endl;
    }
  }

  // infer all blocked communications in this diagram
  if(p_verbose)
  {
    cerr << "+blocked channel communications in " << diagram_name.ToAscii() << ":" << endl;
  }
  arr_channel_comm blocked_comms = infer_communication_blocked(diagram, p_channel_comms);
  if(p_verbose)
  {
    for(unsigned int i=0; i<blocked_comms.GetCount(); ++i)
    {
      cerr << " " << blocked_comms[i].m_name.ToAscii() << endl;
    }
  }

  // remove blocked communications from the non-visible ones
  non_visible_channel_comms = remove_blocked_comms(non_visible_channel_comms, blocked_comms);

  // infer all channels and channel communications connected to the visibility frame in this diagram
  if(p_verbose)
  {
    cerr << "+visible channels and channel communications in " << diagram_name.ToAscii() << ":" << endl;
  }
  p_renameds = infer_communication_channel_renamed(p_doc_root, diagram, p_refs, p_channel_comms);
  if(p_verbose)
  {
    for(unsigned int i=0; i<p_renameds.GetCount(); ++i)
    {
      bool found = false;
      for(unsigned int j=i+1; j<p_renameds.GetCount(); ++j)
      {
        if(p_renameds[i].m_old_name == p_renameds[j].m_old_name)
        {
          found = true;
          break;
        }
      }
      if (!found)
      {
        cerr << " " << p_renameds[i].m_old_name.ToAscii()
             << " -> " << p_renameds[i].m_new.get_name().ToAscii() << endl;
      }
    }
  }

  // construct an mCRL2 architecture diagram specification
  wxString spec;
  spec = _T("proc ") + diagram_name + _T(" =\n");
  // append visibility and communication information
  spec += _T("     ");
  if(p_renameds.GetCount() > 0)
  {
    spec += _T("rename({");
    int unique_rename_counter = 0;
    for(unsigned int i=0; i<p_renameds.GetCount(); ++i)
    {
      bool found = false;
      for(unsigned int j=i+1; j<p_renameds.GetCount(); ++j)
      {
        if(p_renameds[i].m_old_name == p_renameds[j].m_old_name)
        {
          found = true;
          break;
        }
      }
      if (!found)
      {
        if(unique_rename_counter > 0)
        {
          spec += _T(", ");
        }
        spec += p_renameds[i].m_old_name + _T(" -> ") + p_renameds[i].m_new.get_name();
        ++unique_rename_counter;
      }
    }
    spec += _T("}, ");
  }
  if(non_visible_channel_comms.GetCount() > 0)
  {
    spec += _T("hide({");
    for(unsigned int i=0; i<non_visible_channel_comms.GetCount(); ++i)
    {
      if(i > 0)
      {
        spec += _T(", ");
      }
      spec += non_visible_channel_comms[i].m_name;
    }
    spec += _T("}, ");
  }
  if(blocked_comms.GetCount() > 0)
  {
    spec += _T("block({");
    for(unsigned int i=0; i<blocked_comms.GetCount(); ++i)
    {
      if(i > 0)
      {
        spec += _T(", ");
      }
      spec += blocked_comms[i].m_name;
    }
    spec += _T("}, ");
  }
  if(p_channel_comms.GetCount() > 0)
  {
    spec += _T("block({");
    int unique_channel_counter = 0;
    for(unsigned int i=0; i<p_channel_comms.GetCount(); ++i)
    {
      for(unsigned int j=0; j<p_channel_comms[i].m_channels.GetCount(); ++j)
      {
    //    if((i > 0) || ((i == 0) && (j > 0)))
        bool found = false;
        for(unsigned int k=j+1; k<p_channel_comms[i].m_channels.GetCount(); ++k)
        {
          if (p_channel_comms[i].m_channels[j].m_channel_id == p_channel_comms[i].m_channels[k].m_channel_id)
          {
            found = true;
            break;
          }
        }
        if (!found)
        {
          if (unique_channel_counter > 0)
          {
            spec += _T(", ");
          }
          spec += p_channel_comms[i].m_channels[j].m_channel.get_name() + p_channel_comms[i].m_channels[j].m_channel_id;
          ++unique_channel_counter;
        }
      }
    }
    spec += _T("}, ");
  }
  if(p_channel_comms.GetCount() > 0)
  {
    spec += _T("comm({");
    for(unsigned int i=0; i<p_channel_comms.GetCount(); ++i)
    {
      if(i > 0)
      {
        spec += _T(", ");
      }

      int unique_channel_counter = 0;
      for(unsigned int j=0; j<p_channel_comms[i].m_channels.GetCount(); ++j)
      {
        bool found = false;
        for (unsigned int k=j+1; k<p_channel_comms[i].m_channels.GetCount(); ++k)
        {
          if (p_channel_comms[i].m_channels[j].m_channel_id == p_channel_comms[i].m_channels[k].m_channel_id)
          {
            found = true;
            break;
          }
        }
        if (!found)
        {
          if(unique_channel_counter > 0)
          {
            spec += _T("|");
          }
          spec += p_channel_comms[i].m_channels[j].m_channel.get_name() + p_channel_comms[i].m_channels[j].m_channel_id;
          ++unique_channel_counter;
        }
      }
      spec += _T(" -> ");
      spec += p_channel_comms[i].m_name;
    }
    spec += _T("}, ");
  }

  // append references
  int ref_counter = 0;
  for(unsigned int i=0; i<p_refs.GetCount(); ++i)
  {
    if (p_refs[i].m_reference != diagram_name)
    {
      if(ref_counter > 0)
      {
        spec += _T("     || ");
      }
      else
      {
        spec += _T("\n        ");
      }

      if(p_refs[i].m_renamed.GetCount() > 0)
      {
        spec += _T("rename({");
        int unique_renamed_counter = 0;
        for(unsigned int j=0; j<p_refs[i].m_renamed.GetCount(); ++j)
        {
          bool found = false;
          for (unsigned int k=j+1; k<p_refs[i].m_renamed.GetCount(); ++k)
          {
            if (p_refs[i].m_renamed[j].m_channel.get_name() == p_refs[i].m_renamed[k].m_channel.get_name())
            {
              if (p_refs[i].m_renamed[j].m_channel_id == p_refs[i].m_renamed[k].m_channel_id)
              {
                found = true;
                break;
              }
            }
          }
          if (!found)
          {
            if(unique_renamed_counter > 0)
            {
              spec += _T(", ");
            }

            spec += p_refs[i].m_renamed[j].m_channel.get_name();
            spec += _T(" -> ");
            spec += p_refs[i].m_renamed[j].m_channel.get_name() + p_refs[i].m_renamed[j].m_channel_id;
            ++unique_renamed_counter;
          }
        }
        spec += _T("}, ");
      }
      if(p_refs[i].m_hidden.GetCount() > 0)
      {
        spec += _T("hide({");
        int unique_hide_counter = 0;
        for(unsigned int j=0; j<p_refs[i].m_hidden.GetCount(); ++j)
        {
          bool found = false;
          for (unsigned int k=j+1; k<p_refs[i].m_hidden.GetCount(); ++k)
          {
            if (p_refs[i].m_hidden[j].get_name() == p_refs[i].m_hidden[k].get_name())
            {
              found = true;
              break;
            }
          }
          if (!found)
          {
            if(unique_hide_counter > 0)
            {
              spec += _T(", ");
            }
            spec += p_refs[i].m_hidden[j].get_name();
            ++unique_hide_counter;
          }
        }
        spec += _T("}, ");
      }
      if(p_refs[i].m_blockeds.GetCount() > 0)
      {
        spec += _T("block({");
        for(unsigned int j=0; j<p_refs[i].m_blockeds.GetCount(); ++j)
        {
          if(j > 0)
          {
            spec += _T(", ");
          }
          spec += p_refs[i].m_blockeds[j];
        }
        spec += _T("}, ");
      }

      // append reference specification
      spec += p_refs[i].m_reference;

      // append optional parameter initialisation
      if(p_refs[i].m_is_process_reference)
      {
        list_of_varupdate inits = infer_process_reference_initialisation(p_doc_root, diagram, p_refs[i].m_reference_id, p_refs[i].m_reference, datatype_spec);

        if(inits.GetCount() > 0)
        {
          spec += _T("(");
          for(unsigned int j=0; j<inits.GetCount(); ++j)
          {
            if(j > 0)
            {
              spec += _T(", ");
            }
            spec += inits[j].get_rhs();
          }
          spec += _T(")");
        }
      }

      // append closing brackets
      if(p_refs[i].m_blockeds.GetCount() > 0)
      {
        spec += _T(")");
      }
      if(p_refs[i].m_hidden.GetCount() > 0)
      {
        spec += _T(")");
      }
      if(p_refs[i].m_renamed.GetCount() > 0)
      {
        spec += _T(")");
      }
      spec += _T("\n");
      
      ++ref_counter;
    }
  }

  // append closing brackets
  spec += _T("     ");
  if(p_channel_comms.GetCount() > 0)
  {
    spec += _T(")");
  }
  if(p_channel_comms.GetCount() > 0)
  {
    spec += _T(")");
  }
  if(blocked_comms.GetCount() > 0)
  {
    spec += _T(")");
  }
  if(non_visible_channel_comms.GetCount() > 0)
  {
    spec += _T(")");
  }
  if(p_renameds.GetCount() > 0)
  {
    spec += _T(")");
  }
  spec += _T(";");

  if(p_verbose)
  {
    cerr << "+process specification of " << diagram_name.ToAscii() << ":" << endl;
    cerr << spec.ToAscii() << endl;
  }

  return spec;
}

void grape::mcrl2gen::test_export(void)
{
  wxXmlDocument doc;
  doc.Load(_T("testprocessPQAB.xml"));
  if(!doc.IsOk())
  {
    cerr << "loading failed." << endl;
    return;
  }

  wxString diag_id = _T("50");
  wxString fname = _T("test.mcrl2");
  export_architecture_diagram_to_mcrl2(doc, fname, diag_id, true);
}

/**
 * XML process diagram to mCRL2 convertor function.
 * Converts an XML process diagram to an mCRL2 specification.
 * @param p_doc_root The XML specification containing the diagram to be exported.
 * @param p_diagram_id The identifier of the diagram to be exported.
 * @param p_sort_expressions An array containing the mCRL2 sort expressions constructed during the convertion process.
 * @param p_actions An array containing the mCRL2 actions constructed during the convertion process.
 * @param p_internal_specs An array containing the internal mCRL2 specifications constructed during the convertion process.
 * @param p_specs An array containing the mCRL2 specifications constructed during the convertion process.
 * @param p_verbose Flag to be set for verbose output.
 * @param datatype_spec The datatype specification.
 * @pre p_doc_root is a valid pointer to an XML specification, p_diagram_id is a valid reference to an identifier of a valid process diagram, p_sort_expressions is a valid reference to an array of sort expressions, p_actions is a valid reference to an array of actions, p_internal_specs is a valid reference to an array of internal specifications and p_specs is a valid reference to an array of specifications.
 * @post The XML process diagram is converted to mCRL2 and p_sort_expressions, p_actions, p_internal_specs and p_specs are updated accordingly or error messages are produced.
 */
void process_diagram_mcrl2(wxXmlNode *p_doc_root, wxString &p_diagram_id, wxArrayString &p_sort_expressions, list_of_action &p_actions, wxArrayString &p_internal_specs, wxArrayString &p_specs, bool p_verbose, ATermAppl &datatype_spec)
{
  wxString diagram_name = infer_process_diagram_name(p_doc_root, p_diagram_id);
  if(p_verbose)
  {
    cerr << "+exporting process diagram " << diagram_name.ToAscii() << "." << endl;
  }
  wxXmlNode *diagram = get_process_diagram(p_doc_root, p_diagram_id);
  if(diagram == 0)
  {
    // ERROR: the diagram to export could not be found.
    cerr << "mCRL2 conversion error: could not find process diagram "
         << diagram_name.ToAscii() << " in the exported specification." << endl;
    throw CONVERSION_ERROR;
  }

  // get diagrams preamble
  list_of_decl preamble_parameter_decls;
  list_of_decl_init preamble_local_var_decls;
  parse_preamble(diagram, preamble_parameter_decls, preamble_local_var_decls, datatype_spec);
  if(p_verbose)
  {
    cerr << "+preamble:" << endl;
    for(unsigned int i=0; i<preamble_parameter_decls.GetCount(); ++i)
    {
      cerr << " " << preamble_parameter_decls[i].get_name().ToAscii() << ": "
           << preamble_parameter_decls[i].get_type().ToAscii() << endl;
    }
    for(unsigned int i=0; i<preamble_local_var_decls.GetCount(); ++i)
    {
      cerr << " " << preamble_local_var_decls[i].get_name().ToAscii() << ": "
           << preamble_local_var_decls[i].get_type().ToAscii() << " = "
           << preamble_local_var_decls[i].get_value().ToAscii() << endl;
    }
  }

  // determine occuring actions
  list_of_action acts = process_diagram_mcrl2_action(diagram, preamble_parameter_decls, preamble_local_var_decls, datatype_spec);
  if(p_verbose)
  {
    cerr << "+actions:" << endl;
    for(unsigned int i=0; i<acts.GetCount(); ++i)
    {
      cerr << " " << acts[i].get_name().ToAscii();
      wxString action_text = wxEmptyString;
      for(unsigned int j=0; j<acts[i].get_parameters().GetCount(); ++j)
      {
        if (!acts[i].get_parameters()[j].get_type().IsEmpty())
        {
          action_text += acts[i].get_parameters()[j].get_type();
          if (j != acts[i].get_parameters().GetCount()-1)
          {
            action_text += _T(" # ");
          }
        }
      }
      if (!action_text.IsEmpty())
      {
        cerr << ": " << action_text.ToAscii();
      }
      cerr << endl;
	  }
  }
  for(unsigned int i=0; i<acts.GetCount(); ++i)
  {
    bool found = false;
    for(unsigned int j=0; j<p_actions.GetCount(); ++j)
    {
	    if ( acts[i].get_name() == p_actions[j].get_name() )
	    {
	      if ( acts[i].get_parameters().GetCount() == p_actions[j].get_parameters().GetCount() )
	      {
		      if ( acts[i].get_parameters().GetCount() == 0 )
		      {
		        found = true;
            break;
		      }
		      else
		      {
		        list_of_dataexpression acts_params = acts[i].get_parameters();
		        list_of_dataexpression actions_params = p_actions[j].get_parameters();
            found = true;
            unsigned int k = 0;
            while ( found && k < acts_params.GetCount() )
		        {
              found = acts_params[k].get_type() == actions_params[k].get_type();
              ++k;
		        }
            if (found)
            {
              break;
            }
		      }
	      }
      }
    }
    if (!found)
    {
	    p_actions.Add(acts[i]);
    }
  }

  // construct state sort expression
  wxString diagram_sort = process_diagram_mcrl2_sort(diagram);
  if(p_verbose)
  {
    cerr << "+state sort expression:" << endl << " "
         << diagram_sort.ToAscii() << endl;
  }
  p_sort_expressions.Add(diagram_sort);

  // construct internal process specification
  wxString spec_internal = process_diagram_mcrl2_internal_proc(p_doc_root, diagram, preamble_parameter_decls, preamble_local_var_decls, datatype_spec);
  if(p_verbose)
  {
    cerr << "+internal process specification:" << endl << " "
         << spec_internal.ToAscii() << endl;
  }
  p_internal_specs.Add(spec_internal);

  // construct process specification
  wxString spec = process_diagram_mcrl2_proc(diagram, preamble_parameter_decls, preamble_local_var_decls);
  if(p_verbose)
  {
    cerr << "+process specification:" << endl << " "
         << spec.ToAscii() << endl;
  }
  p_specs.Add(spec);
}

void grape::mcrl2gen::init_mcrl2libs(int p_argc, char** p_argv)
{
  // initialize ATerm library and set verbose messaging
  MCRL2_ATERM_INIT(p_argc, p_argv);
  gsSetVerboseMsg();
}

bool grape::mcrl2gen::export_process_diagram_to_mcrl2(wxXmlDocument &p_spec, wxString &p_filename, wxString &p_diagram_id, list_of_decl_init &p_parameters_init, bool p_verbose, bool p_save)
{
  try
  {
    list_of_action actions;                     // all occuring actions
    wxXmlNode       *doc_root;                  // stores the root node of the document
    wxArrayString   sort_expressions;           // stores all state sort expressions
    wxArrayString   internal_specs;             // stores all internal process specifications
    wxArrayString   specs;                      // stores all process specifications

    // Stores all process diagrams that need to be added to the mCRL2 specification.
    wxArrayString to_mcrl2, in_mcrl2;
    to_mcrl2.Add(p_diagram_id);
    in_mcrl2.Empty();

    doc_root = p_spec.GetRoot();

    ATermAppl datatype_spec;
    validate_datatype_specification(doc_root, datatype_spec);

    sort_expressions.Empty();
    internal_specs.Empty();
    specs.Empty();

    while(!to_mcrl2.IsEmpty())
    {
      wxString id = to_mcrl2[0];
      to_mcrl2.RemoveAt(0);
      if(in_mcrl2.Index(id) != wxNOT_FOUND)
      {
        // diagram is already exported
        continue;
      }
      in_mcrl2.Add(id, 1);

      // assure specification is valid
      if (!validate_process_diagram(p_spec, id))
      {
        throw CONVERSION_ERROR;
      }

      // Apply steps of algorithm on XML data.
      process_diagram_mcrl2(doc_root, id, sort_expressions, actions, internal_specs, specs, p_verbose, datatype_spec);
      wxXmlNode *diagram = get_process_diagram(doc_root, id);

      // determine recursive process references
      wxArrayString references = infer_process_references(diagram);
      for(unsigned int i=0; i<references.GetCount(); ++i)
      {
        bool found = false;
        for(unsigned int j=0; j<in_mcrl2.GetCount(); ++j)
        {
          if(in_mcrl2[j] == references[i])
          {
            found = true;
          }
        }

        if(!found)
        {
          to_mcrl2.Add(references[i]);
        }
      }
    }

    // construct general expressions
    wxString dat_spec = datatype_specification_mcrl2(doc_root);
    wxString exp_diagram_name = infer_process_diagram_name(doc_root, p_diagram_id);
    wxString init_spec = process_diagram_mcrl2_init(doc_root, exp_diagram_name, p_parameters_init, datatype_spec);
    if(p_verbose)
    {
      cerr << "+initial process specification:" << endl << " "
           << init_spec.ToAscii() << endl;
    }

    // construct mCRL2 specification
    wxString specification = wxEmptyString;
    specification += dat_spec + _T("\n\n");
    for(unsigned int i=0; i<sort_expressions.GetCount(); ++i)
    {
      specification += sort_expressions[i];
    }
    specification += _T("\n");
    if(actions.GetCount() > 0)
    {
      specification += _T("act \n");
    }
    for(unsigned int i=0; i<actions.GetCount(); ++i)
    {
      specification += _T(" ");
      specification += actions[i].get_name();
      wxString action_text = wxEmptyString;
      if(actions[i].get_parameters().GetCount() != 0)
      {
        for(unsigned int j=0; j<actions[i].get_parameters().GetCount(); ++j)
        {
          if (!actions[i].get_parameters()[j].get_type().IsEmpty())
          {
            action_text += actions[i].get_parameters()[j].get_type();
            if (j != actions[i].get_parameters().GetCount()-1)
            {
              action_text += _T(" # ");
            }
          }
        }
      }
      if (!action_text.IsEmpty())
      {
        specification += _T(": ") + action_text;
      }
      specification += _T(";\n");
    }
    specification += _T("\n");
    for(int i=internal_specs.GetCount()-1; i>-1; --i)
    {
      specification += internal_specs[i] + _T("\n");
      specification += specs[i] + _T("\n\n");
    }
    specification += init_spec;

    cerr << "+spec:" << endl;
    cerr << specification.ToAscii() << endl;

    // try to parse constructed mCRL2 specification
    string mcrl2_specification = string(specification.mb_str());
    const char *m_spec = mcrl2_specification.c_str();
    istringstream iss(m_spec);
    ATermAppl a_parsed_mcrl2_spec = parse_proc_spec(iss);
    if(a_parsed_mcrl2_spec == NULL)
    {
      // ERROR: failed to parse constructed spec
      cerr << "mCRL2 conversion error: could not parse exported specification." << endl;
      throw CONVERSION_ERROR;
    }
    ATermAppl a_typed_mcrl2_spec = type_check_proc_spec(a_parsed_mcrl2_spec);
    if(a_typed_mcrl2_spec == NULL)
    {
      // ERROR: failed to type check constructed spec
      cerr << "mCRL2 conversion error: exported specification is not well-typed." << endl;
      throw CONVERSION_ERROR;
    }

    if(p_save)
    {
      wxFile file_out;
      file_out.Open(p_filename, wxFile::write);
      if(!file_out.IsOpened())
      {
        // ERROR: could not open file to write to
        cerr << "mCRL2 conversion error: could not open file "
             << p_filename.ToAscii() << "." << endl;
        throw CONVERSION_ERROR;
      }
      file_out.Write(specification);
      file_out.Close();
    }
  }
  catch(...)
  {
    cerr << "+mCRL2 conversion failed." << endl;
    return false;
  }

  cerr << "+mCRL2 conversion successful." << endl;
  return true;
}

/**
 * Process diagram inference function.
 * Infers whether a given diagram identifier belongs to a process diagram (or architecture diagram).
 * @param p_doc_root A valid XML specification containing the diagrams.
 * @param p_diagram_id The identifier to infer whether it belongs to a process diagram.
 * @return True if the identifier belongs to a process diagram and false otherwise.
 * @pre p_doc_root is a valid pointer to a valid XML specification and p_diagram_id is a valid reference to a diagram identifier.
 * @post A flag indicating whether the identifier belongs to a process diagram is returned or error messages are produced.
 */
bool is_process_diagram(wxXmlNode *p_doc_root, wxString &p_diagram_id)
{
  wxXmlNode *process_diagrams = get_child(p_doc_root, _T("processdiagramlist"));
  if(process_diagrams == 0)
  {
    // ERROR: no process diagrams contained in specification
    cerr << "mCRL2 conversion error: the converted specification does not contain a list of process diagrams." << endl;
    throw CONVERSION_ERROR;
    return false;
  }

  for(wxXmlNode *diagram = process_diagrams->GetChildren(); diagram != 0; diagram = diagram->GetNext())
  {
    wxString diagram_id = get_child_value(diagram, _T("id"));
    if(diagram_id == p_diagram_id)
    {
      // found match, must be a process diagram
      return true;
    }
  }

  // could not find match, assume it is an architecture diagram
  return false;
}

/**
 * XML architecture diagram to mCRL2 initial process specification convertor function.
 * Constructs an mCRL2 initial process specification for an architecture diagram.
 * @param p_diagram_name The name of the architecture diagram to construct an initial process specification for.
 * @return The mCRL2 initial process specification.
 * @pre p_diagram_name is a valid reference to an XML architecture diagram name.
 * @post The mCRL2 initial process specification is returned.
 */
wxString architecture_diagram_mcrl2_init(wxString &p_diagram_name)
{
  wxString init = _T("init ") + p_diagram_name + _T(";\n");
  return init;
}

bool grape::mcrl2gen::export_architecture_diagram_to_mcrl2(wxXmlDocument &p_spec, wxString &p_filename, wxString &p_diagram_id, bool p_verbose, bool p_save)
{
  try
  {
    // an architecture diagram can contain both architecture references and process references,
    // this has to be taken into account in the main loop

    list_of_action  actions;                    // all occuring actions
    wxXmlNode       *doc_root;                  // stores the root node of the document
    wxArrayString   sort_expressions;           // stores all state sort expressions
    wxArrayString   internal_specs;             // stores all internal process specifications
    wxArrayString   specs;                      // stores all process specifications
    wxArrayString   arch_specs;                 // stores all architecture diagram process specifications

    // stores all diagrams that need to be added to the mCRL2 specification.
    // make difference between arch and proc
    wxArrayString arch_to_mcrl2, proc_to_mcrl2, arch_in_mcrl2, proc_in_mcrl2;
    arch_to_mcrl2.Add(p_diagram_id);
    proc_to_mcrl2.Empty();
    arch_in_mcrl2.Empty();
    proc_in_mcrl2.Empty();

    doc_root = p_spec.GetRoot();

    ATermAppl datatype_spec;
    validate_datatype_specification(doc_root, datatype_spec);

    sort_expressions.Empty();
    internal_specs.Empty();
    specs.Empty();
    arch_specs.Empty();
    // collect all needed archs
    while(!arch_to_mcrl2.IsEmpty())
    {
      wxString arch_id = arch_to_mcrl2[0];
      arch_to_mcrl2.RemoveAt(0);
      arch_in_mcrl2.Add(arch_id);
      
      wxString arch_name = infer_architecture_name(doc_root, arch_id);
      wxXmlNode *diagram = get_architecture_diagram(doc_root, arch_name);
      wxArrayString proc_refs = infer_process_references(diagram);
      for(unsigned int i=0; i<proc_refs.GetCount(); ++i)
      {
        bool found = false;
        for(unsigned int j=0; j<proc_to_mcrl2.GetCount(); ++j)
        {
          if(proc_to_mcrl2[j] == proc_refs[i])
          {
            found = true;
            break;
          }
        }

        if(!found)
        {
          proc_to_mcrl2.Add(proc_refs[i]);
        }
      }

      wxArrayString arch_refs = infer_architecture_references(diagram);
      for(unsigned int i=0; i<arch_refs.GetCount(); ++i)
      {
        bool found = false;
        for(unsigned int j=0; j<arch_in_mcrl2.GetCount(); ++j)
        {
          if(arch_in_mcrl2[j] == arch_refs[i])
          {
            found = true;
            break;
          }
        }

        if(!found)
        {
          arch_to_mcrl2.Add(arch_refs[i]);
        }
      }
    }

    // collect all needed procs and process them
    while(!proc_to_mcrl2.IsEmpty())
    {
      wxString proc_id = proc_to_mcrl2[0];
      proc_to_mcrl2.RemoveAt(0);
      proc_in_mcrl2.Add(proc_id);

      // assure specification is valid
      if (!validate_process_diagram(p_spec, proc_id))
      {
        throw CONVERSION_ERROR;
      }
      // make proc_specification
      process_diagram_mcrl2(doc_root, proc_id, sort_expressions, actions, internal_specs, specs, p_verbose, datatype_spec);
      wxXmlNode *diagram = get_process_diagram(doc_root, proc_id);
      wxArrayString proc_refs = infer_process_references(diagram);
      for(unsigned int i=0; i<proc_refs.GetCount(); ++i)
      {
        bool found = false;
        for(unsigned int j=0; j<proc_in_mcrl2.GetCount(); ++j)
        {
          if(proc_in_mcrl2[j] == proc_refs[i])
          {
            found = true;
            break;
          }
        }

        if(!found)
        {
          proc_to_mcrl2.Add(proc_refs[i]);
        }
      }
    }
    arch_to_mcrl2 = arch_in_mcrl2;
    arch_in_mcrl2.Empty();

//TODO: @arcs: first collect actions from proc_refs (done above), then add type to channels (possible channel actions), then collect blocked, hidden, renamed channels
//             then collect channelcommunications + add type to chan_comms, then collect blocked, hidden, renamed chan_coms
//             make arch specification

    // loop through archs 
    while(!arch_to_mcrl2.IsEmpty())
    {
      wxString arch_id = arch_to_mcrl2[0];
      arch_to_mcrl2.RemoveAt(0);
      arch_in_mcrl2.Add(arch_id);

      // assure specification is valid
      if (!validate_architecture_diagram(p_spec, arch_id))
      {
        throw CONVERSION_ERROR;
      }

      wxString arch_name = infer_architecture_name(doc_root, arch_id);
      wxXmlNode *diagram = get_architecture_diagram(doc_root, arch_name);
      wxArrayString proc_refs = infer_process_references(diagram);

      // export architecture diagram
      arr_action_reference refs;
      arr_renamed renameds;
      arr_channel_comm comms;
      wxString arch_spec = architecture_diagram_mcrl2(doc_root, arch_id, p_verbose, refs, renameds, comms, datatype_spec);
      arch_specs.Add(arch_spec);

      // add renamed new actions to actions
      for(unsigned int i=0; i<renameds.GetCount(); ++i)
      {
        action ren;
        ren.set_name(renameds[i].m_new.get_name());
//        cerr << "action added: " << renameds[i].m_new.get_name().ToAscii() << " ";
        ren.set_parameters(renameds[i].m_new.get_parameters());
/*        for (unsigned int j=0; j<renameds[i].m_new.get_parameters().GetCount(); ++j)
        {
          if (j > 0) 
          {
            cerr << " # ";
          }
          cerr << renameds[i].m_new.get_parameters()[j].get_type().ToAscii();
        }
        cerr << endl;
*/
        actions.Add(ren);
      }
      // add channels
      for(unsigned int i=0; i<refs.GetCount(); ++i)
      {
        for(unsigned int j=0; j<refs[i].m_renamed.GetCount(); ++j)
        {
          wxString ren = refs[i].m_renamed[j].m_channel.get_name() + refs[i].m_renamed[j].m_channel_id;
          action ren_decl;
          ren_decl.set_name( ren );
//          cerr << "channel action added: " << ren.ToAscii() << " ";
          ren_decl.set_parameters( refs[i].m_renamed[j].m_channel.get_parameters() );
/*          for (unsigned int k=0; k<refs[i].m_renamed[j].m_channel.get_parameters().GetCount(); ++k)
          {
            if (k > 0)
            {
              cerr << " # ";
            }
            cerr << refs[i].m_renamed[j].m_channel.get_parameters()[k].get_type().ToAscii();
          }
          cerr << endl;
*/
          actions.Add(ren_decl);
        }
      }
      // add channel communications
      for(unsigned int i=0; i<comms.GetCount(); ++i)
      {
        for (unsigned int j=0; j<comms[i].m_channels.GetCount(); ++j)
        {
          bool found = false;
          for (unsigned int k=j+1; k<comms[i].m_channels.GetCount(); ++k)
          {
            if (comms[i].m_channels[j].m_channel.get_parameters().GetCount() == comms[i].m_channels[k].m_channel.get_parameters().GetCount())
            {
              if (comms[i].m_channels[j].m_channel.get_parameters().GetCount() == 0)
              {
                found = true;
                break;
              }
              else
              {
                for (unsigned int l=0; l<comms[i].m_channels[j].m_channel.get_parameters().GetCount(); ++l)
                {
                  found = true;
                  if (comms[i].m_channels[j].m_channel.get_parameters()[l].get_type() != comms[i].m_channels[k].m_channel.get_parameters()[l].get_type())
                  {
                    found = false;
                    break;
                  }
                }
                if (found)
                {
                  break;
                }
              }
            }
          }
          if (!found)
          {
            action ren;
            ren.set_name( comms[i].m_name );
            ren.set_parameters( comms[i].m_channels[j].m_channel.get_parameters() );
            actions.Add(ren);
          }
        }
      }
    }

    // remove duplicate actions
    actions = compact_list_action(actions);

    // construct general expressions
    wxString dat_spec = datatype_specification_mcrl2(doc_root);
    wxString exp_diagram_name = infer_architecture_name(doc_root, p_diagram_id);
    wxString init_spec = architecture_diagram_mcrl2_init(exp_diagram_name);
    if(p_verbose)
    {
      cerr << "+initial process specification:" << endl << " "
           << init_spec.ToAscii() << endl;
    }

    // construct mCRL2 specification
    wxString specification = wxEmptyString;
    specification += dat_spec + _T("\n\n");
    for(unsigned int i=0; i<sort_expressions.GetCount(); ++i)
    {
      specification += sort_expressions[i];
    }
    specification += _T("\n");
    if(actions.GetCount() > 0)
    {
      specification += _T("act \n");
    }
    for(unsigned int i=0; i<actions.GetCount(); ++i)
    {
      specification += _T(" ");
      specification += actions[i].get_name();
      wxString action_text = wxEmptyString;
      if(actions[i].get_parameters().GetCount() != 0)
      {
        for(unsigned int j=0; j<actions[i].get_parameters().GetCount(); ++j)
        {
          if (!actions[i].get_parameters()[j].get_type().IsEmpty())
          {
            action_text += actions[i].get_parameters()[j].get_type();
            if (j != actions[i].get_parameters().GetCount()-1)
            {
              action_text += _T(" # ");
            }
          }
        }
      }
      if (!action_text.IsEmpty())
      {
        specification += _T(": ") + action_text;
      }
      specification += _T(";\n");
    }
    specification += _T("\n");
    for(int i=internal_specs.GetCount()-1; i>-1; --i)
    {
      specification += internal_specs[i] + _T("\n");
      specification += specs[i] + _T("\n\n");
    }
    for(int i=arch_specs.GetCount()-1; i>-1; --i)
    {
      specification += arch_specs[i] + _T("\n\n");
    }
    specification += init_spec;

    cerr << "+spec:" << endl;
    cerr << specification.ToAscii() << endl;

    // try to parse constructed mCRL2 specification
    string mcrl2_specification = string(specification.mb_str());
    const char *m_spec = mcrl2_specification.c_str();
    istringstream iss(m_spec);
    ATermAppl a_parsed_mcrl2_spec = parse_proc_spec(iss);
    if(a_parsed_mcrl2_spec == NULL)
    {
      // ERROR: failed to parse constructed spec
      cerr << "mCRL2 conversion error: could not parse exported specification." << endl;
      throw CONVERSION_ERROR;
    }
    ATermAppl a_typed_mcrl2_spec = type_check_proc_spec(a_parsed_mcrl2_spec);
    if(a_typed_mcrl2_spec == NULL)
    {
      // ERROR: failed to type check constructed spec
      cerr << "mCRL2 conversion error: exported specification is not well-typed." << endl;
      throw CONVERSION_ERROR;
    }

    if(p_save)
    {
      wxFile file_out;
      file_out.Open(p_filename, wxFile::write);
      if(!file_out.IsOpened())
      {
        // ERROR: could not open file to write to
        cerr << "mCRL2 conversion error: could not open file "
             << p_filename.ToAscii() << "." << endl;
        throw CONVERSION_ERROR;
      }
      file_out.Write(specification);
      file_out.Close();
    }
  }
  catch(...)
  {
    cerr << "+mCRL2 conversion failed." << endl;
    return false;
  }

  cerr << "+mCRL2 conversion successful." << endl;
  return true;
}

void convert_spaces_node(wxXmlNode *p_node)
{
  wxString name = p_node->GetNodeContent();
  name.Replace(_T(" "), _T("_"));
  wxXmlNode *val = p_node->GetChildren();
  //delete val;
  val = new wxXmlNode(p_node, wxXML_TEXT_NODE, _T("value"), name);
}

void grape::mcrl2gen::convert_spaces(wxXmlDocument &p_spec)
{
  wxXmlNode *root_node = p_spec.GetRoot();
  wxString object_name;

  wxXmlNode *procs = get_child(root_node, _T("processdiagramlist"));
  if(procs != 0)
  {
    for(wxXmlNode *diagram = procs->GetChildren(); diagram != 0; diagram = diagram->GetNext())
    {
      convert_spaces_node(get_child(diagram, _T("name")));

      wxXmlNode *objects = get_child(diagram, _T("objectlist"));
      if(objects != 0)
      {
        wxXmlNode *ref_states = get_child(objects, _T("referencestatelist"));
        if(ref_states != 0)
        {
          for(wxXmlNode *ref_state = ref_states->GetChildren(); ref_state != 0; ref_state = ref_state->GetNext())
          {
            convert_spaces_node(get_child(ref_state, _T("name")));
          }
        }
        wxXmlNode *states = get_child(objects, _T("statelist"));
        if(states != 0)
        {
          for(wxXmlNode *state = states->GetChildren(); state != 0; state = state->GetNext())
          {
            convert_spaces_node(get_child(state, _T("name")));
          }
        }
      }
    }
  }

  wxXmlNode *archs = get_child(root_node, _T("architecturediagramlist"));
  if(archs != 0)
  {
    for(wxXmlNode *diagram = archs->GetChildren(); diagram != 0; diagram = diagram->GetNext())
    {
      convert_spaces_node(get_child(diagram, _T("name")));

      wxXmlNode *objects = get_child(diagram, _T("objectlist"));
      if(objects != 0)
      {
        wxXmlNode *visibles = get_child(objects, _T("visiblelist"));
        if(visibles != 0)
        {
          for(wxXmlNode *visible = visibles->GetChildren(); visible != 0; visible = visible->GetNext())
          {
            convert_spaces_node(get_child(visible, _T("name")));
          }
        }
        wxXmlNode *channels = get_child(objects, _T("channellist"));
        if(channels != 0)
        {
          for(wxXmlNode *channel = channels->GetChildren(); channel != 0; channel = channel->GetNext())
          {
            convert_spaces_node(get_child(channel, _T("name")));
          }
        }
      }
    }

  }
}

