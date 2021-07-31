
# build pi pico project
mkdir build
cd build
then run

cmake -G "Eclipse CDT4 - MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE -DCMAKE_ECLIPSE_MAKE_ARGUMENTS=-j3 \
-D"CMAKE_MAKE_PROGRAM:PATH=D:/workspace/mingw32_env/bin/mingw32-make.exe" \
-D"CMAKE_CXX_COMPILER:PATH=C:/GNU_Arm_Embedded_Toolchain/10_2020-q4-major/bin/arm-none-eabi-g++.exe" \
-DCMAKE_ECLIPSE_VERSION=4.1 ../

or

cmake -G "Eclipse CDT4 - MinGW Makefiles" -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE -DCMAKE_BUILD_TYPE=Debug ../



openocd run configuration
 
openocd -f interface/picoprobe.cfg -f target/rp2040.cfg -s tcl

run gdb-multiarch.exe then excute:
target remote localhost:3333
monitor reset init