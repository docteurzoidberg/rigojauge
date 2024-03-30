# Installation

## Ubuntu 20.04 and distros based on Ubuntu 20.04

Update and install the compiler, related build tools, and libraries required to build PixelGameEngine applications.

  sudo apt update

  sudo apt install build-essential libglu1-mesa-dev libpng-dev


# Build

g++ -o olcExampleProgram olcExampleProgram.cpp -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17
