cmake_minimum_required(VERSION 2.8)

project(p7)

add_executable(p7 main.cpp)
target_link_libraries(p7 PGUPV)

include(../PGUPV/pgupv.cmake)

set_target_properties( p7 PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS p7 DESTINATION ${PG_SOURCE_DIR}/bin)
