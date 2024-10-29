working_dir=$(pwd)

echo "Link with lua? y/n"
read link_with_lua

build_dir=build
if [ -d $build_dir ]; then
  rm -r $build_dir
fi

music_src_dir=~/Music/MVSource
if [ -d $music_src_dir ]; then
  echo "Audio source directory already exists."
else
  echo "Creating directory at $music_src_dir"
  mkdir -p $music_src_dir
fi

if [[ "$link_with_lua" == "y" || "$link_with_lua" == "Y" ]]; then
  if [ -e "/c/msys64/mingw64/bin/gcc.exe" ]; then
    cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
      -DCMAKE_C_COMPILER=/c/msys64/mingw64/bin/gcc.exe \
      -DLUA_LINKING_FLAG=OFF \
      -S. \
      -B./build \
      -G "MinGW Makefiles"
  fi
else 
  if [ -e "/c/msys64/mingw64/bin/gcc.exe" ]; then
    cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
      -DCMAKE_C_COMPILER=/c/msys64/mingw64/bin/gcc.exe \
      -DLUA_LINKING_FLAG=ON \
      -S. \
      -B./build \
      -G "MinGW Makefiles"
  fi
fi

cd build
mingw32-make -j$(nproc)

cd $working_dir
share_dir=~/Documents/share/MVis

if [ -d $share_dir ]; then
  echo "Copying font and config to $share_dir"
  cp assets/dogicapixel.ttf $share_dir
  cp lua/config.lua $share_dir
else
  echo "Making directory $share_dir"
  mkdir -p $share_dir
  echo "Copying font and config to $share_dir"
  cp assets/dogicapixel.ttf $share_dir
  cp lua/config.lua $share_dir
fi


bin_dir=~/Documents/MVis/bin
local_bin_dir=bin
if [ -d $bin_dir ]; then
  if [ -d $local_bin_dir ]; then
    echo "Copying executable to $bin_dir"
    cp bin/Vis $bin_dir
  fi
else
  echo "Creating directory $bin_dir"
  mkdir -p $bin_dir
    if [ -d $local_bin_dir ]; then
    echo "Copying executable to $bin_dir"
    cp bin/Vis $bin_dir
  fi
fi
