# Stegano

A steganography tool written in C that hides secret messages inside images using LSB (Least Significant Bit) encoding. Supports BMP and PNG formats.

## How it works

Each pixel in an image has three color channels — Red, Green, Blue — each stored as a byte. Changing the least significant bit of each channel by 1 is completely invisible to the human eye. Stegano hides your message's bits inside these LSBs, leaving the image looking identical to the original.

## Dependencies

- GCC compiler
- libpng (install via vcpkg: `vcpkg install libpng:x64-windows`)

## Setup

1. Clone the repository

        git clone https://github.com/m1n1k30/Stegano.git
        cd Stegano

2. Install libpng via vcpkg then compile

        make

3. Create the image folders

        make init

4. Drop a BMP or PNG image into `images/encode/`

## Usage

Encode a message:

        .\stegano.exe encode images\encode\input.png images\decode\output.png "your message"

Encode with password:

        .\stegano.exe encode images\encode\input.png images\decode\output.png "your message" password

Decode:

        .\stegano.exe decode images\decode\output.png

Decode with password:

        .\stegano.exe decode images\decode\output.png password

## Project structure

        stegano/
        ├── include/
        │   ├── utils.h       — bit manipulation declarations
        │   ├── image.h       — image loading/saving declarations
        │   └── steg.h        — embed/extract declarations
        ├── src/
        │   ├── utils.c       — get_bit and set_bit implementation
        │   ├── image.c       — BMP and PNG file handling
        │   ├── steg.c        — LSB steganography algorithm
        │   └── main.c        — CLI entry point
        └── Makefile

## Built with

- C
- libpng
- zlib