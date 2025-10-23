cmake_minimum_required(VERSION 3.16)

project(PriceTagMaster VERSION 1.0.0 LANGUAGES CXX)


# =====================================================================================================================
# БАЗОВЫЕ НАСТРОЙКИ ПРОЕКТА И СБОРКИ

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Явно добавляем флаг -std=c++17 в CMAKE_CXX_FLAGS
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

#Без этой команды подключить C++ версии старшей чем 14 не получится для Windows!
add_compile_options(
     $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>
)

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


cmake_policy(SET CMP0091 NEW)


# Отключение поиска Vulkan, если он не нужен
set(CMAKE_DISABLE_FIND_PACKAGE_WrapVulkanHeaders ON)

# =====================================================================================================================



# =====================================================================================================================
# ПОДКЛЮЧЕНИЕ СТОРОННИХ ЗАВИСИМОСТЕЙ

add_subdirectory(3rdparty/qxlsx/QXlsx)

# =====================================================================================================================



# =====================================================================================================================
# ПОДКЛЮЧЕНИЕ НЕОБХОДИМЫХ ПРОЕКТУ КОМПОНЕНТОВ QT

# Find required Qt packages (Charts optional)
find_package(Qt6 QUIET COMPONENTS Core Gui Widgets Concurrent PrintSupport)
if (Qt6_FOUND)
    set(QT_VERSION_MAJOR 6)
else ()
    # Qt5: избегаем жесткой зависимости от LinguistTools, она не используется
    find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Concurrent PrintSupport)
    set(QT_VERSION_MAJOR 5)
endif ()


# Попытка найти Qt Charts - по необходимости
find_package(Qt${QT_VERSION_MAJOR} QUIET COMPONENTS Charts)
set(HAVE_QT_CHARTS OFF)
if (TARGET Qt${QT_VERSION_MAJOR}::Charts)
    set(HAVE_QT_CHARTS ON)
endif ()


# Qt projects includes для последующего использования
get_target_property(QtCore_Include_Dir Qt${QT_VERSION_MAJOR}::Core INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(QtWidget_Include_Dir Qt${QT_VERSION_MAJOR}::Widgets INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(QtConcurrent_Include_Dir Qt${QT_VERSION_MAJOR}::Concurrent INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(QtPrintSupport_Include_Dir Qt${QT_VERSION_MAJOR}::PrintSupport INTERFACE_INCLUDE_DIRECTORIES)

# =====================================================================================================================



# =====================================================================================================================
# ПОДГОТОВКА ФАЙЛОВ ПРОЕКТА И ПУТЕЙ ВКЛЮЧЕНИЯ

file(GLOB_RECURSE MAIN_PROJECT_FILES CONFIGURE_DEPENDS
        "src/*.cpp" "src/*.cxx" "src/*.cc" "src/*.c"
        "src/*.h" "src/*.hpp" "src/*.tpp" "src/*.inl"
        "src/*.ui"
        "include/*.h" "include/*.hpp" "include/*.tpp" "include/*.inl" "include/*.cpp"
        "resources/*.qrc"
)

# Сбор путей к директориям проекта для include (только src/ и include/)
set(PROJECT_PATHS "")
foreach (ROOT_DIR IN ITEMS "src" "include")
    file(GLOB_RECURSE SUBDIRS LIST_DIRECTORIES true "${ROOT_DIR}/*")
    list(FILTER SUBDIRS EXCLUDE REGEX ".*\\.git.*")

    foreach (SUBDIR IN LISTS SUBDIRS)
        get_filename_component(ABS_SUBDIR "${SUBDIR}" ABSOLUTE)

        if (IS_DIRECTORY "${ABS_SUBDIR}")
            list(APPEND PROJECT_PATHS "${ABS_SUBDIR}")
        endif ()
    endforeach ()
endforeach ()

list(APPEND PROJECT_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/include")
list(REMOVE_DUPLICATES PROJECT_PATHS)

message(STATUS "PROJECT_PATHS contains:")

foreach (PATH IN LISTS PROJECT_PATHS)
    message(STATUS "  ${PATH}")
endforeach ()

# =====================================================================================================================



# =====================================================================================================================
# КРОССПЛАТФОРМЕННЫЕ КОНФИГУРАЦИИ И СВОЙСТВА ПРОЕКТА

if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(
            ${PROJECT_NAME} MANUAL_FINALIZATION
            ${MAIN_PROJECT_FILES}
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
                ${MAIN_PROJECT_FILES}
                include/Constants.h
                include/Common.h
        )
    endif ()
endif ()

# =====================================================================================================================



# =====================================================================================================================
# КОНФИГУРАЦИЯ ЦЕЛЕВОГО ПРОЕКТА

# Настройка include путей для проекта
target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        ${PROJECT_PATHS}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qxlsx/QXlsx/header
)

# Линковка библиотек Qt и сторонних зависимостей
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

 

# Подключение Qt Charts если доступен
if (HAVE_QT_CHARTS)
    target_link_libraries(${PROJECT_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Charts)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_QT_CHARTS)
endif ()


# AxContainer (ActiveX) is Windows-only; link it conditionally
if (WIN32)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS AxContainer)
    target_link_libraries(${PROJECT_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::AxContainer)
endif ()

# =====================================================================================================================



# =====================================================================================================================
# НАСТРОЙКИ ПЛАТФОРМЕННЫХ ОСОБЕННОСТЕЙ

# Embed Windows application icon into the executable
if (WIN32)
    target_sources(${PROJECT_NAME} PRIVATE resources/windows/app_icon.rc)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif ()

# macOS bundle icon (.icns) support (expects resources/icons/AppIcon.icns if provided)
if (APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE TRUE)
endif ()

# =====================================================================================================================



# =====================================================================================================================
# УСТАНОВКА ПРОЕКТА

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

# =====================================================================================================================



# =====================================================================================================================
# ФИНАЛИЗАЦИЯ ПРОЕКТА ДЛЯ QT6

if (QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif ()

# =====================================================================================================================
