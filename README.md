# LKDS ALKOTest

## OS support

* MacOS 10.13 (High Sierra) or newer
* Windows 7, Windows 8.1/8.2 and Windows 10
* Linux (Arch Linux, Ubuntu)

## Building

The following platform-specific steps will produce a bundle in the build directory (e.g. `build.release`):
* `alkotest.app` application bundle when building for OSX.
* `alkotest` directory when building for Windows or Linux.

### OSX

1. Install `Command Line Tools` if they're not yet installed:
    ```bash
    xcode-select --install
    ```

2. Install `Homebrew` if it's not yet installed:
    ```bash
    ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
    ```

3. Install project dependencies:
    ```bash
    brew install cmake
    ```

4. Clone the project and create a build directory:
    ```bash
    git clone git@github.com:inspera/iceworm.git
    cd iceworm
    mkdir build.release
    cd build.release
    ```

5. Configure and build the project:
    ```bash
    cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
    make -j`nproc`
    ```

### Windows

1. Install [MSYS2](https://www.msys2.org/) into default path (C:\msys64).

2. Open MSYS2 MinGW64 terminal and install the necessary packages:
    ```bash
    pacman -S git diffutils make patch mingw-w64-x86_64-cmake mingw64/mingw-w64-x86_64-gcc
    ```
3. Clone the project and create a build directory (same as for OSX).

4. Configure and build the project:
    ```bash
    cmake -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
    make -j`nproc`
    ```

### Linux

1. Install build dependencies. The command below is for aptitude-based distributions (e.g. Ubuntu).  If you use a distribution with a different package manager, install the corresponding software using its conventional facilities.
    ```bash
    apt-get update && apt-get upgrade
    apt-get install build-essential git cmake
    ```

2. Perform the steps 4 and 5 from the OSX section.

mkdir gtk-build
cd gtk-build
../configure --with-gtk3 --disable-shared --enable-unicode --with-opengl --prefix=/opt/wxwidgets-gtk3/ --with-libmspack --with-sdl
make -j4
make install

