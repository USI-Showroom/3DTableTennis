## What is this repository for?
A 3D table tennis software, developed by Teseo Schneider.


## How do I get set up?
You need (Windows only):

* C++ compiler
* Qt5
* OpenCV
* Kinect SDK v2


### Method 1 (suggested)
* Install Visual Studio (the community version is free) to obtain the C++ compiler. Select "Desktop development"
* Install [vcpkg](https://vcpkg.io/)
* Install the dependencies: `vcpkg install glew qt5-base qt5-multimedia`

### Method 2
* Install Visual Studio (the community version is free) to obtain the C++ compiler. Select "Desktop development"
* Download Qt5 from the [official website](https://www.qt.io/download/). Choose the Open Source version, install it in `C:\`
* Download OpenCV from their [Sourceforge page](https://sourceforge.net/projects/opencvlibrary/) and extract in `C:\`
* Set the environment variable `QT_PATH` to the installation of Qt (e.g. `C:\Qt\5.15.0\msvc2019_64`)
* Add the Qt and OpenCV bin folders (e.g. `C:\Qt\5.15.0\msvc2019_64\bin` and `C:\opencv\build\x64\vc15\bin`) to PATH and reboot your machine


## Compiling
* Open the `tableTennis.sln` file from the `v2` folder
* In Visual Studio, choose "Release" instead of "Debug"
* Under "Build" choose "Build solution"
* Plug the Kinect in and run `bin\Release\tableTennis.exe`

Note: if you get errors about certain DLLs, delete those in the `bin\Release` folder and
run `windeployqt tableTennis.exe` in there


## Run @ USI
See [these instructions](https://github.com/USI-Showroom/3DTableTennis/blob/master/docs/3DTableTennis_manual_ITA.pdf)
