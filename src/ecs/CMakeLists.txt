
add_library(${PROJECT_NAME}_ECS
    ecsengine.cpp
    entity.cpp
)

target_include_directories(${PROJECT_NAME}_ECS
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(${PROJECT_NAME}_ECS PRIVATE
    -Wall -Wextra -pedantic -Werror)
