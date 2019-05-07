
add_library(${PROJECT_NAME}_Graphics
    devicebuffer.cpp
    framebuffer.cpp
    rawbufferview.cpp
    renderbuffer.cpp
    shader.cpp
    texture.cpp
    vertexarray.cpp
    glquery.cpp
)

target_include_directories(${PROJECT_NAME}_Graphics
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(${PROJECT_NAME}_Graphics PRIVATE
    -Wall -Wextra -pedantic -Werror)
