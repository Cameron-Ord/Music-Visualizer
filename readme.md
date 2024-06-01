
# USAGE RESTRICTIONS
 
**By using this music player, you agree to only use music that you have legally obtained the rights to. This includes, but is not limited to, music that you have purchased, obtained through legitimate streaming services, or that is freely available for use under open licenses. Any unauthorized use of copyrighted music is strictly prohibited and may result in legal action.**

## Music Visualizer
![Image](readme_gif/example.gif)

**A music visualizer for Linux systems written in C. This is unfinished software, so keep your expectations low.**

I'm still thinking of a name for this program, haven't really thought of anything yet so I'm just going with Music Visualizer for now. 

## Releases
I am working on creating a release for this program at the moment.

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
- Clang ```sudo apt install clang```

## Building
- ```git clone https://github.com/Cameron-Ord/fft_music_visualizer``` 

- run ```make``` to compile a binary to the build folder.
- run ```sudo make install``` to place binaries and required fonts in /usr/local/bin and /usr/share/fonts/truetype/quicksand/

## Uninstallation

- Simply run make uninstall to remove the binary from your /usr/local/bin/ path.






