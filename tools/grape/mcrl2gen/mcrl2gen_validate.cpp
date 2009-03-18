// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2gen_validate.cpp
//
// Implements internal functions used when validating process diagrams and architecture diagrams.

#include <sstream>
#include <string>

#include "mcrl2gen.h"
#include "mcrl2gen_validate.h"
#include "libgrape/label.h"
#include "libgrape/preamble.h"
#include "libgrape/decl.h"

#include <aterm2.h>
#include "mcrl2/atermpp/table.h"
#include "mcrl2/atermpp/aterm_list.h"

// mCRL2 core libraries
#include "mcrl2/core/detail/struct.h"               // ATerm building blocks.
#include "mcrl2/core/messaging.h"                   // Library for messaging.
#include "mcrl2/core/print.h"

// mCRL2 core/detail libraries
#include "mcrl2/core/parse.h"                // Parse library.
#include "mcrl2/core/typecheck.h"            // Type check library.
#include "mcrl2/core/data_implementation.h"

// mCRL2 utility libraries
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/utilities/sort_info.h"

using namespace grape::mcrl2gen;
using namespace grape::libgrape;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2;
using namespace std;

// Dynamic array implementations
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(arr_channel_id);
WX_DEFINE_OBJARRAY(arr_action_type);
WX_DEFINE_OBJARRAY(arr_action_reference);
WX_DEFINE_OBJARRAY(arr_channel_comm);
WX_DEFINE_OBJARRAY(arr_renamed);

wxXmlNode *grape::mcrl2gen::get_child(wxXmlNode *p_parent, wxString p_child_name)
{
  wxXmlNode *child;
  for(child = p_parent->GetChildren(); child != 0; child = child->GetNext())
  {
    if(child->GetName() == p_child_name)
    {
      break;
    }
  }
  if(child == 0)
  {
    // WARNING: p_parent does not contain <child_name>
    return 0;
  }
  return child;
}

wxString grape::mcrl2gen::get_child_value(wxXmlNode *p_parent, wxString p_child_name)
{
  wxXmlNode *child = get_child(p_parent, p_child_name);
  if(child != 0)
  {
    return child->GetNodeContent();
  }
  else
  {
    return wxEmptyString;
  }
}

wxString grape::mcrl2gen::infer_process_diagram_name(wxXmlNode *p_doc_root, wxString &p_diagram_id)
{
  if(p_doc_root->GetName() == _T("grape"))
  {
    wxXmlNode *diag_list = get_child(p_doc_root, _T("processdiagramlist"));
    if(diag_list == 0)
    {
      // ERROR: <grape> does not contain <processdiagramlist>
      cerr << "mCRL2 conversion error: the specification does not contain any process diagrams." << endl;
      throw CONVERSION_ERROR;
      return wxEmptyString;
    }
    // check all process diagrams
    for(wxXmlNode *diag = diag_list->GetChildren(); diag != 0; diag = diag->GetNext())
    {
      if(p_diagram_id == get_child_value(diag, _T("id")))
      {
        // found matching diagram
        return get_child_value(diag, _T("name"));
      }
    }
  }
  else
  {
    // ERROR: called without valid XML document root
    cerr << "mCRL2 conversion error: infer_process_diagram_name called without a valid XML specification." << endl;
    throw CONVERSION_ERROR;
    return wxEmptyString;
  }

  // no matching diagram found
  return wxEmptyString;
}

wxString grape::mcrl2gen::infer_process_diagram_id(wxXmlNode *p_doc_root, wxString &p_diagram_name)
{
  if(p_doc_root->GetName() == _T("grape"))
  {
    wxXmlNode *diag_list = get_child(p_doc_root, _T("processdiagramlist"));
    if(diag_list == 0)
    {
      // ERROR: <grape> does not contain a list of process diagrams
      cerr << "mCRL2 conversion error: the specification does not contain a list of process diagrams." << endl;
      throw CONVERSION_ERROR;
      return wxEmptyString;
    }
    // check all process diagrams
    for(wxXmlNode *diag = diag_list->GetChildren(); diag != 0; diag = diag->GetNext())
    {
      if(p_diagram_name == get_child_value(diag, _T("name")))
      {
        // found matching diagram
        return get_child_value(diag, _T("id"));
      }
    }
  }
  else
  {
    // ERROR: called without valid XML document root
    cerr << "mCRL2 conversion error: infer_process_diagram_id called without a valid XML specification." << endl;
    throw CONVERSION_ERROR;
    return wxEmptyString;
  }

  // no matching diagram found
  return wxEmptyString;
}

wxXmlNode *grape::mcrl2gen::get_process_diagram(wxXmlNode *p_doc_root, wxString &p_diagram_id)
{
  if(p_doc_root->GetName() == _T("grape"))
  {
    wxXmlNode *diag_list = get_child(p_doc_root, _T("processdiagramlist"));
    if(diag_list == 0)
    {
      // ERROR: <grape> does not contain <processdiagramlist>
      cerr << "mCRL2 conversion error: the specification does not contain any process diagrams." << endl;
      throw CONVERSION_ERROR;
      return 0;
    }
    // check all process diagrams
    for(wxXmlNode *diag = diag_list->GetChildren(); diag != 0; diag = diag->GetNext())
    {
      if(p_diagram_id == get_child_value(diag, _T("id")))
      {
        // found matching diagram
        return diag;
      }
    }
  }
  else
  {
    // ERROR: called without valid XML document root
    cerr << "mCRL2 conversion error: get_process_diagram called without a valid XML specification." << endl;
    throw CONVERSION_ERROR;
    return 0;
  }

  // no matching diagram found
  return 0;
}

wxString grape::mcrl2gen::infer_architecture_name(wxXmlNode *p_doc_root, wxString &p_id)
{
  if(p_doc_root->GetName() == _T("grape"))
  {
    wxXmlNode *arch_list = get_child(p_doc_root, _T("architecturediagramlist"));
    if(arch_list == 0)
    {
      // error: no architecture diagrams
      cerr << "mCRL2 conversion error: no architecture diagram found for architecture reference to identifier "
           << p_id.ToAscii() << "." << endl;
      throw CONVERSION_ERROR;
      return wxEmptyString;
    }

    for(wxXmlNode *arch = arch_list->GetChildren(); arch != 0; arch = arch->GetNext())
    {
      if(get_child_value(arch, _T("id")) == p_id)
      {
        // found matching diagram, return it
        return get_child_value(arch, _T("name"));
      }
    }
    // nothing found, return empty string
    return wxEmptyString;
  }

  // error: not called with valid XML root
  cerr << "mCRL2 conversion error: infer_architecture_name called with an invalid XML specification." << endl;
  throw CONVERSION_ERROR;
  return wxEmptyString;
}

wxXmlNode *grape::mcrl2gen::get_architecture_diagram(wxXmlNode *p_doc_root, wxString &p_diagram_name)
{
  if(p_doc_root->GetName() == _T("grape"))
  {
    wxXmlNode *archs = get_child(p_doc_root, _T("architecturediagramlist"));
    if(archs == 0)
    {
      // ERROR: <grape> does not contain <architecturediagramlist>
      cerr << "mCRL2 conversion error: could not find architecture diagram "
           << p_diagram_name.ToAscii() << "." << endl;
      throw CONVERSION_ERROR;
      return 0;
    }

    for(wxXmlNode *arch = archs->GetChildren(); arch != 0; arch = arch->GetNext())
    {
      if(get_child_value(arch, _T("name")) == p_diagram_name)
      {
        // found diagram, return it
        return arch;
      }
    }

    // found nothing, return 0
    return 0;
  }

  // ERROR: called with invalid XML
  cerr << "mCRL2 conversion error: get_architecture_diagram called with an invalid XML specification." << endl;
  throw CONVERSION_ERROR;
  return 0;
}

bool grape::mcrl2gen::validate(wxXmlDocument &p_spec)
{
  wxXmlNode *doc_root = p_spec.GetRoot();
  bool is_valid = true;

  // validate datatype specification
  ATermAppl datatype_spec;
  if(!validate_datatype_specification(doc_root, datatype_spec))
  {
    is_valid = false;
  }

  // validate all diagrams
  wxXmlNode *process_diagrams = get_child(doc_root, _T("processdiagramlist"));
  for(wxXmlNode *process_diagram = process_diagrams->GetChildren(); process_diagram != 0; process_diagram = process_diagram->GetNext())
  {
    if(!validate_process_diagram(doc_root, process_diagram, datatype_spec))
    {
      is_valid = false;
    }
  }
  wxXmlNode *architecture_diagrams = get_child(doc_root, _T("architecturediagramlist"));
  for(wxXmlNode *architecture_diagram = architecture_diagrams->GetChildren(); architecture_diagram != 0; architecture_diagram = architecture_diagram->GetNext())
  {
    if(!validate_architecture_diagram(doc_root, architecture_diagram, datatype_spec))
    {
      is_valid = false;
    }
  }

  is_valid &= validate_diagram_names(doc_root);

  if(is_valid)
  {
    // check architecture reference acylicy
    for(wxXmlNode *arch_diag = architecture_diagrams->GetChildren(); arch_diag != 0; arch_diag = arch_diag->GetNext())
    {
      wxString diag_name = get_child_value(arch_diag, _T("id"));
      wxArrayString diag_names;
      diag_names.Empty();
      diag_names.Add(diag_name);
      is_valid &= infer_architecture_reference_acyclic(doc_root, diag_names);
    }
  }

  if(is_valid)
  {
    cerr << "+specification is valid." << endl;
  }
  else
  {
    cerr << "+specification is not valid." << endl;
  }

  return is_valid;
}

bool grape::mcrl2gen::validate_process_diagram(wxXmlDocument &p_spec, wxString &p_diagram_id)
{
  wxXmlNode *doc_root = p_spec.GetRoot();

// validate datatype specification
  ATermAppl datatype_spec;
  if(!validate_datatype_specification(doc_root, datatype_spec))
  {
    return false;
  }

  wxXmlNode *process_diagram = 0;
  try
  {
    process_diagram = get_process_diagram(doc_root, p_diagram_id);
  }
  catch(...)
  {
    cerr << "+specification is not valid: process diagram " << p_diagram_id.ToAscii() << " is not present." << endl;
    return false;
  }

  if(!validate_process_diagram(doc_root, process_diagram, datatype_spec))
  {
    cerr << "+process diagram is not valid." << endl;
    return false;
  }

  cerr << "+process diagram is valid." << endl;

  return true;
}

bool grape::mcrl2gen::validate_process_diagram(wxXmlNode *p_doc_root, wxXmlNode *p_process_diagram, ATermAppl &datatype_spec)
{
  bool  designator_list_is_valid = false, ref_state_list_is_valid = false, state_list_is_valid = false,
        term_trans_list_is_valid = false, trans_list_is_valid = false;

  wxString diagram_id = get_child_value(p_process_diagram, _T("id"));
  wxString diagram_name = get_child_value(p_process_diagram, _T("name"));
  if(diagram_name == wxEmptyString)
  {
    cerr << "+specification is not valid: process diagram " << diagram_id.ToAscii() << " has an empty name." << endl;
    return false;
  }

  // check preamble
  list_of_decl preamble_params;
  list_of_decl_init preamble_vars;
  try
  {
    parse_preamble(p_process_diagram, preamble_params, preamble_vars, datatype_spec);
  }
  catch(...)
  {
    cerr << "+specification is not valid: process diagram " << diagram_name.ToAscii() << " has an invalid preamble." << endl;
    return false;
  }

  wxXmlNode *object_list = get_child(p_process_diagram, _T("objectlist"));
  if(object_list == 0)
  {
    cerr << "+specification is not valid: process diagram " << diagram_name.ToAscii() << " contains no list of objects." << endl;
    return false;
  }

  for(wxXmlNode *curr_list = object_list->GetChildren(); curr_list != 0; curr_list = curr_list->GetNext())
  {
    if(curr_list->GetName() == _T("initialdesignatorlist"))
    {
      designator_list_is_valid = validate_initial_designator_list(p_process_diagram, curr_list);
    }
    else if(curr_list->GetName() == _T("referencestatelist"))
    {
      ref_state_list_is_valid = validate_reference_state_list(p_doc_root, p_process_diagram, curr_list, datatype_spec);
    }
    else if(curr_list->GetName() == _T("statelist"))
    {
      state_list_is_valid = validate_state_list(p_process_diagram, curr_list);
    }
    else if(curr_list->GetName() == _T("terminatingtransitionlist"))
    {
      term_trans_list_is_valid = validate_terminating_transition_list(p_process_diagram, curr_list, preamble_params, preamble_vars, datatype_spec);
    }
    else if(curr_list->GetName() == _T("nonterminatingtransitionlist"))
    {
      trans_list_is_valid = validate_nonterminating_transition_list(p_process_diagram, curr_list, preamble_params, preamble_vars, datatype_spec);
    }
  }

  if(!designator_list_is_valid || !ref_state_list_is_valid || !state_list_is_valid || !term_trans_list_is_valid || !trans_list_is_valid)
  {
    return false;
  }

  return true;
}

bool grape::mcrl2gen::validate_architecture_diagram(wxXmlDocument &p_spec, wxString &p_diagram_id)
{
  wxXmlNode *doc_root = p_spec.GetRoot();

  // validate datatype specification
  ATermAppl datatype_spec;
  if(!validate_datatype_specification(doc_root, datatype_spec))
  {
    return false;
  }

  wxXmlNode *arch_diagram = 0;
  try
  {
    wxString arch_name = infer_architecture_name(doc_root, p_diagram_id);
    arch_diagram = get_architecture_diagram(doc_root, arch_name);
  }
  catch(...)
  {
    cerr << "+specification is not valid: architecture diagram " << p_diagram_id.ToAscii() << " is not present." << endl;
    return false;
  }

  if(!validate_architecture_diagram(doc_root, arch_diagram, datatype_spec))
  {
    cerr << "+architecture diagram is not valid." << endl;
    return false;
  }

  cerr << "+architecture diagram is valid." << endl;

  return true;
}

bool grape::mcrl2gen::validate_architecture_diagram(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, ATermAppl &datatype_spec)
{
  bool channel_communication_list_is_valid = false,
       channel_list_is_valid = false, architecture_reference_list_is_valid = false, process_reference_list_is_valid = false;

  wxString diagram_id = get_child_value(p_architecture_diagram, _T("id"));
  wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));
  if(diagram_name == wxEmptyString)
  {
    cerr << "+specification is not valid: architecture diagram " << diagram_id.ToAscii() << " has an empty name." << endl;
    return false;
  }

  wxXmlNode *object_list = get_child(p_architecture_diagram, _T("objectlist"));
  if(object_list == 0)
  {
    cerr << "+specification is not valid: architecture diagram " << diagram_name.ToAscii() << " contains no list of objects." << endl;
    return false;
  }

  for(wxXmlNode *curr_list = object_list->GetChildren(); curr_list != 0; curr_list = curr_list->GetNext())
  {
    if(curr_list->GetName() == _T("architecturereferencelist"))
    {
      architecture_reference_list_is_valid = validate_architecture_reference_list(p_doc_root, p_architecture_diagram, curr_list, datatype_spec);
    }
    else if(curr_list->GetName() == _T("processreferencelist"))
    {
      process_reference_list_is_valid = validate_process_reference_list(p_doc_root, p_architecture_diagram, curr_list, datatype_spec);
    }
  }

  if(architecture_reference_list_is_valid && process_reference_list_is_valid)
  {
    for(wxXmlNode *curr_list = object_list->GetChildren(); curr_list != 0; curr_list = curr_list->GetNext())
    {
      if(curr_list->GetName() == _T("channelcommunicationlist"))
      {
        channel_communication_list_is_valid = validate_channel_communication_list(p_architecture_diagram, curr_list);
      }
      else if(curr_list->GetName() == _T("channellist"))
      {
        channel_list_is_valid = validate_channel_list(p_doc_root, p_architecture_diagram, curr_list, datatype_spec);
      }
    }
  }

  if(!channel_communication_list_is_valid || !channel_list_is_valid || !architecture_reference_list_is_valid || !process_reference_list_is_valid)
  {
    return false;
  }

  return true;
}

bool grape::mcrl2gen::validate_datatype_specification(wxXmlNode *p_doc_root, ATermAppl &datatype_spec)
{
  bool is_valid = true;

  wxXmlNode *datspeclist = get_child(p_doc_root, _T("datatypespecificationlist"));
  wxString datspec = get_child_value(datspeclist, _T("datatypespecification"));

  if (!datspec.IsEmpty()) {
    // try to parse the mCRL2 specification
    string mcrl2_spec = string(datspec.mb_str());
    const char *m_spec = mcrl2_spec.c_str();
    istringstream iss(m_spec);
    ATermAppl a_parsed_mcrl2_spec = parse_data_spec(iss);
    if(a_parsed_mcrl2_spec == 0)
    {
      cerr << "+specification is not valid: could not parse the datatype specification." << endl;
      is_valid = false;
    }
    else
    {
      // parse succeeded: try to type check
      ATermAppl a_type_checked_mcrl2_spec = type_check_data_spec(a_parsed_mcrl2_spec);
      datatype_spec = a_type_checked_mcrl2_spec;
      if(a_type_checked_mcrl2_spec == 0)
      {
        cerr << "+specification is not valid: could not type check the datatype specification." << endl;
        is_valid = false;
      }
    }
  }
  else
  {
    datatype_spec = gsMakeEmptyDataSpec();
  }

  return is_valid;
}

bool grape::mcrl2gen::validate_initial_designator_list(wxXmlNode *p_process_diagram, wxXmlNode *p_designator_list)
{
  // count initial designators
  unsigned int num_designators = 0;
  bool is_valid = true;
  wxString diagram_name = get_child_value(p_process_diagram, _T("name"));
  for(wxXmlNode *designator = p_designator_list->GetChildren(); designator != 0; designator = designator->GetNext())
  {
    ++num_designators;
  }
  if(num_designators < 1)
  {
    cerr << "+specification is not valid: process diagram " << diagram_name.ToAscii()
         << " does not contain an initial designator." << endl;
    is_valid = false;
  }
  if(num_designators > 1)
  {
    cerr << "+specification is not valid: process diagram " << diagram_name.ToAscii()
         << " contains more than one initial designator." << endl;
    is_valid = false;
  }

  // check if the initial designator is attached
  if(is_valid)
  {
    // only check if there is one initial designator
    wxXmlNode *designator = p_designator_list->GetChildren();

    wxString attached_id = get_child_value(designator, _T("propertyof"));
    if(attached_id == _T("-1"))
    {
      // not attached
      cerr << "+specification is not valid: process diagram " << diagram_name.ToAscii()
           << " contains an unconnected initial designator." << endl;
      is_valid = false;
    }
    else
    {
      // search attached state
      wxXmlNode *object_list = get_child(p_process_diagram, _T("objectlist"));
      wxXmlNode *state_list = get_child(object_list, _T("statelist"));
      wxXmlNode *reference_state_list = get_child(object_list, _T("referencestatelist"));
      if(state_list == 0)
      {
        cerr << "+specification is not valid: process diagram " << diagram_name.ToAscii()
             << " does not contain a list of states." << endl;
        return false;
      }
      if(reference_state_list == 0)
      {
        cerr << "+specification is not valid: process diagram " << diagram_name.ToAscii()
             << " does not contain a list of reference states." << endl;
        return false;
      }
      for(wxXmlNode *state = state_list->GetChildren(); state != 0; state = state->GetNext())
      {
        wxString state_id = get_child_value(state, _T("id"));
        if(state_id == attached_id)
        {
          is_valid = true;
          break;
        }
      }
      if(!is_valid)
      {
        for(wxXmlNode *ref_state = reference_state_list->GetChildren(); ref_state != 0; ref_state = ref_state->GetNext())
        {
          wxString ref_state_id = get_child_value(ref_state, _T("id"));
          if(ref_state_id == attached_id)
          {
            is_valid = true;
            break;
          }
        }
      }
    }
  }

  return is_valid;
}

bool grape::mcrl2gen::validate_reference_state_list(wxXmlNode *p_doc_root, wxXmlNode *p_process_diagram, wxXmlNode *p_ref_state_list, ATermAppl &datatype_spec)
{
  // validate each reference: a reference is valid when it points to a valid process diagram
  // AND it has a valid parameter initialization
  bool is_valid = true;
  wxString diagram_name = get_child_value(p_process_diagram, _T("name"));

  for(wxXmlNode *ref_state = p_ref_state_list->GetChildren(); ref_state != 0; ref_state = ref_state->GetNext())
  {
    wxString ref_state_prop = get_child_value(ref_state, _T("propertyof"));
    wxString ref_state_id = get_child_value(ref_state, _T("id"));
    wxXmlNode *referenced_diagram = get_process_diagram(p_doc_root, ref_state_prop);
    if(ref_state_prop == _T("-1"))
    {
      // no target diagram
      cerr << "+specification is not valid: process diagram " << diagram_name.ToAscii()
           << " contains a process reference which does not refer to a process diagram." << endl;
      return false;
    }
    else if(referenced_diagram == 0)
    {
      // target diagram not found
      cerr << "+specification is not valid: process diagram " << diagram_name.ToAscii()
           << " contains a process reference that does not refer to an existing process diagram." << endl;
      return false;
    }

    if (!validate_process_diagram(p_doc_root, referenced_diagram, datatype_spec))
    {
      return false;
    }
    wxString ref_name = get_child_value(referenced_diagram, _T("name"));
    wxString ref_id = get_child_value(referenced_diagram, _T("id"));

    // check parameter initialisation
    list_of_decl preamble_params;
    list_of_decl_init preamble_vars;
    // parsed parameter initialisation to ref_inits
    try
    {
      parse_preamble(referenced_diagram, preamble_params, preamble_vars, datatype_spec);
    }
    catch(...)
    {
      return false;
    }

    list_of_varupdate ref_inits;
    try
    {
      parse_reference_parameters(ref_state, diagram_name, ref_inits, preamble_params, datatype_spec);
    }
    catch(...)
    {
      return false;
    }

    if(preamble_params.GetCount() != ref_inits.GetCount())
    {
      cerr << "+specification is not valid: process diagram " << diagram_name.ToAscii()
           << " contains a process reference to process diagram " << ref_name.ToAscii()
           << " that does not contain the same number of parameters." << endl;
      is_valid = false;
    }

    for(unsigned int i=0; i<preamble_params.GetCount(); ++i)
    {
      bool found = false;
      for(unsigned int j=0; j<ref_inits.GetCount(); ++j)
      {
        if(ref_inits[j].get_lhs() == preamble_params[i].get_name())
        {
          found = true;
          break;
        }
      }
      if(!found)
      {
        cerr << "+specification is not valid: process diagram " << diagram_name.ToAscii()
             << " contains a process reference to process diagram " << ref_name.ToAscii()
             << " that has a parameter initialisation in which parameter " << preamble_params[i].get_name().ToAscii()
             << " does not occur." << endl;
        is_valid = false;
      }
    }

    bool is_connected = false;
    // check transitions
    wxXmlNode *objects = get_child(p_process_diagram, _T("objectlist"));
    wxXmlNode *tts = get_child(objects, _T("terminatingtransitionlist"));
    wxXmlNode *ntts = get_child(objects, _T("nonterminatingtransitionlist"));
    wxXmlNode *designators = get_child(objects, _T("initialdesignatorlist"));
    for(wxXmlNode *tt = tts->GetChildren(); tt != 0; tt = tt->GetNext())
    {
      wxString tt_from = get_child_value(tt, _T("from"));
      if(tt_from == ref_state_id)
      {
        is_connected = true;
        break;
      }
    }
    if(!is_connected)
    {
      for(wxXmlNode *ntt = ntts->GetChildren(); ntt != 0; ntt = ntt->GetNext())
      {
        wxString ntt_from = get_child_value(ntt, _T("from"));
        wxString ntt_to = get_child_value(ntt, _T("to"));
        if(ntt_from == ref_state_id || ntt_to == ref_state_id)
        {
          is_connected = true;
          break;
        }
      }
      if(!is_connected)
      {
        for(wxXmlNode *des = designators->GetChildren(); des != 0; des = des->GetNext())
        {
          // reference must have an initial designator!
          wxString des_id = get_child_value(des, _T("propertyof"));
          if(des_id == ref_state_id)
          {
            is_connected = true;
            break;
          }
        }
      }
    }

    if(!is_connected)
    {
      cerr << "+specification is not valid: process diagram " << diagram_name.ToAscii()
           << " contains a process reference to \"" << ref_name.ToAscii() << "\" that is not connected to a transition." << endl;
      is_valid = false;
    }
  }

  return is_valid;
}

bool grape::mcrl2gen::validate_state_list(wxXmlNode *p_process_diagram, wxXmlNode *p_state_list)
{
  // states are valid when they are connected with a transition
  bool is_valid = true;
  wxString diagram_name = get_child_value(p_process_diagram, _T("name"));

  for(wxXmlNode *state = p_state_list->GetChildren(); state != 0; state = state->GetNext())
  {
    wxString state_name = get_child_value(state, _T("name"));
    wxString state_id = get_child_value(state, _T("id"));

    bool is_connected = false;
    // check transitions
    wxXmlNode *objects = get_child(p_process_diagram, _T("objectlist"));
    wxXmlNode *tts = get_child(objects, _T("terminatingtransitionlist"));
    wxXmlNode *ntts = get_child(objects, _T("nonterminatingtransitionlist"));
    wxXmlNode *designators = get_child(objects, _T("initialdesignatorlist"));
    for(wxXmlNode *tt = tts->GetChildren(); tt != 0; tt = tt->GetNext())
    {
      wxString tt_from = get_child_value(tt, _T("from"));
      if(tt_from == state_id)
      {
        is_connected = true;
        break;
      }
    }
    if(!is_connected)
    {
      for(wxXmlNode *ntt = ntts->GetChildren(); ntt != 0; ntt = ntt->GetNext())
      {
        wxString ntt_from = get_child_value(ntt, _T("from"));
        wxString ntt_to = get_child_value(ntt, _T("to"));
        if(ntt_from == state_id || ntt_to == state_id)
        {
          is_connected = true;
          break;
        }
      }
      if(!is_connected)
      {
        // state must have an initial designator!
        for(wxXmlNode *des = designators->GetChildren(); des != 0; des = des->GetNext())
        {
          wxString des_id = get_child_value(des, _T("propertyof"));
          if(des_id == state_id)
          {
            is_connected = true;
            break;
          }
        }

      }
    }

    if(!is_connected)
    {
      cerr << "+specification is not valid: process diagram " << diagram_name.ToAscii()
           << " contains a state \"" << state_name.ToAscii() << "\" that is not connected to a transition." << endl;
      is_valid = false;
    }
  }

  return is_valid;
}

bool grape::mcrl2gen::validate_terminating_transition_list(wxXmlNode *p_process_diagram, wxXmlNode *p_term_trans_list, list_of_decl &p_preamble_parameters, list_of_decl_init &p_preamble_variables, ATermAppl &datatype_spec)
{
  // for each transition, infer if the beginstates exist and try to parse its label
  bool is_valid = true;
  wxString diagram_name = get_child_value(p_process_diagram, _T("name"));

  for(wxXmlNode *transition = p_term_trans_list->GetChildren(); transition != 0; transition = transition->GetNext())
  {
    wxString trans_from = get_child_value(transition, _T("from"));

    if(trans_from == wxEmptyString || trans_from == _T("-1"))
    {
      cerr << "+specification is not valid: process diagram " << diagram_name.ToAscii()
           << " contains a terminating transition that is not connected to a state or process reference." << endl;
      is_valid = false;
    }

    // parse trans_label <label>
    label trans_label;
    try
    {
      trans_label = parse_transition_label(transition, p_preamble_parameters, p_preamble_variables, diagram_name, datatype_spec);
    }
    catch(...)
    {
      is_valid = false;
    }
  }

  return is_valid;
}

bool grape::mcrl2gen::validate_nonterminating_transition_list(wxXmlNode *p_process_diagram, wxXmlNode *p_trans_list, list_of_decl &p_preamble_parameters, list_of_decl_init &p_preamble_variables, ATermAppl &datatype_spec)
{
  // for each transition, infer if the begin- and endstates exist and try to parse its label
  bool is_valid = true;
  wxString diagram_name = get_child_value(p_process_diagram, _T("name"));

  for(wxXmlNode *transition = p_trans_list->GetChildren(); transition != 0; transition = transition->GetNext())
  {
    wxString trans_from = get_child_value(transition, _T("from"));
    wxString trans_to = get_child_value(transition, _T("to"));

    if(trans_from == wxEmptyString || trans_from == _T("-1")||
       trans_to == wxEmptyString || trans_to == _T("-1"))
    {
      cerr << "+specification is not valid: process diagram " << diagram_name.ToAscii()
           << " contains a non-terminating transition that is not conntected to a state or process reference." << endl;
      is_valid = false;
    }

    // parse trans_label <label>
    label trans_label;
    try
    {
      trans_label = parse_transition_label(transition, p_preamble_parameters, p_preamble_variables, diagram_name, datatype_spec);
    }
    catch(...)
    {
      is_valid = false;
    }
  }

  return is_valid;
}

bool grape::mcrl2gen::parse_preamble(wxXmlNode *p_process_diagram, list_of_decl &p_preamble_parameter_decls, list_of_decl_init &p_preamble_local_var_decls, ATermAppl &datatype_spec)
{
  p_preamble_parameter_decls.Empty();
  p_preamble_local_var_decls.Empty();

  if(p_process_diagram->GetName() == _T("processdiagram"))
  {
    wxString diagram_name = get_child_value(p_process_diagram, _T("name"));

    // extract the preamble
    wxXmlNode *child = get_child(p_process_diagram, _T("preambledeclarations"));
    if(child == 0)
    {
      // ERROR: <processdiagram> does not contain <preambledeclarations>
      cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " does not have a preamble." << endl;
      throw CONVERSION_ERROR;
      return false;
    }
    wxXmlNode *var_list = get_child(child, _T("localvariablelist"));
    wxXmlNode *param_list = get_child(child, _T("parameterlist"));
    if(var_list != 0)
    {
      for(wxXmlNode *local_var = var_list->GetChildren(); local_var != 0; local_var = local_var->GetNext())
      {
        decl_init var_decl;
        if ( local_var->GetName() == _T("var") )
        {
          wxString var_text = local_var->GetNodeContent();
          bool valid = var_decl.set_decl_init( var_text );
          if ( !valid )
          {
            // ERROR: variable declaration is not valid
            cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid local variable " << var_text.ToAscii()
                 << " declaration in its preamble." << endl;
            throw CONVERSION_ERROR;
            return false;
          }

          // parse local variable name (identifier)
          string local_var_id = string(var_decl.get_name().mb_str());
          const char *l_v_id = local_var_id.c_str();
          istringstream iss(l_v_id);
          ATermAppl a_parsed_local_var_id = parse_identifier(iss);
          if ( a_parsed_local_var_id == 0 )
          {
            // ERROR: variable declaration is not valid
            cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid local variable " << var_text.ToAscii()
                 << " declaration in its preamble. The variable name '" << var_decl.get_name().ToAscii() << "' could not be parsed." << endl;
            throw CONVERSION_ERROR;
            return false;
          }

          // parse local variable type (sortexpression)
          string local_var_sort = string(var_decl.get_type().mb_str());
          const char *l_v_sort = local_var_sort.c_str();
          istringstream iss1(l_v_sort);
          ATermAppl a_parsed_local_var_sort = parse_sort_expr(iss1);
          ATermAppl a_type_checked_local_var_sort;
          if ( a_parsed_local_var_sort == 0 )
          {
            // ERROR: variable declaration is not valid
            cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid local variable " << var_text.ToAscii()
                 << " declaration in its preamble. The variable type '" << var_decl.get_type().ToAscii() << "' could not be parsed." << endl;
            throw CONVERSION_ERROR;
            return false;
          }
          else
          {
            // parse succeeded: try to type check
            a_type_checked_local_var_sort = type_check_sort_expr( a_parsed_local_var_sort, datatype_spec );
            if ( a_type_checked_local_var_sort == 0 )
            {
              // ERROR: variable declaration is not valid
              cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid local variable " << var_text.ToAscii()
                   << " declaration in its preamble. The variable type '" << var_decl.get_type().ToAscii() << "' could not be type checked." << endl;
              throw CONVERSION_ERROR;
              return false;
            }
          }

          // parse local variable value (dataexpression)
          string local_var_expr = string(var_decl.get_value().mb_str());
          const char *l_v_expr = local_var_expr.c_str();
          istringstream iss2(l_v_expr);
          ATermAppl a_parsed_local_var_expr = parse_data_expr(iss2);
          if ( a_parsed_local_var_expr == 0 )
          {
            // ERROR: variable declaration is not valid
            cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid local variable " << var_text.ToAscii()
                 << " declaration in its preamble. The variable value '" << var_decl.get_value().ToAscii() << "' could not be parsed." << endl;
            throw CONVERSION_ERROR;
            return false;
          }
          else
          {
            // parse succeeded: try to type check
            ATermAppl a_type_checked_local_var_expr = type_check_data_expr( a_parsed_local_var_expr, a_type_checked_local_var_sort, datatype_spec );
            if ( a_type_checked_local_var_expr == 0 )
            {
              // ERROR: variable declaration is not valid
              cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid local variable " << var_text.ToAscii()
                   << " declaration in its preamble. The variable value '" << var_decl.get_value().ToAscii() << "' could not be type checked." << endl;
              throw CONVERSION_ERROR;
              return false;
            }
          }
        }
        else
        {
          // ERROR: XML invalid
          cerr << "mCRL2 conversion error: parse_preamble called without a valid XML process diagram." << endl;
          throw CONVERSION_ERROR;
          return false;
        }
        p_preamble_local_var_decls.Add( var_decl );
      }
    }

    if(param_list != 0)
    {
      for(wxXmlNode *parameter = param_list->GetChildren(); parameter != 0; parameter = parameter->GetNext())
      {
        decl param_decl;
        if ( parameter->GetName() == _T("param") )
        {
          wxString param_text = parameter->GetNodeContent();
          bool valid = param_decl.set_decl( param_text );
          if ( !valid )
          {
            // ERROR: parameter declaration is not valid
            cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid parameter " << param_text.ToAscii() << " declaration in its preamble." << endl;
            throw CONVERSION_ERROR;
            return false;
          }

          // parse parameter name (identifier)
          string param_id = string(param_decl.get_name().mb_str());
          const char *par_id = param_id.c_str();
          istringstream iss(par_id);
          ATermAppl a_parsed_param_id = parse_identifier(iss);
          if ( a_parsed_param_id == 0 )
          {
            // ERROR: parameter declaration is not valid
            cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid parameter " << param_text.ToAscii()
                 << " declaration in its preamble. The parameter name '" << param_decl.get_name().ToAscii() << "' could not be parsed." << endl;
            throw CONVERSION_ERROR;
            return false;
          }

          // parse parameter type (sortexpression)
          string param_sort = string(param_decl.get_type().mb_str());
          const char *par_sort = param_sort.c_str();
          istringstream iss1(par_sort);
          ATermAppl a_parsed_param_sort = parse_sort_expr(iss1);
          if ( a_parsed_param_sort == 0 )
          {
            // ERROR: parameter declaration is not valid
            cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid parameter " << param_text.ToAscii()
                 << " declaration in its preamble. The parameter type '" << param_decl.get_type().ToAscii() << "' could not be parsed." << endl;
            throw CONVERSION_ERROR;
            return false;
          }
          else
          {
            // parse succeeded: try to type check
            ATermAppl a_type_checked_param_sort = type_check_sort_expr( a_parsed_param_sort, datatype_spec );
            if ( a_type_checked_param_sort == 0 )
            {
              // ERROR: parameter declaration is not valid
              cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid parameter " << param_text.ToAscii()
                   << " declaration in its preamble. The parameter type '" << param_decl.get_type().ToAscii() << "' could not be type checked." << endl;
              throw CONVERSION_ERROR;
              return false;
            }
          }
        }
        else
        {
          // ERROR: XML invalid
          cerr << "mCRL2 conversion error: parse_preamble called without a valid XML process diagram." << endl;
          throw CONVERSION_ERROR;
          return false;
        }
        p_preamble_parameter_decls.Add( param_decl );
      }
    }
    return true;
  }
  // ERROR: XML invalid
  cerr << "mCRL2 conversion error: parse_preamble called without a valid XML process diagram." << endl;
  throw CONVERSION_ERROR;
  return false;
}

label grape::mcrl2gen::parse_transition_label(wxXmlNode *p_process_diagram, list_of_decl &p_preamble_parameter_decls, list_of_decl_init &p_preamble_local_var_decls, wxString p_diagram_name, ATermAppl &datatype_spec)
{
  label label;
  list_of_decl variable_decl_list;
  wxString condition;
  list_of_action actions;
  wxString timestamp;
  list_of_varupdate variable_updates;

  // load label
  wxXmlNode* transition_label = get_child(p_process_diagram, _T("label"));
  if (transition_label != 0)
  {
    // get variable declarations
    wxXmlNode* trans_label_declarations = get_child(transition_label, _T("variabledeclarations"));
    if (trans_label_declarations != 0)
    {
      for (wxXmlNode *trans_label_declaration = trans_label_declarations->GetChildren(); trans_label_declaration != 0; trans_label_declaration = trans_label_declaration->GetNext())
      {
        decl variable_decl;
        if (trans_label_declaration->GetName() == _T("variabledeclaration"))
        {
          wxString variable_declaration_text = trans_label_declaration->GetNodeContent();
          bool valid = variable_decl.set_decl( variable_declaration_text );
          if ( !valid )
          {
            // ERROR: transition label is not valid
            cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii()
                 << " contains a transition label with an invalid variable declaration "
                 << variable_declaration_text.ToAscii() << "." << endl;
            throw CONVERSION_ERROR;
            return label;
          }

          // parse declaration name (identifier)
          string variable_decl_id = string(variable_decl.get_name().mb_str());
          const char *var_decl_id = variable_decl_id.c_str();
          istringstream iss(var_decl_id);
          ATermAppl a_parsed_var_decl_id = parse_identifier(iss);
          if ( a_parsed_var_decl_id == 0 )
          {
            // ERROR: variable declaration is not valid
            cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains a transition label with an invalid variable declaration " << variable_declaration_text.ToAscii()
                 << ". The variable name '" << variable_decl.get_name().ToAscii() << "' could not be parsed." << endl;
            throw CONVERSION_ERROR;
            return label;
          }

          // parse declaration type (sortexpression)
          string variable_decl_sort = string(variable_decl.get_type().mb_str());
          const char *var_decl_sort = variable_decl_sort.c_str();
          istringstream iss1(var_decl_sort);
          ATermAppl a_parsed_var_decl_sort = parse_sort_expr(iss1);
          if ( a_parsed_var_decl_sort == 0 )
          {
            // ERROR: variable declaration is not valid
            cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains a transition label with an invalid variable declaration " << variable_declaration_text.ToAscii()
                 << ". The variable type '" << variable_decl.get_type().ToAscii() << "' could not be parsed." << endl;
            throw CONVERSION_ERROR;
            return label;
          }
          else
          {
            // parse succeeded: try to type check
            ATermAppl a_type_checked_var_decl_sort = type_check_sort_expr( a_parsed_var_decl_sort, datatype_spec );
            if ( a_type_checked_var_decl_sort == 0 )
            {
              // ERROR: variable declaration is not valid
              cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains a transition label with an invalid variable declaration " << variable_declaration_text.ToAscii()
                   << ". The variable type '" << variable_decl.get_type().ToAscii() << "' could not be type checked." << endl;
              throw CONVERSION_ERROR;
              return label;
            }
            variable_decl_list.Add( variable_decl );
          }
        }
        else
        {
          // ERROR: transition label is not valid
          cerr << "mCRL2 conversion error: parse_transition_label called without a valid XML process diagram." << endl;
          throw CONVERSION_ERROR;
          return label;
        }
      }
    }

    // make variable table
    atermpp::table vars;
    for (unsigned int i = 0; i < p_preamble_parameter_decls.GetCount(); ++i)
    {
      istringstream iss( string(p_preamble_parameter_decls.Item(i).get_name().mb_str() ) );
      ATermAppl var_name = parse_identifier( iss );
      assert( var_name != 0 );
      istringstream iss1( string(p_preamble_parameter_decls.Item(i).get_type().mb_str() ) );
      ATermAppl parsed_var_type = parse_sort_expr( iss1 );
      assert( parsed_var_type != 0 );
      ATermAppl var_type = type_check_sort_expr( parsed_var_type, datatype_spec );
      assert( var_type != 0 );
      vars.put( var_name, var_type );
    }
    for (unsigned int i = 0; i < p_preamble_local_var_decls.GetCount(); ++i)
    {
      istringstream iss( string(p_preamble_local_var_decls.Item(i).get_name().mb_str() ) );
      ATermAppl var_name = parse_identifier( iss );
      assert( var_name != 0 );
      istringstream iss1( string(p_preamble_local_var_decls.Item(i).get_type().mb_str() ) );
      ATermAppl parsed_var_type = parse_sort_expr( iss1 );
      assert( parsed_var_type != 0 );
      ATermAppl var_type = type_check_sort_expr( parsed_var_type, datatype_spec );
      assert( var_type != 0 );
      vars.put( var_name, var_type );
    }
    for (unsigned int i = 0; i < variable_decl_list.GetCount(); ++i)
    {
      istringstream iss( string(variable_decl_list.Item(i).get_name().mb_str() ) );
      ATermAppl var_name = parse_identifier( iss );
      assert( var_name != 0 );
      istringstream iss1( string(variable_decl_list.Item(i).get_type().mb_str() ) );
      ATermAppl parsed_var_type = parse_sort_expr( iss1 );
      assert( parsed_var_type != 0 );
      ATermAppl var_type = type_check_sort_expr( parsed_var_type, datatype_spec );
      assert( var_type != 0 );
      vars.put( var_name, var_type );
    }

    // get condition
    wxXmlNode* trans_label_condition = get_child(transition_label, _T("condition"));
    if (trans_label_condition != 0)
    {
      condition = trans_label_condition->GetNodeContent();
      if (!condition.IsEmpty())
      {
        // parse condition (dataexpression)
        string condition_expr = string(condition.mb_str());
        const char *c_expr = condition_expr.c_str();
        istringstream iss(c_expr);
        ATermAppl a_parsed_condition_expr = parse_data_expr(iss);
        if ( a_parsed_condition_expr == 0 )
        {
          // ERROR: condition is not valid
          cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The condition "
               << condition.ToAscii() << " could not be parsed." << endl;
          throw CONVERSION_ERROR;
          return label;
        }
        else
        {
          // parse succeeded: try to type check
          // get type of condition
          wxString condition_type = infer_type( condition, p_preamble_parameter_decls, p_preamble_local_var_decls, variable_decl_list );
          if ( condition_type.IsEmpty() )
          {
            ATermAppl a_type_checked_condition_expr = type_check_data_expr( a_parsed_condition_expr, NULL, datatype_spec, vars);
            if ( a_type_checked_condition_expr == 0 )
            {
              // ERROR: condition is not valid
              cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The condition "
                   << condition.ToAscii() << " could not be type checked." << endl;
              throw CONVERSION_ERROR;
              return label;
            }
          }
          else
          {
            // parse condition type (sortexpression)
            string condition_sort_expr = string(condition_type.mb_str());
            const char *c_sort_expr = condition_sort_expr.c_str();
            istringstream iss(c_sort_expr);
            ATermAppl a_parsed_condition_sort_expr = parse_sort_expr(iss);
            if ( a_parsed_condition_sort_expr == 0 )
            {
              // ERROR: condition is not valid
              cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The type of condition "
                   << condition.ToAscii() << " could not be parsed." << endl;
              throw CONVERSION_ERROR;
              return label;
            }
            else
            {
              //parse succeeded: try to type check
              ATermAppl a_type_checked_condition_sort_expr = type_check_sort_expr( a_parsed_condition_sort_expr, datatype_spec );
              if ( a_type_checked_condition_sort_expr == 0 )
              {
                // ERROR: condition is not valid
                cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The type of condition "
                     << condition.ToAscii() << " could not be type checked." << endl;
                throw CONVERSION_ERROR;
                return label;
              }

              // parse desired condition type (Bool) (sortexpression)
              wxString desired_condition_type = _T("Bool");
              string desired_condition_sort_expr = string(desired_condition_type.mb_str());
              const char *d_c_sort_expr = desired_condition_sort_expr.c_str();
              istringstream iss(d_c_sort_expr);
              ATermAppl a_parsed_desired_condition_sort_expr = parse_sort_expr(iss);
              if ( a_parsed_desired_condition_sort_expr == 0 )
              {
                // ERROR: condition is not valid
                cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The type Bool could not be parsed." << endl;
                throw CONVERSION_ERROR;
                return label;
              }
              else
              {
                //parse succeeded: try to type check
                ATermAppl a_type_checked_desired_condition_sort_expr = type_check_sort_expr( a_parsed_desired_condition_sort_expr, datatype_spec );
                if ( a_type_checked_desired_condition_sort_expr == 0 )
                {
                  // ERROR: condition is not valid
                  cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The type of condition "
                       << condition.ToAscii() << " could not be type checked." << endl;
                  throw CONVERSION_ERROR;
                  return label;
                }

                // check if condition is of type Bool
                if (a_type_checked_condition_sort_expr != a_type_checked_desired_condition_sort_expr)
                {
                  // ERROR: condition is not valid
                  cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The type of condition "
                       << condition.ToAscii() << " is not Bool." << endl;
                  throw CONVERSION_ERROR;
                  return label;
                }
              }
            }
          }
        }
      }
    }

    // get actions
    wxXmlNode* trans_label_actions = get_child(transition_label, _T("actions"));
    if (trans_label_actions != 0)
    {
      for (wxXmlNode *trans_label_action = trans_label_actions->GetChildren(); trans_label_action != 0; trans_label_action = trans_label_action->GetNext())
      {
        action action;
        list_of_dataexpression action_params;
        for (wxXmlNode *action_part = trans_label_action->GetChildren(); action_part != 0; action_part = action_part->GetNext())
        {
          if (action_part->GetName() == _T("name"))
          {
            wxString action_name = action_part->GetNodeContent();

            // parse action identifier
            string action_id = string(action_name.mb_str());
            const char *a_id = action_id.c_str();
            istringstream iss(a_id);
            ATermAppl a_parsed_action_identifier = parse_identifier(iss);
            if ( a_parsed_action_identifier == 0 )
            {
              // ERROR: variable declaration is not valid
              cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The action name "
                   << action_name.ToAscii() << " could not be parsed." << endl;
              throw CONVERSION_ERROR;
              return label;
            }

            action.set_name( action_name );
          }
          else if (action_part->GetName() == _T("param"))
          {
            wxString action_param = action_part->GetNodeContent();

            // parse action parameter (dataexpression)
            string action_param_expr = string(action_param.mb_str());
            const char *a_p_expr = action_param_expr.c_str();
            istringstream iss(a_p_expr);
            ATermAppl a_parsed_action_param_expr = parse_data_expr(iss);
            if ( a_parsed_action_param_expr == 0 )
            {
              // ERROR: variable declaration is not valid
              cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The action parameter "
                   << action_param.ToAscii() << " could not be parsed." << endl;
              throw CONVERSION_ERROR;
              return label;
            }
            else
            {
              // parse succeeded: try to type check

              // get type of action parameter
              wxString action_param_type = infer_type( action_param, p_preamble_parameter_decls, p_preamble_local_var_decls, variable_decl_list );
              if ( action_param_type.IsEmpty() )
              {
                ATermAppl a_type_checked_action_param_expr = type_check_data_expr( a_parsed_action_param_expr, NULL, datatype_spec, vars);
                if ( a_type_checked_action_param_expr == 0 )
                {
                  // ERROR: action parameter is not valid
                  cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The action parameter "
                       << action_param.ToAscii() << " could not be type checked." << endl;
                  throw CONVERSION_ERROR;
                  return label;
                }
                else
                {

                  ATermAppl sort_expr = gsGetSort(a_type_checked_action_param_expr);
                  string sort_expr_string = PrintPart_CXX(ATerm(sort_expr));
                  action_param_type = wxString(sort_expr_string.c_str(), wxConvLocal);

                  // De rest van dit block kan weg
/*                  ATermAppl data_type_spec = datatype_spec;
                  ATermAppl a_implemented_data_expr = implement_data_data_expr( a_type_checked_action_param_expr, data_type_spec );
                  if (a_implemented_data_expr == 0 )
                  {
                    // ERROR: action parameter is not valid
                    cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The action paramter "
                         << action_param.ToAscii() << " could not be inferred." << endl;
                    throw CONVERSION_ERROR;
                    return label;
                  }
                  data_expression data_expr = data_expression( a_implemented_data_expr );
                  sort_identifier sort_expr = data_expr.sort();
                  string sort_expr_string = string(sort_expr.name());
                  action_param_type = wxString(sort_expr_string.c_str(), wxConvLocal);
*/
                }
              }
              else
              {
                // parse action parameter type (sortexpression)
                string action_param_sort_expr = string(action_param_type.mb_str());
                const char *a_p_sort_expr = action_param_sort_expr.c_str();
                istringstream iss(a_p_sort_expr);
                ATermAppl a_parsed_action_param_sort_expr = parse_sort_expr(iss);
                if ( a_parsed_action_param_sort_expr == 0 )
                {
                  // ERROR: variable declaration is not valid
                  cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The type of action parameter "
                       << action_param.ToAscii() << " could not be parsed." << endl;
                  throw CONVERSION_ERROR;
                  return label;
                }
                else
                {
                  //parse succeeded: try to type check
                  ATermAppl a_type_checked_action_param_sort_expr = type_check_sort_expr( a_parsed_action_param_sort_expr, datatype_spec );
                  if ( a_type_checked_action_param_sort_expr == 0 )
                  {
                    // ERROR: variable declaration is not valid
                    cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The type of action parameter "
                         << action_param.ToAscii() << " could not be type checked." << endl;
                    throw CONVERSION_ERROR;
                    return label;
                  }
                }
              }
              dataexpression param;
              param.set_expression( action_param );
			        param.set_type( action_param_type );
              action_params.Add( param );
            }
          }
          else
          {
            // ERROR: transition label is not valid
            cerr << "mCRL2 conversion error: parse_transition_label called without a valid XML process diagram." << endl;
            throw CONVERSION_ERROR;
            return label;
          }
        }
        action.set_parameters( action_params );
        actions.Add( action );
      }
      label.set_actions( actions );

      // NB: In fact this parsing bit isn't neccesary, because it's done per action above
      wxString multiaction_text = label.get_actions_text();
      // parse actions (multiaction)
      if (!multiaction_text.IsEmpty())
      {
        string mult_act = string(multiaction_text.mb_str());
        const char *m_a = mult_act.c_str();
        istringstream iss(m_a);
        ATermAppl a_parsed_multiaction = parse_mult_act(iss);
        if ( a_parsed_multiaction == 0 )
        {
          // ERROR: variable declaration is not valid
          cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The (parameterised) (multi)action "
               << multiaction_text.ToAscii() << " could not be parsed." << endl;
          throw CONVERSION_ERROR;
          return label;
        }
        else
        {
          // parsing succeeded, try to type check
          // problem: type_check_mult_act requires something I don't have.
        }
      }
    }

    // get timestamp
    wxXmlNode* trans_label_timestamp = get_child(transition_label, _T("timestamp"));
    if (trans_label_timestamp != 0)
    {
      timestamp = trans_label_timestamp->GetNodeContent();
      if (!timestamp.IsEmpty())
      {
        // parse timestamp (dataexpression)
        string timestamp_expr = string(timestamp.mb_str());
        const char *c_expr = timestamp_expr.c_str();
        istringstream iss(c_expr);
        ATermAppl a_parsed_timestamp_expr = parse_data_expr(iss);
        if ( a_parsed_timestamp_expr == 0 )
        {
          // ERROR: timestamp is not valid
          cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The timestamp "
               << timestamp.ToAscii() << " could not be parsed." << endl;
          throw CONVERSION_ERROR;
          return label;
        }
        else
        {
          // parse succeeded: try to type check
          // get type of timestamp
          wxString timestamp_type = infer_type( timestamp, p_preamble_parameter_decls, p_preamble_local_var_decls, variable_decl_list );
          if ( timestamp_type.IsEmpty() )
          {
            ATermAppl a_type_checked_timestamp_expr = type_check_data_expr( a_parsed_timestamp_expr, NULL, datatype_spec, vars);
            if ( a_type_checked_timestamp_expr == 0 )
            {
              // ERROR: timestamp is not valid
              cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The timestamp "
                   << timestamp.ToAscii() << " could not be type checked." << endl;
              throw CONVERSION_ERROR;
              return label;
            }
          }
          else
          {
            // parse timestamp type (sortexpression)
            string timestamp_sort_expr = string(timestamp_type.mb_str());
            const char *c_sort_expr = timestamp_sort_expr.c_str();
            istringstream iss(c_sort_expr);
            ATermAppl a_parsed_timestamp_sort_expr = parse_sort_expr(iss);
            if ( a_parsed_timestamp_sort_expr == 0 )
            {
              // ERROR: timestamp is not valid
              cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The type of timestamp "
                   << timestamp.ToAscii() << " could not be parsed." << endl;
              throw CONVERSION_ERROR;
              return label;
            }
            else
            {
              //parse succeeded: try to type check
              ATermAppl a_type_checked_timestamp_sort_expr = type_check_sort_expr( a_parsed_timestamp_sort_expr, datatype_spec );
              if ( a_type_checked_timestamp_sort_expr == 0 )
              {
                // ERROR: timestamp is not valid
                cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The type of timestamp "
                     << timestamp.ToAscii() << " could not be type checked." << endl;
                throw CONVERSION_ERROR;
                return label;
              }

              // parse desired timestamp type (Real) (sortexpression)
              wxString desired_timestamp_type = _T("Real");
              string desired_timestamp_sort_expr = string(desired_timestamp_type.mb_str());
              const char *d_c_sort_expr = desired_timestamp_sort_expr.c_str();
              istringstream iss(d_c_sort_expr);
              ATermAppl a_parsed_desired_timestamp_sort_expr = parse_sort_expr(iss);
              if ( a_parsed_desired_timestamp_sort_expr == 0 )
              {
                // ERROR: timestamp is not valid
                cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The type Real could not be parsed." << endl;
                throw CONVERSION_ERROR;
                return label;
              }
              else
              {
                //parse succeeded: try to type check
                ATermAppl a_type_checked_desired_timestamp_sort_expr = type_check_sort_expr( a_parsed_desired_timestamp_sort_expr, datatype_spec );
                if ( a_type_checked_desired_timestamp_sort_expr == 0 )
                {
                  // ERROR: timestamp is not valid
                  cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The type of timestamp "
                       << timestamp.ToAscii() << " could not be type checked." << endl;
                  throw CONVERSION_ERROR;
                  return label;
                }

                // check if timestamp is of type Real
                if (a_type_checked_timestamp_sort_expr != a_type_checked_desired_timestamp_sort_expr)
                {
                  // ERROR: timestamp is not valid
                  cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The type of timestamp "
                       << timestamp.ToAscii() << " is not Real." << endl;
                  throw CONVERSION_ERROR;
                  return label;
                }
              }
            }
          }
        }
      }
    }

    // get variable updates
    wxXmlNode* trans_label_variable_updates = get_child(transition_label, _T("variableupdates"));
    if (trans_label_variable_updates != 0)
    {
      for (wxXmlNode *trans_label_variable_update = trans_label_variable_updates->GetChildren(); trans_label_variable_update != 0; trans_label_variable_update = trans_label_variable_update->GetNext())
      {
        varupdate variable_update;
        if (trans_label_variable_update->GetName() == _T("variableupdate"))
        {
          wxString variable_update_text = trans_label_variable_update->GetNodeContent();
          bool valid = variable_update.set_varupdate( variable_update_text );
          if ( !valid )
          {
            // ERROR: transition label is not valid
            cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii()
                 << " contains a transition label with an invalid variable update "
                 << variable_update_text.ToAscii() << "." << endl;
            throw CONVERSION_ERROR;
            return label;
          }

          // parse varupdate left hand side (identifier)
          ATermAppl a_type_checked_varupdate_lhs_sort_expr;
          string varupdate_id = string(variable_update.get_lhs().mb_str());
          const char *v_id = varupdate_id.c_str();
          istringstream iss(v_id);
          ATermAppl a_parsed_variable_update_identifier = parse_identifier(iss);
          if ( a_parsed_variable_update_identifier == 0 )
          {
            // ERROR: variable declaration is not valid
            cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The left hand side "
                 << variable_update.get_lhs().ToAscii() << " of variable update "
                 << variable_update_text.ToAscii() << " could not be parsed." << endl;
            throw CONVERSION_ERROR;
            return label;
          }
          else
          {
            // parse as identifier succeeded: try to parse as data expression
            string varupdate_lhs_expr = string(variable_update.get_lhs().mb_str());
            const char *v_l_expr = varupdate_lhs_expr.c_str();
            istringstream iss1(v_l_expr);
            ATermAppl a_parsed_variable_update_lhs_expression = parse_data_expr(iss1);
            if ( a_parsed_variable_update_lhs_expression == 0 )
            {
              // ERROR: variable declaration is not valid
              cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The left hand side "
                   << variable_update.get_lhs().ToAscii() << " of variable update "
                   << variable_update_text.ToAscii() << " could not be parsed." << endl;
              throw CONVERSION_ERROR;
              return label;
            }
            else
            {
              // parse succeeded: try to type check
              // get type of left hand side
              wxString variable_update_lhs = variable_update.get_lhs();
              wxString varupdate_lhs_type = infer_type( variable_update_lhs, p_preamble_parameter_decls, p_preamble_local_var_decls, variable_decl_list );
              if ( varupdate_lhs_type.IsEmpty() )
              {
                // ERROR: variable update is not valid
                cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The left hand side "
                     << variable_update.get_lhs().ToAscii() << " of variable update "
                     << variable_update_text.ToAscii() << " could not be type checked." << endl;
                throw CONVERSION_ERROR;
                return label;
              }
              else
              {
                // parse variable update left hand side type (sortexpression)
                string varupdate_lhs_sort_expr = string(varupdate_lhs_type.mb_str());
                const char *c_sort_expr = varupdate_lhs_sort_expr.c_str();
                istringstream iss(c_sort_expr);
                ATermAppl a_parsed_varupdate_lhs_sort_expr = parse_sort_expr(iss);
                if ( a_parsed_varupdate_lhs_sort_expr == 0 )
                {
                  // ERROR: varupdate_lhs is not valid
                  cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The type of the left hand side "
                       << variable_update.get_lhs().ToAscii() << " of variable update "
                       << variable_update_text.ToAscii() << " could not be parsed." << endl;
                  throw CONVERSION_ERROR;
                  return label;
                }
                else
                {
                  //parse succeeded: try to type check
                  a_type_checked_varupdate_lhs_sort_expr = type_check_sort_expr( a_parsed_varupdate_lhs_sort_expr, datatype_spec );
                  if ( a_type_checked_varupdate_lhs_sort_expr == 0 )
                  {
                    // ERROR: varupdate_lhs is not valid
                    cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The type of the left hand side "
                         << variable_update.get_lhs().ToAscii() << " of variable update "
                         << variable_update_text.ToAscii() << " could not be type checked." << endl;
                    throw CONVERSION_ERROR;
                    return label;
                  }
                }
              }
            }
          }

          // parse varupdate right hand side (dataexpression)
          string varupdate_rhs_expr = string(variable_update.get_rhs().mb_str());
          const char *v_r_expr = varupdate_rhs_expr.c_str();
          istringstream iss1(v_r_expr);
          ATermAppl a_parsed_varupdate_rhs_expr = parse_data_expr(iss1);
          if ( a_parsed_varupdate_rhs_expr == 0 )
          {
            // ERROR: variable declaration is not valid
            cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The right hand side "
                 << variable_update.get_rhs().ToAscii() << " of variable update "
                 << variable_update_text.ToAscii() << " could not be parsed." << endl;
            throw CONVERSION_ERROR;
            return label;
          }
          else
          {
            // parse succeeded: try to type check
            // type checking against type of lhs
            ATermAppl a_type_checked_varupdate_rhs_expr = type_check_data_expr( a_parsed_varupdate_rhs_expr, a_type_checked_varupdate_lhs_sort_expr, datatype_spec, vars);
            if ( a_type_checked_varupdate_rhs_expr == 0 )
            {
              // ERROR: variable declaration is not valid
              cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The right hand side "
                   << variable_update.get_rhs().ToAscii() << " of variable update "
                   << variable_update_text.ToAscii() << " could not be type checked." << endl;
              throw CONVERSION_ERROR;
              return label;
            }
          }
          variable_updates.Add( variable_update );
        }
        else
        {
          // ERROR: transition label is not valid
          cerr << "mCRL2 conversion error: parse_transition_label called without a valid XML process diagram." << endl;
          throw CONVERSION_ERROR;
          return label;
        }
      }
    }

    label.set_declarations( variable_decl_list );
    label.set_condition( condition );
    label.set_timestamp( timestamp );
    label.set_variable_updates( variable_updates );
    return label;
  }
  // ERROR: transition label is not valid
  cerr << "mCRL2 conversion error: parse_transition_label called without a valid XML process diagram." << endl;
  throw CONVERSION_ERROR;
  return label;
}

wxString grape::mcrl2gen::infer_type(wxString &p_name, list_of_decl &p_preamble_parameter_decls, list_of_decl_init &p_preamble_local_var_decls, list_of_decl &p_trans_var_decls)
{
  // search in transition variable declaration first
  for(unsigned int i=0; i<p_trans_var_decls.GetCount(); ++i)
  {
    if(p_trans_var_decls[i].get_name() == p_name)
    {
      return p_trans_var_decls[i].get_type();
    }
  }
  // search in preamble
  for(unsigned int i=0; i<p_preamble_parameter_decls.GetCount(); ++i)
  {
    if(p_preamble_parameter_decls[i].get_name() == p_name)
    {
      return p_preamble_parameter_decls[i].get_type();
    }
  }
  for(unsigned int i=0; i<p_preamble_local_var_decls.GetCount(); ++i)
  {
    if(p_preamble_local_var_decls[i].get_name() == p_name)
    {
      return p_preamble_local_var_decls[i].get_type();
    }
  }
  return wxEmptyString;
}

bool grape::mcrl2gen::validate_channel_communication_list(wxXmlNode *p_architecture_diagram, wxXmlNode *p_channel_communication_list)
{
  // a channel communication is valid when all its connections are to existing channels
  bool is_valid = true;
  wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));

  for(wxXmlNode *comm = p_channel_communication_list->GetChildren(); comm != 0; comm = comm->GetNext())
  {
    wxXmlNode *connectionlist = get_child(comm, _T("connectionlist"));
    bool has_channels = true;
    if(connectionlist == 0)
    {
      cerr << "+specification is not valid: architecture diagram " << diagram_name.ToAscii()
           << " contains a channel communication that is not connected to any channels." << endl;
      is_valid = false;
      has_channels = false;
    }

    if(has_channels)
    {
      wxXmlNode *objects = get_child(p_architecture_diagram, _T("objectlist"));
      // objects != 0, otherwise we couldn't be here
      wxXmlNode *channels = get_child(objects, _T("channellist"));
      if(channels == 0)
      {
        cerr << "+specification is not valid: architecture diagram " << diagram_name.ToAscii()
             << " contains a channel communication, but it does not contain any channels." << endl;
        is_valid = false;
      }
      else
      {
        for(wxXmlNode *connection = connectionlist->GetChildren(); connection != 0; connection = connection->GetNext())
        {
          wxString connection_channel = connection->GetNodeContent();
          bool connection_found = false;

          // search for this connection
          for(wxXmlNode *channel = channels->GetChildren(); channel != 0; channel = channel->GetNext())
          {
            wxString channel_id = get_child_value(channel, _T("id"));
            if(channel_id == connection_channel)
            {
              connection_found = true;
              break;
            }
          }

          if(!connection_found)
          {
            cerr << "+specification is not valid: architecture diagram " << diagram_name.ToAscii()
                 << " contains a channel communication that connected to an existing channel." << endl;
            is_valid = false;
          }
        }
      }
    }
  }

  return is_valid;
}

bool grape::mcrl2gen::validate_channel_list(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, wxXmlNode *p_channel_list, ATermAppl &datatype_spec)
{
  // a channel is valid when its name corresponds to an action inside its reference, its <onreference> tag is valid
  // and its connections are to existing channelcommunications.
  bool is_valid = true;
  wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));

  for(wxXmlNode *channel = p_channel_list->GetChildren(); channel != 0; channel = channel->GetNext())
  {
    // get <onreference> tag
    wxString channel_reference = get_child_value(channel, _T("onreference"));
    bool is_on_reference = true;
    if(channel_reference == wxEmptyString)
    {
      cerr << "+specification is not valid: architecture diagram " << diagram_name.ToAscii()
           << " contains a channel that is not associated with an architecture reference or process reference." << endl;
      is_valid = false;
      is_on_reference = false;
    }

    // get <onchannelcommunication> tag
    wxString channel_on_comm = get_child_value(channel, _T("onchannelcommunication"));

    wxXmlNode *objects = get_child(p_architecture_diagram, _T("objectlist"));
    // objects != 0, otherwise we couldn't be here
    wxXmlNode *comms = get_child(objects, _T("channelcommunicationlist"));
    if(comms != 0)
    {
      // check <onchannelcommunication> tag
      if(channel_on_comm != wxEmptyString)
      {
        bool found_comm = false;

        for(wxXmlNode *comm = comms->GetChildren(); comm != 0; comm = comm->GetNext())
        {
          wxString comm_id = get_child_value(comm, _T("id"));

          if(comm_id == channel_on_comm)
          {
            found_comm = true;
            break;
          }
        }

        if(!found_comm)
        {
          cerr << "+specification is not valid: architecture diagram " << diagram_name.ToAscii()
               << " contains a channel that is associated with a non-existing channel communication." << endl;
          is_valid = false;
        }
      }
    }

    wxXmlNode *reference = 0;
    bool is_process_reference = false;
    wxXmlNode *proc_references = get_child(objects, _T("processreferencelist"));
    wxXmlNode *arch_references = get_child(objects, _T("architecturereferencelist"));

    // check <onreference> tag
    if(proc_references != 0)
    {
      for(wxXmlNode *proc_reference = proc_references->GetChildren(); proc_reference != 0; proc_reference = proc_reference->GetNext())
      {
        wxString proc_id = get_child_value(proc_reference, _T("id"));
        if(proc_id == channel_reference)
        {
          // <propertyof> contains the id of the diagram
          wxString proc_diag = get_child_value(proc_reference, _T("propertyof"));
          try
          {
            reference = get_process_diagram(p_doc_root, proc_diag);
          }
          catch(...)
          {}
          is_process_reference = true;
          break;
        }
      }
    }
    if(arch_references != 0 && reference == 0)
    {
      for(wxXmlNode *arch_reference = arch_references->GetChildren(); arch_reference != 0; arch_reference = arch_reference->GetNext())
      {
        wxString arch_id = get_child_value(arch_reference, _T("id"));
        if(arch_id == channel_reference)
        {
          // <propertyof> contains the id of the diagram
          wxString arch_diag_id = get_child_value(arch_reference, _T("propertyof"));
          try
          {
            wxString arch_diag_name = infer_architecture_name(p_doc_root, arch_diag_id);
            reference = get_architecture_diagram(p_doc_root, arch_diag_name);
          }
          catch(...)
          {}
          is_process_reference = false;
          break;
        }
      }
    }

    if(reference == 0)
    {
      cerr << "+specification is not valid: architecture diagram " << diagram_name.ToAscii()
           << " contains a channel that lies on a reference to a non-existing diagram." << endl;
      is_valid = false;
    }

    wxString channel_name = get_child_value(channel, _T("name"));
    if(channel_name == wxEmptyString)
    {
      cerr << "+specification is not valid: architecture diagram " << diagram_name.ToAscii()
           << " contains a channel with an empty name." << endl;
      is_valid = false;
    }
    else
    {
      if(reference != 0)
      {
        // check name
        list_of_action ref_actions;
        if(is_process_reference)
        {
          // channel lies on a process reference
          wxString proc_ref_id = get_child_value(reference, _T("id"));
          try
          {
            ref_actions = infer_process_actions(p_doc_root, proc_ref_id, datatype_spec);
          }
          catch(...)
          {
          }
        }
        else
        {
          // channel lies on an architecture reference
          try
          {
            ref_actions = infer_architecture_visibles(p_doc_root, reference, datatype_spec);
          }
          catch(...)
          {
          }
        }
        bool found_channel_name = false;
        for(unsigned int i=0; i<ref_actions.GetCount(); ++i)
        {
          if(ref_actions.Item(i).get_name() == channel_name)
          {
            found_channel_name = true;
            break;
          }
        }
        if(!found_channel_name)
        {
          cerr << "+specification is not valid: architecture diagram " << diagram_name.ToAscii()
               << " contains a channel with name \"" << channel_name.ToAscii()
               << "\" that does not correspond to a (visible-) action inside its associated reference." << endl;
          is_valid = false;
        }
      }
    }
  }

  return is_valid;
}

bool grape::mcrl2gen::infer_architecture_reference_acyclic(wxXmlNode *p_doc_root, wxArrayString p_checked)
{
  bool is_valid = true;

  // check the references in the last diagram added to p_checked
  wxString curr_arch_name = infer_architecture_name(p_doc_root, p_checked.Last());
  wxXmlNode *arch_diag = get_architecture_diagram(p_doc_root, curr_arch_name);
  wxXmlNode *objects = get_child(arch_diag, _T("objectlist"));
  wxXmlNode *arch_ref_list = get_child(objects, _T("architecturereferencelist"));
  for(wxXmlNode *arch_ref = arch_ref_list->GetChildren(); arch_ref != 0; arch_ref = arch_ref->GetNext())
  {
    wxString ref_propertyof = get_child_value(arch_ref, _T("propertyof"));
    wxString ref_name = get_child_value(arch_ref, _T("name"));
    // if the reference has the same name as the refs in p_checked, not acyclic
    for(unsigned int i=0; i<p_checked.GetCount(); ++i)
    {
      if(p_checked[i] == ref_propertyof)
      {
        cerr << "+specification is not valid: architecture diagram " << ref_name.ToAscii()
             << " contains an (indirect) reference to itself." << endl;
        is_valid = false;
      }
    }
  }
  if(is_valid)
  {
    for(wxXmlNode *arch_ref = arch_ref_list->GetChildren(); arch_ref != 0; arch_ref = arch_ref->GetNext())
    {
      wxString ref_propertyof = get_child_value(arch_ref, _T("propertyof"));
      p_checked.Add(ref_propertyof);
      is_valid &= infer_architecture_reference_acyclic(p_doc_root, p_checked);
    }
  }
  return is_valid;
}

bool grape::mcrl2gen::validate_architecture_reference_list(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, wxXmlNode *p_reference_list, ATermAppl &datatype_spec)
{
  // an architecture reference is valid when it refers to an existing diagram and every associated channel name is unique
  bool is_valid = true;
  wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));

  for(wxXmlNode *arch_ref = p_reference_list->GetChildren(); arch_ref != 0; arch_ref = arch_ref->GetNext())
  {
    wxString ref_id = get_child_value(arch_ref, _T("id"));
    wxString ref_propertyof = get_child_value(arch_ref, _T("propertyof"));

    if(ref_propertyof == wxEmptyString || ref_propertyof == _T("-1"))
    {
      cerr << "+specification is not valid: architecture diagram " << diagram_name.ToAscii()
           << " contains an architecture reference that does not refer to an existing architecture diagram." << endl;
      is_valid = false;
    }
    else
    {
      wxXmlNode *arch_diag = 0;
      wxString diag_name = wxEmptyString;

      try
      {
        diag_name = infer_architecture_name(p_doc_root, ref_propertyof);
        arch_diag = get_architecture_diagram(p_doc_root, diag_name);
      }
      catch(...)
      {
      }

      if(arch_diag == 0)
      {
        cerr << "+specification is not valid: architecture diagram " << diagram_name.ToAscii()
             << " contains an architecture reference to a non-existing architecture diagram." << endl;
        is_valid = false;
      }
      if (diag_name != diagram_name)
      {
        if (!validate_architecture_diagram(p_doc_root, arch_diag, datatype_spec))
        {
          return false;
        }
      }
    }

    // check channels
    wxArrayString ref_channels;
    ref_channels.Empty();
    wxXmlNode *objects = get_child(p_architecture_diagram, _T("objectlist"));
    wxXmlNode *channels = get_child(objects, _T("channellist"));
    for(wxXmlNode *channel = channels->GetChildren(); channel != 0; channel = channel->GetNext())
    {
      wxString channel_propertyof = get_child_value(channel, _T("onreference"));
      wxString channel_name = get_child_value(channel, _T("name"));
      if(channel_propertyof == ref_id)
      {
        ref_channels.Add(channel_name);
      }
    }
    for(unsigned int i=0; i<ref_channels.GetCount(); ++i)
    {
      for(unsigned int j=0; j<ref_channels.GetCount(); ++j)
      {
        if(ref_channels[i] == ref_channels[j] && i < j)
        {
          cerr << "+specification is not valid: architecture diagram " << diagram_name.ToAscii()
               << " contains an architecture reference with multiple channels referring to action "
               << ref_channels[i].ToAscii() << "." << endl;
          is_valid = false;
          break;
        }
      }
    }
  }

  return is_valid;
}

bool grape::mcrl2gen::validate_process_reference_list(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, wxXmlNode *p_reference_list, ATermAppl &datatype_spec)
{
  // a process reference is valid when it refers to a process diagram, the parameter initialisation matches and every associated channel name is unique
  bool is_valid = true;
  wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));

  for(wxXmlNode *proc_ref = p_reference_list->GetChildren(); proc_ref != 0; proc_ref = proc_ref->GetNext())
  {
    wxString ref_id = get_child_value(proc_ref, _T("id"));
    wxString ref_state_prop = get_child_value(proc_ref, _T("propertyof"));
    wxXmlNode *referenced_diagram = 0;
    try
    {
      referenced_diagram = get_process_diagram(p_doc_root, ref_state_prop);
    }
    catch(...)
    {}
    if(ref_state_prop == _T("-1"))
    {
      // no target diagram
      cerr << "+specification is not valid: architecture diagram " << diagram_name.ToAscii()
           << " contains a process reference which does not refer to a process diagram." << endl;
      return false;
    }
    else if(referenced_diagram == 0)
    {
      // target diagram not found
      cerr << "+specification is not valid: architecture diagram " << diagram_name.ToAscii()
           << " contains a process reference that does not refer to an existing process diagram." << endl;
      return false;
    }

    if (!validate_process_diagram(p_doc_root, referenced_diagram, datatype_spec))
    {
      return false;
    }
    wxString ref_name = get_child_value(referenced_diagram, _T("name"));

    list_of_decl preamble_params;
    list_of_decl_init preamble_vars;
    // parsed parameter initialisation to ref_inits
    try
    {
      parse_preamble(referenced_diagram, preamble_params, preamble_vars, datatype_spec);
    }
    catch(...)
    {
      return false;
    }

    // check parameter initialisation
    list_of_varupdate ref_inits;
    try
    {
      parse_reference_parameters(proc_ref, diagram_name, ref_inits, preamble_params, datatype_spec);
    }
    catch(...)
    {
      return false;
    }

    if(preamble_params.GetCount() != ref_inits.GetCount())
    {
      cerr << "+specification is not valid: architecture diagram " << diagram_name.ToAscii()
           << " contains a process reference to process diagram " << ref_name.ToAscii()
           << " that does not contain the same number of parameters." << endl;
      is_valid = false;
    }

    for(unsigned int i=0; i<preamble_params.GetCount(); ++i)
    {
      bool found = false;
      for(unsigned int j=0; j<ref_inits.GetCount(); ++j)
      {
        if(ref_inits[j].get_lhs() == preamble_params[i].get_name())
        {
          found = true;
          break;
        }
      }
      if(!found)
      {
        cerr << "+specification is not valid: architecture diagram " << diagram_name.ToAscii()
             << " contains a process reference to process diagram " << ref_name.ToAscii()
             << " that has a parameter initialisation in which parameter " << preamble_params[i].get_name().ToAscii()
             << " does not occur." << endl;
        is_valid = false;
      }
    }

    // check channels
    wxArrayString ref_channels;
    ref_channels.Empty();
    wxXmlNode *objects = get_child(p_architecture_diagram, _T("objectlist"));
    wxXmlNode *channels = get_child(objects, _T("channellist"));
    for(wxXmlNode *channel = channels->GetChildren(); channel != 0; channel = channel->GetNext())
    {
      wxString channel_propertyof = get_child_value(channel, _T("onreference"));
      wxString channel_name = get_child_value(channel, _T("name"));
      if(channel_propertyof == ref_id)
      {
        ref_channels.Add(channel_name);
      }
    }
    for(unsigned int i=0; i<ref_channels.GetCount(); ++i)
    {
      for(unsigned int j=0; j<ref_channels.GetCount(); ++j)
      {
        if(ref_channels[i] == ref_channels[j] && i < j)
        {
          cerr << "+specification is not valid: architecture diagram " << diagram_name.ToAscii()
               << " contains a process reference with multiple channels referring to action "
               << ref_channels[i].ToAscii() << "." << endl;
          is_valid = false;
          break;
        }
      }
    }
  }

  return is_valid;
}

list_of_action grape::mcrl2gen::infer_architecture_visibles(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, ATermAppl &datatype_spec)
{
  list_of_action visibles;

  if(p_architecture_diagram->GetName() == _T("architecturediagram"))
  {
    wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));

    wxXmlNode *objects = get_child(p_architecture_diagram, _T("objectlist"));
    if(objects == 0)
    {
      // ERROR: <architecturediagram> has no objects.
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " does not contain any objects." << endl;
      throw CONVERSION_ERROR;
      return visibles;
    }

    // get visible channels
    visibles = infer_architecture_visible_channels(p_doc_root, diagram_name, objects, datatype_spec);
    // get visible channel communications
    list_of_action visible_channel_communications = infer_architecture_visible_channel_communications(p_doc_root, diagram_name, objects, datatype_spec);
    WX_APPEND_ARRAY(visibles, visible_channel_communications);
    return visibles;
  }

  // ERROR: not called with valid XML node
  cerr << "mCRL2 conversion error: infer_architecture_visibles called with an invalid XML architecture diagram" << endl;
  throw CONVERSION_ERROR;
  return visibles;
}

list_of_action grape::mcrl2gen::infer_architecture_visible_channels(wxXmlNode *p_doc_root, wxString &p_diagram_name, wxXmlNode *p_objects, ATermAppl &datatype_spec)
{
  list_of_action visibles;
  wxString diagram_name = p_diagram_name;

  // get process references
  wxXmlNode *proc_refs = get_child(p_objects, _T("processreferencelist"));

  // get channels
  wxXmlNode *channels = get_child(p_objects, _T("channellist"));
  if (channels == 0)
  {
    // ERROR: <objectlist> has no <channellist>
    cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
         << " does not contain any channels." << endl;
    throw CONVERSION_ERROR;
    return visibles;
  }

  bool found = false;
  // walk through channels
  wxXmlNode *channel_node = channels->GetChildren();
  while (channel_node != 0)
  {
    // get channel_type
    wxString channel_type = get_child_value(channel_node, _T("channeltype"));
    if (channel_type == _T("visible")) 
    {
      // get channel id
      wxString channel_id = get_child_value(channel_node, _T("id"));
      // get channel name
      wxString channel_name = get_child_value(channel_node, _T("name"));
      // get visible channel name
      wxString channel_visible_name = get_child_value(channel_node, _T("rename"));
      if (channel_visible_name.IsEmpty())
      {
        channel_visible_name = channel_name;
      }

      // get reference
      wxString on_reference = get_child_value(channel_node, _T("onreference"));
      // walk through process references
      wxXmlNode *proc_ref = proc_refs->GetChildren();
      while (!found && proc_ref != 0)
      {
        // get_reference id
        wxString proc_ref_id = get_child_value(proc_ref, _T("id"));
        if (proc_ref_id == on_reference)
        {
          // get process
          wxString process_id = get_child_value(proc_ref, _T("propertyof"));
          // get_actions
          list_of_action actions = infer_process_actions(p_doc_root, process_id, datatype_spec);
          for (unsigned int i = 0; i < actions.GetCount(); ++i)
          {
            // add correct visibles
            if (actions[i].get_name() == channel_name)
            {
              action new_action;
              new_action.set_name( channel_visible_name );
              new_action.set_parameters ( actions[i].get_parameters() );
              visibles.Add( new_action );
              found = true;
            }
          }
        }
        proc_ref = proc_ref->GetNext();
      }
    }
    channel_node = channel_node->GetNext();
  }
  return visibles;
}

list_of_action grape::mcrl2gen::infer_architecture_visible_channel_communications(wxXmlNode *p_doc_root, wxString &p_diagram_name, wxXmlNode *p_objects, ATermAppl &datatype_spec)
{
  list_of_action visibles;
  wxString diagram_name = p_diagram_name;

  // get process references
  wxXmlNode *proc_refs = get_child(p_objects, _T("processreferencelist"));

  // get channels
  wxXmlNode *channels = get_child(p_objects, _T("channellist"));
  if (channels == 0)
  {
    // ERROR: <objectlist> has no <channellist>
    cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
         << " does not contain any channels." << endl;
    throw CONVERSION_ERROR;
    return visibles;
  }

  // get channel_communications
  wxXmlNode *channel_communications = get_child(p_objects, _T("channelcommunicationlist"));
  if (channel_communications == 0)
  {
    // ERROR: <objectlist> has no <channelcommunicationlist>
    cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
         << " does not contain any channel communications." << endl;
    throw CONVERSION_ERROR;
    return visibles;
  }

  bool found = false;
  // walk through channel communications
  wxXmlNode *channel_communication_node = channel_communications->GetChildren();
  while (channel_communication_node != 0)
  {
    // get channel communication type
    wxString channel_communication_type = get_child_value(channel_communication_node, _T("channelcommunicationtype"));
    if (channel_communication_type == _T("visible"))
    {
      // get channel communication id
      wxString channel_communication_id = get_child_value(channel_communication_node, _T("id"));
      // get visible channel communication name
      wxString channel_communication_visible_name = get_child_value(channel_communication_node, _T("rename"));
      if (channel_communication_visible_name.IsEmpty())
      {
        // ERROR: visible channel communication is not renamed
        cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
             << " has a visible channel communication that is not renamed." << endl;
        throw CONVERSION_ERROR;
        return visibles;
      }

      wxXmlNode *channel_list = get_child(channel_communication_node, _T("connectionlist"));
      if (channel_list == 0)
      {
        // ERROR: <channelcommunication> has no <connectionlist>
        cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
             << " does not contain any channel communication connections." << endl;
        throw CONVERSION_ERROR;
        return visibles;
      }
      
      list_of_action actions_found, new_actions_found;
      // walk through connections
      for (wxXmlNode *connection = channel_list->GetChildren(); connection != 0; connection = connection->GetNext())
      {
        wxString channel_connection;
        if (connection->GetName() == _T("connectedtochannel"))
        {
          channel_connection = connection->GetNodeContent();
        }
        else
        {
          // ERROR: <connectionlist> has no <connectedtochannels>
          cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
               << " does not contain any channel communication connection reference." << endl;
          throw CONVERSION_ERROR;
          return visibles;
        }

        bool channel_found = false;
        new_actions_found.Empty();
        // walk through channels
        wxXmlNode *channel_node = channels->GetChildren();
        while (!channel_found && channel_node != 0)
        {
          // get channel id
          wxString channel_id = get_child_value(channel_node, _T("id"));
          // get channel name
          wxString channel_name = get_child_value(channel_node, _T("name"));
          if (channel_id == channel_connection)
          {
            channel_found = true;
            // get reference
            wxString on_reference = get_child_value(channel_node, _T("onreference"));
            bool proc_found = false;
            // walk through process references
            wxXmlNode *proc_ref = proc_refs->GetChildren();
            while (!proc_found && proc_ref != 0)
            {
              // get_reference_id
              wxString proc_ref_id = get_child_value(proc_ref, _T("id"));
              if (proc_ref_id == on_reference)
              {
                proc_found = true;
                // get process
                wxString process_id = get_child_value(proc_ref, _T("propertyof"));
                // get_actions
                list_of_action actions = infer_process_actions(p_doc_root, process_id, datatype_spec);
                for (unsigned int i = 0; i < actions.GetCount(); ++i)
                {
                  // add correct visibles
                  if (actions[i].get_name() == channel_name)
                  {
               	    bool act_found = false;
                    for (unsigned int j = 0; j < actions_found.GetCount(); ++j)
                    {
                      if ( actions[i].get_parameters().GetCount() == actions_found[j].get_parameters().GetCount() )
                      {
                        if ( actions[i].get_parameters().GetCount() == 0 )
                        {
                          act_found = true;
                          break;
                        }
                        else
                        {
                          list_of_dataexpression actions_params = actions[i].get_parameters();
                          list_of_dataexpression actions_found_params = actions_found[j].get_parameters();
                          act_found = true;
                          unsigned int k = 0;
                          while (act_found && k < actions_params.GetCount())
                          {
                            act_found = actions_params[k].get_type() == actions_found_params[k].get_type();
                            ++k;
                          }
                          if (act_found)
                          {
                            break;
                          }
                        }
                      }
                    } // end for (actions_found)
                    if (act_found)
                    {
                      new_actions_found.Add( actions[i] );
                    }
                    else
                    {
                      if (actions_found.IsEmpty())
                      {
                        new_actions_found.Add( actions[i] );
                      }
                    }
                  } 
                } // end for (actions)
              }
              proc_ref = proc_ref->GetNext();
            }
          }
          channel_node = channel_node->GetNext();
        }
        actions_found = new_actions_found;
      } // end for (connections)
      for (unsigned int i = 0; i < actions_found.GetCount(); ++i)
      {
        action new_action;
        new_action.set_name( channel_communication_visible_name );
        new_action.set_parameters( actions_found[i].get_parameters() );
        visibles.Add( new_action );
        found = true;
      }
    }
    channel_communication_node = channel_communication_node->GetNext();
  }
  return visibles;
}

list_of_action grape::mcrl2gen::infer_process_actions(wxXmlNode *p_doc_root, wxString &p_diagram_id, ATermAppl &datatype_spec)
{
  list_of_action actions;

  list_of_decl preamble_params;
  list_of_decl_init preamble_vars;

  wxArrayString to_process, processed;
  to_process.Empty();
  processed.Empty();

  to_process.Add(p_diagram_id);

  while(!to_process.IsEmpty())
  {
    wxString curr = to_process[0];
    to_process.RemoveAt(0);
    if(processed.Index(curr) != wxNOT_FOUND)
    {
      // diagram is already processed
      continue;
    }
    processed.Add(curr);

    // determine actions
    //wxString curr_id = infer_process_diagram_id(p_doc_root, curr);
    wxXmlNode *curr_diag = get_process_diagram(p_doc_root, curr);
    wxString diagram_name = get_child_value(curr_diag, _T("name"));
    parse_preamble(curr_diag, preamble_params, preamble_vars, datatype_spec);
    list_of_action acts = process_diagram_mcrl2_action(curr_diag, preamble_params, preamble_vars, datatype_spec);
    for(unsigned int i=0; i<acts.GetCount(); ++i)
    {
	    bool found = false;
	    for(unsigned int j=0; j<actions.GetCount(); ++j)
	    {
	      if ( acts[i].get_name() == actions[j].get_name() )
		    {
		      if ( acts[i].get_parameters().GetCount() == actions[j].get_parameters().GetCount() )
		      {
			      if ( acts[i].get_parameters().GetCount() == 0 )
			      {
			        found = true;
              break;
			      }
			      else
			      {
			        list_of_dataexpression acts_params = acts[i].get_parameters();
			        list_of_dataexpression actions_params = actions[j].get_parameters();
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
	      actions.Add(acts[i]);
	    }
    }

    // determine references contained in diagram
    wxXmlNode *objects = get_child(curr_diag, _T("objectlist"));
    if(objects == 0)
    {
      // ERROR
      cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii()
           << " contains no objects." << endl;
      return actions;
    }
    wxXmlNode *refs = get_child(objects, _T("referencestatelist"));
    if(refs == 0)
    {
      // ERROR
      cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii()
           << " does not contain any process references." << endl;
      return actions;
    }

    for(wxXmlNode *ref = refs->GetChildren(); ref != 0; ref = ref->GetNext())
    {
      wxString ref_id = get_child_value(ref, _T("propertyof"));
      if(ref_id == wxEmptyString)
      {
        // ERROR
        cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii()
             << " contains process references which do not reference an existing process diagram." << endl;
        return actions;
      }

      to_process.Add(ref_id);
    }
  }

  // done
  return actions;
}

list_of_action grape::mcrl2gen::process_diagram_mcrl2_action(wxXmlNode *p_process_diagram, list_of_decl &p_preamble_parameter_decls, list_of_decl_init &p_preamble_local_var_decls, ATermAppl &datatype_spec)
{
  list_of_action actions;

  if(p_process_diagram->GetName() == _T("processdiagram"))
  {
    // <processdiagram> node

    wxString diagram_name = get_child_value(p_process_diagram, _T("name"));

    // process terminating and nonterminating transitions of this process diagram
    wxXmlNode *child = get_child(p_process_diagram, _T("objectlist"));
    if(child == 0)
    {
      // ERROR: <processdiagram> does not contain <objectlist>
      cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " does not contain any objects." << endl;
      throw CONVERSION_ERROR;
      return actions;
    }
    // child = <objectlist>
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
            cerr << "mCRL2 conversion warning: process diagram " << diagram_name.ToAscii() << " does not contain a terminating transition where it should." << endl;
            break;
          }
          // parse trans_label = <label>
          label trans_label;
          try
          {
            trans_label = parse_transition_label(child_trans, p_preamble_parameter_decls, p_preamble_local_var_decls, diagram_name, datatype_spec);
          }
          catch(...)
          {
            return actions;
          }
          list_of_action tt_actions = trans_label.get_actions();
          WX_APPEND_ARRAY(actions, tt_actions);
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
            cerr << "mCRL2 conversion warning: process diagram " << diagram_name.ToAscii() << " does not contain a non-terminating transition where it should." << endl;
            break;
          }
          // parse trans_label = <label>
          label trans_label;
          try
          {
            trans_label = parse_transition_label(child_trans, p_preamble_parameter_decls, p_preamble_local_var_decls, diagram_name, datatype_spec);
          }
          catch(...)
          {
            return actions;
          }
          list_of_action ntt_actions = trans_label.get_actions();
          WX_APPEND_ARRAY(actions, ntt_actions);
        }
      }
    }
  }
  else
  {
    // ERROR: called with something else than <processdiagram>
    cerr << "mCRL2 conversion error: process_diagram_mcrl2_action called without a valid XML process diagram." << endl;
    throw CONVERSION_ERROR;
    return actions;
  }

  return actions;
}

bool grape::mcrl2gen::parse_reference_parameters(wxXmlNode *p_reference, wxString &p_diagram_name, list_of_varupdate &p_parameter_initialisation, list_of_decl &p_preamble_parameter_decls, ATermAppl &datatype_spec)
{
  p_parameter_initialisation.Empty();

  if(p_reference->GetName() == _T("processreference") || p_reference->GetName() == _T("referencestate"))
  {
    wxString diagram_name = get_child_value(p_reference, _T("name"));

    wxXmlNode *param_list = get_child(p_reference, _T("parameterassignmentlist"));
    if(param_list != 0)
    {
      for(wxXmlNode *param_assignment = param_list->GetChildren(); param_assignment != 0; param_assignment = param_assignment->GetNext())
      {
        varupdate parameter_assignment;
        if ( param_assignment->GetName() == _T( "parameterassignment" ) )
        {
          wxString parameter_assignment_text = param_assignment->GetNodeContent();
          bool valid = parameter_assignment.set_varupdate( parameter_assignment_text );
          if ( !valid )
          {
            // ERROR: parameter assignment is not valid
            cerr << "mCRL2 conversion error: process reference to process diagram " << diagram_name.ToAscii()
                 << " contains an invalid parameter assignment " << parameter_assignment_text.ToAscii() << "." << endl;
            throw CONVERSION_ERROR;
            return false;
          }

          // parse parameter name (identifier)
          string parameter_assignment_id = string(parameter_assignment.get_lhs().mb_str());
          const char *p_a_id = parameter_assignment_id.c_str();
          istringstream iss(p_a_id);
          ATermAppl a_parsed_parameter_assignment_id = parse_identifier(iss);
          if ( a_parsed_parameter_assignment_id == 0 )
          {
            // ERROR: parameter assignment is not valid
            cerr << "mCRL2 conversion error: process reference to process diagram " << diagram_name.ToAscii() << " contains an invalid parameter assignment " << parameter_assignment_text.ToAscii()
                 << ". The parameter name '" << parameter_assignment.get_lhs().ToAscii() << "' could not be parsed." << endl;
            throw CONVERSION_ERROR;
            return false;
          }

          // parse parameter assignment value (dataexpression)
          string parameter_assignment_expr = string(parameter_assignment.get_rhs().mb_str());
          const char *p_a_expr = parameter_assignment_expr.c_str();
          istringstream iss2(p_a_expr);
          ATermAppl a_parsed_parameter_assignment_expr = parse_data_expr(iss2);
          if ( a_parsed_parameter_assignment_expr == 0 )
          {
            // ERROR: parameter assignment is not valid
            cerr << "mCRL2 conversion error: process reference to process diagram " << diagram_name.ToAscii() << " contains an invalid parameter assignment " << parameter_assignment_text.ToAscii()
                 << ". The parameter assignment value '" << parameter_assignment.get_rhs().ToAscii() << "' could not be parsed." << endl;
            throw CONVERSION_ERROR;
            return false;
          }
          else
          {
            // parse succeeded: try to type check
            // get type
            list_of_decl variable_decls;  // must be empty
            list_of_decl_init preamble_variable_decls; // must be empty
            wxString parameter_assignment_type = infer_type(parameter_assignment.get_lhs(), p_preamble_parameter_decls, preamble_variable_decls, variable_decls);
            if ( parameter_assignment_type.IsEmpty() )
            {
              // type check whithout predefined type
              ATermAppl a_type_checked_parameter_assignment_expr = type_check_data_expr( a_parsed_parameter_assignment_expr, NULL, datatype_spec );
              if ( a_type_checked_parameter_assignment_expr == 0 )
              {
                // ERROR: parameter assignment is not valid
                cerr << "mCRL2 conversion error: process reference to process diagram " << diagram_name.ToAscii() << " contains an invalid parameter assignment " << parameter_assignment_text.ToAscii()
                     << ". The parameter assignment value '" << parameter_assignment.get_rhs().ToAscii() << "' could not be type checked." << endl;
                throw CONVERSION_ERROR;
                return false;
              }
            }
            else
            {
              // parse type
              string parameter_assignment_sort = string(parameter_assignment_type.mb_str());
              const char *p_a_t_expr = parameter_assignment_sort.c_str();
              istringstream iss3(p_a_t_expr);
              ATermAppl a_parsed_parameter_assignment_type = parse_sort_expr(iss3);
              if ( a_parsed_parameter_assignment_type == 0 )
              {
                // ERROR: parameter assignment type is not valid
                cerr << "mCRL2 conversion error: process reference to process diagram " << diagram_name.ToAscii() << " contains an invalid parameter assignment " << parameter_assignment_text.ToAscii()
                     << ". The parameter type could not be parsed." << endl;
                throw CONVERSION_ERROR;
                return false;
              }
              else
              {
                // type check type
                ATermAppl a_type_checked_parameter_assignment_type = type_check_sort_expr( a_parsed_parameter_assignment_type, datatype_spec );
                if ( a_type_checked_parameter_assignment_type == 0 )
                {
                  // ERROR: parameter assignment type is not valid
                  cerr << "mCRL2 conversion error: process reference to process diagram " << diagram_name.ToAscii() << " contains an invalid parameter assignment " << parameter_assignment_text.ToAscii()
                       << ". The parameter type could not be type checked." << endl;
                  throw CONVERSION_ERROR;
                  return false;
                }
                else
                {
                  // type check value with predefined sort
                  ATermAppl a_type_checked_parameter_assignment_expr = type_check_data_expr( a_parsed_parameter_assignment_expr, a_type_checked_parameter_assignment_type, datatype_spec );
                  if ( a_type_checked_parameter_assignment_expr == 0 )
                  {
                    // ERROR: parameter assignment is not valid
                    cerr << "mCRL2 conversion error: process reference to process diagram " << diagram_name.ToAscii() << " contains an invalid parameter assignment " << parameter_assignment_text.ToAscii()
                         << ". The parameter assignment value '" << parameter_assignment.get_rhs().ToAscii() << "' could not be type checked." << endl;
                    throw CONVERSION_ERROR;
                    return false;
                  }
                }
              }
            }
          }
          p_parameter_initialisation.Add( parameter_assignment );
        }
        else
        {
          // ERROR: parameter assignment is not valid
          cerr << "mCRL2 conversion error: process reference to process diagram " << diagram_name.ToAscii()
               << " contains an invalid parameter assignment." << endl;
          throw CONVERSION_ERROR;
          return false;
        }
      }
    }
    else
    {
      // ERROR: <referencestate> does not contain <parameterassignmentlist>
      cerr << "mCRL2 conversion error: process diagram " << p_diagram_name.ToAscii()
           << " has a process reference " << diagram_name.ToAscii()
           << " that does not contain any parameter assignments." << endl;
      throw CONVERSION_ERROR;
      return false;
    }
    return true;
  }
  cerr << "mCRL2 conversion error: parse_reference_parameters called without a valid XML process reference." << endl;
  throw CONVERSION_ERROR;
  return false;
}

bool grape::mcrl2gen::validate_diagram_names(wxXmlNode *p_doc_root)
{
  wxXmlNode *process_diagrams = get_child(p_doc_root, _T("processdiagramlist"));
  wxXmlNode *architecture_diagrams = get_child(p_doc_root, _T("architecturediagramlist"));
  if(process_diagrams == 0 || architecture_diagrams == 0)
  {
    return false;
  }

  wxArrayString diagram_names;
  diagram_names.Empty();

  for(wxXmlNode *process_diagram = process_diagrams->GetChildren(); process_diagram != 0; process_diagram = process_diagram->GetNext())
  {
    wxString diagram_name = get_child_value(process_diagram, _T("name"));
    diagram_names.Add(diagram_name);
  }
  for(wxXmlNode *architecture_diagram = architecture_diagrams->GetChildren(); architecture_diagram != 0; architecture_diagram = architecture_diagram->GetNext())
  {
    wxString diagram_name = get_child_value(architecture_diagram, _T("name"));
    diagram_names.Add(diagram_name);
  }

  bool is_valid = true;
  for(unsigned int i=0; i<diagram_names.GetCount(); ++i)
  {
    for(unsigned int j=0; j<diagram_names.GetCount(); ++j)
    {
      if(diagram_names[i] == diagram_names[j] && i < j)
      {
        cerr << "+specification is not valid: diagram name " << diagram_names[i].ToAscii() << " is not unique." << endl;
        is_valid = false;
      }
    }
  }

  return is_valid;
}
