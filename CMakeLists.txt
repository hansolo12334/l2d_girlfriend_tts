cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0079 NEW)

project(ElaWidget_QT VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add_compile_options(-std=c++17)
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Multimedia OpenGLWidgets WebSockets Sql)

# FILE(GLOB ORIGIN *.cpp *.h)
# FILE(GLOB MODELVIEW ModelView/*.h ModelView/*.cpp)
# FILE(GLOB EAXMPLEPAGE ExamplePage/*.h ExamplePage/*.cpp)
FILE(GLOB SOURCE_FILE src/*.cpp)
FILE(GLOB HEADER_FILE include/*.h)
FILE(GLOB PAGES_FILE include/pages/*.h src/pages/*.cpp)
FILE(GLOB SMALL_WIDGETS_FILE include/small_widgets/*.h src/small_widgets/*.cpp)
FILE(GLOB API_FILE include/api/*.h src/api/*.cpp)

FILE(GLOB LIVE2DAPP_FILE include/pages/Live2dApp/*hpp src/pages/Live2dApp/*cpp)
# FILE(GLOB GL_WIDGET_FILE src/pages/gl_widget/*cpp)

FILE(GLOB TOOLS include/tools/*.h src/tools/*.cpp src/tools/*.c)
# source_group(ModelView FILES ${MODELVIEW})
# source_group(ExamplePage FILES ${EAXMPLEPAGE})

# source_group(PAGES_SOURCE FILES ${PAGES_FILE})
SET(PAGES_SOURCE include/pages/)
SET(API_SOURCE include/api/)
SET(SMALLL_WIDGETS_SOURCE include/small_widgets)
SET(LIVE2DAPP_SOURCE include/pages/Live2dApp/)
# SET(GL_WIDGET_SOURCE include/pages/gl_widget/)
SET(TOOLS_SOURCE include/tools/)

message(STATUS "SOURCE_FILE : ${SOURCE_FILE}")


set(PROJECT_SOURCES
    # ${ORIGIN}
    # ${MODELVIEW}
    # ${EAXMPLEPAGE}
    ${SOURCE_FILE}
    ${HEADER_FILE}
    ${PAGES_FILE}
    ${API_FILE}
    ${SMALL_WIDGETS_FILE}
    ${TOOLS}
    ${LIVE2DAPP_FILE}
    # ${GL_WIDGET_FILE}
)

#gl
set(GLEW_PATH third_party/glew)
set(GLFW_PATH third_party/glfw)

add_subdirectory(${GLEW_PATH}/build/cmake)
add_subdirectory(${GLFW_PATH})

#live2d_SDK
add_subdirectory(Framework)
target_compile_definitions(Framework PUBLIC CSM_TARGET_WIN_GL)
target_include_directories(Framework PUBLIC ${GLEW_PATH}/include)
target_link_libraries(Framework Live2DCubismCore glew_s)

add_library(Live2DCubismCore STATIC IMPORTED)
set(CRT MT)
set_target_properties(Live2DCubismCore
  PROPERTIES
    IMPORTED_LOCATION_DEBUG
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/live2d/lib/142/Live2DCubismCore_${CRT}d.lib
    IMPORTED_LOCATION_RELEASE
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/live2d/lib/142/Live2DCubismCore_${CRT}.lib
    INTERFACE_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/live2d/inc/
)

target_link_libraries(Framework Live2DCubismCore glew_s)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ElaWidget_QT
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )

    #遍历所有资源文件
    file(GLOB_RECURSE RES_PATHS *.png *.jpg *.svg *.ico *.ttf *.webp *.js)
    foreach(filepath ${RES_PATHS})
            string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" filename ${filepath})
            list(APPEND resource_files ${filename})
    endforeach(filepath)

    qt_add_resources(${PROJECT_NAME} "ElaWidget_QT"
        RESOURCES PREFIX "/"
        FILES
        ${resource_files}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ElaWidget_QT APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ElaWidget_QT SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        qt5_add_big_resources(PROJECT_SOURCES
            ../src/include/ElaWidgetTools.qrc
            ElaWidget_QT.qrc
        )
        add_executable(ElaWidget_QT
            ${PROJECT_SOURCES}
        )
    endif()
endif()

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/third_party/elawidget/lib/cmake/)
find_package(ELAWIDGETTOOLS REQUIRED)
if(ELAWIDGETTOOLS_FOUND)
    message(STATUS "ELAWIDGETTOOLS_INCLUDE_DIR = ${ELAWIDGETTOOLS_INCLUDE_DIRS}")
    message(STATUS "ELAWIDGETTOOLS_LIBRARIES = ${ELAWIDGETTOOLS_LIBRARIES}")
    message(STATUS "ELAWIDGETTOOLS_LIB_DIR = ${ELAWIDGETTOOLS_LIBRARY_DIRS}")
endif ()
set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_BINARY_DIR})

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DAPP_DEBUG")
include_directories(include)
include_directories(${GLEW_PATH}/include)
include_directories(Framework/src)
include_directories(third_party/stb)

target_include_directories(ElaWidget_QT PUBLIC
    ${ELAWIDGETTOOLS_INCLUDE_DIRS}
    # ExamplePage
    # ModelView
    ${PAGES_SOURCE}
    ${API_SOURCE}
    ${SMALLL_WIDGETS_SOURCE}
    ${TOOLS_SOURCE}
    ${LIVE2DAPP_SOURCE}
    # ${GL_WIDGET_SOURCE}
)
target_link_libraries(ElaWidget_QT PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::OpenGLWidgets
    Qt${QT_VERSION_MAJOR}::WebSockets

    ElaWidgetTools
    glfw
    Framework
    winmm.lib user32.lib dwmapi.lib
    debug -NODEFAULTLIB:libcmtd.lib
    optimized -NODEFAULTLIB:libcmt.lib
)


target_link_directories(ElaWidget_QT PUBLIC
    ${ELAWIDGETTOOLS_LIBRARY_DIRS}
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ElaWidget_QT)
endif()
set_target_properties(ElaWidget_QT PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE FALSE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ElaWidget_QT)
endif()


set(SOURCE_DLL "${CMAKE_SOURCE_DIR}/third_party/elawidget/bin/ElaWidgetToolsd.dll")
set(TARGET_DLL "${CMAKE_BINARY_DIR}/$<CONFIG>/ElaWidgetToolsd.dll")

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${SOURCE_DLL}"
    "${TARGET_DLL}"
    COMMENT "Copying ElaWidgetToolsd.dll to output directory"
)

set(RES_PATH ${CMAKE_CURRENT_SOURCE_DIR}/resources/character)
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E
      copy_directory ${RES_PATH} $<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources
)




# cmake -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE --no-warn-unused-cli -SD:/Qt_project/2024/l2d_girlfriend_tts -Bd:/Qt_project/2024/l2d_girlfriend_tts/build -G "Visual Studio 16 2019" -T host=x64 -A x64
