project('MVisualizer', 'cpp', 
    default_options : ['cpp_std=c++17']
)


# Directories
src_dir = 'src'
inc_dir = 'include'
build_dir = 'build'
bin_dir = 'bin'
gfx_dir = 'assets'

cflags = ['-O2', '-Wall', '-Wextra', '-g']
ld_flags = ['-lmingw32','-lSDL2main', '-lSDL2', '-lsndfile', '-lgdi32']
inc_dep = include_directories(inc_dir)

sdl2_dep = dependency('sdl2', required: true)
sdl2_ttf_dep = dependency('sdl2_ttf', required: true)
sdl2_image_dep = dependency('sdl2_image', required: true)
sndfile_dep = dependency('sndfile', required: true)

sources = files(
    'src/audio.cpp',
    'src/audio_data.cpp',
    'src/fft.cpp',
    'src/files.cpp',
    'src/fonts.cpp',
    'src/font_render_draw.cpp',
    'src/font_render_set.cpp',
    'src/freq_render_draw.cpp',
    'src/internal.cpp',
    'src/key_events.cpp',
    'src/main.cpp',
    'src/paths.cpp',
    'src/render.cpp',
    'src/theme.cpp',
    'src/window.cpp'
)

executable('MVisualizer', sources, 
    include_directories: inc_dep, 
    dependencies: [sdl2_dep, sdl2_ttf_dep, sdl2_image_dep, sndfile_dep],
    c_args: cflags, 
    cpp_args: cflags,
    link_args: ld_flags
)