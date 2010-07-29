include(BundleUtilities)

# mcrl2_fix_mac_unix_tool provides extended functionality to BundleUtilities.
#
# It fixes a mac unix tool, such that it can be moved, and copied to other machines
# as long as all of the mcrl2 libraries are relative to the executable and the 
# system libraries are compatible.
#
# Gathers all the keys for all the executable fixes each one up according
# to its own list of prerequisites.
#
if( APPLE )
function(mcrl2_fix_mac_unix_tool app dirs)
  message(STATUS "fixup_bundle")
  message(STATUS "  app='${app}'")
  message(STATUS "  dirs='${dirs}'")

    get_filename_component(exepath "${executable}" PATH)

    message(STATUS "mcrl2_fix_mac_unix_tool: preparing...")
    mcrl2_get_bundle_keys("${app}" "" "${dirs}" keys)

    message(STATUS "fixup_unix_tool: copying...")
    list(LENGTH keys n)
    math(EXPR n ${n}*2)

    set(i 0)
    message(STATUS "mcrl2_fix_mac_unix_tool: fixing...")
    foreach(key ${keys})
      math(EXPR i ${i}+1)
        message(STATUS "${i}/${n}: fixing up '${${key}_RESOLVED_EMBEDDED_ITEM}'")
        mcrl2_fixup_bundle_item("${${key}_RESOLVED_EMBEDDED_ITEM}" "${exepath}" "${dirs}")
    endforeach(key)

    message(STATUS "mcrl2_fix_mac_unix_tool: cleaning up...")
    clear_bundle_keys(keys)

  message(STATUS "fixup_unix_tool: done")
endfunction(mcrl2_fix_mac_unix_tool)

function(mcrl2_get_bundle_keys app libs dirs keys_var)
  set(${keys_var} PARENT_SCOPE)

  get_bundle_and_executable("${app}" bundle executable valid)
  if(valid)
#   # Always use the exepath of the main bundle executable for @executable_path
#   # replacements:
#   #
    get_filename_component(exepath "${executable}" PATH)

    # Get keys for for the executable.
    # The list of keys should be complete when all prerequisites of the 
    # binary has been analyzed.
     
    # Add the exe itself to the keys:
    mcrl2_set_bundle_key_values(${keys_var} "${app}" "${app}" "${exepath}" "${dirs}" 0)

    # Add each prerequisite to the keys:
    #
    set(prereqs "")
    get_prerequisites("${app}" prereqs 1 1 "${exepath}" "${dirs}")
    foreach(pr ${prereqs})
      mcrl2_set_bundle_key_values(${keys_var} "${app}" "${pr}" "${exepath}" "${dirs}" 0)
    endforeach(pr)

    # Propagate values to caller's scope:
    #
    set(${keys_var} ${${keys_var}} PARENT_SCOPE)
    foreach(key ${${keys_var}})
      set(${key}_ITEM "${${key}_ITEM}" PARENT_SCOPE)
      set(${key}_RESOLVED_ITEM "${${key}_RESOLVED_ITEM}" PARENT_SCOPE)
      set(${key}_DEFAULT_EMBEDDED_PATH "${${key}_DEFAULT_EMBEDDED_PATH}" PARENT_SCOPE)
      set(${key}_EMBEDDED_ITEM "${${key}_EMBEDDED_ITEM}" PARENT_SCOPE)
      set(${key}_RESOLVED_EMBEDDED_ITEM "${${key}_RESOLVED_EMBEDDED_ITEM}" PARENT_SCOPE)
      set(${key}_COPYFLAG "${${key}_COPYFLAG}" PARENT_SCOPE)
    endforeach(key)
  endif(valid)
endfunction(mcrl2_get_bundle_keys)

function(mcrl2_set_bundle_key_values keys_var context item exepath dirs copyflag)
  get_filename_component(item_name "${item}" NAME)

  get_item_key("${item}" key)

  list(LENGTH ${keys_var} length_before)
  gp_append_unique(${keys_var} "${key}")
  list(LENGTH ${keys_var} length_after)

  if(NOT length_before EQUAL length_after)
    mcrl2_gp_resolve_item("${context}" "${item}" "${exepath}" "${dirs}" resolved_item)

    mcrl2_gp_item_default_embedded_path("${item}" default_embedded_path)

    # embedded path:
    #
    set(embedded_item "${default_embedded_path}/${item_name}")

    # Replace @executable_path and resolve ".." references:
    #
    string(REPLACE "@executable_path" "${exepath}" resolved_embedded_item "${embedded_item}")
    get_filename_component(resolved_embedded_item "${resolved_embedded_item}" ABSOLUTE)

    set(resolved_embedded_item "${resolved_item}")

    set(${keys_var} ${${keys_var}} PARENT_SCOPE)
    set(${key}_ITEM "${item}" PARENT_SCOPE)
    set(${key}_RESOLVED_ITEM "${resolved_item}" PARENT_SCOPE)
    set(${key}_DEFAULT_EMBEDDED_PATH "${default_embedded_path}" PARENT_SCOPE)
    set(${key}_EMBEDDED_ITEM "${embedded_item}" PARENT_SCOPE)
    set(${key}_RESOLVED_EMBEDDED_ITEM "${resolved_embedded_item}" PARENT_SCOPE)
    set(${key}_COPYFLAG "${copyflag}" PARENT_SCOPE)
  else(NOT length_before EQUAL length_after)
    #message("warning: item key '${key}' already in the list, subsequent references assumed identical to first")
  endif(NOT length_before EQUAL length_after)
endfunction(mcrl2_set_bundle_key_values)

function(mcrl2_gp_item_default_embedded_path item default_embedded_path_var)
  set(path "@executable_path/../libs")
  set(${default_embedded_path_var} "${path}" PARENT_SCOPE)
endfunction(mcrl2_gp_item_default_embedded_path)

# gp_resolve_item context item exepath dirs resolved_item_var
#
# Resolve an item into an existing full path file.
#
# Override on a per-project basis by providing a project-specific
# gp_resolve_item_override function.
#
function(mcrl2_gp_resolve_item context item exepath dirs resolved_item_var)
  set(resolved 0)
  set(resolved_item "${item}")

  # Is it already resolved?
  #
  if(EXISTS "${resolved_item}")
    set(resolved 1)
  endif(EXISTS "${resolved_item}")

  if(NOT resolved)
    if(item MATCHES "@executable_path")
      #
      # @executable_path references are assumed relative to exepath
      #
      string(REPLACE "@executable_path" "${exepath}" ri "${item}")
      get_filename_component(ri "${ri}" ABSOLUTE)

      if(EXISTS "${ri}")
        #message(STATUS "info: embedded item exists (${ri})")
        set(resolved 1)
        set(resolved_item "${ri}")
      #else(EXISTS "${ri}")
        #message(STATUS "warning: embedded item does not exist '${ri}'")
      endif(EXISTS "${ri}")
    endif(item MATCHES "@executable_path")
  endif(NOT resolved)

  if(NOT resolved)
    if(item MATCHES "@loader_path")
      #
      # @loader_path references are assumed relative to the
      # PATH of the given "context" (presumably another library)
      #
      get_filename_component(contextpath "${context}" PATH)
      string(REPLACE "@loader_path" "${contextpath}" ri "${item}")
      get_filename_component(ri "${ri}" ABSOLUTE)

      if(EXISTS "${ri}")
        #message(STATUS "info: embedded item exists (${ri})")
        set(resolved 1)
        set(resolved_item "${ri}")
      #else(EXISTS "${ri}")
        #message(STATUS "warning: embedded item does not exist '${ri}'")
      endif(EXISTS "${ri}")
    endif(item MATCHES "@loader_path")
  endif(NOT resolved)

  if(NOT resolved)
    set(ri "ri-NOTFOUND")
    find_file(ri "${item}" ${exepath} ${dirs} NO_DEFAULT_PATH)
    find_file(ri "${item}" ${exepath} ${dirs} /usr/lib)
    if(ri)
      #message(STATUS "info: 'find_file' in exepath/dirs (${ri})")
      set(resolved 1)
      set(resolved_item "${ri}")
      set(ri "ri-NOTFOUND")
    endif(ri)
  endif(NOT resolved)

  if(NOT resolved)
    if(item MATCHES "[^/]+\\.framework/")
      set(fw "fw-NOTFOUND")
      find_file(fw "${item}"
        "~/Library/Frameworks"
        "/Library/Frameworks"
        "/System/Library/Frameworks"
      )
      if(fw)
        #message(STATUS "info: 'find_file' found framework (${fw})")
        set(resolved 1)
        set(resolved_item "${fw}")
        set(fw "fw-NOTFOUND")
      endif(fw)
    endif(item MATCHES "[^/]+\\.framework/")
  endif(NOT resolved)

  # Provide a hook so that projects can override item resolution
  # by whatever logic they choose:
  #
# if(COMMAND gp_resolve_item_override)
#   gp_resolve_item_override("${context}" "${item}" "${exepath}" "${dirs}" resolved_item resolved)
# endif(COMMAND gp_resolve_item_override)

  set(${resolved_item_var} "${resolved_item}" PARENT_SCOPE)
endfunction(mcrl2_gp_resolve_item)

function(mcrl2_fixup_bundle_item resolved_embedded_item exepath dirs)
  # This item's key is "ikey":
  #
  get_item_key("${resolved_embedded_item}" ikey)

  set(prereqs "")
  get_prerequisites("${resolved_embedded_item}" prereqs 1 0 "${exepath}" "${dirs}")

  set(changes "")

  foreach(pr ${prereqs})
    # Each referenced item's key is "rkey" in the loop:
    #
    get_item_key("${pr}" rkey)

    if(NOT "${${rkey}_EMBEDDED_ITEM}" STREQUAL "")
      set(changes ${changes} "-change" "${pr}" "${${rkey}_EMBEDDED_ITEM}")
    else(NOT "${${rkey}_EMBEDDED_ITEM}" STREQUAL "")
      message("warning: unexpected reference to '${pr}'")
    endif(NOT "${${rkey}_EMBEDDED_ITEM}" STREQUAL "")
  endforeach(pr)

  # Change this item's id and all of its references in one call
  # to install_name_tool:
  #
  execute_process(COMMAND install_name_tool
    ${changes} -id "${${ikey}_EMBEDDED_ITEM}" "${resolved_embedded_item}"
  )
endfunction(mcrl2_fixup_bundle_item)

endif( APPLE )

