cmake_minimum_required(VERSION 3.16)
project(Music-Visualizer VERSION 1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE RelWifDebInfo)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

if(WIN32)
  set(CMAKE_REQUIRED_FLAGS "-lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lsndfile -lm")
else()
  set(CMAKE_REQUIRED_FLAGS "-lSDL2 -lSDL2_ttf -lsndfile -lm")
endif()

option(LUA_LINKING_FLAG "Do not link LUA" ON)

file(GLOB SOURCES "src/*.c")

if(WIN32)
  add_executable(MVis WIN32 ${SOURCES})
else()
  add_executable(MVis ${SOURCES})
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
  target_compile_options(MVis PRIVATE -Wall -Wextra -pedantic)
endif()

target_link_libraries(MVis m)

find_library(SDL2_LIBRARY NAMES SDL2)
find_library(SDL2_LIBRARY_MAIN NAMES SDL2main)
find_library(SDL2_TTF_LIBRARY NAMES SDL2_ttf)
find_library(LIBSNDFILE_LIBRARY NAMES sndfile)

if(NOT LUA_LINKING_FLAG)
  message(STATUS "Finding lua paths..")
  find_library(LUALIB NAMES lua Lua LUA)
  find_path(LUA_INC NAMES lua.h lualib.h lauxlib.h)
endif()

find_path(SDL2_INC NAMES SDL2/ SDL.h)
find_path(sndfile_INC NAMES sndfile.h sndfile/)

message("")
if(NOT LUA_INC)
  message(STATUS "Lua includes not found or disabled!")
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
  target_link_libraries(MVis ${SDL2_LIBRARY})
endif()

if(NOT SDL2_LIBRARY_MAIN)
  message(WARNING "SDL2main not found!")
else()
  message(STATUS "SDL2main found at ${SDL2_LIBRARY_MAIN}")
  if(WIN32)
    target_compile_definitions(MVis PUBLIC SDL_MAIN_HANDLED)
    target_link_libraries(MVis ${SDL2_LIBRARY_MAIN})
  endif()
endif()

if(NOT SDL2_TTF_LIBRARY)
  message(FATAL_ERROR "SDL2_ttf not found, please install it via your package manager!")
else()
  message(STATUS "SDL2_ttf found at ${SDL2_TTF_LIBRARY}")
  target_link_libraries(MVis ${SDL2_TTF_LIBRARY})
endif()


if(NOT LIBSNDFILE_LIBRARY)
  message(FATAL_ERROR "libsndfile not found, please install it via your package manager!")
else()
  message(STATUS "libsndfile found at ${LIBSNDFILE_LIBRARY}")
  target_link_libraries(MVis ${LIBSNDFILE_LIBRARY})
endif()

if(NOT LUALIB)
  message(STATUS "LUA not found or disabled!")
else()
  message(STATUS "LUA found at ${LUALIB}")
  target_link_libraries(MVis ${LUALIB})
endif()
message("")




