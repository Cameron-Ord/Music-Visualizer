workingdir=$(pwd)

if [ -d "build" ]; then
  rm -r build
  echo "rm -r -> build"
fi
mkdir build
cd build

cmake ..
make -j8

cd $workingdir

cp -r assets/* bin
