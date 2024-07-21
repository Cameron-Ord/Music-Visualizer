cd ~/Projects/Music-Visualizer/
working_dir=$(pwd)

BUILD_DIR="build"
SRC_DIR="src"
INC_DIR="inc"
CC="clang"
LD_FLAGS=" -lSDL2 -lSDL2_ttf -lSDL2_image -lsndfile -lm"
CFLAGS=" -O2 -Wall -Wextra -g"
BIN="bin"
GFX_DIR="assets"
CPP_FLAGS=" -I${INC_DIR} -MMD -MP"
TARGET="fftplayer"

if [ -d $BUILD_DIR ]; then
  echo "rm -r ${BUILD_DIR}"
  rm -r $BUILD_DIR
fi

if [ -d $BIN ]; then
  echo "rm -r ${BIN}"
  rm -r $BIN
fi

mkdir -p "$BUILD_DIR"
mkdir -p "$BIN"

cd $SRC_DIR
SRC_FILES=$(find -type f -name "*.c")
cd $working_dir

echo "Compiling source files..."

echo "FLAGS" $CC $CPP_FLAGS $CFLAGS

for file in $SRC_FILES; do
  $CC $CPP_FLAGS $CFLAGS -c ${SRC_DIR}/$file -o ${BUILD_DIR}/$file.o
done

echo "Linking object files..."

cd $BUILD_DIR
OBJ_FILES=$(find -type f -name "*.o" -printf '%P\n')

echo "BUILD STEP" $CC $BUILD_DIR $OBJ_FILES $LD_FLAGS

$CC $OBJ_FILES $LD_FLAGS -o ${working_dir}/${BIN}/${TARGET}

buffer=("${working_dir}/assets/"*)
for file in "${buffer[@]}"; do
  cp $file ${working_dir}/${BIN}
  echo "cp ${file} -> ${working_dir}/${BIN}"
done

cd $working_dir
echo "Build complete."
