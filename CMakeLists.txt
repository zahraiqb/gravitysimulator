cmake_minimum_required(VERSION 3.20)
project(GravitySim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 2 COMPONENTS graphics window system REQUIRED)
find_package(ImGui-SFML REQUIRED)

add_executable(gravity-sim
    src/main.cpp
    src/Body.cpp
    src/Simulation.cpp
    src/Renderer.cpp
    src/UI.cpp
)

target_include_directories(gravity-sim PRIVATE src)

target_link_libraries(gravity-sim PRIVATE
    sfml-graphics sfml-window sfml-system
    ImGui-SFML::ImGui-SFML
)

# Auto-copy assets folder next to the binary
add_custom_command(TARGET gravity-sim POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:gravity-sim>/assets"
    COMMENT "Copying assets..."
)
