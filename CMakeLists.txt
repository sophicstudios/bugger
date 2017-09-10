cmake_minimum_required(VERSION 3.9)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

# We do not want build artifacts to be put in the source tree
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)

if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(SEND_ERROR "In Source builds are not allowed.")
endif()

# Uncomment to use verbose build output
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(bugger
        VERSION 0.1.0
        DESCRIPTION "Bugger Game")

add_subdirectory(aegis)

find_library(COREVIDEO_LIBRARY CoreVideo)
find_library(APPKIT_LIBRARY AppKit)

################################################################################
# cmake/modules
################################################################################

set(${PROJECT_NAME}_CMAKE_MODULE_SOURCES
    cmake/modules/Info.plist.in
)

source_group(cmake/modules FILES ${${PROJECT_NAME}_CMAKE_MODULE_SOURCES})

################################################################################
# appmac
################################################################################

set(${PROJECT_NAME}_APPMAC_SOURCES
    src/app/appmac/appmac_appdelegate.h
    src/app/appmac/appmac_appdelegate.mm
    src/app/appmac/appmac_main.mm
)

source_group(app/appmac FILES ${${PROJECT_NAME}_APPMAC_SOURCES})

################################################################################
# game
################################################################################

set(${PROJECT_NAME}_GAME_SOURCES
    src/game/game_client.cpp
    src/game/game_client.h
)

source_group(game FILES ${${PROJECT_NAME}_GAME_SOURCES})

################################################################################
# uimac
################################################################################

set(${PROJECT_NAME}_UIMAC_SOURCES
    src/ui/uimac/uimac_displaytimer.h
    src/ui/uimac/uimac_displaytimer.mm
    src/ui/uimac/uimac_openglwindow.h
    src/ui/uimac/uimac_openglwindow.mm
    src/ui/uimac/uimac_renderingcontext.h
    src/ui/uimac/uimac_renderingcontext.mm
)

source_group(ui/uimac FILES ${${PROJECT_NAME}_UIMAC_SOURCES})

################################################################################
# util
################################################################################

set(${PROJECT_NAME}_UTIL_SOURCES
    src/util/util_bundlefilesystem.cpp
    src/util/util_bundlefilesystem.h
    src/util/util_convert.cpp
    src/util/util_convert.h
)

source_group(util FILES ${${PROJECT_NAME}_UTIL_SOURCES})

################################################################################
# shader
################################################################################

set(${PROJECT_NAME}_SHADER_RESOURCES
    resources/mac/shaders/sprite.fsh
    resources/mac/shaders/sprite.vsh
)

source_group(resources/shaders FILES ${${PROJECT_NAME}_SHADER_RESOURCES})

set_property(SOURCE ${${PROJECT_NAME}_SHADER_RESOURCES}
    PROPERTY MACOSX_PACKAGE_LOCATION "Resources/shaders"
)

set(${PROJECT_NAME}_SOURCES
    ${${PROJECT_NAME}_APPMAC_SOURCES}
    ${${PROJECT_NAME}_GAME_SOURCES}
    ${${PROJECT_NAME}_UIMAC_SOURCES}
    ${${PROJECT_NAME}_UTIL_SOURCES}
    ${${PROJECT_NAME}_SHADER_RESOURCES}
    ${${PROJECT_NAME}_CMAKE_MODULE_SOURCES}
)

set(${PROJECT_NAME}_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/src/app/appmac
    ${PROJECT_SOURCE_DIR}/src/game
    ${PROJECT_SOURCE_DIR}/src/ui/uimac
    ${PROJECT_SOURCE_DIR}/src/util
)

include_directories(
    ${${PROJECT_NAME}_INCLUDE_DIRS}
    ${agt_INCLUDE_DIRS}
    ${act_INCLUDE_DIRS}
    ${aft_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME Bugger
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_COPYRIGHT "Copyright © 2017 Sophic Studios, LLC"
    RESOURCE "${${PROJECT_NAME}_RESOURCES}"
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_MODULE_PATH}/Info.plist.in"
)

target_link_libraries(${PROJECT_NAME} agt act aft ${APPKIT_LIBRARY} ${COREVIDEO_LIBRARY})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)

message(STATUS "CMAKE_MODULE_PATH:" ${CMAKE_MODULE_PATH})
