# 下载解压
wget https://github.com/eclipse/paho.mqtt.c/archive/refs/tags/v1.3.0.tar.gz
tar -xzf v1.3.0.tar.gz && cd paho.mqtt.c-1.3.0

# 创建并进入编译路径
mkdir build && cd build


# 不需要支持mqtts
#配置环境变量
export PATH=$PATH:/opt/fuhan/toolchain/arm-fullhanv3-linux-uclibcgnueabi-b6/bin/:/opt/fullhan/toolchain/arc-fullhan-elf32/bin/
export CFLAGS="-mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4"
export CPPFLAGS="-mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4"
export CC=arm-fullhanv3-linux-uclibcgnueabi-gcc
export CXX=arm-fullhanv3-linux-uclibcgnueabi-g++
export AR=arm-fullhanv3-linux-uclibcgnueabi-ar
export AS=arm-fullhanv3-linux-uclibcgnueabi-as
export LD=arm-fullhanv3-linux-uclibcgnueabi-ld
export RANLIB=arm-fullhanv3-linux-uclibcgnueabi-ranlib
export NM=arm-fullhanv3-linux-uclibcgnueabi-nm

# 使用CMake进行编译配置
{
    参数解析：
    DPAHO_BUILD_SHARED：是否生成动态库
    DPAHO_BUILD_STATIC：是否生成静态库
    DPAHO_ENABLE_TESTING：是否编译测试程序
    DPAHO_ENABLE_CPACK：禁用 CPack 打包功能（如生成 DEB/RPM 安装包）
    DPAHO_WITH_SSL：添加 OpenSSL 验证支持
}
cmake .. -DCMAKE_INSTALL_PREFIX=$PWD/install \
    -DCMAKE_SYSTEM_NAME=Linux -DCMAKE_SYSTEM_PROCESSOR=armv7-a \
    -DPAHO_BUILD_SHARED=FALSE \
    -DPAHO_BUILD_STATIC=TRUE \
    -DPAHO_ENABLE_TESTING=FALSE \
    -DPAHO_ENABLE_CPACK=FALSE \
    -DPAHO_WITH_SSL=TRUE \
    -DOPENSSL_DIR=/home/l/share/sda4/unit-test/openssl/openssl-1.1.1s/build/install \
    -DOPENSSL_LIB=/home/l/share/sda4/unit-test/openssl/openssl-1.1.1s/build/install/lib \
    -DOPENSSL_INCLUDE_DIR=/home/l/share/sda4/unit-test/openssl/openssl-1.1.1s/build/install/include \
    -DOPENSSLCRYPTO_LIB=/home/l/share/sda4/unit-test/openssl/openssl-1.1.1s/build/install/lib

# 执行make编译。
make
# 执行make install安装到当前目录下的install目录里面
make install
