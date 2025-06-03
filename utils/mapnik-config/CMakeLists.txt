set(MAPNIK_CMAKE_CXX_FLAGS "Where to get these?")

find_program(GIT_EXECUTABLE git REQUIRED)
execute_process(COMMAND ${GIT_EXECUTABLE} rev-list --max-count=1 HEAD
  OUTPUT_VARIABLE GIT_REVISION
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(COMMAND ${GIT_EXECUTABLE} describe
  OUTPUT_VARIABLE GIT_DESCRIBE
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

find_program(ICU_CONFIG_EXECUTABLE icu-config)
execute_process(COMMAND ${ICU_CONFIG_EXECUTABLE} --icudatadir
  OUTPUT_VARIABLE ICU_DATA_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

pkg_get_variable(GDAL_DATA_DIR gdal datadir)
pkg_get_variable(PROJ_LIB_DIR proj datadir)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/mapnik-config.in.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/mapnik-config.cpp
)

add_executable(mapnik-config mapnik-config.cpp)
target_include_directories(mapnik-config BEFORE PRIVATE
  ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(mapnik-config PRIVATE
    ICU::uc
)

mapnik_install_utility(mapnik-config)
