cmake_minimum_required(VERSION 3.12) 
set(CRONO_TARGET_NAME "xhptdc8_util_test")
project(${CRONO_TARGET_NAME})

# _____________________________________________________________________________
# Build Windows/Linux library for (Debug/Release) configurations.
# _____________________________________________________________________________

# General Validations and Configurations ______________________________________
# cd indirection from /tools to the project source code, "." if no shift
set(PROJ_SRC_INDIR ..)  

# Get the platform and architecure ____________________________________________
# Check supported architecture, and set `CRONO_GEN_ARCH` to `x86_64` for all
# variations of `x64` platforms, and to `x86` for all variations of `x86` 
# platforms.
# Set `CRONO_GEN_PLATFORM` to either `windows` or `linux`
SET(CRONO_GEN_PLATFORM "windows")
IF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(CRONO_GEN_PLATFORM "linux")
ENDIF()

list(APPEND x64_archs Win64 AMD64 x86_64)
IF(CMAKE_SYSTEM_PROCESSOR IN_LIST x64_archs)
        set(CRONO_GEN_ARCH "x86_64")
ENDIF()

# Include directories paths ___________________________________________________
# On repository `./include`
include_directories(${CRONO_TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/${PROJ_SRC_INDIR}/../../include
)

# Link to driver library ______________________________________________________
# On repository `./lib/x64`
set(CRONO_DEP_PKG_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${PROJ_SRC_INDIR}/../../lib/x64)
link_directories(${CRONO_DEP_PKG_LIB_DIR})

# Set output directory_________________________________________________________
# On `./util/bin` folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)

# Add the target  _____________________________________________________________
set(SOURCE ${PROJ_SRC_INDIR}/src/xhptds8_test_util.cpp)
add_executable(${CRONO_TARGET_NAME} "${SOURCE}")

# Windows-specific Configurations _____________________________________________
IF ( CRONO_GEN_PLATFORM  STREQUAL "windows")
    add_library(xhptdc8_driver_64 SHARED IMPORTED GLOBAL)
    set_target_properties(xhptdc8_driver_64 PROPERTIES IMPORTED_IMPLIB xhptdc8_driver_64.lib)  
    add_library(xhptdc8_util SHARED IMPORTED GLOBAL)
    set_target_properties(xhptdc8_util PROPERTIES IMPORTED_IMPLIB xhptdc8_util.lib)  
    target_link_libraries(${CRONO_TARGET_NAME} xhptdc8_util xhptdc8_driver_64)
    
    # Create exe directly on `CMAKE_RUNTIME_OUTPUT_DIRECTORY` without `Debug`
    # /`Release` sub-folder
    set_target_properties(${CRONO_TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )

    # Copy library to be able to run exe directly
    file(COPY ${CRONO_DEP_PKG_LIB_DIR}/xhptdc8_driver_64.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    file(COPY ${CRONO_DEP_PKG_LIB_DIR}/xhptdc8_util.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Linux-specific Configurations _______________________________________________
ELSEIF (CRONO_GEN_PLATFORM STREQUAL "linux") 
    # Compilation options _____________________________________________________
    add_compile_options(-fPIC -Wall $<$<CONFIG:Debug>:-g>)
    target_compile_definitions(${CRONO_TARGET_NAME} PRIVATE 
        $<$<CONFIG:Debug>:CRONO_DEBUG_ENABLED>)

    # Link to util library ____________________________________________________
    target_link_libraries(${CRONO_TARGET_NAME} libxhptdc8_util.so)

    # Copy library to be able to run exe directly
    file(COPY ${CRONO_DEP_PKG_LIB_DIR}/libxhptdc8_util.so DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
ENDIF()
