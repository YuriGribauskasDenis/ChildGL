cmake_minimum_required(VERSION 2.8)

project(p9)

add_executable(p9 main.cpp)
target_link_libraries(p9 PGUPV)

include(../PGUPV/pgupv.cmake)

set_target_properties( p9 PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS p9 DESTINATION ${PG_SOURCE_DIR}/bin)
