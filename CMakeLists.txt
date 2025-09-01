cmake_minimum_required(VERSION 3.16)

project(PriceTagMaster VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexec-charset=UTF-8")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Явно добавляем флаг -std=c++17 в CMAKE_CXX_FLAGS
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

# Добавляем опции компиляции для поддержки C++17 (Windows)
add_compile_options(
     $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>
)

cmake_policy(SET CMP0091 NEW)

add_subdirectory(3rdparty/qxlsx/QXlsx)


# Отключение поиска Vulkan, если он не нужен
set(CMAKE_DISABLE_FIND_PACKAGE_WrapVulkanHeaders ON)

# Find required packages
find_package(Qt6 QUIET COMPONENTS Core Gui Widgets Concurrent AxContainer PrintSupport)
if (Qt6_FOUND)
    set(QT_VERSION_MAJOR 6)
else ()
    find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Concurrent AxContainer PrintSupport)
    set(QT_VERSION_MAJOR 5)
endif ()

# Qt projects includes:
get_target_property(QtCore_Include_Dir Qt${QT_VERSION_MAJOR}::Core INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(QtWidget_Include_Dir Qt${QT_VERSION_MAJOR}::Widgets INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(QtConcurrent_Include_Dir Qt${QT_VERSION_MAJOR}::Concurrent INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(QtCore_Include_Dir Qt${QT_VERSION_MAJOR}::Core INTERFACE_INCLUDE_DIRECTORIES)

file(GLOB_RECURSE ALL_PROJECT_FILES "src/*.h" "src/*.cpp" "src/*.ui" "include/*.h" "include/*.cpp" "include/*.ui")

if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(
            ${PROJECT_NAME} MANUAL_FINALIZATION
            ${ALL_PROJECT_FILES}
    )
    # Define target properties for Android with Qt 6 as:
    set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
            ${CMAKE_CURRENT_SOURCE_DIR}/android)
else ()
    if (ANDROID)
        add_library(
                ${PROJECT_NAME} SHARED
                ${ALL_PROJECT_FILES}
        )
        set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else ()
        add_executable(
                ${PROJECT_NAME}
                ${ALL_PROJECT_FILES}
        )
    endif ()
endif ()

set(PROJECT_PATHS "")

file(GLOB_RECURSE SUBDIRS LIST_DIRECTORIES true "src/*")

foreach (SUBDIR IN LISTS SUBDIRS)
    get_filename_component(ABS_SUBDIR "${SUBDIR}" ABSOLUTE)

    if (IS_DIRECTORY "${ABS_SUBDIR}")
        list(APPEND PROJECT_PATHS "${ABS_SUBDIR}")
    endif ()

endforeach ()

list(APPEND PROJECT_PATHS "src/")

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        ${PROJECT_PATHS}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qxlsx/QXlsx/header
)

# Ensure new editor sources are linked even if CMake hasn't regenerated globs
target_sources(
        ${PROJECT_NAME}
        PRIVATE
        src/templateeditor.cpp
        include/templateeditor.h
        include/templateeditordialog.h
)

target_link_libraries(
        ${PROJECT_NAME}
        PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::AxContainer
        Qt${QT_VERSION_MAJOR}::Concurrent
        Qt${QT_VERSION_MAJOR}::PrintSupport
        QXlsx
)

# Embed Windows application icon into the executable
if (WIN32)
    target_sources(${PROJECT_NAME} PRIVATE resources/windows/app_icon.rc)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif ()

# Install
install(TARGETS
        ${PROJECT_NAME}
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

#Other settings:
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if (QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif ()
