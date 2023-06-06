# gen_contour
add_executable(gen_contour gen_contour.cc)
add_dependencies(stlread shellprogress)
add_dependencies(gen_contour stlread)

target_include_directories(gen_contour PUBLIC ${Boost_INCLUDE_DIRS} ${Python3_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/src)
if (${Boost_FOUND} AND ${Python3_FOUND})
  target_compile_definitions(gen_contour PUBLIC PYTHON_EXPORT)
  target_link_libraries(gen_contour Boost::python Boost::numpy stlread Python3::Python)
else()
  target_link_libraries(gen_contour stlread)
endif()

## plot
find_package(Python3 COMPONENTS Interpreter Development)
if (NOT ${Python3_FOUND})
  message(STATUS "Python3 not FOUND, plot_contour not building")
  return()
endif()


add_executable(plot_contour plot_contour.cc)
add_dependencies(plot_contour stlread)

target_compile_features(plot_contour INTERFACE cxx_std_11)

target_link_libraries(plot_contour Python3::Python Python3::Module)

find_package(Python3 COMPONENTS NumPy)
if(${Python3_NumPy_FOUND})
  message(STATUS "**Numpy Found")
  target_link_libraries(plot_contour Python3::NumPy)
else()
  message(STATUS "**Numpy Not Found")
  target_compile_definitions(plot_contour WITHOUT_NUMPY)
endif()

target_include_directories(plot_contour PUBLIC ${PROJECT_SOURCE_DIR}/src 
                        ${PROJECT_SOURCE_DIR}/3rdParts ${Python3_INCLUDE_DIRS} .)
target_link_libraries(plot_contour stlread)