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

Testing a procedural landscape generation algorythm with four layers of background hill

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
```

# Tools & other code samples

## Generate new font sprite png

Exemple to generate png sprite from arial.ttf:

```bash
  pip3 install requirements.txt
  python3 generatefont.py Arial.ttf 20
```

## Landscape generator in python example

Python 2D landscape generator using midpoint displacement.

From https://github.com/juangallostra/Landscape-generator.git

To read an in depth explanation visit this [blog entry](https://bitesofcode.wordpress.com/2016/12/23/landscape-generation-using-midpoint-displacement/).

```bash
pip3 install requirements.txt
python3 generatelandscape.py
```