# ===========================================================================
# If this module fails to find CLHEP headers and libraries, you can
# try to set them directly in CMakeCached.txt
#
#  CLHEP_INCLUDE_DIR    The path to the CLHEP include directory: cached
#
#  CLHEP_LIBRARY        The path to the CLHEP library: cached
#
# Author  2013  Bogdan Pawlik <bogdan.pawlik@ifj.edu.pl>>.
#============================================================================

# Define CLHEP_ROOT_DIR environment variable in bashrc. Then it will work.
    set(CLHEP_ROOT_DIR $ENV{CLHEP_ROOT_DIR})

    if(CLHEP_CONFIG_EXECUTABLE-NOTFOUND)
        FIND_PROGRAM(CLHEP_CONFIG_EXECUTABLE clhep-config PATHSS ${CLHEP_ROOT_DIR}
        PATHS_SUFFIXES bin
        NO_DEFAULT_PATH)

        if(NOT CLHEP_ROOT_DIR)
            FIND_PROGRAM( CLHEP_CONFIG_EXECUTABLE clhep-config)
        else()
            message(STATUS "CLHEP is found: ${CLHEP_ROOT_DIR}")
        endif()
    endif()

    if(CLHEP_CONFIG_EXECUTABLE)
    # clhep version
        execute_process(COMMAND "${CLHEP_CONFIG_EXECUTABLE}" --version
            OUTPUT_VARIABLE _output
            RESULT_VARIABLE _exit_code
            OUTPUT_STRIP_TRAILING_WHITESPACE)

        if(_exit_code EQUAL 0)
            set( CLHEP_VERSION ${_output})
            separate_arguments(CLHEP_VERSION)
            list(REMOVE_AT CLHEP_VERSION 0) 
        else()
            set(CLHEP_VERSION)
        endif()

    # clhep prefix
        execute_process( COMMAND "${CLHEP_CONFIG_EXECUTABLE}" --prefix |sed {s/"//}|sed {s/"//}
            OUTPUT_VARIABLE CLHEP_ROOT
            RESULT_VARIABLE _exit_code
            OUTPUT_STRIP_TRAILING_WHITESPACE)

        if(NOT _exit_code EQUAL 0)
            set(CLHEP_ROOT)
        endif()
    endif()

# clhep headers dir
    find_path(CLHEP_INCLUDE_DIR 
        NAMES CLHEP/Units/defs.h
        PATHS ${CLHEP_ROOT_DIR}
        PATH_SUFFIXES include
        NO_DEFAULT_PATH
        DOC "Path to the CLHEP headers")

# Find the CLHEP library. Prefer lib64 if available.
    find_library(CLHEP_LIBRARY
        NAMES  CLHEP
        HINTS ${CLHEP_ROOT_DIR}
        PATH_SUFFIXES lib64 lib
        DOC "Path to the CLHEP library")

    if(CLHEP_LIBRARY)
        execute_process(COMMAND dirname ${CLHEP_LIBRARY}
        OUTPUT_VARIABLE CLHEP_LIBRARY_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    endif()

    set(CLHEP_LIBRARIES_DIR ${CLHEP_LIBRARY_DIR})
    set(CLHEP_LIBRARIES ${CLHEP_LIBRARY})
    set(CLHEP_INCLUDE_DIRS ${CLHEP_INCLUDE_DIR})


# Check for version compatability
    if(CLHEP_VERSION)
        message(STATUS "Found CLHEP Version ${CLHEP_VERSION}")  
        if(CLHEP_FIND_VERSION)
            set(CLHEP_VERSIONING_TESTS CLHEP_VERSION_COMPATIBLE)
            if("${CLHEP_VERSION}" VERSION_LESS "${CLHEP_FIND_VERSION}")
                set(CLHEP_VERSION_COMPATIBLE FALSE)
            else()
                set(CLHEP_VERSION_COMPATIBLE TRUE)
                if(CLHEP_FIND_VERSION_EXACT)
                    if("${CLHEP_VERSION}" VERSION_EQUAL "${CLHEP_FIND_VERSION}")
                        set(CLHEP_VERSION_EXACT TRUE)
                    endif()
                    list(APPEND CLHEP_VERSIONING_TESTS CLHEP_VERSION_EXACT)
                endif()
            endif()
        endif()
    endif(CLHEP_VERSION)

# Construct an error message

    set(CLHEP_DEFAULT_MSG "Could NOT find CLHEP:\n")
    if(NOT CLHEP_INCLUDE_DIR)
        set(CLHEP_DEFAULT_MSG "${CLHEP_DEFAULT_MSG}CLHEP Header Path Not Found\n")
    endif()

    if(NOT CLHEP_LIBRARY)
        set(CLHEP_DEFAULT_MSG "${CLHEP_DEFAULT_MSG}CLHEP Library Not Found\n")
    endif()


    if(CLHEP_FIND_VERSION)
        if(NOT CLHEP_VERSION_COMPATIBLE)
            set(CLHEP_DEFAULT_MSG "${CLHEP_DEFAULT_MSG}Incompatible versions, ${CLHEP_VERSION}(found) < ${CLHEP_FIND_VERSION}(required)\n")
        endif()

        if(CLHEP_FIND_VERSION_EXACT)
            if(NOT CLHEP_VERSION_EXACT)
                set(CLHEP_DEFAULT_MSG "${CLHEP_DEFAULT_MSG}Non-exact versions, ${CLHEP_VERSION}(found) != ${CLHEP_FIND_VERSION}(required)\n")
            endif()
        endif()
    endif()



#----------------------------------------------------------------------------
# Handle the QUIETLY and REQUIRED arguments, setting CLHEP_FOUND to TRUE if
# all listed variables are TRUE
#
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(
        CLHEP 
        "${CLHEP_DEFAULT_MSG}"
        CLHEP_LIBRARY
        CLHEP_INCLUDE_DIR
        CLHEP_LIBRARY_DIR
        ${CLHEP_VERSIONING_TESTS})


#----------------------------------------------------------------------------
# If we found CLHEP, set the needed non-cache variables
#
    if(CLHEP_FOUND)
        set(CLHEP_LIBRARIES ${CLHEP_LIBRARY})
        set(CLHEP_INCLUDE_DIRS ${CLHEP_INCLUDE_DIR})
    endif()

#----------------------------------------------------------------------------
# Mark cache variables that can be adjusted as advanced
#
    mark_as_advanced(CLHEP_CONFIG_EXECUTABLE)
    mark_as_advanced(CLHEP_INCLUDE_DIRS)
    mark_as_advanced(CLHEP_LIBRARY)

