cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(Lucas)

add_definitions("-Wall")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX $ENV{HOME}/Software/Lucas CACHE PATH "Lucas install prefix" FORCE)
endif()

option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

add_executable(Lucas Lucas.cc ${sources} ${headers})
target_link_libraries(Lucas ${Geant4_LIBRARIES})

install(TARGETS Lucas DESTINATION bin)
