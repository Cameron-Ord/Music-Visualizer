
# USAGE RESTRICTIONS
 
**By using this music player, you agree to only use music that you have legally obtained the rights to. This includes, but is not limited to, music that you have purchased, obtained through legitimate streaming services, or that is freely available for use under open licenses. Any unauthorized use of copyrighted music is strictly prohibited and may result in legal action.**

## Music Visualizer
![Image](readme_gif/example.gif)

**A music visualizer for Linux systems written in C. This is unfinished software, so keep your expectations low.**

I'm still thinking of a name for this program, haven't really thought of anything yet so I'm just going with fftplayer for now. 

## Releases

If you're interested in giving it a try, I currently have a **deb package and a zip file**.

> Go to the releases page on the repo and select the **beta tag**.

---
> For the deb package you can just run it and it will **install the libraries as dependencies.**
- ```sudo mv fftplayer_86_64.deb /tmp```
- ```sudo apt install ./fftplayer_86_64.deb```

> To run the application:
- Type ```fftplayer``` in a terminal or launcher. The binary is located in /usr/local/bin.

**If you don't want that, or you just aren't using a Debian based distrobution, there is also the zip file which just includes the shared library object files**.

> For the zip file, unzip it and launch the install.sh script.
- ```unzip fftplayer_86_64.zip```
- ```sudo ./install.sh```

> To run the application:
- Type ```runfft``` in a terminal or launcher. The binary is located in /usr/local/bin.
---

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

> Keep in mind that you will need to create a folder in /usr/share/fonts called fftplayer_fonts and move NotoSansMono-SemiBold.ttf into it from your system fonts, or edit the source code in the main.c to go to your own font path.

- run ```make``` inside the working dir. Executable will be in the build folder that gets generated.





