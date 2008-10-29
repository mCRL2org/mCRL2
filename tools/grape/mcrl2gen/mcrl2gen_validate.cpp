// Author(s): VitaminB100
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

#include <wx/tokenzr.h>

#include "mcrl2gen.h"
#include "mcrl2gen_validate.h"
#include "libgrape/label.h"
#include "libgrape/preamble.h"
#include "libgrape/decl.h"

#include <aterm2.h>

// mCRL2 core libraries
#include "mcrl2/core/detail/struct.h"               // ATerm building blocks.
#include "mcrl2/core/messaging.h"                   // Library for messaging.

// mCRL2 core/detail libraries
#include "mcrl2/core/parse.h"                // Parse library.
#include "mcrl2/core/typecheck.h"            // Type check library.

// mCRL2 utility libraries
#include "mcrl2/utilities/aterm_ext.h"

using namespace grape::mcrl2gen;
using namespace grape::libgrape;
using namespace mcrl2::utilities;
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
  bool blocked_list_is_valid = false, visible_list_is_valid = false, channel_communication_list_is_valid = false,
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
      architecture_reference_list_is_valid = validate_architecture_reference_list(p_doc_root, p_architecture_diagram, curr_list);
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
      if(curr_list->GetName() == _T("blockedlist"))
      {
        blocked_list_is_valid = validate_blocked_list(p_architecture_diagram, curr_list);
      }
      else if(curr_list->GetName() == _T("visiblelist"))
      {
        visible_list_is_valid = validate_visible_list(p_architecture_diagram, curr_list);
      }
      else if(curr_list->GetName() == _T("channelcommunicationlist"))
      {
        channel_communication_list_is_valid = validate_channel_communication_list(p_architecture_diagram, curr_list);
      }
      else if(curr_list->GetName() == _T("channellist"))
      {
        channel_list_is_valid = validate_channel_list(p_doc_root, p_architecture_diagram, curr_list, datatype_spec);
      }
    }
  }

  if(!blocked_list_is_valid || !visible_list_is_valid || !channel_communication_list_is_valid || !channel_list_is_valid || !architecture_reference_list_is_valid || !process_reference_list_is_valid)
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

// why was this added?????
// datspec += _T("\ninit tau;");

  // try to parse the mCRL2 specification
  string mcrl2_spec = string(datspec.mb_str());
  const char *m_spec = mcrl2_spec.c_str();
  istringstream iss(m_spec);
//  ATermAppl a_parsed_mcrl2_spec = parse_proc_spec(iss);
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
      is_valid = false;
    }
    else if(referenced_diagram == 0)
    {
      // target diagram not found
      cerr << "+specification is not valid: process diagram " << diagram_name.ToAscii()
           << " contains a process reference that does not refer to an existing process diagram." << endl;
      is_valid = false;
    }
    if(!is_valid)
    {
      return false;
    }

    wxString ref_name = get_child_value(referenced_diagram, _T("name"));
    wxString ref_id = get_child_value(referenced_diagram, _T("id"));

    // check parameter initialisation
    list_of_decl_init ref_inits;
    ref_inits.Empty();
    wxXmlNode *ass_list = get_child(ref_state, _T("parameterassignmentlist"));
    bool has_parameters = true;
    if(ass_list == 0)
    {
      // no list of parameter assignments
      has_parameters = false;
    }

    if(ass_list != 0)
    {
      wxString params = ass_list->GetNodeContent();
      ref_inits = parse_reference_parameters(params);
    }

    if(is_valid)
    {
      // parsed parameter initialisation to ref_inits
      list_of_decl preamble_params;
      list_of_decl_init preamble_vars;
      try
      {
        parse_preamble(referenced_diagram, preamble_params, preamble_vars, datatype_spec);
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
          if(ref_inits[j].get_name() == preamble_params[i].get_name())
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
  // for each label, infer if the beginstates exist and try to parse it
  bool is_valid = true;
  wxString diagram_name = get_child_value(p_process_diagram, _T("name"));

  for(wxXmlNode *transition = p_term_trans_list->GetChildren(); transition != 0; transition = transition->GetNext())
  {
    wxString trans_from = get_child_value(transition, _T("from"));

    if(trans_from == wxEmptyString || trans_from == _T("-1"))
    {
      cerr << "+specification is not valid: process diagram " << diagram_name.ToAscii()
           << " contains a terminating transition that is not conntected to a state or process reference." << endl;
      is_valid = false;
    }

    wxString trans_label = get_child_value(transition, _T("label"));
    arr_action_type trans_actions;
    if(trans_label == wxEmptyString)
    {
      // an empty label is always correct (translated as tau)
      continue;
    }
    bool label_valid;
    try
    {
      label_valid = parse_transition_label_action(p_process_diagram, p_preamble_parameters, p_preamble_variables, trans_label, trans_actions, datatype_spec);
    }
    catch(...)
    {
      is_valid = false;
    }
    if(!label_valid)
    {
      is_valid = false;
    }
  }

  return is_valid;
}

bool grape::mcrl2gen::validate_nonterminating_transition_list(wxXmlNode *p_process_diagram, wxXmlNode *p_trans_list, list_of_decl &p_preamble_parameters, list_of_decl_init &p_preamble_variables, ATermAppl &datatype_spec)
{
  // for each label, infer if the begin- and endstates exist and try to parse it
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

    wxString trans_label = get_child_value(transition, _T("label"));
    if(trans_label == wxEmptyString)
    {
      // an empty transition label is always correct (translated as tau)
      continue;
    }
    arr_action_type trans_actions;
    bool label_valid;
    try
    {
      label_valid = parse_transition_label_action(p_process_diagram, p_preamble_parameters, p_preamble_variables, trans_label, trans_actions, datatype_spec);
    }
    catch(...)
    {
      is_valid = false;
    }
    if(!label_valid)
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
	wxString var = local_var->GetNodeContent();
	// process local variable declaration
        wxStringTokenizer tkt( var, _T(":") );
        if ( tkt.CountTokens() != 2 || var.IsEmpty() )
        {
          // ERROR: variable declaration is not valid
          cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid local variable " << var.ToAscii() << " declaration in its preamble." << endl;
          throw CONVERSION_ERROR;
          return false;
        }
        wxString var_name = tkt.GetNextToken();
        var_name.Trim( true );
        var_name.Trim( false );
        wxStringTokenizer tks( var_name );
        if ( tks.CountTokens() != 1 || var_name.IsEmpty() )
        {
          // ERROR: variable declaration is not valid
          cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid local variable " << var.ToAscii() << " declaration in its preamble." << endl;
          throw CONVERSION_ERROR;
          return false;
        }
        wxString var_rest = tkt.GetNextToken();
        wxStringTokenizer tkr( var_rest, _T("=") );
        if ( tkr.CountTokens() != 2 || var_rest.IsEmpty() )
        {
          // ERROR: variable declaration is not valid
          cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid local variable " << var.ToAscii() << " declaration in its preamble." << endl;
          throw CONVERSION_ERROR;
          return false;
        }
        wxString var_type = tkr.GetNextToken();
        var_type.Trim( true );
        var_type.Trim( false );
        tks.SetString( var_type );
        if ( tks.CountTokens() != 1 || var_type.IsEmpty() )
        {
          // ERROR: variable declaration is not valid
          cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid local variable " << var.ToAscii() << " declaration in its preamble." << endl;
          throw CONVERSION_ERROR;
          return false;
        }
        wxString var_val = tkr.GetNextToken();
        var_val.Trim( true );
        var_val.Trim( false );
        tks.SetString( var_val );
        if ( tks.CountTokens() != 1 || var_val.IsEmpty() )
        {
          // ERROR: variable declaration is not valid
          cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid local variable " << var.ToAscii() 
               << " declaration in its preamble." << endl;
          throw CONVERSION_ERROR;
          return false;
        }

        // parse local variable name (identifier)
        string local_var_id = string(var_name.mb_str());
        const char *l_v_id = local_var_id.c_str();
        istringstream iss(l_v_id);
        ATermAppl a_parsed_local_var_id = parse_identifier(iss);
        if ( a_parsed_local_var_id == 0 )
        {
          // ERROR: variable declaration is not valid
          cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid local variable <" << var.ToAscii() 
               << "> declaration in its preamble. The variable name '" << var_name.ToAscii() << "' could not be parsed." << endl;
          throw CONVERSION_ERROR;
          return false;
        }

        // parse local variable type (sortexpression)
        string local_var_sort = string(var_type.mb_str());
        const char *l_v_sort = local_var_sort.c_str();
        istringstream iss1(l_v_sort);
        ATermAppl a_parsed_local_var_sort = parse_sort_expr(iss1);
        ATermAppl a_type_checked_local_var_sort;
        if ( a_parsed_local_var_sort == 0 )
        {
          // ERROR: variable declaration is not valid
          cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid local variable <" << var.ToAscii() 
               << "> declaration in its preamble. The variable type '" << var_type.ToAscii() << "' could not be parsed." << endl;
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
            cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid local variable <" << var.ToAscii() 
                 << "> declaration in its preamble. The variable type '" << var_type.ToAscii() << "' could not be type checked." << endl;
            throw CONVERSION_ERROR;
            return false;
          }
        }

        // parse local variable value (dataexpression)
        string local_var_expr = string(var_val.mb_str());
        const char *l_v_expr = local_var_expr.c_str();
        istringstream iss2(l_v_expr);
        ATermAppl a_parsed_local_var_expr = parse_data_expr(iss2);
        if ( a_parsed_local_var_expr == 0 )
        {
          // ERROR: variable declaration is not valid
          cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid local variable <" << var.ToAscii() 
               << "> declaration in its preamble. The variable value '" << var_val.ToAscii() << "' could not be parsed." << endl;
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
            cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid local variable <" << var.ToAscii() 
                 << "> declaration in its preamble. The variable value '" << var_val.ToAscii() << "' could not be type checked." << endl;
            throw CONVERSION_ERROR;
            return false;
          }
        }
        // save declaration
        decl_init var_decl;
        var_decl.set_name( var_name );
        var_decl.set_type( var_type );
        var_decl.set_value( var_val );
        p_preamble_local_var_decls.Add( var_decl );
      }
    }

    if(param_list != 0)
    {
      for(wxXmlNode *parameter = param_list->GetChildren(); parameter != 0; parameter = parameter->GetNext())
      {
        wxString param = parameter->GetNodeContent();
	// process parameter declaration
        wxStringTokenizer tkt( param, _T(":") );
        if ( tkt.CountTokens() != 2 || param.IsEmpty() )
        {
          // ERROR: parameter declaration is not valid
          cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid parameter <" << param.ToAscii() << "> declaration in its preamble." << endl;
          throw CONVERSION_ERROR;
          return false;
        }
        wxString param_name = tkt.GetNextToken();
        param_name.Trim( true );
        param_name.Trim( false );
        wxStringTokenizer tks( param_name );
        if ( tks.CountTokens() != 1 || param_name.IsEmpty() )
        {
          // ERROR: parameter declaration is not valid
          cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid parameter <" << param.ToAscii() << "> declaration in its preamble." << endl;
          throw CONVERSION_ERROR;
          return false;
        }
        wxString param_type = tkt.GetNextToken();
        param_type.Trim( true );
        param_type.Trim( false );
        tks.SetString( param_type );
        if ( tks.CountTokens() != 1 || param_type.IsEmpty() )
        {
          // ERROR: variable declaration is not valid
          cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid parameter <" << param.ToAscii() << "> declaration in its preamble." << endl;
          throw CONVERSION_ERROR;
          return false;
        }

        // parse parameter name (identifier)
        string param_id = string(param_name.mb_str());
        const char *par_id = param_id.c_str();
        istringstream iss(par_id);
        ATermAppl a_parsed_param_id = parse_identifier(iss);
        if ( a_parsed_param_id == 0 )
        {
          // ERROR: parameter declaration is not valid
          cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid parameter <" << param.ToAscii() 
               << "> declaration in its preamble. The parameter name '" << param_name.ToAscii() << "' could not be parsed." << endl;
          throw CONVERSION_ERROR;
          return false;
        }

        // parse parameter type (sortexpression)
        string param_sort = string(param_type.mb_str());
        const char *par_sort = param_sort.c_str();
        istringstream iss1(par_sort);
        ATermAppl a_parsed_param_sort = parse_sort_expr(iss1);
        if ( a_parsed_param_sort == 0 )
        {
          // ERROR: parameter declaration is not valid
          cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid parameter <" << param.ToAscii() 
               << "> declaration in its preamble. The parameter type '" << param_type.ToAscii() << "' could not be parsed." << endl;
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
            cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid parameter <" << param.ToAscii() 
                 << "> declaration in its preamble. The parameter type '" << param_type.ToAscii() << "' could not be type checked." << endl;
            throw CONVERSION_ERROR;
            return false;
          }
        }
        // save declaration
        decl param_decl;
        param_decl.set_name( param_name );
        param_decl.set_type( param_type );
        p_preamble_parameter_decls.Add( param_decl );
      }
    }

    return true;
  }

  cerr << "mCRL2 conversion error: parse_preamble called without a valid XML process diagram." << endl;
  return false;
}

bool grape::mcrl2gen::parse_transition_label_action(wxXmlNode *p_process_diagram, list_of_decl &p_preamble_parameter_decls, list_of_decl_init &p_preamble_local_var_decls, wxString p_label, arr_action_type &p_actions, ATermAppl &datatype_spec)
{
/*  wxString diagram_name = get_child_value(p_process_diagram, _T("name"));

  // load label
  label trans_label;
  if(!trans_label.set_text(p_label))
  {
    // ERROR: transition label is not valid
    cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " contains an invalid transition label: "
         << p_label.ToAscii() << endl;
    throw CONVERSION_ERROR;
    return false;
  }
  wxString actions = trans_label.get_actions().get_expression();
  ATermAppl a_parsed_multi_action, a_type_checked_multi_action;
  if(actions != wxEmptyString)
  {
    // parse label to ATerm
    string mult_action = std::string(actions.mb_str());
    const char *str = mult_action.c_str();
    istringstream iss(str);
    a_parsed_multi_action = parse_data_expr(iss);
    if (a_parsed_multi_action == 0)
    {
      cerr << "multiaction in transition label: " << p_label.ToAscii()
           << " in process diagram " << diagram_name.ToAscii()
           << " could not be parsed. " << endl;
      throw CONVERSION_ERROR;
      return false;
    }
    else
    {
      // parse succeeded: try to type check
      a_type_checked_multi_action = type_check_data_expr(a_parsed_multi_action, NULL, datatype_spec, NULL);
      if (a_type_checked_multi_action == 0)
      {
        cerr << "multiaction in transition label: " << p_label.ToAscii()
             << " in process diagram " << diagram_name.ToAscii()
             << " could not be type checked. " << endl;
        throw CONVERSION_ERROR;
        return false;
      }
    }

*/
/*    ATermAppl a_multi_action = parse_mult_act(iss);
    if(a_multi_action == NULL)
    {
      // ERROR: label is not a valid (parameterised) multiaction
      cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii()
           << " contains an invalid (parameterised) multiaction in a transition label: " << p_label.ToAscii() << endl;
      throw CONVERSION_ERROR;
      return false;
    }
*/
/*
    // parse transition label variable declarations
    list_of_decl trans_var_decls;
    trans_var_decls.Empty();
    wxString var_decls = trans_label.get_declarations().get_expression();
    if(var_decls != wxEmptyString)
    {
      // parse
      wxStringTokenizer tkv(var_decls, _T(","));
      while(tkv.HasMoreTokens())
      {
        wxString var_decl_token = tkv.GetNextToken();
        decl v_decl;
        wxStringTokenizer tkvd(var_decl_token, _T(":"));
        wxString var_decl_var = tkvd.GetNextToken();
        if(var_decl_var == wxEmptyString)
        {
          // ERROR: no variable here
          cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii()
               << " contains an invalid parameter declaration in a transition label: " << var_decls.ToAscii() << endl;
          throw CONVERSION_ERROR;
          return false;
        }
        var_decl_var.Trim(true);
        var_decl_var.Trim(false);
        v_decl.set_name(var_decl_var);
        var_decl_var = tkvd.GetNextToken();
        if(var_decl_var == wxEmptyString)
        {
          // ERROR: no type here
          cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii()
               << " contains an invalid parameter declaration in a transition label: " << var_decls.ToAscii() << endl;
          throw CONVERSION_ERROR;
          return false;
        }
        var_decl_var.Trim(true);
        var_decl_var.Trim(false);
        sortexpression var_decl_var_sort;
        var_decl_var_sort.set_expression(var_decl_var);
        v_decl.set_type(var_decl_var_sort);
        trans_var_decls.Add(v_decl);
      }
    }

    // get multiactions
//    ATermList a_actions = (ATermList)ATgetArgument(a_multi_action, 0); // 1st parameter of MultAct is a list of ParamId's
    ATermList a_actions = (ATermList)ATgetArgument(a_type_checked_multi_action, 0); // 1st parameter of MultAct is a list of ParamId's
    int list_length = ATgetLength(a_actions);
    for(int i=0; i<list_length; ++i)
    {
      ATermAppl a_param_id = (ATermAppl)ATelementAt(a_actions, i); // get i'th ParamId
      char *a_action_name = ATwriteToString(ATgetArgument(a_param_id, 0)); // first arg of ParamId is action name
      wxString act_name(a_action_name, wxConvUTF8);
      act_name = act_name.Mid(1, act_name.Len()-2); // contains the name of the action
      ATermList a_param_id_act_type = (ATermList)ATgetArgument(a_param_id, 1); // second arg of ParamId is a list of Ids
      wxString act_type;

      act_type = wxEmptyString;
      bool more_params = false;

      while(ATgetLength(a_param_id_act_type) > 0)
      {
        ATermAppl a_type_id = (ATermAppl)ATgetFirst(a_param_id_act_type);
        ATerm a_type_id_name = ATgetArgument(a_type_id, 0);
        char *a_action_type = ATwriteToString(a_type_id_name);
        wxString t_act_type(a_action_type, wxConvUTF8);
        t_act_type = t_act_type.Mid(1, t_act_type.Len()-2);
        sortexpression typed_act_type = infer_action_type(t_act_type, p_preamble_parameter_decls, p_preamble_local_var_decls, trans_var_decls);

        if(typed_act_type.get_expression() == wxEmptyString)
        {
          cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii()
               << " contains a parameterised action in a transition label of which the type of parameter "
               << t_act_type.ToAscii() << " could not be inferred." << endl;
          throw CONVERSION_ERROR;
          return false;
        }

        if(more_params)
        {
          act_type += _T(" # ");
        }
        act_type += typed_act_type.get_expression();

        more_params = true;
        a_param_id_act_type = (ATermList)ATgetNext(a_param_id_act_type);
      }

      action_type t_action_type;
      t_action_type.m_action = act_name;
      t_action_type.m_type = act_type;
      p_actions.Add(t_action_type);
    }
  }
*/
  return true;
}

wxString grape::mcrl2gen::infer_action_type(wxString &p_type, list_of_decl &p_preamble_parameter_decls, list_of_decl_init &p_preamble_local_var_decls, list_of_decl &p_trans_var_decls)
{
  // search in transition variable declaration first
  for(unsigned int i=0; i<p_trans_var_decls.GetCount(); ++i)
  {
    if(p_trans_var_decls[i].get_name() == p_type)
    {
      return p_trans_var_decls[i].get_type();
    }
  }
  // search in preamble
  for(unsigned int i=0; i<p_preamble_parameter_decls.GetCount(); ++i)
  {
    if(p_preamble_parameter_decls[i].get_name() == p_type)
    {
      return p_preamble_parameter_decls[i].get_type();
    }
  }
  for(unsigned int i=0; i<p_preamble_local_var_decls.GetCount(); ++i)
  {
    if(p_preamble_local_var_decls[i].get_name() == p_type)
    {
      return p_preamble_local_var_decls[i].get_type();
    }
  }

  return sortexpression().get_expression();
}

bool grape::mcrl2gen::validate_blocked_list(wxXmlNode *p_architecture_diagram, wxXmlNode *p_blocked_list)
{
  // a blocked is valid when its <propertyof> tag points to a channel or channel communication
  bool is_valid = true;
  wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));

  for(wxXmlNode *blocked = p_blocked_list->GetChildren(); blocked != 0; blocked = blocked->GetNext())
  {
    wxString blocked_propertyof = get_child_value(blocked, _T("propertyof"));
    bool found_property = false;
    bool propertyof_empty = false;

    if(blocked_propertyof == wxEmptyString || blocked_propertyof == _T("-1"))
    {
      cerr << "+specification is not valid: architecture diagram " << diagram_name.ToAscii()
           << " contains a blocked property that is not connected to a channel or channel communication." << endl;
      is_valid = false;
      propertyof_empty = true;
    }

    wxXmlNode *objects = get_child(p_architecture_diagram, _T("objectlist"));
    // objects != 0, otherwise we couldn't be here
    wxXmlNode *channels = get_child(objects, _T("channellist"));
    if(channels != 0)
    {
      for(wxXmlNode *channel = channels->GetChildren(); channel != 0; channel = channel->GetNext())
      {
        wxString channel_id = get_child_value(channel, _T("id"));

        if(channel_id == blocked_propertyof)
        {
          found_property = true;
          break;
        }
      }
    }
    wxXmlNode *comms = get_child(objects, _T("channelcommunicationlist"));
    if(comms != 0 && !found_property)
    {
      for(wxXmlNode *comm = comms->GetChildren(); comm != 0; comm = comm->GetNext())
      {
        wxString comm_id = get_child_value(comm, _T("id"));

        if(comm_id == blocked_propertyof)
        {
          found_property = true;
          break;
        }
      }
    }

    if(!found_property && !propertyof_empty)
    {
      cerr << "+specification is not valid: architecture diagram " << diagram_name.ToAscii()
           << " contains a blocked property that is not connected to an existing channel or channel communication." << endl;
      is_valid = false;
    }
  }

  return is_valid;
}

bool grape::mcrl2gen::validate_visible_list(wxXmlNode *p_architecture_diagram, wxXmlNode *p_visible_list)
{
  // a visible is valid when its <propertyof> tag points to a channel or channel communication and its <name> tag is not empty
  bool is_valid = true;
  wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));

  for(wxXmlNode *visible = p_visible_list->GetChildren(); visible != 0; visible = visible->GetNext())
  {
    wxString visible_propertyof = get_child_value(visible, _T("propertyof"));
    bool found_property = false;
    bool propertyof_empty = false;

    wxString visible_name = get_child_value(visible, _T("name"));
    if(visible_name == wxEmptyString)
    {
      cerr << "+specification is not valid: architecture diagram " << diagram_name.ToAscii()
           << " contains a visible property that has an empty name." << endl;
      is_valid = false;
    }

    if(visible_propertyof == wxEmptyString || visible_propertyof == _T("-1"))
    {
      cerr << "+specification is not valid: architecture diagram " << diagram_name.ToAscii()
           << " contains a visible property that is not connected to a channel or channel communication." << endl;
      is_valid = false;
      propertyof_empty = true;
    }

    wxXmlNode *objects = get_child(p_architecture_diagram, _T("objectlist"));
    // objects != 0, otherwise we couldn't be here
    wxXmlNode *channels = get_child(objects, _T("channellist"));
    if(channels != 0)
    {
      for(wxXmlNode *channel = channels->GetChildren(); channel != 0; channel = channel->GetNext())
      {
        wxString channel_id = get_child_value(channel, _T("id"));

        if(channel_id == visible_propertyof)
        {
          found_property = true;
          break;
        }
      }
    }
    wxXmlNode *comms = get_child(objects, _T("channelcommunicationlist"));
    if(comms != 0 && !found_property)
    {
      for(wxXmlNode *comm = comms->GetChildren(); comm != 0; comm = comm->GetNext())
      {
        wxString comm_id = get_child_value(comm, _T("id"));

        if(comm_id == visible_propertyof)
        {
          found_property = true;
          break;
        }
      }
    }

    if(!found_property && !propertyof_empty)
    {
      cerr << "+specification is not valid: architecture diagram " << diagram_name.ToAscii()
           << " contains a visible property that is not connected to an existing channel or channel communication." << endl;
      is_valid = false;
    }
  }

  return is_valid;
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
  // and its <onchannelcommunication> tag is also valid
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
        wxArrayString ref_actions;
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
            ref_actions = infer_architecture_visibles(reference);
          }
          catch(...)
          {
          }
        }
        bool found_channel_name = false;
        for(unsigned int i=0; i<ref_actions.GetCount(); ++i)
        {
          if(ref_actions[i] == channel_name)
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

bool grape::mcrl2gen::validate_architecture_reference_list(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, wxXmlNode *p_reference_list)
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

      try
      {
        wxString diag_name = infer_architecture_name(p_doc_root, ref_propertyof);
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
      is_valid = false;
    }
    else if(referenced_diagram == 0)
    {
      // target diagram not found
      cerr << "+specification is not valid: architecture diagram " << diagram_name.ToAscii()
           << " contains a process reference that does not refer to an existing process diagram." << endl;
      is_valid = false;
    }
    if(!is_valid)
    {
      return false;
    }

    wxString ref_name = get_child_value(referenced_diagram, _T("name"));

    // check parameter initialisation
    list_of_decl_init ref_inits;
    ref_inits.Empty();
    wxXmlNode *ass_list = get_child(proc_ref, _T("parameterassignmentlist"));
    bool has_parameters = true;
    if(ass_list == 0)
    {
      // no list of parameter assignments
      has_parameters = false;
    }

    if(ass_list != 0)
    {
      wxString params = ass_list->GetNodeContent();
      ref_inits = parse_reference_parameters(params);
    }

    if(is_valid)
    {
      // parsed parameter initialisation to ref_inits
      list_of_decl preamble_params;
      list_of_decl_init preamble_vars;
      try
      {
        parse_preamble(referenced_diagram, preamble_params, preamble_vars, datatype_spec);
      }
      catch(...)
      {
        return false;
      }

      if(preamble_params.GetCount() != ref_inits.GetCount())
      {
        cerr << "+specification is not valid: architecture diagram " << diagram_name.ToAscii()
             << " contains a process reference to process diagram " << ref_name.ToAscii()
             << " that does not contain the same number of parameters." << preamble_params.GetCount() << "!=" << ref_inits.GetCount() << endl;
        is_valid = false;
      }

      for(unsigned int i=0; i<preamble_params.GetCount(); ++i)
      {
        bool found = false;
        for(unsigned int j=0; j<ref_inits.GetCount(); ++j)
        {
          if(ref_inits[j].get_name() == preamble_params[i].get_name())
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

wxArrayString grape::mcrl2gen::infer_architecture_visibles(wxXmlNode *p_architecture_diagram)
{
  wxArrayString visibles;
  visibles.Empty();

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

    wxXmlNode *vis = get_child(objects, _T("visiblelist"));
    if(vis == 0)
    {
      // ERROR: <objectlist> has no <visiblelist>
      cerr << "mCRL2 conversion error: architecture diagram " << diagram_name.ToAscii()
           << " does not contain any visibles." << endl;
      throw CONVERSION_ERROR;
      return visibles;
    }

    for(wxXmlNode *vis_node = vis->GetChildren(); vis_node != 0; vis_node = vis_node->GetNext())
    {
      // extract visibles
      visibles.Add(get_child_value(vis_node, _T("name")));
    }

    return visibles;
  }

  // ERROR: not called with valid XML node
  cerr << "mCRL2 conversion error: infer_architecture_visibles called with an invalid XML architecture diagram" << endl;
  throw CONVERSION_ERROR;
  return visibles;
}

wxArrayString grape::mcrl2gen::infer_process_actions(wxXmlNode *p_doc_root, wxString &p_diagram_id, ATermAppl &datatype_spec)
{
  wxArrayString actions;
  actions.Empty();

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
    arr_action_type acts = process_diagram_mcrl2_action(curr_diag, preamble_params, preamble_vars);
    for(unsigned int i=0; i<acts.GetCount(); ++i)
    {
      actions.Add(acts[i].m_action);
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

arr_action_type grape::mcrl2gen::process_diagram_mcrl2_action(wxXmlNode *p_process_diagram, list_of_decl &p_preamble_parameter_decls, list_of_decl_init &p_preamble_local_var_decls)
{
  arr_action_type actions;

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
          wxXmlNode *child_trans_label = get_child(child_trans, _T("label"));
          if(child_trans_label == 0)
          {
            // ERROR: <terminatingtransition> does not contain <label>
            cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " has a terminating transition without an associated label." << endl;
            throw CONVERSION_ERROR;
            return actions;
          }
          // child_trans_label = <label>
          ATermAppl expr;
          parse_transition_label_action(p_process_diagram, p_preamble_parameter_decls, p_preamble_local_var_decls, child_trans_label->GetNodeContent(), actions, expr);
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
          wxXmlNode *child_trans_label = get_child(child_trans, _T("label"));
          if(child_trans_label == 0)
          {
            // ERROR: <nonterminatingtransition> does not contain <label>
            cerr << "mCRL2 conversion error: process diagram " << diagram_name.ToAscii() << " has a non-terminating transition without an associated label." << endl;
            return actions;
          }
          // child_trans_label = <label>
          ATermAppl expr1;
          parse_transition_label_action(p_process_diagram, p_preamble_parameter_decls, p_preamble_local_var_decls, child_trans_label->GetNodeContent(), actions, expr1);
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

list_of_decl_init grape::mcrl2gen::parse_reference_parameters(wxString &p_parameter_initialisation)
{
  list_of_decl_init params;
  params.Empty();

  // the parameter initialisation is of the form: param1 := expression1, ..., paramn := expressionn

  wxStringTokenizer tkp(p_parameter_initialisation, _T(";"));
  while(tkp.HasMoreTokens())
  {
    wxString param_text = tkp.GetNextToken();
    decl_init param_init;
    wxString param_var, param_expr;
    int pos1 = param_text.Find(_T(":"));
    int pos2 = param_text.Find(_T("="));
    param_var = param_text.Mid(0, pos1);
    param_var.Trim(true); param_var.Trim(false);
    param_text = param_text.Mid(pos2+1);
    param_expr = param_text;
    param_expr.Trim(true); param_expr.Trim(false);
    param_init.set_name(param_var);
    param_init.set_value(param_expr);
    if(param_var != wxEmptyString && param_expr != wxEmptyString)
    {
      params.Add(param_init);
    }
  }

  return params;
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
