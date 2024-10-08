
# USAGE RESTRICTIONS
 
**By using this music player, you agree to only use music that you have legally obtained the rights to. This includes, but is not limited to, music that you have purchased, obtained through legitimate streaming services, or that is freely available for use under open licenses. Any unauthorized use of copyrighted music is strictly prohibited and may result in legal action.**

## Music Visualizer
> Music is from the game Crypt of the Necrodancer and composed by Danny Baranowsky. You can buy the soundtrack off steam. 
- [Video Demo](https://www.youtube.com/watch?v=6UWzQOlIfTw)

## Images
![Image](example/example_desktop_1.png)
![Image](example/example_desktop_2.png)
![Image](example/example_desktop_3.png)
![Image](example/example_desktop_4.png)

## Releases
There are currently no releases. If you want to use this program, you will have to build it. Thankfully it's relatively painless. Build steps are at the end of the readme. Thanks!
 
The program relies on directories located in the **~/Music dir located in the home path**.


## Configuration
You can use a config.txt to set the the colors you want the program to use at runtime. The defaults are what are shown above. Check out CONFIG_EXAMPLE.txt to see how to set that up. It uses RGBA in SDL2s format. This is pretty lazy set up right now and you need to launch the program in the working directory for it to find the files.

If you are using a window manager like i3 or sway, You can pass ```--no-mouse-grab``` as a command line argument when running the program to disable the grab and drag code I have written for the window. I would recommend it, as it doesn't play nice for the most part.

> Foundational directories will be **created on the initial program launch if they do not already exist:**
- ```MVLogs``` which contains ```errlog.txt``` and ```log.txt```
- ```MVSource``` which is where your music will be located. **The program expects folders inside this directory**, of which contain the audio files.
---


## Controls
**General controls**

- Up : Navigate cursor up through current list.
- Down : Navigate cursor down through current list.
- Space : Select the current item under cursor.
- Mouse : Click and drag the window.
- Right : Navigate to the right.
- Left : Navigate left.
- H J K L : Change window sizing.
- T : Toggle window border.
- P : Toggle Pause.
- Q or close : Quit.

**In settings**
- Shift + L/R Arrow keys : Navigate settings.
- U/D Arrow keys : Select between settings.
- Shift + U/D Arrow keys : Change setting values.

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
    cmake -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
        -DCMAKE_C_COMPILER=/c/msys64/mingw64/bin/gcc.exe \
        -DCMAKE_CXX_COMPILER=/c/msys64/mingw64/bin/g++.exe \
        -S. \
        -B./build \
        -G "MinGW Makefiles"
```


- ```cd build```
- ```mingw32-make```







