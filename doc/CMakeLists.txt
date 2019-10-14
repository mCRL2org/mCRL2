find_package(PythonModules 3.7 COMPONENTS sphinx dparser REQUIRED)
find_package(Doxygen REQUIRED)
find_package(Xsltproc REQUIRED)

add_subdirectory(sphinx)
