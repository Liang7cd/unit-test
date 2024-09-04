rm -rf build
rm -rf library/libmbed*.a
export PATH=$PATH:/opt/fuhan/toolchain/arm-fullhanv3-linux-uclibcgnueabi-b6/bin/
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=./fullhan8852.cmake -DENABLE_TESTING=Off -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
make -j10
cd -
cp -avf ./build/library/libmbed*.a ../telecom/

