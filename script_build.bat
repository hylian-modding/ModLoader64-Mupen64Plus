:: Go build the submodules first
cd .\subprojects\m64p-frontend-cli\project
call qmake
call make
cd ..\..\m64p-frontend-gui\project
call qmake
call make
cd ..\..\..

:: Build mupen64plus.node
npm install

call npm install

:: Keep console open when script finishes
pause
