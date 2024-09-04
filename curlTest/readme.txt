参考：https://www.cnblogs.com/codingnutter/p/5634467.html



# 下载curl源码和mbedtls源码
git clone git@172.20.60.2:pub/aws/curl.git
git clone git@172.20.60.2:pub/aws/mbedtls.git
git checkout origin/dev_tele
./build-8852.sh

# 进入curl源码目录里面，切换到tag: curl-8_4_0标签，执行buidconf生成configure文件。
cd curl/
git checkout curl-8_4_0
./buidconf
# 创建并进入编译路径
mkdir build && cd build

# 不需要支持https
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
../configure --prefix=$PWD/_install --host=arm-fullhanv3-linux-uclibcgnueabi -enable-static=true --enable-shared=no --without-ssl
# 执行make编译。
make
# 执行make install安装到当前目录下的_install目录里面
make install


# 需要支持https
# 配置环境变量
export CFLAGS="-mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4 -I/home/l/share/sda4/spark_aws_kaadas/cloud_telecom/include/"
export LDFLAGS="-L/home/l/share/sda4/spark_aws_kaadas/lib/fuhan8852/telecom/"
../configure --prefix=$PWD/_install --host=arm-fullhanv3-linux-uclibcgnueabi -enable-static=true --enable-shared=no --with-mbedtls
# 执行make编译。
make
# 执行make install安装到当前目录下的_install目录里面
make install

