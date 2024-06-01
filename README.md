# Apps and tools

## Outrun

  A fake outrun like pseudo3d racing render of accelerometers inputs.

### Run
 ```bash
 ./outrun
 ```
### Controls

  M - toggle circular mask

  D - toggle debug infos

  SPACE - start game

## Testhills

Testing a procedural landscape generation algorithm with four layers of background hill

 ```bash
 ./testhills
 ```

## Tractocompass

A fun compass using magnometer

### Run
 ```bash
 ./tractocompass
 ```

### Controls

Q
A
S
Z
X
UP/DOWN
LEFT/RIGHT

## Xlevel

A fun level themable level indicator

### Run
 ```bash
 ./xlevel
 ```
### Controls
TODO

# Installation

## Ubuntu 20.04 and distros based on Ubuntu 20.04

Update and install the compiler, related build tools, and libraries required to build PixelGameEngine applications.

 ```bash
sudo apt update
sudo apt install build-essential libglu1-mesa-dev libpng-dev libx11-dev libpng-dev
``` 
# Build & run

 ```bash
make
./outrun
./testhills
./tractocompass
...
```

# Tools & other code samples

## Generate new font sprite png

Exemple to generate png sprite from arial.ttf:

```bash
pip3 install requirements.txt
python3 tools/generatefont.py Arial.ttf 20
```
## Landscape generator in python example

Python 2D landscape generator using midpoint displacement.

From https://github.com/juangallostra/Landscape-generator

To read an in depth explanation visit this [blog entry](https://bitesofcode.wordpress.com/2016/12/23/landscape-generation-using-midpoint-displacement/).

```bash
pip3 install requirements.txt
python3 tools/generatelandscape.py
```

## Voxel to png

Tool to convert Magicavoxel files and output png for sprite stacking.
Taken from https://github.com/jemisa/vox2png

```bash
cd tools/vox2png
gcc -o vox2png vox2png.c
./vox2png voxels.vox output.png
```


# Outrun from "Let's make 16 games in C++" video series

Let's make 16 games in C++: Outrun (Pseudo 3d racing)
https://www.youtube.com/watch?v=N60lBZDEwJ8

code of all games can be found in examples/ folder

outrun code is in "examples/16_Games/08 Outrun" folder

/!\ requires libSFML

```bash
  sudo apt install libsfml-dev
  cd "examples/16_Games/08 Outrun"   
  make
  ./outrun
```
