cmake_minimum_required(VERSION 3.13.0)
set(CMAKE_CXX_STANDARD 17)
set(SDL2_INCLUDE_DIR /usr/include/SDL2/include)

project(cenario LANGUAGES CXX)

# find_package(SDL2 REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp) # replace 'main.cpp' with your actual main file
# target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(include)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
