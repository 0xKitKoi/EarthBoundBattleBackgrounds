# EarthBound Battle Backgrounds C++

This is a C++ Port of Earthbound's Battle Backgrounds. This project depends on Simple Direct Media Layer 2 (SDL2). 

If you like this project, you might also like the [Earthbound Battle Backgrounds JS](https://github.com/gjtorikian/Earthbound-Battle-Backgrounds-JS), which this project is based on. 

![Usage Animation](https://github.com/0xKitKoi/EarthBoundBattleBackgrounds/blob/84bce3270b16ac2c6c070e950fd68d91d0d5bafb/Usage.gif)

## Credits

I owe this project to [gjtorikian](https://github.com/gjtorikian) who published the JavaScript Implimentation, and by extension, Mr. Accident of forum.starmen.net for the original battle background generation code.
I could not find the original code, and had only the JS port to work with. The dat files here are directly from the JS implimentation, as well as the code for addressing the dat file. 

## Usage 

This program takes in the dat file as command line arguments, as well as the two desired layers.
  #### USAGE: ``` ./ebbg <path-to-rom> [layer1] [layer2] [frameskip] [aspectRatio] ```
  - layer1: 0-326 (default: 270)
  - layer2: 0-326 (default: 269)
  - frameskip: 1-10   (default: 1)
  - aspectRatio: 0, 16, 48, 64 (default: 0)


## Building
This project uses [Premake5](https://premake.github.io/) for a nice and clean cross platform building system.  
#### If you are on WINDOWS
please run the batch scripts: [SDL Setup](https://github.com/0xKitKoi/EarthBoundBattleBackgrounds/blob/main/SDLSetup.bat) and [Visual Studio Setup](https://github.com/0xKitKoi/EarthBoundBattleBackgrounds/blob/main/visualstudiosetup.bat). 
These download and set up SDL2 for compiling for you. Then simply open the Visual Solution file and build. 
#### If you are on Linux
Download [Premake5](https://github.com/premake/premake-core/releases/download/v5.0.0-beta7/premake-5.0.0-beta7-linux.tar.gz) and install SDL2 using your package manager.

You'll need cmake and Simple Direct Media Layer 2 (SDL2) :
```bash
wget https://github.com/premake/premake-core/releases/download/v5.0.0-beta7/premake-5.0.0-beta7-linux.tar.gz

# For Debian-based systems (e.g., Ubuntu):
sudo apt update
sudo apt install cmake build-essential
sudo apt install -y \
  libsdl2-dev \
  libsdl2-image-dev \
  libsdl2-ttf-dev

# For Fedora-based systems:
sudo dnf install -y \
  SDL2-devel \
  SDL2_image-devel \
  SDL2_ttf-devel

# For Arch-based systems:
sudo pacman -S --noconfirm \
  sdl2 \
  sdl2_image \
  sdl2_ttf
```
After installing SDL2, simply run premake5 to generate the MakeFiles, and run make. ``` ./premake5 gmake && make ```

## How it works
See [Distorter.hpp](https://github.com/0xKitKoi/EarthBoundBattleBackgrounds/blob/main/rom/Distorter.hpp) for the implimentation

Every battle background is composed of two layers, each with 327 possible
styles (including "blank"/zero). The layer styles can be interchanged, such that
[Layer 1: 50 and Layer 2: 300](https://gjtorikian.online/Earthbound-Battle-Backgrounds-JS/?layer1=50&layer2=300)
is the same as [Layer 1: 300 and Layer 2: 50](https://gjtorikian.online/Earthbound-Battle-Backgrounds-JS/?layer1=300&layer2=50).
Thus, there are C(n,r) = 52,650 possible different background combinations. Obviously,
this many don't exist in the game--the SNES's graphical capabilities only allow it to
properly render 3,176 of these combinations, and of those, only 225 are ever used.

The data for each of the 327 styles is bundled within the SNES cartridge.
Tiles are
constructed from various memory addresses in Earthbound game data. To create
the distortion effect, the following function is used:

```
Offset (y, t) = A sin ( F*y + S*t )
```

where:

- _y_ is the vertical coordinate being transformed
- _t_ is time that's elapsed
- _A_ is the amplitude
- _F_ is the frequency
- _S_ is the speed or frameskip of the transformation

Offest refers to the _y_ direction of the shift at a given time _t_.

There are also three types of distortions that use the result of the Offset
function:

- Horizontal translations, where each line is shifted left by the given number
  of pixels
- Horizontal interlaced translations, where every other line is shifted right
  by the given number of pixels
- Vertical compression translations, where each line is shifted up or down by
  the given number of pixels

Different backgrounds use different distortion effects.

## License

This app is in no way endorsed or affiliated by Nintendo, Ape, HAL Laboratory,
Shigesato Itoi, etc. It's licensed under MIT.
