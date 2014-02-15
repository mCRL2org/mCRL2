if(NOT WIN32)
  set(COMPILING_REWRITER_SRC detail/rewrite/jittyc.cpp)
  set(COMPILING_REWRITER_DEPS dl)
endif()

add_mcrl2_library(data
  INSTALL_HEADERS TRUE
  SOURCES
    data.cpp
    data_specification.cpp
    typecheck.cpp
    detail/enumerate/enum_standard.cpp
    detail/prover/smt_lib_solver.cpp
    detail/rewrite/with_prover.cpp
    detail/rewrite/jitty.cpp
    detail/rewrite/rewrite.cpp
    ${COMPILING_REWRITER_SRC}
  DEPENDS
    mcrl2_core
    mcrl2_utilities
    ${COMPILING_REWRITER_DEPS}
)

if(APPLE)
  # For MacOS, we also install the Boost header files that the compiling rewriter depends on. On
  # *nix distributions, we do not do this, but instead require the user to have the Boost headers
  # available. This is because most distributions do not want you to distribute (sources of)
  # software that is not maintained by you, because it prevents that software (source) to be
  # updated when security issues are found.

  set(MCRL2_BOOST_HEADER_FILES
    aligned_storage.hpp      implicit_cast.hpp     ref.hpp
    assert.hpp               integer_traits.hpp    shared_ptr.hpp
    array.hpp                is_placeholder.hpp    signals2.hpp
    bind.hpp                 iterator.hpp          smart_ptr.hpp
    call_traits.hpp          lexical_cast.hpp      static_assert.hpp
    checked_delete.hpp       limits.hpp            swap.hpp
    concept_check.hpp        mem_fn.hpp            throw_exception.hpp
    config.hpp               memory_order.hpp      type.hpp
    cstdint.hpp              next_prior.hpp        utility.hpp
    current_function.hpp     noncopyable.hpp       visit_each.hpp
    get_pointer.hpp
  )

  set(MCRL2_BOOST_HEADER_DIRS
    bind        functional    preprocessor
    concept     iterator      range
    config      mpl           signals2
    container   math          smart_ptr
    detail      multi_index   type_traits
    exception   numeric       utility
  )

  foreach( hfile ${MCRL2_BOOST_HEADER_FILES})
    install(FILES "${Boost_INCLUDE_DIRS}/boost/${hfile}" DESTINATION "${MCRL2_INCLUDE_PATH}/boost" COMPONENT Headers)
  endforeach()

  foreach( hdir ${MCRL2_BOOST_HEADER_DIRS})
    install(DIRECTORY "${Boost_INCLUDE_DIRS}/boost/${hdir}" DESTINATION "${MCRL2_INCLUDE_PATH}/boost" COMPONENT Headers)
  endforeach()
endif()
