## What is this repository for?
A 3D table tennis software, developed by Teseo Schneider. There are two versions of the software:
the v1 is in the root whereas the v2 is in the v2 folder.


## How do I get set up?
This works on Windows only. You need:

* C++ compiler
* Qt5
* OpenCV
* Kinect SDK or [libfreenect2](https://github.com/OpenKinect/libfreenect2) (see below)

Note: as of September 2021, the latest Nvidia driver that supports stereo 3D is the 425.31,
be sure to not use a more recent one!

* Install Visual Studio (the community version is free) to obtain the C++ compiler. Select "Desktop development"
* Install the Kinect SDK:
    * for the v1, install the official [Kinect SDK v1.7](https://www.microsoft.com/en-us/download/details.aspx?id=36996)
    * for the v2, if on Windows 7 install `libfreenect2` via [vcpkg](https://vcpkg.io/), 
    otherwise install the official [Kinect SDK v2](https://www.microsoft.com/en-us/download/details.aspx?id=44561)
* Download Qt5 from the [official website](https://www.qt.io/download/). Choose the Open Source version, install it in `C:\`
* Download OpenCV from their [Sourceforge page](https://sourceforge.net/projects/opencvlibrary/) and extract in `C:\`
* Set the environment variable `QT_PATH` to the installation of Qt (e.g. `C:\Qt\5.15.0\msvc2019`)
* Add the Qt and OpenCV bin folders (e.g. `C:\Qt\5.15.0\msvc2019\bin` and `C:\opencv\build\x64\vc15\bin`) to PATH and reboot your machine


## Compiling

### v1
* Open the cmake GUI
* Under "Where is the source code", select the root folder of the project
* Under "Where to build the binaries" select the `bin` folder in the root directory
* Click "Configure" and choose your version of Visual Studio
* Click "Generate" to generate the Visual Studio solution
* Click "Open Project" to open it
* In Visual Studio, choose "Release" instead of "Debug"
* Under "Build" choose "Build Solution"
* Run `bin\Release\3DTableTennis.exe`

### v2
* Open the `tableTennis.sln` file from the `v2` folder
* In Visual Studio, choose "Release" instead of "Debug"
* Under "Build" choose "Build solution"
* Run `v2\bin\Release\tableTennis.exe`

Note: if you get errors about certain DLLs, delete those (if present) in the `Release` folder and
run `windeployqt 3DTableTennis.exe` or `windeployqt tableTennis.exe` in there


## Run @ USI
See [these instructions](https://github.com/USI-Showroom/3DTableTennis/blob/master/docs/3DTableTennis_Manual_ITA.pdf)
