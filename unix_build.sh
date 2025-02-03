working_dir=$(pwd)

build_dir=build
cmake -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=gcc \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
  -S. \
  -B./build \
  -G "Unix Makefiles"

cd build
make -j$(nproc)

cd $working_dir
share_dir=~/.local/share/MVis

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

bin_dir=/usr/local/bin
local_bin_dir=bin
if [ -d $bin_dir ]; then
  if [ -d $local_bin_dir ]; then
    echo "Copying executable to $bin_dir - Requires sudo elevation!"
    sudo cp bin/MVis $bin_dir
  fi
fi
