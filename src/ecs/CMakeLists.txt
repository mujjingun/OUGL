
add_library(${PROJECT_NAME}_ECS
    ecsengine.cpp
    entity.cpp
)

target_include_directories(${PROJECT_NAME}_ECS
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
