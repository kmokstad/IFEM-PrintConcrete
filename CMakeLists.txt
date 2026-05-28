cmake_minimum_required(VERSION 3.22)

project(PrintConcrete LANGUAGES C CXX)

if(NOT TARGET IFEM)
  if(IFEM_AS_SUBMODULE)
    add_subdirectory(../.. IFEM)
    set(IFEM_PATH ${PROJECT_SOURCE_DIR}/../..)
  else()
    get_filename_component(BUILD_DIR ${PROJECT_BINARY_DIR} NAME)
    set(IFEM_DIR ${PROJECT_SOURCE_DIR}/../../${BUILD_DIR})
    find_package(IFEM REQUIRED)
  endif()
endif()

ifem_add_dependency_dir(
  APP
    Elasticity
  DIR
    FiniteDeformation
  TARGET
    IFEM::FiniteDeformation
)

ifem_add_dependency_dir(
  APP
    Elasticity
  DIR
    Elasticity
  TARGET
    IFEM::Elasticity
)

ifem_add_application(
  NAME
    PrintConcrete
  SOURCES
    src/main.C
    src/SIM3DPrinter.C
    src/DruckerPrager.C
  HEADERS
    src/SIM3DPrinter.h
    src/DruckerPrager.h
  LIBRARIES
    IFEM::FiniteDeformation
)

# Testing
enable_testing()

ifem_add_regression_test(
  TARGET
    PrintConcrete
  TEST_FILES
    PrintConcrete2D-p2.reg
    PrintConcrete2D2-p2.reg
)

ifem_add_hdf5_test(
  TARGET
    PrintConcrete
  TEST_FILES
    PrintConcrete2D2-p2.hreg
)

if(IFEM_COMMON_APP_BUILD)
  set(TEST_APPS ${TEST_APPS} PARENT_SCOPE)
else()
  ifem_add_check_target()
endif()

# For generating the doxy
ifem_add_doc_target(
  TARGET
    PrintConcrete
  DEPENDENCIES
    FiniteDeformation
)

# Installation
include(GNUInstallDirs)
install(TARGETS PrintConcrete DESTINATION ${CMAKE_INSTALL_BINDIR})
