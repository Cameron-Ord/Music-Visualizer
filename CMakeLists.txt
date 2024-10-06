cmake_minimum_required(VERSION 3.16)
project(Music-Visualizer VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)


if(WIN32)
  set(CMAKE_REQUIRED_FLAGS "-lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lsndfile")
else()
  set(CMAKE_REQUIRED_FLAGS "-lSDL2 -lSDL2_ttf -lSDL2_image -lsndfile")
endif()

find_library(SDL2_LIBRARY NAMES SDL2)
find_library(SDL2_LIBRARY_MAIN NAMES SDL2main)
find_library(SDL2_TTF_LIBRARY NAMES SDL2_ttf)
find_library(SDL2_IMAGE_LIBRARY NAMES SDL2_image)
find_library(LIBSNDFILE_LIBRARY NAMES sndfile)

message("")
if(NOT SDL2_LIBRARY_MAIN)
  message(WARNING "SDL2main not found!")
else()
  message(STATUS "SDL2main found at ${SDL2_LIBRARY_MAIN}")
endif()

if(NOT SDL2_LIBRARY)
  message(FATAL_ERROR "SDL2 not found, please install it via your package manager!")
else()
  message(STATUS "SDL2 found at ${SDL2_LIBRARY}")
endif()

if(NOT SDL2_TTF_LIBRARY)
  message(FATAL_ERROR "SDL2_ttf not found, please install it via your package manager!")
else()
  message(STATUS "SDL2_ttf found at ${SDL2_TTF_LIBRARY}")
endif()

if(NOT SDL2_IMAGE_LIBRARY)
  message(FATAL_ERROR "SDL2_image not found, please install it via your package manager!")
else()
  message(STATUS "SDL2_image found at ${SDL2_IMAGE_LIBRARY}")
endif()

if(NOT LIBSNDFILE_LIBRARY)
  message(FATAL_ERROR "libsndfile not found, please install it via your package manager!")
else()
  message(STATUS "libsndfile found at ${LIBSNDFILE_LIBRARY}")
endif()
message("")

file(GLOB SOURCES "src/*.cpp")

if(WIN32)
  add_executable(Vis WIN32 ${SOURCES})
else()
  add_executable(Vis ${SOURCES})
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(Vis PRIVATE -Wall -Wextra -pedantic)
endif()

if(WIN32)
  target_compile_definitions(Vis PUBLIC SDL_MAIN_HANDLED)
  target_link_libraries(Vis ${SDL2_LIBRARY_MAIN} ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${LIBSNDFILE_LIBRARY})
else()
  target_link_libraries(Vis ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${LIBSNDFILE_LIBRARY})
endif()



