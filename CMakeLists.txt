# Setup the project
    cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

# project name
    project(Lucas)

# project version
    set(${PROJECT_NAME}_VERSION_MAJOR 1)
    set(${PROJECT_NAME}_VERSION_MINOR 0)
    set(${PROJECT_NAME}_VERSION_PATCH 0)
    set(PROJECT_VERSION ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH})

# require proper c++
    add_definitions("-Wall -ansi -pedantic")
    add_definitions("-Wno-long-long")

# find Lucas cmake modules
    set(CMAKE_MODULE_PATH  ${PROJECT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_path})

# set default installation destination
    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(CMAKE_INSTALL_PREFIX $ENV{HOME}/Software/${PROJECT_NAME}/${PROJECT_VERSION} CACHE PATH "Lucas install prefix" FORCE)
    endif()


# Find Geant4, activate all available UI and Vis drivers
    find_package(Geant4 9.6.1 REQUIRED ui_all vis_all)

# Setup Geant4 include directories and compile definitions
    if(GEANT4_FOUND)
        include(${Geant4_USE_FILE})
    else()
        message(FATAL_ERROR "Geant4 is not found :(")
        return()
    endif()

# Find ROOT
    find_package(ROOT REQUIRED)
    if(NOT ROOT_FOUND)
        message(FATAL_ERROR "ROOT is not found :(")
        return()
    endif()

# Lucas requires shared libraries
    if(BUILD_STATIC_LIBS AND NOT BUILD_SHARED_LIBS)
        message(STATUS "Geant4 is built with only static libraires. Lucas require share libraries")
        return()
    endif()

# Locate sources and headers for this project
    set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
    include_directories(${PROJECT_INCLUDE_DIR} ${GEANT4_INCLUDE_DIRS} ${ROOT_INCLUDE_DIR})
    file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
    file(GLOB headers ${PROJECT_INCLUDE_DIR}/*.hh)
    file(GLOB headers ${PROJECT_INCLUDE_DIR}/*.h)
    file(GLOB headers ${ROOT_INCLUDE_DIR}/*.h)


    set(libsuffix .so)
    set(ROOT_LIBRARY_PROPERTIES ${ROOT_LIBRARY_PROPERTIES} SUFFIX ${libsuffix})

# Add scripts to configure run time enviroment
    configure_file("${PROJECT_SOURCE_DIR}/cmake/templates/setup.sh.in"  "setup.sh"  @ONLY)
    configure_file("${PROJECT_SOURCE_DIR}/cmake/templates/setup.csh.in" "setup.csh" @ONLY)

# Add the library and executable, and link it to the Geant4 libraries
    add_library(Lucas SHARED ${sources})
    target_link_libraries(Lucas ${GEANT4_LIBRARIES} ${ROOT_LIBRARIES})

    add_executable(bin_Lucas Lucas.cc)
    set_target_properties(bin_Lucas PROPERTIES OUTPUT_NAME Lucas)
    target_link_libraries(bin_Lucas Lucas ${GEANT4_LIBRARIES} ${ROOT_LIBRARIES})

# Copy all scripts to the build directory, i.e. the directory in which we
# build Lucas. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.

# Install the executable and libraries to 'bin'and 'lib' directory under CMAKE_INSTALL_PREFIX
    install(PROGRAMS ${PROJECT_BINARY_DIR}/setup.sh ${PROJECT_BINARY_DIR}/setup.csh DESTINATION bin)
    install(TARGETS bin_Lucas DESTINATION bin)
    install(TARGETS Lucas DESTINATION lib)


