cd build
cmake --version
call "D:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Auxiliary/Build/vcvarsall.bat" x64
cmake.exe --fresh -G "Ninja" -DCMAKE_C_COMPILER="D:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/Llvm/x64/bin/clang-cl.exe" -DCMAKE_CXX_COMPILER="D:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/Llvm/x64/bin/clang-cl.exe" -DCMAKE_TOOLCHAIN_FILE=D:/All/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
pause
