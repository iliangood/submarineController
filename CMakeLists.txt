cmake_minimum_required(VERSION 3.28.3)
project(submarineController)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(EasyUDPLibrary CONFIG REQUIRED)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2)


add_executable(submarineController src/main.cpp)

target_link_libraries(submarineController PUBLIC EasyUDPLibrary::udp_library SDL2::SDL2)