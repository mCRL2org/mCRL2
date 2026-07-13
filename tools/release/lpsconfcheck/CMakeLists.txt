mcrl2_add_tool(lpsconfcheck
  SOURCES
    lpsconfcheck.cpp
  DEPENDS
    mcrl2_lps
)

# lpsconfcheck.cpp produces a very large object file due to deeply nested template
# instantiations (replace_capture_avoiding + where_clause). On macOS ARM64 the linker
# uses 24-bit branch fixups that have a ±128MB range; when a single .o's __text section
# exceeds this the build fails with "fixup error (kind=arm32_b24), invalid branch
# instruction". -ffunction-sections places every function in its own section so the
# linker can arrange them at arbitrary distances.
if(APPLE)
  target_compile_options(lpsconfcheck PRIVATE -ffunction-sections)
endif()
