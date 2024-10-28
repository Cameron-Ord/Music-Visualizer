working_dir=$(pwd)

echo "Link with lua? y/n"
$link_with_lua
read $link_with_lua

if [[ "$link_with_lua" == "y" || "$link_with_lua" == "Y" ]]; then
  echo "building with lua.."
  cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
    -DLUA_LINKING_FLAG=OFF \
    -S. \
    -B./build \
    -G "Unix Makefiles"
else
  echo "building without lua.."
  cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
    -DLUA_LINKING_FLAG=ON \
    -S. \
    -B./build \
    -G "Unix Makefiles"
fi

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
    sudo cp bin/Vis $bin_dir
  fi
fi
