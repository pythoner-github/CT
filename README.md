环境准备

    1) 安装操作系统(32位)
        ubuntu 14.04.5(windows上, 可以安装在VirtualBox虚拟机上)
        ISO下载地址: http://mirrors.163.com/ubuntu-releases/14.04.5/ubuntu-14.04.5-desktop-i386.iso

    2) 更新操作系统
        apt update
        apt upgrade -y

    3) 安装git
        apt-get install git

    3) 安装需要的库文件
        apt install libgoocanvas-dev libxtst-dev libdbus-glib-1-dev libopencv-dev libsqlite3-dev libcups2-dev libx264-dev
        apt install libxml2-dev libssl-dev libwrap0-dev

版本编译
    make
    strip 09001Color

i18n文件生成
    ./update_po.sh

版本调试
    1) 将文件09001Color拷贝到U盘(例如U盘的CT目录下)
    2) 将U盘插到设备上
    3) 按Ctrl+Alt+T, 将会弹出命令行窗口
    4) 在命令行窗口, 输入top命令，查看09001Color进程对应的PID(例如3550)
    5) 按Ctrl+C, 结束top命令, 输入kill命令杀死09001Color(例如 kill 3550)
    6) 创建一个挂载U盘的目录(例如a)
    7) 使用mount命令挂载U盘(例如mount /dev/sdb1 a), 此时，在目录a下就可以看到U盘的内容
    8) 将U盘CT目录下的程序09001Color拷贝到系统, 替换原来的程序
    9) 使用umount命令卸载U盘(例如umount a)
    10) 执行启动脚本, 启动程序
