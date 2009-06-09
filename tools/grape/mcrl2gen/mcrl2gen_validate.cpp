// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2gen_validate.cpp
//
// Implements internal functions used when validating process diagrams and architecture diagrams.

#include "mcrl2gen_validate.h"

#include <sstream>
#include "mcrl2/core/detail/struct.h"        // ATerm building blocks.
#include "mcrl2/core/parse.h"                // Parse library.
#include "mcrl2/core/typecheck.h"            // Type check library.
#include "mcrl2/core/print.h"

using namespace grape::mcrl2gen;
using namespace grape::libgrape;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace std;

ATermAppl grape::mcrl2gen::parse_identifier(wxString p_identifier)
{
  istringstream r(string(p_identifier.mb_str()).c_str());
  return mcrl2::core::parse_identifier(r);
}

ATermAppl grape::mcrl2gen::parse_sort_expr(wxString p_sort_expression)
{
  istringstream r(string(p_sort_expression.mb_str()).c_str());
  return mcrl2::core::parse_sort_expr(r);
}

ATermAppl grape::mcrl2gen::parse_data_expr(wxString p_data_expression)
{
  istringstream r(string(p_data_expression.mb_str()).c_str());
  return mcrl2::core::parse_data_expr(r);
}

ATermAppl grape::mcrl2gen::parse_proc_spec(wxString p_proc_spec)
{
 istringstream r(string(p_proc_spec.mb_str()).c_str());
 return mcrl2::core::parse_proc_spec(r);
}

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

wxXmlNode *grape::mcrl2gen::get_diagram(wxXmlNode *p_doc_root, wxString &p_diagram_id)
{
  if(p_doc_root->GetName() == _T("grape"))
  {
    // get process diagrams
    wxXmlNode *proc_diag_list = get_child(p_doc_root, _T("processdiagramlist"));
    if(proc_diag_list == 0)
    {
      // ERROR: <grape> does not contain <processdiagramlist>
      cerr << "The specification does not contain any process diagrams." << endl;
      throw CONVERSION_ERROR;
      return 0;
    }
    // check all process diagrams
    for(wxXmlNode *proc_diag = proc_diag_list->GetChildren(); proc_diag != 0; proc_diag = proc_diag->GetNext())
    {
      if(p_diagram_id == get_child_value(proc_diag, _T("id")))
      {
        // found matching diagram
        return proc_diag;
      }
    }

    // get architecture diagrams
    wxXmlNode *arch_diag_list = get_child(p_doc_root, _T("architecturediagramlist"));
    if(arch_diag_list == 0)
    {
      // ERROR: <grape> does not contain <architecturediagramlist>
      cerr << "The specification does not contain any architecture diagrams." << endl;
      throw CONVERSION_ERROR;
      return 0;
    }
    // check all architecture diagrams
    for(wxXmlNode *arch_diag = arch_diag_list->GetChildren(); arch_diag != 0; arch_diag = arch_diag->GetNext())
    {
      if(p_diagram_id == get_child_value(arch_diag, _T("id")))
      {
        // found matching diagram
        return arch_diag;
      }
    }
  }
  else
  {
    // ERROR: called without valid XML document root
    cerr << "Get_process_diagram called without a valid XML specification." << endl;
    throw CONVERSION_ERROR;
    return 0;
  }

  // no matching diagram found
  return 0;
}

atermpp::table grape::mcrl2gen::get_variable_table(list_of_decl &p_preamble_parameter_decls, list_of_decl_init &p_preamble_local_var_decls, list_of_decl &p_trans_var_decls, ATermAppl &datatype_spec)
{
  // make variable table
  atermpp::table vars;
  for (unsigned int i = 0; i < p_preamble_parameter_decls.GetCount(); ++i)
  {
    decl parameter = p_preamble_parameter_decls.Item(i);
    ATermAppl var_name = parse_identifier(parameter.get_name());
    assert( var_name != 0 );
    ATermAppl parsed_var_type = parse_sort_expr(parameter.get_type());
    assert( parsed_var_type != 0 );
    ATermAppl var_type = type_check_sort_expr(parsed_var_type, datatype_spec);
    assert( var_type != 0 );
    vars.put( var_name, var_type );
  }
  for (unsigned int i = 0; i < p_preamble_local_var_decls.GetCount(); ++i)
  {
    decl_init local_variable = p_preamble_local_var_decls.Item(i);
    ATermAppl var_name = parse_identifier(local_variable.get_name());
    assert( var_name != 0 );
    ATermAppl parsed_var_type = parse_sort_expr( local_variable.get_type() );
    assert( parsed_var_type != 0 );
    ATermAppl var_type = type_check_sort_expr( parsed_var_type, datatype_spec );
    assert( var_type != 0 );
    vars.put( var_name, var_type );
  }
  for (unsigned int i = 0; i < p_trans_var_decls.GetCount(); ++i)
  {
    decl variable = p_trans_var_decls.Item(i);
    ATermAppl var_name = parse_identifier( variable.get_name() );
    assert( var_name != 0 );
    ATermAppl parsed_var_type = parse_sort_expr( variable.get_type() );
    assert( parsed_var_type != 0 );
    ATermAppl var_type = type_check_sort_expr( parsed_var_type, datatype_spec );
    assert( var_type != 0 );
    vars.put( var_name, var_type );
  }
  return vars;
}

list_of_action grape::mcrl2gen::get_architecture_visibles(wxXmlNode *p_doc_root, wxString &p_diagram_id, ATermAppl &datatype_spec)
{
  list_of_action visibles;
  wxXmlNode* arch_diag = get_diagram(p_doc_root, p_diagram_id);
  wxString diagram_name = get_child_value(arch_diag, _T("name"));

  wxXmlNode *objects = get_child(arch_diag, _T("objectlist"));
  if(objects == 0)
  {
    // ERROR: <architecturediagram> has no objects.
    cerr << "Architecture diagram " << diagram_name.ToAscii()
         << " does not contain any objects." << endl;
    throw CONVERSION_ERROR;
    return visibles;
  }

  // get visible channels
  visibles = get_architecture_visible_channels(p_doc_root, diagram_name, objects, datatype_spec);
  // get visible channel communications
  list_of_action visible_channel_communications = get_architecture_visible_channel_communications(p_doc_root, diagram_name, objects, datatype_spec);
  WX_APPEND_ARRAY(visibles, visible_channel_communications);
  return visibles;
}

list_of_action grape::mcrl2gen::get_architecture_visible_channels(wxXmlNode *p_doc_root, wxString &p_diagram_name, wxXmlNode *p_objects, ATermAppl &datatype_spec)
{
  list_of_action visibles;

  // get process references
  wxXmlNode *proc_refs = get_child(p_objects, _T("processreferencelist"));

  // get channels
  wxXmlNode *channels = get_child(p_objects, _T("channellist"));
  if (channels == 0)
  {
    // ERROR: <objectlist> has no <channellist>
    cerr << "mCRL2 conversion error: architecture diagram " << p_diagram_name.ToAscii()
         << " does not contain any channels." << endl;
    throw CONVERSION_ERROR;
    return visibles;
  }

  // loop through channels
  bool found = false;
  wxXmlNode *channel_node = channels->GetChildren();
  while (channel_node != 0)
  {
    // get channel_type
    wxString channel_type = get_child_value(channel_node, _T("channeltype"));
    if (channel_type == _T("visible")) 
    {
      wxString channel_id = get_child_value(channel_node, _T("id"));              // get channel id
      wxString channel_name = get_child_value(channel_node, _T("name"));          // get channel name
      wxString channel_visible_name = get_child_value(channel_node, _T("rename"));// get visible channel name
      if (channel_visible_name.IsEmpty())
      {
        channel_visible_name = channel_name;
      }

      // get reference
      wxString on_reference = get_child_value(channel_node, _T("onreference"));
      // loop through process references
      wxXmlNode *proc_ref = proc_refs->GetChildren();
      while (!found && proc_ref != 0)
      {
        // get_reference id
        wxString proc_ref_id = get_child_value(proc_ref, _T("id"));
        if (proc_ref_id == on_reference)
        {
          // get process diagram id
          wxString process_id = get_child_value(proc_ref, _T("propertyof"));
          // get_actions
          list_of_action actions = get_process_actions(p_doc_root, process_id, datatype_spec);
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

list_of_action grape::mcrl2gen::get_architecture_visible_channel_communications(wxXmlNode *p_doc_root, wxString &p_diagram_name, wxXmlNode *p_objects, ATermAppl &datatype_spec)
{
  list_of_action visibles;

  // get process references
  wxXmlNode *proc_refs = get_child(p_objects, _T("processreferencelist"));

  // get channels
  wxXmlNode *channels = get_child(p_objects, _T("channellist"));
  if (channels == 0)
  {
    // ERROR: <objectlist> has no <channellist>
    cerr << "Architecture diagram " << p_diagram_name.ToAscii()
         << " does not contain any channels." << endl;
    throw CONVERSION_ERROR;
    return visibles;
  }

  // get channel_communications
  wxXmlNode *channel_communications = get_child(p_objects, _T("channelcommunicationlist"));
  if (channel_communications == 0)
  {
    // ERROR: <objectlist> has no <channelcommunicationlist>
    cerr << "Architecture diagram " << p_diagram_name.ToAscii()
         << " does not contain any channel communications." << endl;
    throw CONVERSION_ERROR;
    return visibles;
  }

  bool found = false;
  // loop through channel communications
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
      wxString channel_communication_visible_name = get_child_value(channel_communication_node, _T("name"));
      if (channel_communication_visible_name.IsEmpty())
      {
        // ERROR: visible channel communication is not named
        cerr << "Architecture diagram " << p_diagram_name.ToAscii()
             << " has a visible channel communication that is unnamed." << endl;
        throw CONVERSION_ERROR;
        return visibles;
      }

      wxXmlNode *channel_list = get_child(channel_communication_node, _T("connectionlist"));
      if (channel_list == 0)
      {
        // ERROR: <channelcommunication> has no <connectionlist>
        cerr << "Architecture diagram " << p_diagram_name.ToAscii()
             << " does not contain any channel communication connections." << endl;
        throw CONVERSION_ERROR;
        return visibles;
      }
      
      list_of_action actions_found, new_actions_found;
      // loop through connections
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
          cerr << "Architecture diagram " << p_diagram_name.ToAscii()
               << " does not contain any channel communication connection reference." << endl;
          throw CONVERSION_ERROR;
          return visibles;
        }

        bool channel_found = false;
        new_actions_found.Empty();
        // loop through channels
        wxXmlNode *channel_node = channels->GetChildren();
        while (!channel_found && channel_node != 0)
        {
          wxString channel_id = get_child_value(channel_node, _T("id"));    // get channel id
          wxString channel_name = get_child_value(channel_node, _T("name"));// get channel name
          if (channel_id == channel_connection)
          {
            channel_found = true;
            // get reference
            wxString on_reference = get_child_value(channel_node, _T("onreference"));
            bool proc_found = false;
            // loop through process references
            wxXmlNode *proc_ref = proc_refs->GetChildren();
            while (!proc_found && proc_ref != 0)
            {
              // get_reference_id
              wxString proc_ref_id = get_child_value(proc_ref, _T("id"));
              if (proc_ref_id == on_reference)
              {
                proc_found = true;
                // get process diagram id
                wxString process_id = get_child_value(proc_ref, _T("propertyof"));
                // get_actions
                list_of_action actions = get_process_actions(p_doc_root, process_id, datatype_spec);
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
                        act_found = true;
                        list_of_dataexpression actions_params = actions[i].get_parameters();
                        list_of_dataexpression actions_found_params = actions_found[j].get_parameters();
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
                    } // end for (actions_found)
                    if (act_found || actions_found.IsEmpty())
                    {
                      new_actions_found.Add( actions[i] );
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

list_of_action grape::mcrl2gen::get_process_actions(wxXmlNode *p_doc_root, wxString &p_diagram_id, ATermAppl &datatype_spec)
{
  list_of_action actions;
  list_of_action acts;

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
    wxXmlNode *curr_diag = get_diagram(p_doc_root, curr);
    wxString diagram_name = get_child_value(curr_diag, _T("name"));
    validate_preamble(curr_diag, preamble_params, preamble_vars, datatype_spec);
    acts.Empty();

    // process terminating and nonterminating transitions of this process diagram
    wxXmlNode *objects = get_child(curr_diag, _T("objectlist"));
    if(objects == 0)
    {
      // ERROR: <processdiagram> does not contain <objectlist>
      cerr << "Process diagram " << diagram_name.ToAscii() << " does not contain any objects." << endl;
      throw CONVERSION_ERROR;
      return actions;
    }
    // child = <objectlist>
    for(wxXmlNode *child = objects->GetChildren(); child != 0; child = child->GetNext())
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
            cerr << "Process diagram " << diagram_name.ToAscii() << " does not contain a terminating transition where it should." << endl;
            break;
          }
          // parse trans_label = <label>
          label trans_label;
          try
          {
            validate_transition_label(child_trans, preamble_params, preamble_vars, trans_label, diagram_name, datatype_spec);
          }
          catch(...)
          {
            return actions;
          }
          list_of_action tt_actions = trans_label.get_actions();
          WX_APPEND_ARRAY(acts, tt_actions);
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
            cerr << "Process diagram " << diagram_name.ToAscii() << " does not contain a non-terminating transition where it should." << endl;
            break;
          }
          // parse trans_label = <label>
          label trans_label;
          try
          {
            validate_transition_label(child_trans, preamble_params, preamble_vars, trans_label, diagram_name, datatype_spec);
          }
          catch(...)
          {
            return actions;
          }
          list_of_action ntt_actions = trans_label.get_actions();
          WX_APPEND_ARRAY(acts, ntt_actions);
        }
      }
    }

    compact_list_action(acts, actions);

    wxXmlNode *refs = get_child(objects, _T("referencestatelist"));
    if(refs == 0)
    {
      // ERROR
      cerr << "Process diagram " << diagram_name.ToAscii()
           << " does not contain any process references." << endl;
      return actions;
    }

    for(wxXmlNode *ref = refs->GetChildren(); ref != 0; ref = ref->GetNext())
    {
      wxString ref_id = get_child_value(ref, _T("propertyof"));
      if(ref_id.IsEmpty())
      {
        // ERROR
        cerr << "Process diagram " << diagram_name.ToAscii()
             << " contains process references which do not reference an existing process diagram." << endl;
        return actions;
      }

      to_process.Add(ref_id);
    }
  }

  // done
  return actions;
}

void grape::mcrl2gen::compact_list_action(list_of_action &p_actions, list_of_action &new_actions)
{
  // compact members
  for (unsigned int i=0; i<p_actions.GetCount(); ++i)
  {
    bool found = false;
    for (unsigned int j=0; j<new_actions.GetCount(); ++j)
    {
      if (new_actions[j].get_name() == p_actions[i].get_name())
      {
        list_of_dataexpression acts_params = new_actions[j].get_parameters();
        list_of_dataexpression actions_params = p_actions[i].get_parameters();
        if (acts_params.GetCount() == actions_params.GetCount())
        {
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
    if (!found)
    {
      new_actions.Add(p_actions[i]);
    }
  }
}

bool grape::mcrl2gen::is_reference_acyclic(wxXmlNode *p_doc_root, wxArrayString p_checked)
{
  // check the references in the last diagram added to p_checked
  wxXmlNode *diag = get_diagram(p_doc_root, p_checked.Last());
  wxXmlNode *objects = 0;
  wxXmlNode *arch_ref_list = 0;
  wxXmlNode *proc_ref_list = 0;
  wxXmlNode *init_des_list = 0;
  // only ask for child if the diagram exists
  if (diag != 0) 
  {
    objects = get_child(diag, _T("objectlist"));
    // only ask for lists if the object exists
    if (objects != 0)
    {
      arch_ref_list = get_child(objects, _T("architecturereferencelist"));
      proc_ref_list = get_child(objects, _T("referencestatelist"));
      init_des_list = get_child(objects, _T("initialdesignatorlist"));
    }
  }

  if (arch_ref_list != 0)
  {
    // loop through architecture references
    for(wxXmlNode *arch_ref = arch_ref_list->GetChildren(); arch_ref != 0; arch_ref = arch_ref->GetNext())
    {
      wxString ref_propertyof = get_child_value(arch_ref, _T("propertyof"));
      // if the reference has the same name as the refs in p_checked, not acyclic
      for(unsigned int i=0; i<p_checked.GetCount(); ++i)
      {
        if(p_checked[i] == ref_propertyof)
        {
          wxString ref_name = get_child_value(arch_ref, _T("name"));
          cerr << "Architecture diagram " << ref_name.ToAscii()
               << " contains an (indirect) reference to itself." << endl;
          return false;
        }
      }
    }

    // loop through architecture references
    for(wxXmlNode *arch_ref = arch_ref_list->GetChildren(); arch_ref != 0; arch_ref = arch_ref->GetNext())
    {
      wxString ref_propertyof = get_child_value(arch_ref, _T("propertyof"));
      p_checked.Add(ref_propertyof);
      if (!is_reference_acyclic(p_doc_root, p_checked))
      {
        return false;
      }
    }
  }
  else if (proc_ref_list != 0)
  {
    // loop through process references
    for(wxXmlNode *proc_ref = proc_ref_list->GetChildren(); proc_ref != 0; proc_ref = proc_ref->GetNext())
    {
      wxString ref_propertyof = get_child_value(proc_ref, _T("propertyof"));
      // if the reference has the same name as the refs in p_checked, not acyclic
      for(unsigned int i=0; i<p_checked.GetCount(); ++i)
      {
        if(p_checked[i] == ref_propertyof)
        {
          wxString ref_name = get_child_value(proc_ref, _T("name"));
          cerr << "Process diagram " << ref_name.ToAscii()
               << " contains an (indirect) reference to itself." << endl;
          return false;
        }
      }
    }

    // loop through initial designators
    for (wxXmlNode *init_des = init_des_list->GetChildren(); init_des != 0; init_des = init_des->GetNext())
    {
      wxString property_of = get_child_value(init_des, _T("propertyof"));
      // loop through process references
      for(wxXmlNode *proc_ref = proc_ref_list->GetChildren(); proc_ref != 0; proc_ref = proc_ref->GetNext())
      {
        wxString ref_id = get_child_value(proc_ref, _T("id"));
        // check if initial designator designates process reference
        if (property_of == ref_id)
        {
          wxString ref_propertyof = get_child_value(proc_ref, _T("propertyof"));
          p_checked.Add(ref_propertyof);
          if (!is_reference_acyclic(p_doc_root, p_checked))
          {
            return false;
          }
        }
      }
    }
  }
  return true;
}

bool grape::mcrl2gen::validate(wxXmlDocument &p_spec)
{
  // initialise variables
  wxXmlNode *doc_root = p_spec.GetRoot();
  bool is_valid = true;

  // validate datatype specification
  ATermAppl datatype_spec;
  is_valid &= validate_datatype_specification(doc_root, datatype_spec);

  // validate all process diagrams
  wxXmlNode *process_diagrams = get_child(doc_root, _T("processdiagramlist"));
  for(wxXmlNode *process_diagram = process_diagrams->GetChildren(); process_diagram != 0; process_diagram = process_diagram->GetNext())
  {
    is_valid &= validate_process_diagram(doc_root, process_diagram, datatype_spec);
  }

  // validate all architecture diagrams
  wxXmlNode *architecture_diagrams = get_child(doc_root, _T("architecturediagramlist"));
  for(wxXmlNode *architecture_diagram = architecture_diagrams->GetChildren(); architecture_diagram != 0; architecture_diagram = architecture_diagram->GetNext())
  {
    is_valid &= validate_architecture_diagram(doc_root, architecture_diagram, datatype_spec);
  }

  // validate diagram names
  is_valid &= validate_diagram_names(doc_root);

  if(is_valid)
  {
    // check architecture reference acylicy
    for(wxXmlNode *architecture_diagram = architecture_diagrams->GetChildren(); architecture_diagram != 0; architecture_diagram = architecture_diagram->GetNext())
    {
      wxString diagram_id = get_child_value(architecture_diagram, _T("id"));
      wxArrayString diagram_ids;
      diagram_ids.Add(diagram_id);
      is_valid &= is_reference_acyclic(doc_root, diagram_ids);
    }
  }

  if(!is_valid)
  {
    // cerr << "Specification is not valid." << endl;
    return false;
  }
 // cerr << "Specification is valid.";
  return true;
}

bool grape::mcrl2gen::validate_datatype_specification(wxXmlDocument &p_spec)
{
  wxXmlNode *doc_root = p_spec.GetRoot();
  ATermAppl datatype_spec;
  if (!validate_datatype_specification(doc_root, datatype_spec))
  {
    return false;
  }
  cerr << "Data type specification is valid." << endl;
  return true;
}

bool grape::mcrl2gen::validate_datatype_specification(wxXmlNode *p_doc_root, ATermAppl &datatype_spec)
{
  // initialise variables
  datatype_spec = gsMakeEmptyDataSpec();

  // get datatype specification list
  wxXmlNode *datatype_specification_list = get_child(p_doc_root, _T("datatypespecificationlist"));

  // get datatype specification
  wxString datatype_specification = get_child_value(datatype_specification_list, _T("datatypespecification"));

  // make datatype specification
  if (!datatype_specification.IsEmpty()) {
    // try to parse the mCRL2 specification
    istringstream r(string(datatype_specification.mb_str()).c_str());
    ATermAppl a_parsed_mcrl2_datatype_specification = parse_data_spec(r);
    if(a_parsed_mcrl2_datatype_specification == 0)
    {
      cerr << "Could not parse the data type specification." << endl;
      return false;
    }
    else
    {
      // parse succeeded: try to type check
      ATermAppl a_type_checked_mcrl2_datatype_specification = type_check_data_spec(a_parsed_mcrl2_datatype_specification);
      if(a_type_checked_mcrl2_datatype_specification == 0)
      {
        cerr << "Could not type check the data type specification." << endl;
        return false;
      }
      // update datatype specification
      datatype_spec = a_type_checked_mcrl2_datatype_specification;
    }
  }
  return true;
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
    process_diagram = get_diagram(doc_root, p_diagram_id);
  }
  catch(...)
  {
    cerr << "Process diagram " << p_diagram_id.ToAscii() << " is not present." << endl;
    return false;
  }

  if(!validate_process_diagram(doc_root, process_diagram, datatype_spec))
  {
    //cerr << "Process diagram is not valid." << endl;
    return false;
  }

  cerr << "Process diagram is valid.";
  return true;
}

bool grape::mcrl2gen::validate_process_diagram(wxXmlNode *p_doc_root, wxXmlNode *p_process_diagram, ATermAppl &datatype_spec)
{
  // initialise variables
  bool  designator_list_is_valid = true, 
        ref_state_list_is_valid  = true, 
        state_list_is_valid      = true,
        term_trans_list_is_valid = true, 
        trans_list_is_valid      = true;
  wxString diagram_id = get_child_value(p_process_diagram, _T("id"));     // get diagram id
  wxString diagram_name = get_child_value(p_process_diagram, _T("name")); // get diagram name
  if(diagram_name.IsEmpty())
  {
    cerr << "Process diagram " << diagram_id.ToAscii() << " has no name." << endl;
    return false;
  }
  // parse process diagram name  
  if (!gsIsUserIdentifier(std::string(diagram_name.fn_str())))
  {
    // ERROR: process name is not an identifier
    cerr << "Process diagram name " << diagram_name.ToAscii() << " is not a valid identifier." << endl;
    return false;
  }

  // validate preamble
  list_of_decl preamble_params;
  list_of_decl_init preamble_vars;
  try
  {
    validate_preamble(p_process_diagram, preamble_params, preamble_vars, datatype_spec);
  }
  catch(...)
  {
    cerr << "Process diagram " << diagram_name.ToAscii() << " has an invalid preamble." << endl;
    return false;
  }

  // validate objects
  wxXmlNode *object_list = get_child(p_process_diagram, _T("objectlist"));
  if(object_list == 0)
  {
    cerr << "Process diagram " << diagram_name.ToAscii() << " contains no list of objects." << endl;
    return false;
  }

  // validate all different objects
  for(wxXmlNode *curr_list = object_list->GetChildren(); curr_list != 0; curr_list = curr_list->GetNext())
  {
    if(curr_list->GetName() == _T("initialdesignatorlist"))
    {
      designator_list_is_valid = validate_initial_designator_list(p_doc_root, p_process_diagram, curr_list);
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
  return designator_list_is_valid && ref_state_list_is_valid && state_list_is_valid && term_trans_list_is_valid && trans_list_is_valid;
}

bool grape::mcrl2gen::validate_preamble(wxXmlNode *p_process_diagram, list_of_decl &p_preamble_parameter_decls, list_of_decl_init &p_preamble_local_var_decls, ATermAppl &datatype_spec)
{
  // initialise variables
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
      cerr << "Process diagram " << diagram_name.ToAscii() << " does not have a preamble." << endl;
      throw CONVERSION_ERROR;
      return false;
    }

    // validate parameter list
    try
    {
      validate_preamble_parameters(child, diagram_name, p_preamble_parameter_decls, datatype_spec);
    }
    catch(...)
    {
      cerr << "Process diagram " << diagram_name.ToAscii() << " has an invalid local variable list." << endl;
      throw CONVERSION_ERROR;
      return false;
    }

    // validate local variable list
    try
    {
      validate_preamble_local_variables(child, diagram_name, p_preamble_local_var_decls, datatype_spec);
    }
    catch(...)
    {
      cerr << "Process diagram " << diagram_name.ToAscii() << " has an invalid local variable list." << endl;
      throw CONVERSION_ERROR;
      return false;
    }
    return true;
  }
  // ERROR: XML invalid
  cerr << "Validate_preamble called without a valid XML process diagram." << endl;
  throw CONVERSION_ERROR;
  return false;
}

bool grape::mcrl2gen::validate_preamble_local_variables(wxXmlNode *p_preamble, wxString &p_diagram_name, list_of_decl_init &p_preamble_local_var_decls, ATermAppl &datatype_spec)
{
  // initialize variables
  wxString diagram_name = p_diagram_name;
  // get local variable list
  wxXmlNode *var_list = get_child(p_preamble, _T("localvariablelist"));
  if(var_list != 0)
  {
    // loop through all local variables
    for(wxXmlNode *local_var = var_list->GetChildren(); local_var != 0; local_var = local_var->GetNext())
    {
      decl_init var_decl;
      if ( local_var->GetName() == _T("var") )
      {
        // get local variable
        wxString var_text = local_var->GetNodeContent();
        if ( !var_decl.set_decl_init( var_text ) )
        {
          // ERROR: variable declaration is not valid
          cerr << "Process diagram " << diagram_name.ToAscii() << " contains an invalid local variable " << var_text.ToAscii()
               << " declaration in its preamble." << endl;
          throw CONVERSION_ERROR;
          return false;
        }

        // parse local variable name (identifier)
        ATermAppl a_parsed_local_var_id = parse_identifier(var_decl.get_name());
        if ( a_parsed_local_var_id == 0 )
        {
          // ERROR: variable declaration is not valid
          cerr << "Process diagram " << diagram_name.ToAscii() << " contains an invalid local variable " << var_text.ToAscii()
               << " declaration in its preamble. The variable name '" << var_decl.get_name().ToAscii() << "' could not be parsed." << endl;
          throw CONVERSION_ERROR;
          return false;
        }

        // parse local variable type (sortexpression)
        ATermAppl a_parsed_local_var_sort = parse_sort_expr(var_decl.get_type());
        if ( a_parsed_local_var_sort == 0 )
        {
          // ERROR: variable declaration is not valid
          cerr << "Process diagram " << diagram_name.ToAscii() << " contains an invalid local variable " << var_text.ToAscii()
               << " declaration in its preamble. The variable type '" << var_decl.get_type().ToAscii() << "' could not be parsed." << endl;
          throw CONVERSION_ERROR;
          return false;
        }

        // type check local variable type (sortexpression)
        ATermAppl a_type_checked_local_var_sort = type_check_sort_expr( a_parsed_local_var_sort, datatype_spec );
        if ( a_type_checked_local_var_sort == 0 )
        {
          // ERROR: variable declaration is not valid
          cerr << "Process diagram " << diagram_name.ToAscii() << " contains an invalid local variable " << var_text.ToAscii()
               << " declaration in its preamble. The variable type '" << var_decl.get_type().ToAscii() << "' could not be type checked." << endl;
          throw CONVERSION_ERROR;
          return false;
        }

        // parse local variable value (dataexpression)
        ATermAppl a_parsed_local_var_expr = parse_data_expr(var_decl.get_value());
        if ( a_parsed_local_var_expr == 0 )
        {
          // ERROR: variable declaration is not valid
          cerr << "Process diagram " << diagram_name.ToAscii() << " contains an invalid local variable " << var_text.ToAscii()
               << " declaration in its preamble. The variable value '" << var_decl.get_value().ToAscii() << "' could not be parsed." << endl;
          throw CONVERSION_ERROR;
          return false;
        }

        // type check local variable value (dataexpression)
        ATermAppl a_type_checked_local_var_expr = type_check_data_expr( a_parsed_local_var_expr, a_type_checked_local_var_sort, datatype_spec );
        if ( a_type_checked_local_var_expr == 0 )
        {
          // ERROR: variable declaration is not valid
          cerr << "Process diagram " << diagram_name.ToAscii() << " contains an invalid local variable " << var_text.ToAscii()
               << " declaration in its preamble. The variable value '" << var_decl.get_value().ToAscii() << "' could not be type checked." << endl;
          throw CONVERSION_ERROR;
          return false;
        }
      }
      else
      {
        // ERROR: XML invalid
        cerr << "Validate_preamble_local_variables called without a valid XML process diagram." << endl;
        throw CONVERSION_ERROR;
        return false;
      }
      // add local variable
      p_preamble_local_var_decls.Add( var_decl );
    }
  }
  return true;
}

bool grape::mcrl2gen::validate_preamble_parameters(wxXmlNode *p_preamble, wxString &p_diagram_name, list_of_decl &p_preamble_parameter_decls, ATermAppl &datatype_spec)
{
  // initialize variables
  wxString diagram_name = p_diagram_name;
  // get parameter list
  wxXmlNode *param_list = get_child(p_preamble, _T("parameterlist"));
  if(param_list != 0)
  {
    // loop through all parameters
    for(wxXmlNode *parameter = param_list->GetChildren(); parameter != 0; parameter = parameter->GetNext())
    {
      decl param_decl;
      if ( parameter->GetName() == _T("param") )
      {
        // get parameter
        wxString param_text = parameter->GetNodeContent();
        if ( !param_decl.set_decl( param_text ) )
        {
          // ERROR: parameter declaration is not valid
          cerr << "Process diagram " << diagram_name.ToAscii() << " contains an invalid parameter " << param_text.ToAscii() << " declaration in its preamble." << endl;
          throw CONVERSION_ERROR;
          return false;
        }

        // parse parameter name (identifier)
        ATermAppl a_parsed_param_id = parse_identifier(param_decl.get_name());
        if ( a_parsed_param_id == 0 )
        {
          // ERROR: parameter declaration is not valid
          cerr << "Process diagram " << diagram_name.ToAscii() << " contains an invalid parameter " << param_text.ToAscii()
               << " declaration in its preamble. The parameter name '" << param_decl.get_name().ToAscii() << "' could not be parsed." << endl;
          throw CONVERSION_ERROR;
          return false;
        }

        // parse parameter type (sortexpression)
        ATermAppl a_parsed_param_sort = parse_sort_expr(param_decl.get_type());
        if ( a_parsed_param_sort == 0 )
        {
          // ERROR: parameter declaration is not valid
          cerr << "Process diagram " << diagram_name.ToAscii() << " contains an invalid parameter " << param_text.ToAscii()
               << " declaration in its preamble. The parameter type '" << param_decl.get_type().ToAscii() << "' could not be parsed." << endl;
          throw CONVERSION_ERROR;
          return false;
        }

        // type check parameter type (sortexpression)
        ATermAppl a_type_checked_param_sort = type_check_sort_expr( a_parsed_param_sort, datatype_spec );          
        if ( a_type_checked_param_sort == 0 )
        {
          
          // ERROR: parameter declaration is not valid
          cerr << "Process diagram " << diagram_name.ToAscii() << " contains an invalid parameter " << param_text.ToAscii()
               << " declaration in its preamble. The parameter type '" << param_decl.get_type().ToAscii() << "' could not be type checked." << endl;
          throw CONVERSION_ERROR;
          return false;
        }
      }
      else
      {
        // ERROR: XML invalid
        cerr << "Validate_preamble_parameters called without a valid XML process diagram." << endl;
        throw CONVERSION_ERROR;
        return false;
      }
      // add parameter
      p_preamble_parameter_decls.Add( param_decl );
    }
  }
  return true;
}

bool grape::mcrl2gen::validate_initial_designator_list(wxXmlNode *p_doc_root, wxXmlNode *p_process_diagram, wxXmlNode *p_designator_list)
{
  // count initial designators
  unsigned int num_designators = 0;
  wxString diagram_name = get_child_value(p_process_diagram, _T("name"));
  for(wxXmlNode *designator = p_designator_list->GetChildren(); designator != 0; designator = designator->GetNext())
  {
    ++num_designators;
  }
  if(num_designators < 1)
  {
    cerr << "Process diagram " << diagram_name.ToAscii() << " does not contain an initial designator." << endl;
    return false;
  }
  if(num_designators > 1)
  {
    cerr << "Process diagram " << diagram_name.ToAscii() << " contains more than one initial designator." << endl;
    return false;
  }

  // check if the initial designator is attached
  wxXmlNode *designator = p_designator_list->GetChildren();

  wxString attached_id = get_child_value(designator, _T("propertyof"));
  if(attached_id == _T("-1"))
  {
    // not attached
    cerr << "Process diagram " << diagram_name.ToAscii() << " contains an unconnected initial designator." << endl;
    return false;
  }
  // search attached state
  wxXmlNode *object_list = get_child(p_process_diagram, _T("objectlist"));

  // get states
  wxXmlNode *state_list = get_child(object_list, _T("statelist"));
  if(state_list == 0)
  {
    cerr << "Process diagram " << diagram_name.ToAscii() << " does not contain a list of states." << endl;
    return false;
  }

  // loop through states
  for(wxXmlNode *state = state_list->GetChildren(); state != 0; state = state->GetNext())
  {
    wxString state_id = get_child_value(state, _T("id"));
    if(state_id == attached_id)
    {
      return true;
    }
  }

  // get reference states
  wxXmlNode *reference_state_list = get_child(object_list, _T("referencestatelist"));
  if(reference_state_list == 0)
  {
    cerr << "Process diagram " << diagram_name.ToAscii() << " does not contain a list of reference states." << endl;
    return false;
  }

  // loop through reference states
  wxArrayString proc_refs;
  for(wxXmlNode *ref_state = reference_state_list->GetChildren(); ref_state != 0; ref_state = ref_state->GetNext())
  {
    wxString ref_state_id = get_child_value(ref_state, _T("id"));
    if(ref_state_id == attached_id)
    {
      // if it is a match, check if acyclic
      proc_refs.Add(get_child_value(p_process_diagram, _T("id")));
      if (!is_reference_acyclic(p_doc_root, proc_refs))
      {
        return false;
      }
      return true;
    }
  }
  // ERROR: initial designator is connected to a non existing state
  cerr << "Process diagram " << diagram_name.ToAscii() << " does not contain a (reference) state connected to the initial designator." << endl;
  return false;
}

bool grape::mcrl2gen::validate_reference_state_list(wxXmlNode *p_doc_root, wxXmlNode *p_process_diagram, wxXmlNode *p_ref_state_list, ATermAppl &datatype_spec)
{
  // initialize parameters
  wxString diagram_name = get_child_value(p_process_diagram, _T("name"));

  // loop through all reference states
  for(wxXmlNode *ref_state = p_ref_state_list->GetChildren(); ref_state != 0; ref_state = ref_state->GetNext())
  {
    wxString ref_state_prop = get_child_value(ref_state, _T("propertyof"));
    wxString ref_state_id = get_child_value(ref_state, _T("id"));
    wxXmlNode *referenced_diagram = get_diagram(p_doc_root, ref_state_prop);
    if(ref_state_prop == _T("-1"))
    {
      // no target diagram
      cerr << "Process diagram " << diagram_name.ToAscii()
           << " contains a process reference which does not refer to a process diagram." << endl;
      return false;
    }
    if(referenced_diagram == 0)
    {
      // target diagram not found
      cerr << "Process diagram " << diagram_name.ToAscii()
           << " contains a process reference that does not refer to an existing process diagram." << endl;
      return false;
    }

    // if it is a match, check if acyclic
    wxArrayString proc_refs;
    proc_refs.Add(get_child_value(referenced_diagram, _T("id")));
    if (!is_reference_acyclic(p_doc_root, proc_refs))
    {
      return false;
    }

    // validate referenced diagram
    if (!validate_process_diagram(p_doc_root, referenced_diagram, datatype_spec))
    {
      return false;
    }
    wxString ref_name = get_child_value(referenced_diagram, _T("name"));
    wxString ref_id = get_child_value(referenced_diagram, _T("id"));

    // check parameter initialisation
    try
    {
      list_of_varupdate ref_inits;
      validate_reference_parameters(p_doc_root, ref_state, diagram_name, ref_inits, datatype_spec);
    }
    catch(...)
    {
      cerr << "Process diagram " << diagram_name.ToAscii() << " has a reference state with an invalid list of reference parameters." << endl;
      return false;
    }

    // check state-transition connections
    if(!validate_transition_connection(p_process_diagram, ref_state_id))
    {
      cerr << "Process diagram " << diagram_name.ToAscii()
           << " contains a process reference to process diagram " << ref_name.ToAscii() << " that is not connected to a transition." << endl;
      return false;
    }
  }
  return true;
}

bool grape::mcrl2gen::validate_reference_parameters(wxXmlNode *p_doc_root, wxXmlNode *p_reference, wxString &p_diagram_name, list_of_varupdate &p_parameter_initialisation, ATermAppl &datatype_spec)
{
  // initialize variables
  p_parameter_initialisation.Empty();

  if(p_reference->GetName() == _T("processreference") || p_reference->GetName() == _T("referencestate"))
  {
    wxString diagram_name = get_child_value(p_reference, _T("name"));
    wxString referenced_diagram_id = get_child_value(p_reference, _T("propertyof"));
    wxXmlNode *referenced_diagram = get_diagram(p_doc_root, referenced_diagram_id);
    list_of_decl variable_decls;  // must be empty
    list_of_decl preamble_parameter_decls;
    list_of_decl_init preamble_variable_decls; // must be empty
    try
    {
      validate_preamble(referenced_diagram, preamble_parameter_decls, preamble_variable_decls, datatype_spec);
    }
    catch(...)
    {
      cerr << "Process diagram " << diagram_name.ToAscii() << " referenced in " << p_diagram_name << " has an invalid preamble." << endl;
      throw CONVERSION_ERROR;
      return false;
    }

    // make variable table
    variable_decls.Empty();
    preamble_variable_decls.Empty();
    atermpp::table vars = get_variable_table(preamble_parameter_decls, preamble_variable_decls, variable_decls, datatype_spec);

    // get parameter assignment list
    wxXmlNode *param_list = get_child(p_reference, _T("parameterassignmentlist"));
    if(param_list == 0)
    {
      // ERROR: <referencestate> does not contain <parameterassignmentlist>
      cerr << "Diagram " << p_diagram_name.ToAscii()
           << " has a process reference " << diagram_name.ToAscii()
           << " that does not contain any parameter assignments." << endl;
      throw CONVERSION_ERROR;
      return false;
    }
    // loop through parameter assignments
    for(wxXmlNode *param_assignment = param_list->GetChildren(); param_assignment != 0; param_assignment = param_assignment->GetNext())
    {
      if ( param_assignment->GetName() == _T( "parameterassignment" ) )
      {
        varupdate parameter_assignment;
        // get parameter assignment
        wxString parameter_assignment_text = param_assignment->GetNodeContent();
        if ( !parameter_assignment.set_varupdate( parameter_assignment_text ) )
        {
          // ERROR: parameter assignment is not valid
          cerr << "Process reference to process diagram " << diagram_name.ToAscii()
               << " contains an invalid parameter assignment " << parameter_assignment_text.ToAscii() << "." << endl;
          throw CONVERSION_ERROR;
          return false;
        }

        // parse parameter name (identifier)
        ATermAppl a_parsed_parameter_assignment_id = parse_identifier(parameter_assignment.get_lhs());
        if ( a_parsed_parameter_assignment_id == 0 )
        {
          // ERROR: parameter assignment is not valid
          cerr << "Process reference to process diagram " << diagram_name.ToAscii() << " contains an invalid parameter assignment " << parameter_assignment_text.ToAscii()
               << ". The parameter name '" << parameter_assignment.get_lhs().ToAscii() << "' could not be parsed." << endl;
          throw CONVERSION_ERROR;
          return false;
        }

        // parse as identifier succeeded: try to parse as data expression
        ATermAppl a_parsed_parameter_assignment_lhs_expression = parse_data_expr(parameter_assignment.get_lhs());
        if ( a_parsed_parameter_assignment_lhs_expression == 0 )
        {
          // ERROR: parameter assignment is not valid
          cerr << "Process reference to process diagram " << diagram_name.ToAscii() << " contains an invalid parameter assignment " << parameter_assignment_text.ToAscii()
               << ". The parameter name '" << parameter_assignment.get_lhs().ToAscii() << "' could not be parsed." << endl;
          throw CONVERSION_ERROR;
          return false;
        }
        // type check as data expression
        ATermAppl a_type_checked_parameter_assignment_lhs_expression = type_check_data_expr(a_parsed_parameter_assignment_lhs_expression, NULL, datatype_spec, vars);
        if ( a_type_checked_parameter_assignment_lhs_expression == 0 )
        {
          // ERROR: parameter assignment lhs is not valid
          cerr << "Process reference to process diagram " << diagram_name.ToAscii() << " contains an invalid parameter assignment " << parameter_assignment_text.ToAscii()
               << ". The parameter name '" << parameter_assignment.get_lhs().ToAscii() << "' could not be type checked." << endl;
          throw CONVERSION_ERROR;
          return false;
        }

        // parse parameter assignment value (dataexpression)
        ATermAppl a_parsed_parameter_assignment_expr = parse_data_expr(parameter_assignment.get_rhs());
        if ( a_parsed_parameter_assignment_expr == 0 )
        {
          // ERROR: parameter assignment is not valid
          cerr << "Process reference to process diagram " << diagram_name.ToAscii() << " contains an invalid parameter assignment " << parameter_assignment_text.ToAscii()
               << ". The parameter assignment value '" << parameter_assignment.get_rhs().ToAscii() << "' could not be parsed." << endl;
          throw CONVERSION_ERROR;
          return false;
        }
        // type check parameter assignment value (dataexpression)
        ATermAppl a_type_checked_parameter_assignment_expr = type_check_data_expr( a_parsed_parameter_assignment_expr, gsGetSort(a_type_checked_parameter_assignment_lhs_expression), datatype_spec, vars );
        if ( a_type_checked_parameter_assignment_expr == 0 )
        {
          // ERROR: parameter assignment is not valid
          cerr << "Process reference to process diagram " << diagram_name.ToAscii() << " contains an invalid parameter assignment " << parameter_assignment_text.ToAscii()
               << ". The parameter assignment value '" << parameter_assignment.get_rhs().ToAscii() << "' could not be type checked." << endl;
          throw CONVERSION_ERROR;
          return false;
        }
        p_parameter_initialisation.Add( parameter_assignment );
      }
      else
      {
        // ERROR: parameter assignment is not valid
        cerr << "Process reference to process diagram " << diagram_name.ToAscii()
             << " contains an invalid parameter assignment." << endl;
        throw CONVERSION_ERROR;
        return false;
      }
    }

    // check preamble params count = reference initialisation count
    if(preamble_parameter_decls.GetCount() != p_parameter_initialisation.GetCount())
    {
      // not the same number of parameters
      cerr << "Diagram " << p_diagram_name.ToAscii()
           << " contains a process reference to process diagram " << diagram_name.ToAscii()
           << " that does not contain the same number of parameters." << endl;
      throw CONVERSION_ERROR;
      return false;
    }

    // loop through preamble parameters
    for(unsigned int i=0; i<preamble_parameter_decls.GetCount(); ++i)
    {
      bool found = false;
      for(unsigned int j=0; j<p_parameter_initialisation.GetCount(); ++j)
      {
        if(p_parameter_initialisation[j].get_lhs() == preamble_parameter_decls[i].get_name())
        {
          found = true;
          break;
        }
      }
      if(!found)
      {
        cerr << "Diagram " << p_diagram_name.ToAscii()
             << " contains a process reference to process diagram " << diagram_name.ToAscii()
             << " that has a parameter initialisation in which parameter " << preamble_parameter_decls[i].get_name().ToAscii()
             << " does not occur." << endl;
        throw CONVERSION_ERROR;
        return false;
      }
    }
    return true;
  }
  // ERROR: XML invalid
  cerr << "Validate_reference_parameters called without a valid XML process reference." << endl;
  throw CONVERSION_ERROR;
  return false;
}

bool grape::mcrl2gen::validate_state_list(wxXmlNode *p_process_diagram, wxXmlNode *p_state_list)
{
  // initialize variables
  wxString diagram_name = get_child_value(p_process_diagram, _T("name"));

  // loop through state list
  for(wxXmlNode *state = p_state_list->GetChildren(); state != 0; state = state->GetNext())
  {
    wxString state_name = get_child_value(state, _T("name"));
    wxString state_id = get_child_value(state, _T("id"));

    // validate state name as identifier
    wxString state_full_name = state_name + _T("_") + state_id;
    if (!gsIsUserIdentifier(std::string(state_full_name.fn_str())))
    {
      // ERROR: state name is not a valid identifier
      cerr << "Process diagram " << diagram_name.ToAscii()
           << " contains a state " << state_name.ToAscii() << " which name is not a valid identifier." << endl;
      return false;
    }

    // check state-transition connections
    if (!validate_transition_connection(p_process_diagram, state_id))
    {
      cerr << "Process diagram " << diagram_name.ToAscii()
           << " contains a state " << state_name.ToAscii() << " that is not connected to a transition." << endl;
      return false;
    }
  }
  return true;
}

bool grape::mcrl2gen::validate_transition_connection(wxXmlNode *p_process_diagram, wxString &p_state_id)
{
  // get objects
  wxXmlNode *objects = get_child(p_process_diagram, _T("objectlist"));

  // get terminating transition list
  wxXmlNode *tts = get_child(objects, _T("terminatingtransitionlist"));
  // loop through terminating transitions
  for(wxXmlNode *tt = tts->GetChildren(); tt != 0; tt = tt->GetNext())
  {
    wxString tt_from = get_child_value(tt, _T("from"));
    if(tt_from == p_state_id)
    {
      return true;
    }
  }

  // get non terminating transition list
  wxXmlNode *ntts = get_child(objects, _T("nonterminatingtransitionlist"));
  // loop through non terminating transitions
  for(wxXmlNode *ntt = ntts->GetChildren(); ntt != 0; ntt = ntt->GetNext())
  {
    wxString ntt_from = get_child_value(ntt, _T("from"));
    wxString ntt_to = get_child_value(ntt, _T("to"));
    if(ntt_from == p_state_id || ntt_to == p_state_id)
    {
      return true;
    }
  }

  // get initial designator list
  wxXmlNode *designators = get_child(objects, _T("initialdesignatorlist"));
  // loop through initial designators
  for(wxXmlNode *des = designators->GetChildren(); des != 0; des = des->GetNext())
  {
    wxString des_id = get_child_value(des, _T("propertyof"));
    if(des_id == p_state_id)
    {
      return true;
    }
  }
  return false;
}

bool grape::mcrl2gen::validate_state_connection(wxXmlNode *p_process_diagram, wxString &p_transition_id)
{
  // get objects
  wxXmlNode *objects = get_child(p_process_diagram, _T("objectlist"));

  // get state list
  wxXmlNode *states = get_child(objects, _T("statelist"));
  // loop through states
  for(wxXmlNode *state = states->GetChildren(); state != 0; state = state->GetNext())
  {
    wxString state_id = get_child_value(state, _T("id"));
    if(p_transition_id == state_id)
    {
      return true;
    }
  }
  
  // get reference state list
  wxXmlNode *ref_states = get_child(objects, _T("referencestatelist"));
  // loop through reference states
  for(wxXmlNode *ref_state = ref_states->GetChildren(); ref_state != 0; ref_state = ref_state->GetNext())
  {
    wxString ref_state_id = get_child_value(ref_state, _T("id"));
    if(p_transition_id == ref_state_id)
    {
      return true;
    }
  }
  return false;
}

bool grape::mcrl2gen::validate_terminating_transition_list(wxXmlNode *p_process_diagram, wxXmlNode *p_term_trans_list, list_of_decl &p_preamble_parameters, list_of_decl_init &p_preamble_variables, ATermAppl &datatype_spec)
{
  // for each transition, infer if the beginstates exist and try to parse its label
  wxString diagram_name = get_child_value(p_process_diagram, _T("name"));

  // loop through terminating transitions
  for(wxXmlNode *transition = p_term_trans_list->GetChildren(); transition != 0; transition = transition->GetNext())
  {
    wxString trans_from = get_child_value(transition, _T("from"));

    if(trans_from.IsEmpty() || trans_from == _T("-1") || !validate_state_connection(p_process_diagram, trans_from))
    {
      cerr << "Process diagram " << diagram_name.ToAscii()
           << " contains a terminating transition that is not connected to a state or process reference." << endl;
      return false;
    }

    // validate transition label
    try
    {
      label trans_label;
      validate_transition_label(transition, p_preamble_parameters, p_preamble_variables, trans_label, diagram_name, datatype_spec);
    }
    catch(...)
    {
      cerr << "Process diagram " << diagram_name.ToAscii() << " contains a terminating transition with an invalid transition label." << endl;
      return false;
    }
  }
  return true;
}

bool grape::mcrl2gen::validate_nonterminating_transition_list(wxXmlNode *p_process_diagram, wxXmlNode *p_trans_list, list_of_decl &p_preamble_parameters, list_of_decl_init &p_preamble_variables, ATermAppl &datatype_spec)
{
  // for each transition, infer if the begin- and endstates exist and try to parse its label
  wxString diagram_name = get_child_value(p_process_diagram, _T("name"));

  // loop through non terminating transitions
  for(wxXmlNode *transition = p_trans_list->GetChildren(); transition != 0; transition = transition->GetNext())
  {
    wxString trans_from = get_child_value(transition, _T("from"));
    wxString trans_to = get_child_value(transition, _T("to"));

    if(trans_from.IsEmpty() || trans_from == _T("-1") || trans_to.IsEmpty() || trans_to == _T("-1")
      || !validate_state_connection(p_process_diagram, trans_from) || !validate_state_connection(p_process_diagram, trans_to))
    {
      cerr << "Process diagram " << diagram_name.ToAscii()
           << " contains a non terminating transition that is not conntected to a state or process reference." << endl;
      return false;
    }

    // validate transition label
    try
    {
      label trans_label;
      validate_transition_label(transition, p_preamble_parameters, p_preamble_variables, trans_label, diagram_name, datatype_spec);
    }
    catch(...)
    {
      cerr << "Process diagram " << diagram_name.ToAscii() << " contains a non terminating transition with an invalid transition label." << endl;
      return false;
    }
  }
  return true;
}

bool grape::mcrl2gen::validate_transition_label(wxXmlNode *p_process_diagram, list_of_decl &p_preamble_parameter_decls, list_of_decl_init &p_preamble_local_var_decls, label &p_trans_label, wxString p_diagram_name, ATermAppl &datatype_spec)
{
  // load label
  wxXmlNode* transition_label = get_child(p_process_diagram, _T("label"));
  if (transition_label != 0)
  {
    // validate variable declarations
    list_of_decl variable_decl_list;
    if (!validate_transition_label_variable_declarations(transition_label, p_diagram_name, variable_decl_list, datatype_spec)) return false;
    
    // get variable table
    atermpp::table vars = get_variable_table(p_preamble_parameter_decls, p_preamble_local_var_decls, variable_decl_list, datatype_spec);

    // validate condition
    wxString condition;
    if (!validate_transition_label_condition(transition_label, p_diagram_name, condition, datatype_spec, vars)) return false;

    // validate actions
    list_of_action actions;
    if (!validate_transition_label_actions(transition_label, p_diagram_name, actions, datatype_spec, vars)) return false;

    // validate timestamp
    wxString timestamp;
    if (!validate_transition_label_timestamp(transition_label, p_diagram_name, timestamp, datatype_spec, vars)) return false;

    // validate variable updates
    list_of_varupdate variable_updates;
    if (!validate_transition_label_variable_updates(transition_label, p_diagram_name, variable_updates, datatype_spec, vars)) return false;

    // update transition label
    p_trans_label.set_declarations( variable_decl_list );
    p_trans_label.set_condition( condition );
    p_trans_label.set_actions( actions );
    p_trans_label.set_timestamp( timestamp );
    p_trans_label.set_variable_updates( variable_updates );
    return true;
  }
  // ERROR: transition label is not valid
  cerr << "Validate_transition_label called without a valid XML process diagram." << endl;
  throw CONVERSION_ERROR;
  return false;
}

bool grape::mcrl2gen::validate_transition_label_variable_declarations(wxXmlNode *p_transition_label, wxString &p_diagram_name, list_of_decl &p_local_var_decls, ATermAppl &datatype_spec)
{
  // initialize variables
  wxString diagram_name = p_diagram_name;
  p_local_var_decls.Clear();

  // get variable declarations
  wxXmlNode* trans_label_declarations = get_child(p_transition_label, _T("variabledeclarations"));
  if (trans_label_declarations != 0)
  {
    // loop through variable declarations
    for (wxXmlNode *trans_label_declaration = trans_label_declarations->GetChildren(); trans_label_declaration != 0; trans_label_declaration = trans_label_declaration->GetNext())
    {
      decl variable_decl;
      if (trans_label_declaration->GetName() == _T("variabledeclaration"))
      {
        // get variable declaration
        wxString variable_declaration_text = trans_label_declaration->GetNodeContent();
        if ( !variable_decl.set_decl( variable_declaration_text ) )
        {
          // ERROR: transition label is not valid
          cerr << "Process diagram " << p_diagram_name.ToAscii()
               << " contains a transition label with an invalid variable declaration "
               << variable_declaration_text.ToAscii() << "." << endl;
          throw CONVERSION_ERROR;
          return false;
        }

        // parse declaration name (identifier)
        ATermAppl a_parsed_var_decl_id = parse_identifier(variable_decl.get_name());
        if ( a_parsed_var_decl_id == 0 )
        {
          // ERROR: variable declaration is not valid
          cerr << "Process diagram " << p_diagram_name.ToAscii() << " contains a transition label with an invalid variable declaration " << variable_declaration_text.ToAscii()
               << ". The variable name '" << variable_decl.get_name().ToAscii() << "' could not be parsed." << endl;
          throw CONVERSION_ERROR;
          return false;
        }

        // parse declaration type (sortexpression)
        ATermAppl a_parsed_var_decl_sort = parse_sort_expr(variable_decl.get_type());
        if ( a_parsed_var_decl_sort == 0 )
        {
          // ERROR: variable declaration is not valid
          cerr << "Process diagram " << p_diagram_name.ToAscii() << " contains a transition label with an invalid variable declaration " << variable_declaration_text.ToAscii()
               << ". The variable type '" << variable_decl.get_type().ToAscii() << "' could not be parsed." << endl;
          throw CONVERSION_ERROR;
          return false;
        }

        // parse succeeded: try to type check
        ATermAppl a_type_checked_var_decl_sort = type_check_sort_expr( a_parsed_var_decl_sort, datatype_spec );
        if ( a_type_checked_var_decl_sort == 0 )
        {
          // ERROR: variable declaration is not valid
          cerr << "Process diagram " << p_diagram_name.ToAscii() << " contains a transition label with an invalid variable declaration " << variable_declaration_text.ToAscii()
               << ". The variable type '" << variable_decl.get_type().ToAscii() << "' could not be type checked." << endl;
          throw CONVERSION_ERROR;
          return false;
        }
      }
      else
      {
        // ERROR: transition label is not valid
        cerr << "Validate_transition_label_variable_declarations called without a valid XML process diagram." << endl;
        throw CONVERSION_ERROR;
        return false;
      }
      // add variable declaration
      p_local_var_decls.Add( variable_decl );
    }
  }
  return true;
}

bool grape::mcrl2gen::validate_transition_label_condition(wxXmlNode *p_transition_label, wxString &p_diagram_name, wxString &p_condition, ATermAppl &datatype_spec, atermpp::table &vars)
{
  // initialize variables
  wxXmlNode* trans_label_condition = get_child(p_transition_label, _T("condition"));
  if (trans_label_condition != 0)
  {
    p_condition = trans_label_condition->GetNodeContent();
    if (!p_condition.IsEmpty())
    {
      // parse condition (dataexpression)
      ATermAppl a_parsed_condition_expr = parse_data_expr(p_condition);
      if ( a_parsed_condition_expr == 0 )
      {
        // ERROR: condition is not valid
        cerr << "Process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The condition "
             << p_condition.ToAscii() << " could not be parsed." << endl;
        throw CONVERSION_ERROR;
        return false;
      }
      // type check condition (dataexpression)
      ATermAppl a_type_checked_condition_expr = type_check_data_expr( a_parsed_condition_expr, NULL, datatype_spec, vars);
      if ( a_type_checked_condition_expr == 0 )
      {
        // ERROR: condition is not valid
        cerr << "Process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The condition "
             << p_condition.ToAscii() << " could not be type checked." << endl;
        throw CONVERSION_ERROR;
        return false;
      }
      if ( !gsIsSortExprBool(gsGetSort(a_type_checked_condition_expr)) )
      {
        // ERROR: condition is of wrong type
        cerr << "Process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The sort of condition "
             << p_condition.ToAscii() << " is not Bool." << endl;
        throw CONVERSION_ERROR;
        return false;
      }
    }
  }
  return true;
}

bool grape::mcrl2gen::validate_transition_label_actions(wxXmlNode *p_transition_label, wxString &p_diagram_name, list_of_action &p_actions, ATermAppl &datatype_spec, atermpp::table &vars)
{
  // initialize variables
  p_actions.Empty();
  wxXmlNode* trans_label_actions = get_child(p_transition_label, _T("actions"));
  if (trans_label_actions != 0)
  {
    // loop through all actions
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
          ATermAppl a_parsed_action_identifier = parse_identifier(action_name);
          if ( a_parsed_action_identifier == 0 )
          {
            // ERROR: action identifier is not valid
            cerr << "Process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The action name "
                 << action_name.ToAscii() << " could not be parsed." << endl;
            throw CONVERSION_ERROR;
            return false;
          }
          action.set_name( action_name );
        }
        else if (action_part->GetName() == _T("param"))
        {
          wxString action_param = action_part->GetNodeContent();

          // parse action parameter (dataexpression)
          ATermAppl a_parsed_action_param_expr = parse_data_expr(action_param);
          if ( a_parsed_action_param_expr == 0 )
          {
            // ERROR: action parameter is not valid
            cerr << "Process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The action parameter "
                 << action_param.ToAscii() << " could not be parsed." << endl;
            throw CONVERSION_ERROR;
            return false;
          }
          // type check action parameter (dataexpression)
          ATermAppl a_type_checked_action_param_expr = type_check_data_expr( a_parsed_action_param_expr, NULL, datatype_spec, vars);
          if ( a_type_checked_action_param_expr == 0 )
          {
            // ERROR: action parameter is not valid
            cerr << "Process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The action parameter "
                 << action_param.ToAscii() << " could not be type checked." << endl;
            throw CONVERSION_ERROR;
            return false;
          }

          // get action parameter type
          ATermAppl sort_expr = gsGetSort(a_type_checked_action_param_expr);
          string sort_expr_string = PrintPart_CXX(ATerm(sort_expr));
          wxString action_param_type = wxString(sort_expr_string.c_str(), wxConvLocal);

          dataexpression param;
          param.set_expression( action_param );
	        param.set_type( action_param_type );
          action_params.Add( param );
        }
        else
        {
          // ERROR: transition label is not valid
          cerr << "Validate_transition_label_actions called without a valid XML process diagram." << endl;
          throw CONVERSION_ERROR;
          return false;
        }
      }
      action.set_parameters( action_params );
      p_actions.Add( action );
    }
  }
  return true;
}

bool grape::mcrl2gen::validate_transition_label_timestamp(wxXmlNode *p_transition_label, wxString &p_diagram_name, wxString &p_timestamp, ATermAppl &datatype_spec, atermpp::table &vars)
{
  // initialize variables
  wxXmlNode* trans_label_timestamp = get_child(p_transition_label, _T("timestamp"));
  if (trans_label_timestamp != 0)
  {
    p_timestamp = trans_label_timestamp->GetNodeContent();
    if (!p_timestamp.IsEmpty())
    {
      // parse timestamp (dataexpression)
      ATermAppl a_parsed_timestamp_expr = parse_data_expr(p_timestamp);
      if ( a_parsed_timestamp_expr == 0 )
      {
        // ERROR: timestamp is not valid
        cerr << "Process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The timestamp "
             << p_timestamp.ToAscii() << " could not be parsed." << endl;
        throw CONVERSION_ERROR;
        return false;
      }
      // type check timestamp (dataexpression)
      ATermAppl a_type_checked_timestamp_expr = type_check_data_expr( a_parsed_timestamp_expr, NULL, datatype_spec, vars);
      if ( a_type_checked_timestamp_expr == 0 )
      {
        // ERROR: timestamp is not valid
        cerr << "Process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The timestamp "
             << p_timestamp.ToAscii() << " could not be type checked." << endl;
        throw CONVERSION_ERROR;
        return false;
      }
      if ( !gsIsSortExprReal(gsGetSort(a_type_checked_timestamp_expr)) )
      {
        // ERROR: condition is of wrong type
        cerr << "Process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The sort of timestamp "
             << p_timestamp.ToAscii() << " is not Real." << endl;
        throw CONVERSION_ERROR;
        return false;
      }
    }
  }
  return true;
}

bool grape::mcrl2gen::validate_transition_label_variable_updates(wxXmlNode *p_transition_label, wxString &p_diagram_name, list_of_varupdate &p_variable_updates, ATermAppl &datatype_spec, atermpp::table &vars)
{
  // initialize variable updates
  p_variable_updates.Empty();
  wxXmlNode* trans_label_variable_updates = get_child(p_transition_label, _T("variableupdates"));
  if (trans_label_variable_updates != 0)
  {
    // loop through all variable updates
    for (wxXmlNode *trans_label_variable_update = trans_label_variable_updates->GetChildren(); trans_label_variable_update != 0; trans_label_variable_update = trans_label_variable_update->GetNext())
    {
      if (trans_label_variable_update->GetName() == _T("variableupdate"))
      {
        varupdate variable_update;
        wxString variable_update_text = trans_label_variable_update->GetNodeContent();
        if ( !variable_update.set_varupdate( variable_update_text ) )
        {
          // ERROR: transition label is not valid
          cerr << "Process diagram " << p_diagram_name.ToAscii()
               << " contains a transition label with an invalid variable update "
               << variable_update_text.ToAscii() << "." << endl;
          throw CONVERSION_ERROR;
          return false;
        }

        // parse varupdate left hand side (identifier)
        ATermAppl a_parsed_variable_update_identifier = parse_identifier(variable_update.get_lhs());
        if ( a_parsed_variable_update_identifier == 0 )
        {
          // ERROR: variable declaration is not valid
          cerr << "Process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The left hand side "
               << variable_update.get_lhs().ToAscii() << " of variable update "
               << variable_update_text.ToAscii() << " could not be parsed." << endl;
          throw CONVERSION_ERROR;
          return false;
        }
        // parse as identifier succeeded: try to parse as data expression
        ATermAppl a_parsed_variable_update_lhs_expression = parse_data_expr(variable_update.get_lhs());
        if ( a_parsed_variable_update_lhs_expression == 0 )
        {
          // ERROR: variable declaration is not valid
          cerr << "Process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The left hand side "
               << variable_update.get_lhs().ToAscii() << " of variable update "
               << variable_update_text.ToAscii() << " could not be parsed." << endl;
          throw CONVERSION_ERROR;
          return false;
        }
        // type check as data expression
        ATermAppl a_type_checked_variable_update_lhs_expression = type_check_data_expr(a_parsed_variable_update_lhs_expression, NULL, datatype_spec, vars);
        if ( a_type_checked_variable_update_lhs_expression == 0 )
        {
          // ERROR: varupdate_lhs is not valid
          cerr << "Process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The left hand side "
               << variable_update.get_lhs().ToAscii() << " of variable update "
               << variable_update_text.ToAscii() << " could not be type checked." << endl;
          throw CONVERSION_ERROR;
          return false;
        }

        // parse varupdate right hand side (dataexpression)
        ATermAppl a_parsed_varupdate_rhs_expr = parse_data_expr(variable_update.get_rhs());
        if ( a_parsed_varupdate_rhs_expr == 0 )
        {
          // ERROR: variable declaration is not valid
          cerr << "Process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The right hand side "
               << variable_update.get_rhs().ToAscii() << " of variable update "
               << variable_update_text.ToAscii() << " could not be parsed." << endl;
          throw CONVERSION_ERROR;
          return false;
        }
        // type check varupdate right hand side (dataexpression)
        // type checking against type of lhs
        ATermAppl a_type_checked_varupdate_rhs_expr = type_check_data_expr( a_parsed_varupdate_rhs_expr, gsGetSort(a_type_checked_variable_update_lhs_expression), datatype_spec, vars);
        if ( a_type_checked_varupdate_rhs_expr == 0 )
        {
          // ERROR: variable declaration is not valid
          cerr << "Process diagram " << p_diagram_name.ToAscii() << " contains an invalid label. The right hand side "
               << variable_update.get_rhs().ToAscii() << " of variable update "
               << variable_update_text.ToAscii() << " could not be type checked." << endl;
          throw CONVERSION_ERROR;
          return false;
        }
        p_variable_updates.Add( variable_update );
      }
      else
      {
        // ERROR: transition label is not valid
        cerr << "Validate_transition_label_variable_updates called without a valid XML process diagram." << endl;
        throw CONVERSION_ERROR;
        return false;
      }
    }
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
    arch_diagram = get_diagram(doc_root, p_diagram_id);
  }
  catch(...)
  {
    cerr << "Architecture diagram " << p_diagram_id.ToAscii() << " is not present." << endl;
    return false;
  }

  if(!validate_architecture_diagram(doc_root, arch_diagram, datatype_spec))
  {
    //cerr << "Architecture diagram is not valid." << endl;
    return false;
  }

  cerr << "Architecture diagram is valid.";
  return true;
}

bool grape::mcrl2gen::validate_architecture_diagram(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, ATermAppl &datatype_spec)
{
  // initialize variables
  bool channel_communication_list_is_valid  = true,
       channel_list_is_valid                = true,
       architecture_reference_list_is_valid = true, 
       process_reference_list_is_valid      = true;
  wxString diagram_id = get_child_value(p_architecture_diagram, _T("id"));      // get diagram id
  wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));  // get diagram name
  if(diagram_name.IsEmpty())
  {
    cerr << "Architecture diagram " << diagram_id.ToAscii() << " has no name." << endl;
    return false;
  }
  // parse architecture diagram name
  if (!gsIsUserIdentifier(std::string(diagram_name.fn_str())))
  {
    // ERROR: architecture name is not an identifier
    cerr << "Architecture diagram name " << diagram_name.ToAscii() << " is not a valid identifier." << endl;
    return false;
  }

  // validate objects
  wxXmlNode *object_list = get_child(p_architecture_diagram, _T("objectlist"));
  if(object_list == 0)
  {
    cerr << "Architecture diagram " << diagram_name.ToAscii() << " contains no list of objects." << endl;
    return false;
  }

  // validate all diferent objects
  bool is_not_empty = false;
  for(wxXmlNode *curr_list = object_list->GetChildren(); curr_list != 0; curr_list = curr_list->GetNext())
  {
    if(curr_list->GetName() == _T("architecturereferencelist"))
    {
      if (curr_list->GetChildren() != 0)
      {
        is_not_empty = true;
      }
      architecture_reference_list_is_valid = validate_architecture_reference_list(p_doc_root, p_architecture_diagram, curr_list, datatype_spec);
    }
    else if(curr_list->GetName() == _T("processreferencelist"))
    {
      if (curr_list->GetChildren() != 0)
      {
        is_not_empty = true;
      }
      process_reference_list_is_valid = validate_process_reference_list(p_doc_root, p_architecture_diagram, curr_list, datatype_spec);
    }
  }

  if (!is_not_empty)
  {
    cerr << "Architecture diagram " << diagram_name.ToAscii() << " is empty." << endl;
    return false;
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

  return channel_communication_list_is_valid && channel_list_is_valid && architecture_reference_list_is_valid && process_reference_list_is_valid;
}

bool grape::mcrl2gen::validate_architecture_reference_list(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, wxXmlNode *p_reference_list, ATermAppl &datatype_spec)
{
  // an architecture reference is valid when it refers to an existing diagram and every associated channel name is unique
  // initialize variables
  wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));  // get diagram name
  wxArrayString arch_refs;
  wxArrayString arch_diags;  

  // loop through all architecture references
  bool is_valid = true;
  for(wxXmlNode *arch_ref = p_reference_list->GetChildren(); arch_ref != 0; arch_ref = arch_ref->GetNext())
  {
    wxString ref_id = get_child_value(arch_ref, _T("id"));
    wxString ref_propertyof = get_child_value(arch_ref, _T("propertyof"));

    if(ref_propertyof.IsEmpty() || ref_propertyof == _T("-1"))
    {
      is_valid = false;
      break;
    }
    wxXmlNode *arch_diag = 0;

    try
    {
      arch_diag = get_diagram(p_doc_root, ref_propertyof);
    }
    catch(...)
    {
      is_valid = false;
      break;
    }

    if(arch_diag == 0)
    {
      is_valid = false;
      break;
    }

    // check channels
    wxArrayString ref_channels;
    ref_channels.Empty();
    wxXmlNode *objects = get_child(p_architecture_diagram, _T("objectlist"));
    wxXmlNode *channels = get_child(objects, _T("channellist"));
    // loop through all channels
    for(wxXmlNode *channel = channels->GetChildren(); channel != 0; channel = channel->GetNext())
    {
      wxString channel_propertyof = get_child_value(channel, _T("onreference"));
      wxString channel_name = get_child_value(channel, _T("name"));
      if(channel_propertyof == ref_id)
      {
        for(unsigned int i=0; i<ref_channels.GetCount(); ++i)
        {
          if (ref_channels[i] == channel_name)
          {
            cerr << "Architecture diagram " << diagram_name.ToAscii()
                 << " contains an architecture reference with multiple channels referring to action "
                 << ref_channels[i].ToAscii() << "." << endl;
            return false;
          }
        }
        ref_channels.Add(channel_name);
      }
    }

    // make unique architecture diagram list
    bool found = false;
    for(unsigned int i=0; i<arch_diags.GetCount(); ++i)
    {
      if (arch_diags[i] == ref_propertyof)
      {
        found = true;
        break;
      }
    }
    if (!found)
    {
      arch_diags.Add(ref_propertyof);
    }
  }
  
  if (!is_valid)
  {
      cerr << "Architecture diagram " << diagram_name.ToAscii()
           << " contains an architecture reference that does not refer to an existing architecture diagram." << endl;
      return false;
  }

  // check if acyclic
  arch_refs.Add(get_child_value(p_architecture_diagram, _T("id")));
  if (!is_reference_acyclic(p_doc_root, arch_refs))
  {
    return false;
  }

  // loop through unique architecture reference diagrams and validate them
  for(unsigned int i=0; i<arch_diags.GetCount(); ++i)
  {
    wxXmlNode *arch_diag = get_diagram(p_doc_root, arch_diags[i]);
    if (!validate_architecture_diagram(p_doc_root, arch_diag, datatype_spec))
    {
      return false;
    }
  }
  return true;
}

bool grape::mcrl2gen::validate_process_reference_list(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, wxXmlNode *p_reference_list, ATermAppl &datatype_spec)
{
  // a process reference is valid when it refers to a process diagram, the parameter initialisation matches and every associated channel name is unique
  // initialize variables
  wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));
  wxArrayString proc_diags;

  // loop through all process references
  bool is_valid = true;
  for(wxXmlNode *proc_ref = p_reference_list->GetChildren(); proc_ref != 0; proc_ref = proc_ref->GetNext())
  {
    wxString ref_id = get_child_value(proc_ref, _T("id"));
    wxString ref_state_prop = get_child_value(proc_ref, _T("propertyof"));

    if(ref_state_prop.IsEmpty() || ref_state_prop == _T("-1"))
    {
      is_valid = false;
      break;
    }
    wxXmlNode *referenced_diagram = 0;

    try
    {
      referenced_diagram = get_diagram(p_doc_root, ref_state_prop);
    }
    catch(...)
    {
      is_valid = false;
      break;
    }

    if(referenced_diagram == 0)
    {
      is_valid = false;
      break;
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
        for(unsigned int i=0; i<ref_channels.GetCount(); ++i)
        {
          if (ref_channels[i] == channel_name)
          {
            cerr << "Architecture diagram " << diagram_name.ToAscii()
                 << " contains a process reference with multiple channels referring to action "
                 << ref_channels[i].ToAscii() << "." << endl;
            return false;
          }
        }
        ref_channels.Add(channel_name);
      }
    }

    // check parameter initialisation
    try
    {
      list_of_varupdate ref_inits;
      validate_reference_parameters(p_doc_root, proc_ref, diagram_name, ref_inits, datatype_spec);
    }
    catch(...)
    {
      return false;
    }

    // make unique process diagram list
    bool found = false;
    for(unsigned int i=0; i<proc_diags.GetCount(); ++i)
    {
      if (proc_diags[i] == ref_state_prop)
      {
        found = true;
        break;
      }
    }
    if (!found)
    {
      proc_diags.Add(ref_state_prop);
    }
  }
  if (!is_valid)
  {
      cerr << "Architecture diagram " << diagram_name.ToAscii()
           << " contains a process reference that does not refer to an existing process diagram." << endl;
      return false;
  }

  // loop through unique process reference diagrams and validate them
  for(unsigned int i=0; i<proc_diags.GetCount(); ++i)
  {
    wxXmlNode *referenced_diagram = get_diagram(p_doc_root, proc_diags[i]);
    if (!validate_process_diagram(p_doc_root, referenced_diagram, datatype_spec))
    {
      return false;
    }
  }
  return true;
}

bool grape::mcrl2gen::validate_channel_communication_list(wxXmlNode *p_architecture_diagram, wxXmlNode *p_channel_communication_list)
{
  // a channel communication is valid when all its connections are to existing channels
  // initialize variables
  wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));

  // loop through channel communications
  for(wxXmlNode *comm = p_channel_communication_list->GetChildren(); comm != 0; comm = comm->GetNext())
  {
    // get connection list
    wxXmlNode *connectionlist = get_child(comm, _T("connectionlist"));
    if(connectionlist == 0)
    {
      cerr << "Architecture diagram " << diagram_name.ToAscii()
           << " contains a channel communication that is not connected to any channels." << endl;
      return false;
    }

    // get objects
    wxXmlNode *objects = get_child(p_architecture_diagram, _T("objectlist"));
    // get channels
    wxXmlNode *channels = get_child(objects, _T("channellist"));
    if(channels == 0)
    {
      cerr << "Architecture diagram " << diagram_name.ToAscii()
           << " contains a channel communication, but it does not contain any channels." << endl;
      return false;
    }

    // get channel communication type
    wxString channel_communication_type = get_child_value(comm, _T("channelcommunicationtype"));
    if (channel_communication_type == _T("visible"))
    {
      // get visible channel communication name
      wxString channel_communication_visible_name = get_child_value(comm, _T("name"));
      if (channel_communication_visible_name.IsEmpty())
      {
        // ERROR: visible channel communication is not named
        cerr << "Architecture diagram " << diagram_name.ToAscii()
             << " contains a visible channel communication that is unnamed." << endl;
        return false;
      }
      // parse channel communication name
      if (!gsIsUserIdentifier(std::string(channel_communication_visible_name.fn_str())))
      {
        // ERROR: channel communication name is not an identifier
        cerr << "Architecture diagram " << diagram_name.ToAscii()
             << " contains a channel communication name " << channel_communication_visible_name.ToAscii()
             << " which is not a valid identifier." << endl;
        return false;
      }
    }

    // loop through connections
    for(wxXmlNode *connection = connectionlist->GetChildren(); connection != 0; connection = connection->GetNext())
    {
      wxString connection_channel = connection->GetNodeContent();
      // loop through channels
      bool found = false;
      for(wxXmlNode *channel = channels->GetChildren(); channel != 0; channel = channel->GetNext())
      {
        wxString channel_id = get_child_value(channel, _T("id"));
        if(channel_id == connection_channel)
        {
          found = true;
          break;
        }
      }
      if(!found)
      {
        cerr << "Architecture diagram " << diagram_name.ToAscii()
             << " contains a channel communication that is connected to a non-existing channel." << endl;
        return false;
      }
    }
  }

  return true;
}

bool grape::mcrl2gen::validate_channel_list(wxXmlNode *p_doc_root, wxXmlNode *p_architecture_diagram, wxXmlNode *p_channel_list, ATermAppl &datatype_spec)
{
  // a channel is valid when its name corresponds to an action inside its reference, its <onreference> tag is valid
  // and its connections are to existing channelcommunications.
  // initialize variables
  wxString diagram_name = get_child_value(p_architecture_diagram, _T("name"));

  // loop through channels
  for(wxXmlNode *channel = p_channel_list->GetChildren(); channel != 0; channel = channel->GetNext())
  {
    // get connection list
    wxXmlNode *connectionlist = get_child(channel, _T("connectionlist"));
    if(connectionlist == 0)
    {
      cerr << "Architecture diagram " << diagram_name.ToAscii()
           << " contains a channel that is not connected to any channel communications." << endl;
      return false;
    }

    // get objects
    wxXmlNode *objects = get_child(p_architecture_diagram, _T("objectlist"));
    // get channels
    wxXmlNode *comms = get_child(objects, _T("channelcommunicationlist"));
    if(comms == 0)
    {
      cerr << "Architecture diagram " << diagram_name.ToAscii()
           << " contains a channel, but it does not contain any channel communications." << endl;
      return false;
    }

    // loop through connections
    for(wxXmlNode *connection = connectionlist->GetChildren(); connection != 0; connection = connection->GetNext())
    {
      wxString connection_channel_communication = connection->GetNodeContent();
      // loop through channel communications
      bool found = false;
      for(wxXmlNode *comm = comms->GetChildren(); comm != 0; comm = comm->GetNext())
      {
        wxString comm_id = get_child_value(comm, _T("id"));
        if (comm_id == connection_channel_communication)
        {
          found = true;
          break;
        }
      }
      if (!found)
      {
        cerr << "Architecture diagram " << diagram_name.ToAscii()
             << " contains a channel communication that is connected to a non-existing channel communication." << endl;
        return false;
      }
    }

    wxString channel_reference = get_child_value(channel, _T("onreference"));

    wxXmlNode *reference = 0;
    bool is_process_reference = false;
    wxXmlNode *proc_references = get_child(objects, _T("processreferencelist"));
    wxXmlNode *arch_references = get_child(objects, _T("architecturereferencelist"));

    // loop through process references
    for(wxXmlNode *proc_reference = proc_references->GetChildren(); proc_reference != 0; proc_reference = proc_reference->GetNext())
    {
      wxString proc_id = get_child_value(proc_reference, _T("id"));
      if(proc_id == channel_reference)
      {
        wxString proc_diag = get_child_value(proc_reference, _T("propertyof"));
        try
        {
          reference = get_diagram(p_doc_root, proc_diag);
        }
        catch(...)
        {}
        is_process_reference = true;
        break;
      }
    }
    if(reference == 0)
    {
      for(wxXmlNode *arch_reference = arch_references->GetChildren(); arch_reference != 0; arch_reference = arch_reference->GetNext())
      {
        wxString arch_id = get_child_value(arch_reference, _T("id"));
        if(arch_id == channel_reference)
        {
          wxString arch_diag = get_child_value(arch_reference, _T("propertyof"));
          try
          {
            reference = get_diagram(p_doc_root, arch_diag);
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
      cerr << "Architecture diagram " << diagram_name.ToAscii()
           << " contains a channel that lies on a reference to a non-existing diagram." << endl;
      return false;
    }
    else
    {
      // check name
      wxString channel_name = get_child_value(channel, _T("name"));
      if(channel_name.IsEmpty())
      {
        cerr << "Architecture diagram " << diagram_name.ToAscii()
             << " contains a channel with no name." << endl;
        return false;
      }
      // parse channel name
      if (!gsIsUserIdentifier(std::string(channel_name.fn_str())))
      {
        // ERROR: channel communication name is not an identifier
        cerr << "Architecture diagram " << diagram_name.ToAscii()
             << " contains a channel name " << channel_name.ToAscii()
             << " which is not a valid identifier." << endl;
        return false;
      }

      // check rename
      wxString channel_rename = get_child_value(channel, _T("rename"));
      if (!channel_rename.IsEmpty())
      {
        // parse channel name
        if (!gsIsUserIdentifier(std::string(channel_rename.fn_str())))
        {
          // ERROR: channel rename is not an identifier
          cerr << "Architecture diagram " << diagram_name.ToAscii()
               << " contains a channel rename " << channel_rename.ToAscii()
               << " which is not a valid identifier." << endl;
          return false;
        }
      }

      // get actions of referenced diagram
      list_of_action ref_actions;
      wxString ref_id = get_child_value(reference, _T("id"));
      if(is_process_reference)
      {
        // channel lies on a process reference
        try
        {
          ref_actions = get_process_actions(p_doc_root, ref_id, datatype_spec);
        }
        catch(...)
        {
          return false;
        }
      }
      else
      {
        // channel lies on an architecture reference
        try
        {
          ref_actions = get_architecture_visibles(p_doc_root, ref_id, datatype_spec);
        }
        catch(...)
        {
          return false;
        }
      }

      // loop through actions of refenced diagram
      bool found = false;
      for(unsigned int i=0; i<ref_actions.GetCount(); ++i)
      {
        if(ref_actions[i].get_name() == channel_name)
        {
          found = true;
          break;
        }
      }
      if(!found)
      {
        cerr << "Architecture diagram " << diagram_name.ToAscii()
             << " contains a channel with name \"" << channel_name.ToAscii()
             << "\" that does not correspond to a (visible-) action inside its associated reference." << endl;
        return false;
      }
    }
  }
  return true;
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

  for(unsigned int i=0; i<diagram_names.GetCount(); ++i)
  {
    for(unsigned int j=i+1; j<diagram_names.GetCount(); ++j)
    {
      if(diagram_names[i] == diagram_names[j])
      {
        cerr << "Diagram name " << diagram_names[i].ToAscii() << " is not unique." << endl;
        return false;
      }
    }
  }

  return true;
}
