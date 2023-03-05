cmake_minimum_required(VERSION 3.15)
project(codraw)

# add_subdirectory(raylib)
include(FetchContent)
FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG 4.2.0
)
FetchContent_MakeAvailable(raylib)

add_executable(codraw main.cpp)
target_link_libraries(codraw raylib Winmm)