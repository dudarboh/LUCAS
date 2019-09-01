cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(Lucas)

add_definitions("-Wall")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX $ENV{HOME}/Lucas CACHE PATH "Lucas install prefix" FORCE)
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

# set(COPY_FILES noise.txt)
# foreach(file ${COPY_FILES})
#     configure_file(${PROJECT_SOURCE_DIR}/${file} ${PROJECT_BINARY_DIR}/${file} COPYONLY)
# endforeach()

install(TARGETS Lucas DESTINATION bin)
install(FILES init_vis.mac startRun.mac noise.txt DESTINATION . 
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
        GROUP_READ GROUP_WRITE GROUP_EXECUTE
        WORLD_READ WORLD_WRITE WORLD_EXECUTE)
