# chip-8-emu

An emulator/interpreter of [Chip-8](https://en.wikipedia.org/wiki/CHIP-8) built using C and SDL2.

Play a whole bunch of retro games!  

![Slippery Slope by JohnEarnest](https://raw.githubusercontent.com/srenegado/chip-8-emu/refs/heads/main/doc/showcase.gif)

## Dependencies
First you'll need:
- GCC
- make
- [SDL2](https://wiki.libsdl.org/SDL2/Installation)

## Usage

### Building

To build the emulator:
```
make
```

Load up a game:
```
./chip8 /path/to/game_rom.ch8
```
There are handful of Chip-8 games out and about. The [CHIP-8 Archive](https://johnearnest.github.io/chip8Archive/?sort=platform) has a nice collection; check games under the "chip-8" platform.

### Controls
The keypad is 

| `1` | `2` | `3` | `4` |
| :-: | :-: | :-: | :-: |
| `Q` | `W` | `E` | `R` |
| `A` | `S` | `D` | `F` |
| `Z` | `X` | `C` | `V` |

`ESC`: Shutdown.

## Future extensions
If I ever wanted to implement them:
- [ ] Add Super-Chip support
- [ ] Add XO-Chip support
- [ ] Add configurability for different platforms (Chip-8 / Super-Chip / XO-Chip)

## References
- [Tobias' Guide to making a CHIP-8 emulator](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/) - Overall was very helpful to get the concepts down.
- [Cowgod's Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM) - A handy guide for checking opcodes.
- [Playing a single note with SDL2](https://blog.fredrb.com/2023/08/08/audio-programming-note-sdl/) - Audio programming was pretty new for me and this covered the basics pretty well.
