cmake_minimum_required(VERSION 3.11...3.28)

project(integratecpp
  VERSION 1.0.0
  DESCRIPTION "An example of a C++ header-only library using CMake, doxygen, and Catch."
  HOMEPAGE_URL "https://github.com/hsloot/integratecpp"
  LANGUAGES CXX
)

option(BUILD_DOCS "Build and Install Documents (Requires Doxygen)" OFF)

## Make sure that custom modules like FindXYZ are found
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

include(GNUInstallDirs)

## Define Target
## =============

add_library(${CMAKE_PROJECT_NAME} INTERFACE)
target_include_directories(${CMAKE_PROJECT_NAME}
  INTERFACE
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inst/include>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/include>
)
target_compile_features(${CMAKE_PROJECT_NAME} INTERFACE cxx_std_17)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_DOCS)
  find_package(Doxygen REQUIRED dot)
  if(Doxygen_FOUND)
    add_subdirectory(pkgdown/cpp)
  else()
    message(STATUS "Doxygen not found, not building docs")
  endif()
endif()
