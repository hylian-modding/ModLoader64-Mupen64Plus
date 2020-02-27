# Go build the submodules first
cd ./subprojects/m64p-frontend-cli/project
qmake
make
cd ../../m64p-frontend-gui/project
qmake
make
cd ../../..

# Build mupen64plus.node
npm install

# Keep console open when script finishes
echo "Press any key to continue"
read 1
