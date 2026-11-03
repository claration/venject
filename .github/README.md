# Vencord Launcher

[![GitHub Release](https://img.shields.io/github/v/release/claration/venject?include_prereleases)](https://github.com/claration/venject/releases)
[![GitHub Downloads (all assets, all releases)](https://img.shields.io/github/downloads/claration/venject/total)](https://github.com/claration/venject/releases)

Custom launcher for Discord which loads [Vencord](https://github.com/Vendicated/Vencord)

![Demo of app](demo.png)

### Features
- User friendly and clean UI.
- Customizable existing Discord instances.
  - Specify custom launch arguments.
  - Specify custom Vencord installation paths.
- Add custom shortcuts that automatically launch Discord with Vencord.
- View Discord's normally hidden logs.

## Download

Visit [releases](https://github.com/claration/venject/releases) and get the latest version for your computer.

## Building

Building is going to be a bit convoluted for each platform, each having their own unique specifications, but the best reference for building should be looking at how [GitHub actions](./.github/workflows/build.yml) does it.

```sh
# Supported params:
#  Whether to run QT's install script       ("ON" default):
#   ENABLE_QT_DEPLOY=ON
#  Custom package generator for cpack       (NSIS (Windows) default, AppImage (Linux) default):
#   PACKAGE_GENERATOR=
#  Cores to use when compiling              (1 default):
#   JOBS=1
#  Debug or release build                   ("Debug" default):
#   BUILD_TYPE="Release"
```

### Debian / Ubuntu Linux

For packaging an AppImage, you're gonna need to do some extra steps outside of the package manager, you can see [here](https://github.com/claration/venject/blob/trunk/.github/workflows/build.yml#L16-L23) on how to do that.


```sh
sudo apt install -y \
    build-essential \
    cmake \
    qt6-base-dev \
    qt6-declarative-dev \
    qt6-tools-dev \
    qt6-svg-dev
```

### Windows

- [MSYS2](https://www.msys2.org/)

```sh
# Or a clang toolchain for Windows Arm64
pacman -S \
    git \
    mingw-w64-x86_64-toolchain \
    mingw-w64-x86_64-cmake \
    mingw-w64-x86_64-nsis \
    mingw-w64-x86_64-qt6-base \
    mingw-w64-x86_64-qt6-declarative \
    mingw-w64-x86_64-qt6-tools \
    mingw-w64-x86_64-qt6-svg \
    make \
    patch
```


### Compiling

```sh
# Linux / Windows for testing
make
# Linux / Windows packaging (Default)
make package JOBS=$(nproc) BUILD_TYPE=Release
# Linux packaging (Custom)
make package JOBS=$(nproc) BUILD_TYPE=Release PACKAGE_GENERATOR=AppImage ENABLE_QT_DEPLOY=ON
make package JOBS=$(nproc) BUILD_TYPE=Release PACKAGE_GENERATOR=RPM ENABLE_QT_DEPLOY=OFF
make package JOBS=$(nproc) BUILD_TYPE=Release PACKAGE_GENERATOR=DEB ENABLE_QT_DEPLOY=OFF
```

## Acknowledgements

- [CLARATION](https://github.com/claration) – The maker.
- [Instellate](https://github.com/Instellate) – Windows support & QT stuff.
- [Paige](https://github.com/ssalggnikool) – Fixes & QT stylistic choices.
- [Megumin](https://github.com/MeguminSama/Electron-Hook) – Injection references.

## License

This project is licensed under the GPL-3.0 license. You can see the full details of the license [here](https://github.com/claration/venject/blob/main/LICENSE).

By contributing to this project, you agree to license your code under the GPL-3.0 license as well (including agreeing to license exceptions), ensuring that your work, like all other contributions, remains freely accessible and open.
