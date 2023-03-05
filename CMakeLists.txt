cmake_minimum_required(VERSION 3.15)
project(codraw)

include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)


add_executable(codraw main.cpp)
target_link_libraries(codraw raylib Winmm)