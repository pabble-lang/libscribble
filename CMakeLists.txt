project(libscribble CXX C)
cmake_minimum_required(VERSION 2.8)
set(PROJECT_DESCRIPTION "Scribble programming library for C/C++")

#
# Version.
#
set(libscribble_VERSION_MAJOR 2)
set(libscribble_VERSION_MINOR 0)
set(libscribble_VERSION_PATCH 0)
set(libscribble_VERSION ${libscribble_VERSION_MAJOR}.${libscribble_VERSION_MINOR}.${libscribble_VERSION_PATCH})


#
# Directories.
#
set(libscribble_SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(libscribble_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(libscribble_BINARY_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(libscribble_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(libscribble_TESTS_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set(libscribble_DOCS_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/docs)
set(libscribble_THIRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)


# Copy the version to main header file.
configure_file(
    ${libscribble_INCLUDE_DIR}/scribble/scribble.h.in
    ${PROJECT_BINARY_DIR}/include/scribble/scribble.h)


#
# Build dependencies.
#
include(FindPkgConfig)
pkg_check_modules(SESSTYPE REQUIRED libsesstype>=2.0.0)

#
# Z3
#
include(FindPythonInterp)
if(NOT EXISTS ${libscribble_THIRDPARTY_DIR}/z3/build/libz3.so AND
        IS_DIRECTORY ${libscribble_THIRDPARTY_DIR}/z3 AND
        PYTHONINTERP_FOUND)
  execute_process(
      COMMAND ${PYTHON_EXECUTABLE} scripts/mk_make.py
      WORKING_DIRECTORY "${libscribble_THIRDPARTY_DIR}/z3")
  execute_process(
      COMMAND make
      WORKING_DIRECTORY "${libscribble_THIRDPARTY_DIR}/z3/build")
endif()

#
# Build configurations.
#
set(CMAKE_C_FLAGS   "-std=gnu11 -O0 -Wall -Werror -D__DEBUG__")
set(CMAKE_CXX_FLAGS "-std=c++11 -O0 -Wall -Werror -D__DEBUG__")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#
# Sources: libscribble
#
include_directories(${libscribble_INCLUDE_DIR} ${libscribble_SOURCE_DIR} ${PROJECT_BINARY_DIR}/include)
add_subdirectory(${libscribble_SOURCE_DIR})
add_library(scribble SHARED ${libscribble_SOURCE})

#
#parser Lex and Yacc
#
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

if(FLEX_FOUND)
    flex_target(lexer
        ${libscribble_SOURCE_DIR}/parser/lexer.l
        ${CMAKE_CURRENT_BINARY_DIR}/src/lexer.c)
endif()

if(BISON_FOUND)
    bison_target(parser
        ${libscribble_SOURCE_DIR}/parser/parser.y
        ${CMAKE_CURRENT_BINARY_DIR}/src/parser.c)
endif()

add_flex_bison_dependency(lexer parser)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)
add_executable(parser-tool
    ${libscribble_SOURCE_DIR}/tool/parser-tool.c
    ${BISON_parser_OUTPUTS}
    ${FLEX_lexer_OUTPUTS})
target_link_libraries(parser-tool ${SESSTYPE_LIBRARIES})
target_link_libraries(parser-tool scribble)

#
# Documentation.
#
find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(
        ${libscribble_DOCS_DIR}/Doxyfile.in
        ${PROJECT_BINARY_DIR}/docs/Doxyfile @ONLY
    )
    add_custom_target(docs
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/docs/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
        )
endif(DOXYGEN_FOUND)


#
# Tests.
#
if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/third_party/gtest)
  enable_testing(true)
  add_subdirectory(third_party/gtest)
  message(STATUS "Looking for gtest (unit test) - found")
  include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
  link_directories(${LIBRARY_OUTPUT_PATH})
  add_subdirectory(${libscribble_TESTS_DIR})
else()
  message(WARNING "Looking for gtest (unit test) - not found (no unit test will be run).")
endif()


#
# Install.
#
set(CMAKE_INSTALL_PREFIX "/usr")
install(DIRECTORY ${libscribble_INCLUDE_DIR}/
        DIRECTORY ${PROJECT_BINARY_DIR}/include/
        DESTINATION include
        FILES_MATCHING PATTERN *.h)
install(TARGETS scribble
        EXPORT scribble
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc"
        DESTINATION lib/pkgconfig)


#
# pkg-config.
# Generate a .pc file for other to find
#
set(PKG_CONFIG_REQUIRES "")
set(PKG_CONFIG_LIBDIR "\${prefix}/lib")
set(PKG_CONFIG_INCLUDEDIR "\${prefix}/include")
set(PKG_CONFIG_LIBS "-L\${libdir} -lsesstype")
set(PKG_CONFIG_CFLAGS "-I\${includedir}")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/pkg-config.pc.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc")


#
# Packaging.
#
include(package.cmake)
