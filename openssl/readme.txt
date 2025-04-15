# 下载解压
wget https://www.openssl.org/source/openssl-1.1.1s.tar.gz
tar -xzf openssl-1.1.1s.tar.gz && cd openssl-1.1.1s

# 创建并进入编译路径
mkdir build && cd build

# 不需要支持mqtts
#配置环境变量
export PATH=$PATH:/opt/fuhan/toolchain/arm-fullhanv3-linux-uclibcgnueabi-b6/bin/:/opt/fullhan/toolchain/arc-fullhan-elf32/bin/
export CFLAGS="-mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4"
export CPPFLAGS="-mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4"
export AR=arm-fullhanv3-linux-uclibcgnueabi-ar
export AS=arm-fullhanv3-linux-uclibcgnueabi-as
export LD=arm-fullhanv3-linux-uclibcgnueabi-ld
export RANLIB=arm-fullhanv3-linux-uclibcgnueabi-ranlib
export CC=arm-fullhanv3-linux-uclibcgnueabi-gcc
export NM=arm-fullhanv3-linux-uclibcgnueabi-nm

# 执行configure生存Makefile文件。
# no-async禁用异步 I/O 支持，no-asm禁用汇编优化代码，no-shared生成静态链接库
../Configure linux-armv4 no-async no-asm no-shared --prefix=$PWD/install
# 执行make编译。
make -j4
# 执行make install安装到当前目录下的install目录里面
make install

# 设置 pkg-config 自动发现 OpenSSL 的头文件和库路径（当别的库需要依赖openssl进行编译时可以设置）
export PKG_CONFIG_PATH=$PWD/install/lib/pkgconfig:$PKG_CONFIG_PATH
