# Asterion <!-- omit in toc -->

TODO: introduction

- [Windows Setup](#windows-setup)
  - [Install Msys2](#install-msys2)
  - [Install toolchain for msys2](#install-toolchain-for-msys2)
  - [Install SDL](#install-sdl)
  - [Install SDL2\_image](#install-sdl2_image)
  - [Building and running the project](#building-and-running-the-project)

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
> the path I added was `C:\tools\msys64\ucrt64\bin`

### Install SDL

This project uses [SDL](https://github.com/libsdl-org/SDL) version [2.38.8](https://github.com/libsdl-org/SDL/releases/tag/release-2.32.8)

The installation steps are:

- download [SDL2](https://github.com/libsdl-org/SDL/releases/download/release-2.32.8/SDL2-2.32.8.zip)
- extract it
- open the directory in the UCRT64
- run `./configure; make; make install`

> This will add SDL2 to UCRT64 cmake library  
> `C:\tools\msys64\ucrt64\lib\cmake`

### Install SDL2_image

This project uses [SDL_image](https://github.com/libsdl-org/SDL_image) version [2.8.8](https://github.com/libsdl-org/SDL_image/releases/tag/release-2.8.8)

The installation steps are:

- download [SDL2_image](https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.8/SDL2_image-2.8.8.zip)
- extract it
- open the directory in the UCRT64
- run `./configure; make; make install`

> This will add SDL2_image to UCRT64 cmake library  
> `C:\tools\msys64\ucrt64\lib\cmake`

### Building and running the project

This project uses [make](https://www.gnu.org/software/make/manual/html_node/index.html#SEC_Contents) to build and run the project

Simply run the following command to build and run the game

```bash
make
```

---

For Visual Studio Code it is reccomended to use the [MakeFile Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.makefile-tools) extension

Although that is not needed as running the project with `F5` will work just fine
