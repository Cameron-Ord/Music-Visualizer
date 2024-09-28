
if [ -e "/c/msys64/mingw64/bin/g++.exe" ]; then
    cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
        -DCMAKE_C_COMPILER=/c/msys64/mingw64/bin/gcc.exe \
        -DCMAKE_CXX_COMPILER=/c/msys64/mingw64/bin/g++.exe \
        -S. \
        -B./build \
        -G "MinGW Makefiles"
fi

cd build
mingw32-make