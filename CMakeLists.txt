# Tested only with 3.3 version
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
# Project name
project(Lucas)

# Print all warning to possibly improve the code
add_definitions("-Wall")

# Default install directory if --prefix is not mentioned is
# $HOME/Software/Lucas
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX $ENV{HOME}/Software/Lucas CACHE PATH "Lucas install prefix" FORCE)
endif()

# Change to off if you want to build without visualization drivers
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)

if(WITH_GEANT4_UIVIS)
    # Find geant with vis drivers
    find_package(Geant4 REQUIRED ui_all vis_all)
else()
    # Find geant without vis drivers
    find_package(Geant4 REQUIRED)
endif()

# All include files directories
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

# Projects source files and headers
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

# Compile executable from sources and headers
add_executable(Lucas Lucas.cc ${sources} ${headers})
# Link libraries
target_link_libraries(Lucas ${Geant4_LIBRARIES})

# Install - copies Lucas binary file to path/to/install/bin/ folder
install(TARGETS Lucas DESTINATION bin)
