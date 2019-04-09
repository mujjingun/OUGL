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
    src/planet.cpp
    src/player.cpp
    src/voxelcoords.cpp
    src/parameters.cpp
    src/terrain.cpp
)

target_link_libraries(OUGL
    ${PROJECT_NAME}_ECS
    ${PROJECT_NAME}_Graphics
    GLEW::GLEW
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARIES}
    ${GLM_LIBRARIES}
)
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS})

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
