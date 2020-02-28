# mupen64plus-npm

### Requirements:
- [Node 12.x](https://nodejs.org/dist/v12.16.1/node-v12.16.1-x86.msi)

(Node-GYP requirements)
- [Python 2.7](https://www.python.org/ftp/python/2.7.17/python-2.7.17.msi)
- [VS 2017 Community](https://my.visualstudio.com/Downloads?q=visual%20studio%202017&wt.mc_id=o~msft~vscom~older-downloads) (Windows only)

(Frontend requirements)
- [QT 5 / QMake](https://www.qt.io/download-thank-you?hsLang=en)

- Windows QT setup/build instructions:
    Install with default settings. when prompted for a QT version,
    select latest 5.14.1 build, only the x86 MSVC 2017 extension.

    (If Qt or VS2017 was installed to different directories the
    following instructions would need modified to reflect the
    altered directories. The next steps assume default directories)

    Open the file 'qtenv2.bat' for editing located in:
    "C:\Qt\5.14.1\msvc2017\bin" and add the following line to the bottom
    
    call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86

    Next, save the file and setup is complete.
    
    Now to build, open "Qt 5.14.1 (MSVC 2017 32-bit)" which
    can be found by searching qt in your windows start menu.

    cd command to the directory where you unpacked this repository
    and navigate further into 'subprojects\modloader64plus-frontend(cli or gui)\
    project' and then type these next commands to build

    qmake -makefile -spec win32-msvc mupen64plus-frontend.pro
    nmake

    Finally, you can run the script-build.bat to complete the
    mupen64plus.node build.

### Build instructions:
Open a command-prompt or power-shell and navigate to
this directory. Then type the command:
- npm install

(Optionally run the script_build.bat/sh)

(NOTE FOR LINUX): If the npm install fails to aquire packages, run:
- sudo chown -R $(whoami) ~/.npm
- sudo chown -R $(whoami) /usr/local/lib/node_modules

### DISCLAIMER:
We do not own the rights to some of the source provided.
The original sources can be found on the official mupen
development git: https://github.com/mupen64plus

The sub-project frontend sources (while modified) are borrowed
from the mupen64plus-ui-console source at the git page listed above,
and the mupen64plus-gui at https://github.com/m64p/mupen64plus-gui.

### CREDITS:
- SpiceyWolf ~ Creation/Maintenance of the repository.
- denoflions ~ Debugging and guidence of features.
