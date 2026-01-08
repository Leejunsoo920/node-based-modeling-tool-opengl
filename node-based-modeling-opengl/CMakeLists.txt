cmake_minimum_required(VERSION 3.16)
project(node-based-modeling-opengl)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    include
    Dependencies/include
    Dependencies/external
)

file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "Dependencies/include/*.cpp"
    "Dependencies/external/*.cpp"
)

set(MAIN_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/glad.c
)

add_executable(${PROJECT_NAME}
    ${MAIN_SRC}
    ${SOURCES}
)

set(ASSET_DIRS
    resources
    shader
    example_node
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/imgui.ini
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/imgui.ini


    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/imgui.ini
        ${CMAKE_BINARY_DIR}/imgui.ini
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "=== Copying asset directories ==="
)

foreach(ASSET_DIR ${ASSET_DIRS})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD

        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/${ASSET_DIR}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/${ASSET_DIR}


        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/${ASSET_DIR}
            ${CMAKE_BINARY_DIR}/${ASSET_DIR}
    )
endforeach()

target_link_libraries(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp-vc143-mtd.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/glew32s.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw3.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/jsoncpp.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/nlopt.lib"
)
