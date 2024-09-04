先下载源码
wget http://www.ijg.org/files/jpegsrc.v9d.tar.gz
tar -xzvf jpegsrc.v9d.tar.gz
cd jpeg-9d
mkdir build && cd build


配置环境
export CFLAGS="-mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4"
export CPPFLAGS="-mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4"
export AR=arm-fullhanv3-linux-uclibcgnueabi-ar
export AS=arm-fullhanv3-linux-uclibcgnueabi-as
export LD=arm-fullhanv3-linux-uclibcgnueabi-ld
export RANLIB=arm-fullhanv3-linux-uclibcgnueabi-ranlib
export CC=arm-fullhanv3-linux-uclibcgnueabi-gcc
export NM=arm-fullhanv3-linux-uclibcgnueabi-nm

# CC=你自己的交叉编译器的路径
../configure --host=arm-linux CC=arm-fullhanv3-linux-uclibcgnueabi-gcc LDFLAGS=-static --prefix=$PWD/install
make
make install
