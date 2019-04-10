cmake_minimum_required(VERSION 3.7)
project(OUGL)

find_package(OpenGL REQUIRED)
find_package(GLEW 2.0 REQUIRED)
find_package(GLUT REQUIRED)
find_package(glm REQUIRED)

add_subdirectory(src/ecs)
add_subdirectory(src/graphics)

add_executable(OUGL
    src/main.cpp
    src/scene.cpp
    src/voxelcoords.cpp
    src/parameters.cpp
    src/terrain.cpp
    src/input.cpp
    src/planetmath.cpp

    src/entitysystems/camerasystem.cpp
    src/entitysystems/rendersystem.cpp
    src/entitysystems/inputsystem.cpp
)

set_target_properties(OUGL PROPERTIES
    CXX_STANDARD 14
    CXX_EXTENSIONS OFF
)

target_link_libraries(OUGL
    ${PROJECT_NAME}_ECS
    ${PROJECT_NAME}_Graphics
    GLEW::GLEW
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARIES}
    ${GLM_LIBRARIES}
)
target_include_directories(OUGL PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS})

target_compile_definitions(OUGL PUBLIC
    GLM_ENABLE_EXPERIMENTAL)

# Copy shaders
set(SHADER_FILES
    shaders/planet.vert.glsl
    shaders/planet.frag.glsl
    shaders/hdr.vert.glsl
    shaders/hdr.frag.glsl
    shaders/terrain.comp.glsl
)

foreach(SHADER_FILE ${SHADER_FILES})
    configure_file(${SHADER_FILE} ${SHADER_FILE} COPYONLY)
endforeach()

# Testing
#enable_testing()
#find_package(GTest REQUIRED)
#
#add_executable(runUnitTests
#    tests/unittests.cpp
#)
#
#target_link_libraries(runUnitTests gtest gtest_main)
#
#add_test(
#    NAME runUnitTests
#    COMMAND runUnitTests
#)
