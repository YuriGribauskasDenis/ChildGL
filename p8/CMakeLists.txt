cmake_minimum_required(VERSION 2.8)

project(p8)

add_executable(p8 main.cpp)
target_link_libraries(p8 PGUPV)

include(../PGUPV/pgupv.cmake)

set_target_properties( p8 PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS p8 DESTINATION ${PG_SOURCE_DIR}/bin)
