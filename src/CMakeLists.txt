add_library(shellprogress STATIC ShellProgress.cc)

add_library(stlread SHARED STLReader.cc)
target_include_directories(stlread PUBLIC ${Boost_INCLUDE_DIRS} ${Python3_INCLUDE_DIRS} .)
if (${Boost_FOUND} AND ${Python3_FOUND})
  target_compile_definitions(stlread PUBLIC PYTHON_EXPORT)
  target_link_libraries(stlread Boost::python Boost::numpy Python3::Python shellprogress)
else()
  target_link_libraries(stlread shellprogress)
endif()




