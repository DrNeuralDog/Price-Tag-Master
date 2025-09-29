cmake_minimum_required(VERSION 3.16)

project(PriceTagMaster VERSION 1.0.0 LANGUAGES CXX)


set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


message(STATUS "Using C++ standard: ${CMAKE_CXX_STANDARD}")

#Обеспечивает выполнение автоматического запуска qwindeploy после сборки проекта - раскомментировать только, если вы уверенны что система сборки позволяет выполнять кастомные команды cmake
#set(WHETHER_QTWINDEPLOY_NECESSARY ON)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # На Linux PIC включается автоматически для SHARED/LIBRARY; для EXECUTABLE не требуется
    # Останемся без глобального -fPIC, чтобы не ломать сборку на macOS/Windows
endif ()


# Флаги компиляции для MinGW по обязательной проверки наличия возвращаемого значения
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-type")
endif()


#Без этой команды подключить C++ версии старшей чем 14 не получится!
# -std=c++17 выставляется через CMAKE_CXX_STANDARD; лишние флаги не добавляем


cmake_policy(SET CMP0091 NEW)

add_subdirectory(3rdparty/qxlsx/QXlsx)


# Отключение поиска Vulkan, если он не нужен
set(CMAKE_DISABLE_FIND_PACKAGE_WrapVulkanHeaders ON)

# Find required Qt packages (Charts optional)
find_package(Qt6 QUIET COMPONENTS Core Gui Widgets Concurrent PrintSupport)
if (Qt6_FOUND)
    set(QT_VERSION_MAJOR 6)
else ()
    # Qt5: избегаем жесткой зависимости от LinguistTools, она не используется
    find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Concurrent PrintSupport)
    set(QT_VERSION_MAJOR 5)
endif ()

# Try to find Qt Charts optionally
find_package(Qt${QT_VERSION_MAJOR} QUIET COMPONENTS Charts)
set(HAVE_QT_CHARTS OFF)
if (TARGET Qt${QT_VERSION_MAJOR}::Charts)
    set(HAVE_QT_CHARTS ON)
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
        src/templateeditordialog.cpp
        src/configmanager.cpp
        include/templateeditor.h
        include/templateeditordialog.h
        include/configmanager.h
        resources/resources.qrc
)


target_link_libraries(
        ${PROJECT_NAME}
        PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Concurrent
        Qt${QT_VERSION_MAJOR}::PrintSupport
        QXlsx
)
if (HAVE_QT_CHARTS)
    target_link_libraries(${PROJECT_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Charts)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_QT_CHARTS)
endif ()

# AxContainer (ActiveX) is Windows-only; link it conditionally
if (WIN32)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS AxContainer)
    target_link_libraries(${PROJECT_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::AxContainer)
endif ()


# Enable charts code paths only if Charts found (handled above)

# Embed Windows application icon into the executable
if (WIN32)
    target_sources(${PROJECT_NAME} PRIVATE resources/windows/app_icon.rc)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif ()

# macOS bundle icon (.icns) support (expects resources/icons/AppIcon.icns if provided)
if (APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE TRUE)
    # Uncomment if an .icns is added later:
    # set_source_files_properties(resources/icons/AppIcon.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    # target_sources(${PROJECT_NAME} PRIVATE resources/icons/AppIcon.icns)
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
