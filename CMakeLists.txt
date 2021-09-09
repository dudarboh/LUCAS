cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(Lucas)

# Add color output for important messages
if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(ColourBold  "${Esc}[1m")
  set(Red         "${Esc}[31m")
  set(Green       "${Esc}[32m")
  set(Yellow      "${Esc}[33m")
  set(Blue        "${Esc}[34m")
  set(Magenta     "${Esc}[35m")
  set(Cyan        "${Esc}[36m")
  set(White       "${Esc}[37m")
  set(BoldRed     "${Esc}[1;31m")
  set(BoldGreen   "${Esc}[1;32m")
  set(BoldYellow  "${Esc}[1;33m")
  set(BoldBlue    "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan    "${Esc}[1;36m")
  set(BoldWhite   "${Esc}[1;37m")
endif()

# Print all warning to possibly improve the code
add_definitions("-O3 -pedantic -Wall -Wextra")

# Change to off if you want to build without visualization drivers
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)

if(WITH_GEANT4_UIVIS)
    # Find geant with vis drivers
    find_package(Geant4 REQUIRED ui_all vis_all)
else()
    # Find geant without vis drivers
    find_package(Geant4 REQUIRED)
endif()
message(STATUS ${Green}"Version of found Geant4: " ${Geant4_VERSION} ${ColourReset})

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


set(SCRIPTS
  init_vis.mac
  startRun.mac
  )

foreach(_script ${SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

# Install - copies Lucas binary file to path/to/install/bin/ folder
install(TARGETS Lucas DESTINATION ${PROJECT_SOURCE_DIR}/bin)
