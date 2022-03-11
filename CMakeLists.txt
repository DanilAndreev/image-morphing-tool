cmake_minimum_required(VERSION 3.16)
project(image-morphing-tool)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH $ENV{QT6_DIR})

function(loadQt6DllsIntoTarget target)
    if (WIN32)
        set(DEBUG_SUFFIX)
        if (MSVC)
            if (CMAKE_BUILD_TYPE MATCHES "Debug")
                set(DEBUG_SUFFIX "d")
            endif ()
        endif ()
        set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
            if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
                set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
            endif ()
        endif ()
        if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
            add_custom_command(TARGET ${target} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory
                    "$<TARGET_FILE_DIR:${target}>/plugins/platforms/")
            add_custom_command(TARGET ${target} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                    "$<TARGET_FILE_DIR:${target}>/plugins/platforms/")
        endif ()
        foreach (QT_LIB ${REQUIRED_LIBS})
            add_custom_command(TARGET ${target} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    "${QT_INSTALL_PATH}/bin/Qt${QT_VERSION}${QT_LIB}${DEBUG_SUFFIX}.dll"
                    "$<TARGET_FILE_DIR:${target}>")
        endforeach (QT_LIB)
    endif ()
endfunction()

# -------------------------------------------------- Setting up QT
set(QT_VERSION 6)
set(REQUIRED_LIBS Core Gui Widgets OpenGL)
set(REQUIRED_LIBS_QUALIFIED Qt6::Core Qt6::Gui Qt6::Widgets Qt6::OpenGL)

file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/include/*.h" "${PROJECT_SOURCE_DIR}/include/*.hpp")
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
# Exclude main.cpp from sources
list(REMOVE_ITEM SOURCES "${PROJECT_SOURCE_DIR}/src/main.cpp")

file(GLOB_RECURSE TEST_SOURCES "${PROJECT_SOURCE_DIR}/tests/*.test.cpp")

include_directories(${CMAKE_SOURCE_DIR}/include)
add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} "${PROJECT_SOURCE_DIR}/src/main.cpp" include/core/exceptions/FileIOError.h src/tools/shaders/ShadersFactory.cpp include/tools/shaders/ShadersFactory.h)

if (NOT CMAKE_PREFIX_PATH)
    message(WARNING "CMAKE_PREFIX_PATH is not defined, you may need to set it "
            "(-DCMAKE_PREFIX_PATH=\"path/to/Qt/lib/cmake\" or -DCMAKE_PREFIX_PATH=/usr/include/{host}/qt{version}/ on Ubuntu)")
endif ()

find_package(OpenGL REQUIRED)
if (NOT WIN32) # TODO: change to if linux
    find_package(GLUT REQUIRED)
endif ()

find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)
target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED} ${OPENGL_LIBRARIES})
loadQt6DllsIntoTarget(${PROJECT_NAME})

# -------------------------------------------------- Vulkan
find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC "$ENV{VULKAN_SDK}/Third-Party/Include") # TODO: dont read from env
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)

# -------------------------------------------------- Include glslang lib
set(GLSLANG_LIBRARY_ROOT_PATH ${CMAKE_SOURCE_DIR}/libs/glslang)
set(BUILD_TESTING OFF CACHE BOOL "Disable CTest" FORCE)
set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "Disable binary app" FORCE)
set(ENABLE_GLSLANG_JS OFF CACHE BOOL "Disable js" FORCE)
set(ENABLE_GLSLANG_WEBMIN OFF CACHE BOOL "Disable glsl web" FORCE)
add_subdirectory(${GLSLANG_LIBRARY_ROOT_PATH})
target_include_directories(${PROJECT_NAME} PUBLIC ${GLSLANG_LIBRARY_ROOT_PATH}/glslang/include)
target_link_libraries(${PROJECT_NAME} glslang SPIRV)

# -------------------------------------------------- Include event_emitter lib
include_directories(${PROJECT_SOURCE_DIR}/libs/event_emitter/include)

# -------------------------------------------------- Include render_doc lib
include_directories(${PROJECT_SOURCE_DIR}/libs/render_doc/include)

# -------------------------------------------------- Copy GLSL shaders sources
#file(COPY ${PROJECT_SOURCE_DIR}/shaders DESTINATION ${PROJECT_BINARY_DIR})
#file(COPY ${PROJECT_SOURCE_DIR}/style DESTINATION ${PROJECT_BINARY_DIR})
configure_file("${PROJECT_SOURCE_DIR}/style/style.qss" "${PROJECT_BINARY_DIR}/style/style.qss" COPYONLY)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PUBLIC IMT_DEBUG=1)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

add_compile_definitions(NOMINMAX=1)

target_link_options(${PROJECT_NAME} PUBLIC -static-libgcc -static-libstdc++)

find_package(Python3 REQUIRED COMPONENTS Interpreter)
add_custom_target(imt_pre_build
        COMMAND ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/building/pre_build.py
        USES_TERMINAL)
add_dependencies(${PROJECT_NAME} imt_pre_build)
target_include_directories(${PROJECT_NAME} PRIVATE src/tools/shaders/bin)
