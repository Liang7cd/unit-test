下载地址
https://www.tcpdump.org/

根据情况配置环境变量
export PATH=$PATH:/opt/fuhan/toolchain/arm-fullhanv3-linux-uclibcgnueabi-b6/bin

首先编译libpcap

解压创建编译目录
tar -xvzf libpcap-1.10.1.tar.gz
cd libpcap-1.10.1/
mkdir build && cd build

配置编译信息，编译安装
../configure --host=arm-linux CC=arm-fullhanv3-linux-uclibcgnueabi-gcc --prefix=$PWD/install
make && make install

然后编译tcpdump

解压创建编译目录
tar -xvzf libpcap-1.10.1.tar.gz
cd tcpdump-4.99.1/
mkdir build && cd build

链接libpcap库
ln -s ../../libpcap-1.10.1/build/install/ libpcap

配置编译信息，开始编译（静态编译）
../configure --host=arm-linux CC=arm-fullhanv3-linux-uclibcgnueabi-gcc  CFLAGS="-I./libpcap/include/ -L./libpcap/lib/ -static"
make

把tcpdump拷贝到板端运行
./tcpdump -i wlan0 -w /mnt/out.pcap

《交叉编译tcpdump》
https://blog.csdn.net/qwqwqw1232/article/details/121253015
