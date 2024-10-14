
# USAGE RESTRICTIONS
 
**By using this music player, you agree to only use music that you have legally obtained the rights to. This includes, but is not limited to, music that you have purchased, obtained through legitimate streaming services, or that is freely available for use under open licenses. Any unauthorized use of copyrighted music is strictly prohibited and may result in legal action.**

## Music Visualizer
I am making this for myself only, but if you wanna use it, check out the build section. Note that as of right now it will not work to linux as I have not completely implemented the filesystem search

### Releases
There are currently no releases. If you want to use this program, you will have to build it. Thankfully it's relatively painless. Build steps are at the end of the readme. Thanks!
 
The program relies on directories located in the **~/Music dir located in the home path**.


### Configuration

> Foundational directories will be **created on the initial program launch if they do not already exist:**
- ```MVSource``` which is where your music will be located. **The program expects folders inside this directory**, of which contain the audio files.
---


## Controls
**General controls**

- Up : Navigate cursor up through current list.
- Down : Navigate cursor down through current list.
- Space : Select the current item under cursor.
- Right : Navigate to the right.
- Left : Navigate left.
- P : Toggle Pause.
- Q or close : Quit.

## BUILDING
My program depends on these libs:

1. libsndfile
2. SDL2-devel
3. SDL2_ttf-devel

# OTHER REQUIREMENTS
1. CMake
2. MinGW(For windows)

> For windows the most painless option is to use MSYS2 and install all the related packages from there

Building is relatively painless. Just make sure you have the afforementioned libs installed so that **CMake** can search for them.

## Build steps for linux
- ```mkdir build```
- ```cd build && cmake ..```
- ```cmake --build .``` or just ```make```

## Build steps for windows
You can simply run the **win64_make.sh** script inside the working directory of the project but I will give a rundown of what it does here.

```
    cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
        -DCMAKE_C_COMPILER=/c/msys64/mingw64/bin/gcc.exe \
        -DCMAKE_CXX_COMPILER=/c/msys64/mingw64/bin/g++.exe \
        -S. \
        -B./build \
        -G "MinGW Makefiles"
```


- ```cd build```
- ```mingw32-make```







