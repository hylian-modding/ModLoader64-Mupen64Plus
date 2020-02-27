# mupen64plus-npm

### Requirements:
- Node 12.x.x or newer(32-bit on Windows, 64-bit on Linux)

(Node-GYP requirements)
- Python 2.7
- VC++(v141) Build Tools (On Windows)

(Frontend requirements)
- QT 5 / QMake

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
