cmake_minimum_required(VERSION 3.16)
project(Music-Visualizer VERSION 1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)


if(WIN32)
  set(CMAKE_REQUIRED_FLAGS "-lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lsndfile -lm")
else()
  set(CMAKE_REQUIRED_FLAGS "-lSDL2 -lSDL2_ttf -lSDL2_image -lsndfile -lm")
endif()

file(GLOB SOURCES "src/*.c")

if(WIN32)
  add_executable(Vis ${SOURCES})
else()
  add_executable(Vis ${SOURCES})
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
  target_compile_options(Vis PRIVATE -Wall -Wextra -pedantic)
endif()

find_library(SDL2_LIBRARY NAMES SDL2)
find_library(SDL2_LIBRARY_MAIN NAMES SDL2main)
find_library(SDL2_TTF_LIBRARY NAMES SDL2_ttf)
find_library(SDL2_IMAGE_LIBRARY NAMES SDL2_image)
find_library(LIBSNDFILE_LIBRARY NAMES sndfile)
find_library(LUALIB NAMES lua)

find_path(SDL2_INC NAMES SDL2/ SDL.h)
find_path(sndfile_INC NAMES sndfile.h sndfile/)
find_path(LUA_INC NAMES lua.h lualib.h luaxlib.h)

message("")
if(NOT LUA_INC)
  message(FATAL_ERROR "Lua includes not found!")
else()
  message(STATUS "Lua includes found at ${LUA_INC}")
  include_directories(${LUA_INC})
  add_definitions(-DLUA_FLAG)
endif()

if(NOT SDL2_INC)
  message(FATAL_ERROR "SDL2 includes not found!")
else()
  message(STATUS "SDL2 includes found at ${SDL2_INC}")
  include_directories(${SDL2_INC})
endif()

if(NOT sndfile_INC)
  message(FATAL_ERROR "sndfile includes not found!")
else()
  message(STATUS "sndfile include found at ${sndfile_INC}")
  include_directories(${sndfile_INC})
endif()

message("")
if(NOT SDL2_LIBRARY)
  message(FATAL_ERROR "SDL2 not found, please install it via your package manager!")
else()
  message(STATUS "SDL2 found at ${SDL2_LIBRARY}")
  target_link_libraries(Vis ${SDL2_LIBRARY})
endif()

if(NOT SDL2_LIBRARY_MAIN)
  message(WARNING "SDL2main not found!")
else()
  message(STATUS "SDL2main found at ${SDL2_LIBRARY_MAIN}")
  if(WIN32)
    target_compile_definitions(Vis PUBLIC SDL_MAIN_HANDLED)
    target_link_libraries(Vis ${SDL2_LIBRARY_MAIN})
  endif()
endif()

if(NOT SDL2_TTF_LIBRARY)
  message(FATAL_ERROR "SDL2_ttf not found, please install it via your package manager!")
else()
  message(STATUS "SDL2_ttf found at ${SDL2_TTF_LIBRARY}")
  target_link_libraries(Vis ${SDL2_TTF_LIBRARY})
endif()

if(NOT SDL2_IMAGE_LIBRARY)
  message(FATAL_ERROR "SDL2_image not found, please install it via your package manager!")
else()
  message(STATUS "SDL2_image found at ${SDL2_IMAGE_LIBRARY}")
  target_link_libraries(Vis ${SDL2_IMAGE_LIBRARY})
endif()

if(NOT LIBSNDFILE_LIBRARY)
  message(FATAL_ERROR "libsndfile not found, please install it via your package manager!")
else()
  message(STATUS "libsndfile found at ${LIBSNDFILE_LIBRARY}")
  target_link_libraries(Vis ${LIBSNDFILE_LIBRARY})
endif()

if(NOT LUALIB)
  message(WARNING "LUA not found!")
else()
  message(STATUS "LUA found at ${LUALIB}")
  target_link_libraries(Vis ${LUALIB})
endif()
message("")

target_link_libraries(Vis m)



