# Chip-8 emulator

An emulator/interpreter of [Chip-8](https://en.wikipedia.org/wiki/CHIP-8) built using C and SDL2.

Play a whole bunch of retro games or even some that are modern and community-made!  

## Dependencies
- [SDL2](https://wiki.libsdl.org/SDL2/Installation)

## Usage

To build the emulator:
```
make
```

Load up a game and have some funs.
```
./chip8 games/super_cool_game.ch8
```

## TODOs
Extensions if I ever wanted to implement them:
- [ ] Add Super-Chip support
- [ ] Add XO-Chip support
- [ ] Add configurability for different platforms (Chip-8 / Super-Chip / XO-Chip)

## References
- [Tobias' Guide to making a CHIP-8 emulator](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/) was very helpful to get the concepts down.
- [Cowgod's Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM) for checking opcodes.
- Audio programming was pretty new for me and this covered the basics pretty well: [Playing a single note with SDL2](https://blog.fredrb.com/2023/08/08/audio-programming-note-sdl/).
