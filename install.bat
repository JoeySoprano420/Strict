@echo off
echo === Building Strict Compiler ===
if not exist build mkdir build
cd build
cmake ..
cmake --build . --config Release
cd ..
echo Done. You can now run: build\Release\strictc.exe examples\hello.strict -o hello.exe
pause
