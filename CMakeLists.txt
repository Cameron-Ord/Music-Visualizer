cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release)
project(MVis VERSION 1.0)
set(SRC_DIR "src")

set(SOURCES
    ${SRC_DIR}/audio.cpp
    ${SRC_DIR}/audio_data.cpp
    ${SRC_DIR}/fft.cpp
    ${SRC_DIR}/files.cpp
    ${SRC_DIR}/fonts.cpp
    ${SRC_DIR}/font_render_draw.cpp
    ${SRC_DIR}/font_render_set.cpp
    ${SRC_DIR}/freq_render_draw.cpp
    ${SRC_DIR}/internal.cpp
    ${SRC_DIR}/key_events.cpp
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/paths.cpp
    ${SRC_DIR}/render.cpp
    ${SRC_DIR}/theme.cpp
    ${SRC_DIR}/window.cpp
)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SndFile REQUIRED)

add_executable(MVis WIN32 ${SOURCES})

target_include_directories(MVis PRIVATE 
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
    ${SndFile_INCLUDE_DIRS}
)

target_link_libraries(MVis PRIVATE 
    SDL2::SDL2
    SDL2::SDL2main
    SDL2_ttf::SDL2_ttf
    SndFile::sndfile
)