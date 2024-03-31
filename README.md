# Installation

## Ubuntu 20.04 and distros based on Ubuntu 20.04

Update and install the compiler, related build tools, and libraries required to build PixelGameEngine applications.

  sudo apt update
  sudo apt install build-essential libglu1-mesa-dev libpng-dev

# Build & run

  g++ -o rigo rigojauge.cpp -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17
  ./rigo

# Generate new font sprite png

  python3 generatefont.py Arial.ttf 20


# Controls

M - toggle circular mask
D - toggle debug infos
SPACE - start game