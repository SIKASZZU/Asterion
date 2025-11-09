# Asterion <!-- omit in toc -->


GAMEPLAY: Introduction
- [Game](#game)
  - [Gameplay preview](#preview)
  - [Keys](#key-functionality)

Introductions to setup project
- [Windows Setup](#windows-setup)
  - [Install Msys2](#install-msys2)
  - [Install toolchain for msys2](#install-toolchain-for-msys2)
  - [Install SDL](#install-sdl)
  - [Install SDL3\_image](#install-sdl3_image)
  - [Building and running the project](#building-and-running-the-project)


## Game

### Preview

https://github.com/user-attachments/assets/10ef7af4-9685-42c5-8c61-f2241fd20a4b

### Key Functionality

Below is a breakdown of in-game keybindings and their functionalities:

- W, A, S, D	  Move the player in the respective direction. Includes subtle sliding after movement.
- F         	  Print detailed player info (coordinates, grid position, tile values, offset, etc).
- C	Toggle      Player collision on/off.
- R	Toggle      Raycasting system toggle on/off.
- V	Toggle      Vision system toggle on/off.
- PLUS  +	      Increase renderRadius and update raycast range.
- MINUS -	      Decrease renderRadius (minimum of 5) and update raycast range.
- . (Period)	  Increase tileSize (and player size accordingly).
- , (Comma)	    Decrease tileSize (with a minimum cap).
- PageDown	    Set pathfinding start point to player's current grid tile.
- PageUp	      Set pathfinding end point to player's current grid tile, then compute path.
- Left Shift	  Temporarily reduce movement speed (1/4th); enables "sneak" or precision


## Windows Setup

### Install Msys2

Install [msys2](https://www.msys2.org/#installation) or do it with [chocolatey](https://community.chocolatey.org/packages/msys2)

```bash
choco install msys2
```

---

Feel free to add it into your windows terminal
> <https://www.msys2.org/docs/terminals/>

by adding it into your windows terminal json

```json
"profiles": {
  "list":
  [
    // ...
    {
      "name": "UCRT64 / MSYS2",
      "commandline": "C:\\tools\\msys64\\msys2_shell.cmd -defterm -here -no-start -ucrt64",
      "icon": "C:\\tools\\msys64\\ucrt64.ico",
    },
    {
      "name": "MSYS / MSYS2",
      "commandline": "C:\\tools\\msys64\\msys2_shell.cmd -defterm -here -no-start -msys",
      "icon": "C:\\tools\\msys64\\msys2.ico",
    },
    // ...
  ]
}
```

You can get the location of msys2 with windows powershell by running

```bash
Get-Command -All msys2
```

### Install toolchain for msys2

Install required libraries for compiling c++

```bash
pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain
```

> This will install them on the UCRT64 [environment](https://www.msys2.org/docs/environments/)

Make sure to add them to your windows `PATH` environment variable

Paths added to user variables:

- `*\msys64\usr\bin`
- `*\msys64\ucrt64\bin`

### Install SDL

SDL3 version [3.2.18](https://github.com/libsdl-org/SDL/releases/tag/release-3.2.18)

Manually install SDL3-devel-3.2.18-mingw.zip. 
Extract files.
Use MSYS and cd to *\SDL3-devel-3.2.18-mingw\SDL3-3.2.18
Then use `make`
Run cmd to install 64-bit version
- make install-x86_64"

Run commands below to move SDL3 files to mingw64
- cp -r /usr/local/include/SDL3/* /mingw64/include/SDL3/
- cp /usr/local/lib/libSDL3.dll.a /mingw64/lib/
- cp /usr/local/bin/SDL3.dll /mingw64/bin/


### Install SDL3_image

SDL3_image version [3.2.4](https://github.com/libsdl-org/SDL_image/releases/tag/release-3.2.4)

Manually install SDL3_image-devel-3.2.4-mingw.zip.
Extract files.
Use MSYS and cd to *\SDL3-devel-3.2.4-mingw\SDL3-3.2.4
Then use `make`
Run cmd to install 64-bit version
- make install-x86_64"

Run commands below to move SDL3_image files to mingw64
- cp -r /usr/local/include/SDL3_image/* /mingw64/include/SDL3/
- cp /usr/local/lib/libSDL3_image.dll.a /mingw64/lib/
- cp /usr/local/bin/SDL3_image.dll /mingw64/bin/

### Building and running the project

This project uses [make](https://www.gnu.org/software/make/manual/html_node/index.html#SEC_Contents) to build and run the project

Simply run the following command to build and run the game

```bash
make
```

---

For Visual Studio Code it is reccomended to use the [MakeFile Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.makefile-tools) extension


Although that is not needed as running the project with `F5` will work just fine

