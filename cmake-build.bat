cd build
call "D:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" x64
cmake.exe --build . --target learn_3d_computer_graphics_programming
cd ..

start x64/learn_3d_computer_graphics_programming.exe
