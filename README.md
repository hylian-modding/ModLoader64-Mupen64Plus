# mupen64plus-npm

### Requirements:
- Node 10.16.0 or newer(32-bit on Windows, 64-bit on Linux)

(Node-GYP requirements)
- Python 2.7
- VC++(v141) Build Tools (On Windows)

### Build instructions:
Open a command-prompt or power-shell and navigate to
this directory. Then type the following commands in order:
- npm install
- npm run clean
- npm run build
(Optionally run the script_build.bat/sh)

(NOTE FOR LINUX): If the npm install fails to aquire packages, run:
- sudo chown -R $(whoami) ~/.npm
- sudo chown -R $(whoami) /usr/local/lib/node_modules

### DISCLAIMER:
We do not own the rights to some of the source provided.
The original sources can be found on the official mupen
development git: https://github.com/mupen64plus

A majority of the source (while modified) is borrowed from
the mupen64plus-ui-console source at the git page listed above.

### CREDITS:
- SpiceyWolf ~ Creation/Maintenance of the repository.
- Drahsid ~ Memory Tools.
- denoflions ~ Debugging and guidence of features.
