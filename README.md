# EarthBound Battle Backgrounds C++

This is a C++ Port of Earthbound's Battle Backgrounds. This project depends on Simple Direct Media Layer 2 (SDL2). 

If you like this project, you might also like the [Earthbound Battle Backgrounds JS](https://github.com/gjtorikian/Earthbound-Battle-Backgrounds-JS), which this project is based on. 

## Credits

I owe this project to [gjtorikian](https://github.com/gjtorikian) who published the JavaScript Implimentation, and by extension, Mr. Accident of forum.starmen.net for the original battle background generation code.
I could not find the original code, and had only the JS port to work with. The dat files here are directly from the JS implimentation, as well as the code for addressing the dat file. 


## License

This app is in no way endorsed or affiliated by Nintendo, Ape, HAL Laboratory,
Shigesato Itoi, etc. It's licensed under MIT.

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

