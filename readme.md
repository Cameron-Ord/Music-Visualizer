
# USAGE RESTRICTIONS
 
**By using this music player, you agree to only use music that you have legally obtained the rights to. This includes, but is not limited to, music that you have purchased, obtained through legitimate streaming services, or that is freely available for use under open licenses. Any unauthorized use of copyrighted music is strictly prohibited and may result in legal action.**

## Music Visualizer
![Image](example/example.png)

**A music visualizer for Linux systems written in C. This is unfinished software, and the program is mostly written with only my personal use in mind. So please be considerate of this.**

I'm still thinking of a name for this program, haven't really thought of anything yet so I'm just going with Music Visualizer for now. 

## Todo
> - Setting up a settings menu for manipulation of the internal variables of the program.

## Releases
I have builds for Windows on x86 architecture at the releases page.

For Linux, go to the build section. I will be whipping up a release for Linux soon as well.


**For more details on how the program works, go below.**

## Usage Manual
**For understanding on how to properly use the program please read the following**.

---
> When the program looks for directories and files, it will replace spaces and comas from folders and files with underscores using rename().
 
The program relies on directories located in the **~/Music dir located in the home path**.

> Foundational directories will be **created on the initial program launch if they do not already exist:**
- ```fftmlogs``` which contains ```errlog.txt``` and ```log.txt```
- ```fftmplayer``` which is where your music will be located. **The program expects folders inside this directory**, of which contain the audio files.
---

**Your music and folders will be listed when there is no music playing ; you can scroll through with the mouse wheel**. 

## Controls
- Space : Play or Stop
- p : Pause or Resume
- Left and right arrow keys : Cycle through songs
- r : random song
- q : Quit

## Build Requirements
* **[A Linux Distribution]**

>  The following are required to build. The packages I am providing here are for Debian repos.

- SDL2 ```sudo apt install libsdl2-dev```
- libsndfile ```sudo apt install libsndfile1``` 
- SDL_ttf ```sudo apt install libsdl2-ttf-dev```                  
- SDL_image ```sudo apt install libsdl2-image-dev```

## Building
- ```git clone https://github.com/Cameron-Ord/fft_music_visualizer``` 

> MinGW x86_64 is required to build the windows binary

> Clang or GCC is required to build the linux binary

I'm using make for this project, so it will be easiest to build on a linux host. For building the linux version, just make sure you have the required system dev packages as listed above and run ```make linux```. executable will be in the LINUX dir.

For the win binary im cross compiling with mingw so make sure you have mingw64. you will also need the libs manually downloaded and set up. See the makefile itself to see how to set up the dir structure. All the header files for sdl2, including ttf and image just go into the sdl2 include, then then it points to the seperate tff and image folders just for the libs.

Alternatively, I have release for Windows with an exe!!!1










