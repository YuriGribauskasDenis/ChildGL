cmake_minimum_required(VERSION 2.8)

project(p10)

add_executable(p10 main.cpp)
target_link_libraries(p10 PGUPV)

include(../PGUPV/pgupv.cmake)

set_target_properties( p10 PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS p10 DESTINATION ${PG_SOURCE_DIR}/bin)
