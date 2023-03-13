下载地址：
https://mirrors.edge.kernel.org/pub/linux/libs/security/linux-privs/libcap2/

//根据情况配置环境变量
export PATH=$PATH:/opt/fuhan/toolchain/arm-fullhanv3-linux-uclibcgnueabi-b6/bin

//解压下载的包
tar -xvzf libcap-2.66.tar.gz
cd libcap-2.66/

# 方案1
{
    //先用Ubuntu的gcc编译出‘cap_names.h’和‘cap_names.list.h’这两个文件，并备份
    make
    cp libcap/cap_names.h libcap/cap_names.list.h ../temp/plan1/

    //清除Ubuntu的gcc编译出来的文件
    make clean
    //拷贝备份文件回来
    cp ../temp/plan1/cap_names.h ../temp/plan1/cap_names.list.h libcap/

    //拷贝已经修改好的Makefile替换原来的Makefile
    cp ../temp/plan1/Makefile libcap/Makefile
}
# 方案2
{
    //先用Ubuntu的gcc编译出可执行程序‘_makenames’，并备份
    make
    cp libcap/_makenames ../temp/plan2/

    //清除Ubuntu的gcc编译出来的文件
    make clean
    //拷贝备份文件回来
    cp ../temp/plan2/_makenames libcap/

    //拷贝已经修改好的Makefile替换原来的Makefile
    cp ../temp/plan2/Makefile libcap/Makefile
}

//开始编译并安装（指定编译链以及指定编译时使用静态库）
make CROSS_COMPILE=arm-fullhanv3-linux-uclibcgnueabi- DYNAMIC=no
make DESTDIR=$PWD/install install

《libcap 交叉编译》
https://www.cnblogs.com/AaronD-Chain/p/6179505.html
