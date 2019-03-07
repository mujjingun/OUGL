cmake_minimum_required(VERSION 3.7)
project(OUGL)
find_package(OpenGL REQUIRED)
find_package(GLEW 2.0 REQUIRED)
find_package(GLUT REQUIRED)
find_package(glm REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS})

add_executable(OUGL main.cpp scene.cpp planet.cpp shader.cpp vertexarray.cpp vertexbuffer.cpp rawbufferview.cpp framebuffer.cpp player.cpp)
target_link_libraries(OUGL GLEW::GLEW ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLM_LIBRARIES})

configure_file(shaders/planet.frag.glsl shaders/planet.frag.glsl COPYONLY)
configure_file(shaders/planet.vert.glsl shaders/planet.vert.glsl COPYONLY)
