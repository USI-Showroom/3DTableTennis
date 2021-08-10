## Material

* Computer
* Kinect (v1)
* Kinect mount (3 pieces)
* Projector
* 3D sync material
* 2 long dvi cable
* Mouse keyboard + usb connector
* Wooden box
* Glasses
* Glasses chargers
* Paddle
* Speakers
* Power switch


## Setup

* Put projectors on top of wooden box
* Connect all cables (2 dvi on projector, 1 usb, 1 jack, 3d sync box) and power
* Enter the projector menu, under stereo, enable stereoscopic
* Boot windows 7 and use the pinpongproject user
* Launch 3d table tennis (shortcut on desktop)


## Building the software

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
* Open the project `.sln` file
* In Visual Studio, choose "Release" instead of "Debug"
* Under "Build" choose "Build solution"
* Run `bin\Release\3d_table_tennis.exe`

Note: if you get errors about certain DLLs, delete those in the `bin\Release` folder and
run `windeployqt 3d_table_tennis.exe` in there


## Run @ USI
See [these instructions](https://github.com/USI-Showroom/3DTableTennis/blob/master/docs/3DTableTennis_manual_ITA.pdf)
