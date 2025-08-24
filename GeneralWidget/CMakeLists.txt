cmake_minimum_required(VERSION 3.16)

project(GeneralWidget VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

FILE(GLOB INCLUDE *.h)
FILE(GLOB SOURCE  *.cpp)

# 分组显示（VS友好）
source_group(01_Include FILES ${INCLUDE})
source_group(02_Source FILES  ${SOURCE})

set(PROJECT_SOURCES
    ${INCLUDE}
    ${SOURCE}
)

if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    # 遍历资源文件
    file(GLOB_RECURSE RES_PATHS *.png *.jpg *.svg *.ico *.ttf *.webp *.js)
    set(resource_files "")
    foreach (filepath ${RES_PATHS})
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" filename ${filepath})
        list(APPEND resource_files ${filename})
    endforeach ()

    # 添加共享库
    add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCES})

    # 添加 Qt6 资源
    qt_add_resources(${PROJECT_NAME} "${PROJECT_NAME}_resources"
        RESOURCES PREFIX "/"
        FILES ${resource_files}
    )
else()
    # Qt5 直接使用 qrc
    qt5_add_resources(PROJECT_SOURCES AutoThinkStation.qrc)
    add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCES})
endif()

if (WIN32)
    if (BUILD_ELAPACKETIO)
        target_link_libraries(${PROJECT_NAME} PRIVATE
            Qt${QT_VERSION_MAJOR}::Widgets
            ElaWidgetTools
            ElaPacketIO
        )
    else ()
        target_link_libraries(${PROJECT_NAME} PRIVATE
            Qt${QT_VERSION_MAJOR}::Widgets
            ElaWidgetTools
            GeneralUtils
        )
    endif ()
else ()
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        ElaWidgetTools
        GeneralUtils
    )
endif ()

# 复制编译产物到 AutoFlowStation
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    # DLL / EXE
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:${PROJECT_NAME}>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/../AutoFlowStation/

    # PDB (调试符号)
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_PDB_FILE:${PROJECT_NAME}>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/../AutoFlowStation/
)