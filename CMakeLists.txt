cmake_minimum_required(VERSION 3.28.3)
project(submarineController)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -pedantic)

find_package(EasyUDPLibrary CONFIG REQUIRED)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2)
find_package(spdlog REQUIRED)

find_package(SDL2 REQUIRED)

include_directories(include)

add_library(axis STATIC src/axis.cpp)

add_library(controller STATIC src/controller.cpp)

add_library(attitudeIndicator STATIC src/attitudeIndicator.cpp)

add_executable(submarineController src/main.cpp)

target_link_libraries(controller PUBLIC SDL2::SDL2 axis spdlog::spdlog)
target_link_libraries(attitudeIndicator PUBLIC SDL2::SDL2 axis spdlog::spdlog)
target_link_libraries(submarineController PUBLIC EasyUDPLibrary::udp_library SDL2::SDL2 controller attitudeIndicator)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
)

add_custom_command(
    TARGET submarineController POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/extern/SDL_GameControllerDB/gamecontrollerdb.txt
            $<TARGET_FILE_DIR:submarineController>/gamecontrollerdb.txt
    COMMENT "Copying gamecontrollerdb.txt from submodule"
)

add_custom_command(
    TARGET submarineController POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/processing/AttitudeIndicator/application.linux64
            $<TARGET_FILE_DIR:submarineController>/AttitudeIndicator
    COMMENT "Copying exported Processing standalone application"
)