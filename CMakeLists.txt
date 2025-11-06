cmake_minimum_required(VERSION 3.28.3)
project(submarineController)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(EasyUDPLibrary CONFIG REQUIRED)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2)

find_package(SDL2 REQUIRED)

add_executable(submarineController src/main.cpp)

target_link_libraries(submarineController PUBLIC EasyUDPLibrary::udp_library SDL2::SDL2 )

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