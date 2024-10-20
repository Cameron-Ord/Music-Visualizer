working_dir=$(pwd)

cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
  -S. \
  -B./build \
  -G "Unix Makefiles"

cd build
make -j$(nproc)

cd $working_dir
cp assets/dogicapixel.ttf bin/
