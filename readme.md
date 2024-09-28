
# USAGE RESTRICTIONS
 
**By using this music player, you agree to only use music that you have legally obtained the rights to. This includes, but is not limited to, music that you have purchased, obtained through legitimate streaming services, or that is freely available for use under open licenses. Any unauthorized use of copyrighted music is strictly prohibited and may result in legal action.**

## Music Visualizer
![Image](example/example.png)

## Usage Manual
**For understanding on how to properly use the program please read the following**.
---

## Releases

This version of the program is only available for linux, I am working on builds for windows at the moment. If you want a windows version take a look at my releases and use those. Keep in mind that this is the old version of my program.
 
The program relies on directories located in the **~/Music dir located in the home path**.

> Foundational directories will be **created on the initial program launch if they do not already exist:**
- ```MVLogs``` which contains ```errlog.txt``` and ```log.txt```
- ```MVSource``` which is where your music will be located. **The program expects folders inside this directory**, of which contain the audio files.
---


## Controls
- Right : Navigate to the right/Start song
- Left : Navigate left
- Up : Navigate up through list
- Down : Navigate down through list
- Shift + Up : Cycle upwards through list
- Shift + Down : Cycle downwards through list
- Space : Toggle Pause

## BUILDING
My program depends on these libs:

1. libsndfile
2. SDL2-devel
3. SDL2_ttf-devel
4. SDL2_image-devel

# OTHER REQUIREMENTS
1. CMake
2. MinGW(For windows)

> For windows the most painless option is to use MSYS2 and install all the related packages from there

Building is relatively painless. Just make sure you have the afforementioned libs installed so that **CMake** can search for them.

## Build steps for linux
```mkdir build```
```cd build && cmake ..```
```cmake --build .```

## Build steps for windows
You can simply run the **win64_make.sh** script inside the working directory of the project but I will give a rundown of what it does here.

```
    cmake -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
        -DCMAKE_C_COMPILER=/c/msys64/mingw64/bin/gcc.exe \
        -DCMAKE_CXX_COMPILER=/c/msys64/mingw64/bin/g++.exe \
        -S. \
        -B./build \
        -G "MinGW Makefiles"
```


```cd build```
```mingw32-make```







