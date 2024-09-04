
主要参考
https://blog.csdn.net/qq8512852/article/details/132224066

下载依赖：
git clone git://git.netfilter.org/libmnl
git clone git://git.netfilter.org/libnftnl

下载iptables源码：
网络好可以直接git
git clone git://git.netfilter.org/iptables

网络不好可以手动去下载
https://www.netfilter.org/projects/iptables/downloads.html


依赖autoconf，先安装
sudo apt install -y autoconf automake libtool --fix-missing
依赖pkg-config，先安装
sudo apt-get install pkg-config

# 创建build编译
cd  libmnl
./autogen.sh
mkdir build && cd build
# CC=你自己的交叉编译器的路径
../configure --host=arm-linux CC=arm-fullhanv3-linux-uclibcgnueabi-gcc LDFLAGS=-static --prefix=$PWD/install
make
make install
export PKG_CONFIG_PATH=$PWD/install/lib/pkgconfig:$PKG_CONFIG_PATH




# 同样，在libnftnl内创建一个__install用于存放输出
cd  libnftnl
./autogen.sh
mkdir build && cd build
# 这里有个细节就是需要通过PKG_CONFIG_PATH=指定刚才编译的libmnl的pkg的路径，如果不知道，会提示你No package 'libmnl' found；你当然可以通过设置变量跳过这个功能，但是为了保证其完整性我们做全套。
# 另外这个路径就是刚才编译的libmnl的__install/lib/pkgconfig，只要成功编译就会有这个输出
# 记得把下面的交叉编译器换成你自己的交叉编译器路径
../configure --host=arm-linux CC=arm-fullhanv3-linux-uclibcgnueabi-gcc LDFLAGS=-static --prefix=$PWD/install
make
make install
export PKG_CONFIG_PATH=$PWD/install/lib/pkgconfig:$PKG_CONFIG_PATH




# 先解压
tar -xf iptables-1.8.9.tar.xz
cd iptables-1.8.9
./autogen.sh
mkdir build && cd build
# 将libmnl和libnftnl的pkgconfig路径都添加进去才能顺利配置编译
../configure --host=arm-linux CC=arm-fullhanv3-linux-uclibcgnueabi-gcc --enable-static --disable-shared --disable-ipv6 --disable-largefile --prefix=$PWD/install
make
make install
