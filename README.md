# Parking System
This is a demo project.
Based on Qt, Sqlite, OpenCV, HpyerLPR.

# Screenshot
![pic](https://github.com/424778940z/parking_system/blob/master/screenshot.jpg?raw=true)

```
PC side:
parking_system_backend
parking_system_serial_test

Microcontroller side:
parking_system_superio_ArduinoUno
parking_system_superio_STF767ZI
```

# How to build
```
PC side:
Install MSYS2, install Qt and Qt Creator Package.
Download OpenCV 3.4.10 source (since MSYS2 only have latest version) and build with default config under MSYS2 console.
After OpenCV "make install", copy everything under install folder into "opencv3410".
Clone HyperLPR then copy everything under HyperLPR-master\Prj-Win\lpr folder into "lpr."
Open project in Qt Creator then build.
Note: 
OpenCV and HyperLPR only needed for parking_system_backend.
You may have to change LIBS += D:\...\opencv3410\x64\mingw\lib\libopencv_*.a to your real abs. path in parking_system_backend.pro.
You may want remove icon by removing RC_ICONS = resource\main.ico from parking_system_backend.pro.

Microcontroller side:
Install Visual Studio Code + PlatformIO.
Opent the project folder, build.
Done
```
