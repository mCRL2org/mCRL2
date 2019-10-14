find_package(PythonModules COMPONENTS sphinx dparser REQUIRED)
find_package(Doxygen REQUIRED)
find_package(Xsltproc REQUIRED)

add_subdirectory(sphinx)
